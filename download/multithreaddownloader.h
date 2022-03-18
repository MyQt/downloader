#ifndef MULTITHREADDOWNLOADER_H
#define MULTITHREADDOWNLOADER_H
#include "downloadmission.h"
#include "multithreadeddownloaderwriter.h"

class QNetworkAccessManager;
class FileModel;
class ClassifyModel;
class SqlDataHandle;
class MultithreadDownloader : public AbstractMission
{
    Q_OBJECT
public:
    enum Error {
        OpenFileFailed,
        DownloadFailed
    };

    explicit MultithreadDownloader(QObject* parent = nullptr);
    ~MultithreadDownloader() Q_DECL_OVERRIDE;
    /**
     * @brief 线程数量操作
    */
    void setThreadCount(int num) { mThreadCount = num; }
    int threadCount() const { return  mThreadCount; }
    /**
     * @brief 初始化下载
     * @note 获取下载文件大小，文件名
     * @return 成功标志
    */
    bool load(FileModel* fileModel = nullptr);
    void setFileName(const QString& name) { mWriter->setFileName(name); }
    const QString& fileName() const { return  mWriter->fileName(); }
    qint64 downloadedSize() const { return mWriter->size(); }
    bool isRangeSupport() const { return mIsRangeSupport; }
    void setFolderName(const QString& name) { mWriter->setDownloadPath(name); }
    void setNotifyInterval(int interval) { mNotifyInterval = interval; }
    int notifyInterval() const { return mNotifyInterval; }
    QNetworkReply::NetworkError networkError() const { return mNetworkError; }
    const QString& networkErrorString() const { return mNetworkErrorString; }
    void start() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void restart() Q_DECL_OVERRIDE;
    FileModel* fileModel() const { return mFileModel; }
    MultithreadedDownloaderWriter* writer() const { return mWriter;}
signals:
    void error(int id, const MultithreadDownloader::Error err);
    void downloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);
private slots:
    void errorHanding(QNetworkReply::NetworkError err);
    void onFinished();
    void onUpdateMission(qint64 newStart, qint64 end);

private:
    QNetworkAccessManager* mManager = nullptr;
    MultithreadedDownloaderWriter* mWriter = nullptr;
    int mThreadCount = 0; // 线程数量
    int mFinishedCount = 0; // 已经完成的线程数量
    int mTimerId = 0; // 定时器ID
    int mNotifyInterval = 500; // 通知间隔
    bool mIsRangeSupport = false; // 是否支持断点续传
    QList<DownloadMission*> mListMission; // 分断任务列表
    QNetworkReply::NetworkError mNetworkError = QNetworkReply::NoError;
    QString mNetworkErrorString;
    inline DownloadMission* createMission(qint64 start, qint64 end);
    inline void destroyMissions();
    inline void updateProgress();
    inline void reset();
    void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
    FileModel* mFileModel = nullptr;
    ClassifyModel* mClassifyModel = nullptr;
    SqlDataHandle* mSQLHandle = nullptr;
};

#endif // MULTITHREADDOWNLOADER_H
