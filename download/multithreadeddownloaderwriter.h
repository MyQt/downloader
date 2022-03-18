#ifndef MULTITHREADEDDOWNLOADERWRITER_H
#define MULTITHREADEDDOWNLOADERWRITER_H

#include<QFile>
#include<QMutex>
#include<QThread>

typedef  QPair<QByteArray, qint64> WriteMission;

class MultithreadedDownloaderWriter Q_DECL_FINAL : public QThread
{
    Q_OBJECT
public:
    explicit MultithreadedDownloaderWriter(QObject* parent = nullptr);
    ~MultithreadedDownloaderWriter() Q_DECL_OVERRIDE;
    /*
     * @brief 打开文件
     * @return 操作是否成功
    */
    bool open() {
        return m_downloadFile.open(QFile::WriteOnly) && m_downloadFile.resize(m_fileSize);
    }
    void close() {
        m_downloadFile.close();
    }
    void deleteFile() { m_downloadFile.remove(); }
    void setFileName(const QString& name) {
        mFileName = name;
    }
    void setDownloadName() {
        m_downloadFile.setFileName(mFilePath+"/"+mFileName);
    }
    void setDownloadPath(const QString& name) { mFilePath = name; }
    QString fileName() const { return mFileName; }
    void setSize(const qint64 size) { m_fileSize = size; }
    qint64 size() const { return m_fileSize; }
    /**
 * @brief 添加写任务
 * @param data 写入数据
 * @param seek 写入偏移量
 */
    void write(const QByteArray& data, const qint64 seek);

private:
    void run() Q_DECL_OVERRIDE;

    qint64 m_fileSize = 0;
    QFile m_downloadFile;
    QString mFilePath;
    QString mFileName;
    mutable QMutex m_mutex; // 提供了线程之间的序列化的权限
    QList<WriteMission> m_writeList;
};

#endif // MULTITHREADEDDOWNLOADERWRITER_H
