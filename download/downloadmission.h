#ifndef DOWNLOADMISSION_H
#define DOWNLOADMISSION_H

#include<QtNetwork/QNetworkReply>
#include "abstractmission.h"
#include "multithreadeddownloaderwriter.h"
class QNetworkAccessManager;

class DownloadMission : public AbstractMission
{
    Q_OBJECT
public:
    explicit DownloadMission(QObject* parent);
    ~DownloadMission() Q_DECL_OVERRIDE;
    /**
 * @brief 设置下载范围
 * @param start 下载起始位置
 * @param end 下载结束位置
 */
    void setRange(qint64 start, qint64 end) {
        if (mState == Stopped) {
            mStart = start;
            mEnd = end;
            mTotalSize = end - start;
        }
    }

    void setManager(QNetworkAccessManager* manager) { mManager = manager; }
    QNetworkAccessManager* manager() const { return mManager; }

    void setWriter(MultithreadedDownloaderWriter* writer) { mWriter = writer; }
    MultithreadedDownloaderWriter* writer() const { return mWriter; }
    qint64  downloadedSize() const { return  mDownloadedSize; }
    QString replyErrorString() const {
        return mReply == nullptr ? QString() : mReply->errorString();
    }
    void start() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void restart() override;
signals:
    void replyError(QNetworkReply::NetworkError err);
    void updateMission(qint64 newStart, qint64 end);
private slots:
    void onFinished();
    void writeData();
    void onReplyError(QNetworkReply::NetworkError err){}

private:
    MultithreadedDownloaderWriter* mWriter = nullptr;
    QNetworkAccessManager* mManager = nullptr;
    QNetworkReply* mReply = nullptr;
    qint64 mStart = -1;
    qint64 mEnd = -1;
    qint64 mTotalSize = 0;
    qint64 mDownloadedSize = 0;
    inline void reset();
    inline void destroyReply();
};

#endif // DOWNLOADMISSION_H
