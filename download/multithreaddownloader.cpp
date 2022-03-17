#include "multithreaddownloader.h"
#include "model/filemodel.h"
#include "model/classifymodel.h"
#include "operator/sqldatahandle.h"
#include "operator/handlefactory.h"
#include "operator/sqlclassifyadapter.h"
#include "operator/sqlfileadapter.h"
#include "until.h"
#include<QPointer>
#include<QEventLoop>
#include<QTimerEvent>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkAccessManager>
#include<QFileInfo>
#include<QDateTime>
#pragma execution_character_set("utf-8")

MultithreadDownloader::MultithreadDownloader(QObject *parent)
    :AbstractMission(parent),
     mManager(new QNetworkAccessManager(this)),
     mWriter(new MultithreadedDownloaderWriter(this)),
     mThreadCount(QThread::idealThreadCount())
{

}

MultithreadDownloader::~MultithreadDownloader()
{
    if (mState == Running) {
        this->MultithreadDownloader::stop();
    }
    if (mWriter->isRunning()) {
        mWriter->terminate();
    }
}

bool MultithreadDownloader::load(FileModel* fileModel)
{
    mFileModel = fileModel;
    if (!mUrl.isValid() || mState != Stopped) {
        return false;
    }
    QNetworkRequest request(mUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    QPointer<QNetworkReply> reply = mManager->head(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);
    qint64 size = 0;
    bool ok = false;
    if (reply->hasRawHeader("Content-Length")) {
        size = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(&ok);
    }
    QString fileName;
    if (reply->hasRawHeader("Content-Disposition")) {
        QString strFileName = reply->header(QNetworkRequest::ContentDispositionHeader)
                .toString();
        QStringList strList;
        if (strFileName.contains("*=")) {
            strList = strFileName.split("*=utf-8");
        } else {
            strList = strFileName.split("filename=");
        }
        fileName = strList.last();
        fileName = fileName.replace("utf-8","");
    } else {
        fileName = reply->url().fileName();
    }
    fileName = Until::validateString(fileName);
    mIsRangeSupport = reply->rawHeader("Accept-Ranges") == "bytes";
    if (fileName.isEmpty() || (!ok) || size <= 0) {
        return false;
    } else {
        QList<QPair<qint64,qint64>> listSeg;
        mSQLHandle = dynamic_cast<SqlDataHandle*>(HandleFactory::getInstance()->getProduct(EHT_SQL));

        if (mFileModel == nullptr) {
            mWriter->setFileName(fileName);
            mWriter->setSize(size);
            if (mIsRangeSupport) {
                qint64 start = 0, end = 0;
                qint64 segmentSize = mWriter->size() / mThreadCount;
                listSeg.push_back(QPair<qint64,qint64>(size,size));
                for (int i = 0; i < mThreadCount; i++) {
                    start = i * segmentSize;
                    if (i != mThreadCount - 1) { // 不是最后一个任务
                        end = start + segmentSize - 1;
                    } else {
                        end = mWriter->size();
                    }
                    listSeg.push_back(QPair<qint64,qint64>(start,end));
                }
            } else { // 不支持断点续传，则一次性下载完成
                listSeg.push_back(QPair<qint64,qint64>(0, size));
                mThreadCount = 1;
            }
            QList<QString> strListVal;
            strListVal.push_back(fileName);
            strListVal.push_back(mUrl.toString());
            QString strSegList;
            int nSegIndex = 0;
            foreach(auto seg, listSeg) {
                strSegList +=QString("%1").arg(seg.first);
                strSegList +=",";
                strSegList +=QString("%1").arg(seg.second);
                if (nSegIndex < listSeg.size()-1) {
                    strSegList+=",";
                }
                nSegIndex++;
            }
            strListVal.push_back(strSegList);
            QFileInfo fileInfo(fileName);
            SQLClassifyAdapter* pAdapter = (SQLClassifyAdapter*)mSQLHandle->sqlAdapter(EAT_CLASSIFY);

            mClassifyModel = pAdapter->findModel(fileInfo.suffix());
            if (mClassifyModel == nullptr)
                mClassifyModel = pAdapter->defaultModel();
            setFolderName(mClassifyModel->savePath());
            mWriter->setDownloadName();
            strListVal.push_back(QString::number(0));
            strListVal.push_back(QString::number(mClassifyModel->id()));
            strListVal.push_back(QString("说明"));
            QDateTime nowTime = QDateTime::currentDateTime();
            strListVal.push_back(nowTime.toString("yyyy-MM-dd hh:mm:ss"));
            strListVal.push_back(nowTime.toString("yyyy-MM-dd hh:mm:ss"));

            mFileModel = new FileModel();

            mFileModel->setName(fileName);
            mFileModel->setUrl(mUrl.toString());
            mFileModel->setDownloadList(strSegList);
            mFileModel->setClassifyID(mClassifyModel->id());
            mFileModel->setTips("说明");
            mFileModel->setCreateTime(nowTime);
            mFileModel->setLastTime(nowTime);

            int fileId = mSQLHandle->sqlAdapter(EAT_FILE)->insert(strListVal);
            mFileModel->setID(fileId);
            mSQLHandle->sqlAdapter(EAT_FILE)->insertModel(mFileModel);
        } else {
            mClassifyModel = dynamic_cast<ClassifyModel*>(mSQLHandle->sqlAdapter(EAT_CLASSIFY)->findDataModel(mFileModel->classifyID()));
            mWriter->setFileName(fileName);
            mWriter->setSize(size);
            setFolderName(mClassifyModel->savePath());
            mWriter->setDownloadName();
            mThreadCount = mFileModel->downloadList().size() - 1;

        }

    }

    return true;
}

void MultithreadDownloader::start()
{
    if (mState == Running || mWriter->fileName().isEmpty() ||
            mWriter->size() <= 0 || !mUrl.isValid())
        return;
    if (mState == Stopped) {
        if (!mWriter->open()) {
            emit error(mFileModel->id(), OpenFileFailed);
            return;
        }
        int nIndex = 0;
        foreach (auto seg, mFileModel->downloadList()) {
            if (nIndex > 0)
                mListMission.push_back(createMission(seg.first, seg.second));
            nIndex++;
        }
        setFinished(false);
    } else { // 目前处于暂停状态
        foreach(auto mission, mListMission) {
            if (!mission->isFinished()) {
                mission->start();
            }
        }
    }
    mTimerId = startTimer(mNotifyInterval);
    updateState(Running);
}

void MultithreadDownloader::pause()
{
    if (mState == Running) {
        if (!mIsRangeSupport) {
            qWarning() <<__FUNCTION__<<" :不支持暂停";
            return;
        }
        foreach(auto mission, mListMission) {
            if (!mission->isFinished()) {
                mission->pause();
            }
        }
        killTimer(mTimerId);
        updateState(Paused);
    }
}

void MultithreadDownloader::stop()
{
    if (mState != Stopped) {
        if (mState == Running) {
            killTimer(mTimerId);
        }
        destroyMissions();
        if (mWriter->isRunning()) {
            QEventLoop loop;
            QObject::connect(mWriter, &MultithreadedDownloaderWriter::finished,
                             &loop, &QEventLoop::quit);
            loop.exec();
        }
        mWriter->close();
        mFinishedCount = 0;
        reset();
        updateState(Stopped);
    }
}

void MultithreadDownloader::errorHanding(QNetworkReply::NetworkError err)
{
    mNetworkError = err;
    mNetworkErrorString = qobject_cast<DownloadMission*>
            (this->sender())->replyErrorString();
    if (err == QNetworkReply::OperationCanceledError ||
            err == QNetworkReply::NoError) {
        return;
    }
    pause();
    emit error(mFileModel->id(), DownloadFailed);
}

void MultithreadDownloader::onUpdateMission(qint64 newStart, qint64 end)
{
    for (int i = 1; i < mFileModel->downloadList().size(); i++) {
        if (mFileModel->downloadList()[i].second == end) {

            if (newStart == end) { // 本段下载完成
                mFileModel->downloadList().removeAt(i);
            } else {
                mFileModel->downloadList()[i] = QPair<qint64,qint64>(newStart, end);
            }
            QDateTime nowTime = QDateTime::currentDateTime();
            mFileModel->setLastTime(nowTime);
            QString lastTime = nowTime.toString("yyyy-MM-dd hh:mm:ss");
            mSQLHandle->sqlAdapter(EAT_FILE)->update(QStringList{"download_list", "lastTime"}, QList<QString>{mFileModel->downloadListToString(), lastTime}, QList<DataType>{EDT_STRING,EDT_DATETIME}, mFileModel->id());
            break;
        }
    }

    return;
}

void MultithreadDownloader::onFinished()
{
    mFinishedCount++;
    qDebug()<<"MultithreadDownloader: finishedCount: " << mFinishedCount;
    if (mIsRangeSupport && mFinishedCount != mThreadCount) {
        return;
    }
    updateProgress();
    stop();
    mFileModel->setStatus(true);
    mFileModel->setFinishDownList();
    mSQLHandle->sqlAdapter(EAT_FILE)->update(QStringList{"download_list", "status"}, QList<QString>{mFileModel->downloadListToString(), QString::number(mFileModel->status())}, QList<DataType>{EDT_STRING,EDT_BOOL}, mFileModel->id());
    emit finished(mFileModel->id());
}

DownloadMission* MultithreadDownloader::createMission(qint64 start, qint64 end)
{
    DownloadMission* mission = new DownloadMission(this);
    QObject::connect(mission, &DownloadMission::finished, this,
                     &MultithreadDownloader::onFinished);
    QObject::connect(mission, &DownloadMission::replyError, this,
                     &MultithreadDownloader::errorHanding);
    QObject::connect(mission, &DownloadMission::updateMission, this, &MultithreadDownloader::onUpdateMission);
    mission->setManager(mManager);
    mission->setWriter(mWriter);
    mission->setRange(start, end);
    mission->setUrl(mUrl);
    mission->start();

    return mission;
}

void MultithreadDownloader::destroyMissions()
{
    foreach(auto mission, mListMission) {
        if (mission->state() != Stopped) {
            mission->stop();
        }
        mission->deleteLater();
    }
    mListMission.clear();
}

void MultithreadDownloader::updateProgress()
{
    qint64 bytesReceived = 0;
    foreach(auto mission, mListMission) {
        bytesReceived += mission->downloadedSize();
    }
    emit downloadProgress(mFileModel->id(), bytesReceived, mWriter->size());
}

void MultithreadDownloader::reset()
{
    mWriter->setFileName("");
    mWriter->setSize(0);
    mTimerId = -1;
    mIsRangeSupport = false;
}

void MultithreadDownloader::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == mTimerId) {
        updateProgress();
    }
}
