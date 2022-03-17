#include "downloadmission.h"
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkAccessManager>

DownloadMission::DownloadMission(QObject* parent)
    :AbstractMission(parent)
{

}

DownloadMission::~DownloadMission()
{
    if (mState == Running) {
        this->DownloadMission::stop();
    }
}

void DownloadMission::start()
{
    if (mState == Running) return;
    QNetworkRequest request(mUrl);
    if (mStart >= 0 && mEnd > 0) {
        request.setRawHeader("Range", QString("bytes=%1-%2")
                             .arg(mStart + mDownloadedSize).arg(mEnd).toLatin1());
    }
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    mReply = mManager->get(request);
    QObject::connect(mReply, &QNetworkReply::finished, this, &DownloadMission::onFinished);
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
    QObject::connect(mReply, &QNetworkReply::errorOccurred, this, &DownloadMission::onReplyError);
#else
    QObject::connect(mReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                     this, &DownloadMission::onReplyError);
#endif
    QObject::connect(mReply, &QNetworkReply::readyRead, this,
                     &DownloadMission::writeData);
    this->setFinished(false);
    this->updateState(Running);
}

void DownloadMission::pause()
{
    if (mState == Running) {
        updateState(Paused);
        mReply->abort();
        destroyReply();
    }
}

void DownloadMission::stop()
{
    if (mState != Stopped) {
        if (mState == Running) {
            mReply->abort();
            destroyReply();
        }
        updateState(Stopped);
        reset();
    }
}

void DownloadMission::onFinished()
{
    if (mState == Running) {
        updateState(Stopped);
        setFinished();
    }
}

void DownloadMission::writeData()
{
    QByteArray data = mReply->readAll();
    mWriter->write(data, mStart + mDownloadedSize);
    mDownloadedSize += data.size();
    emit updateMission(mStart+mDownloadedSize, mEnd);

}

void DownloadMission::reset()
{
    mStart = -1;
    mEnd = -1;
    mTotalSize = 0;
    mDownloadedSize = 0;
}

void DownloadMission::destroyReply()
{
    mReply->deleteLater();
    mReply = nullptr;
}
