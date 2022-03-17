#ifndef FILEMODEL_H
#define FILEMODEL_H
#include<QString>
#include<QDateTime>
#include<QPair>
#include "datamodel.h"

class FileModel : public DataModel
{

public:
    explicit FileModel(){}
    explicit FileModel(FileModel& other)=delete;
    FileModel& operator=(const FileModel&)=delete;
    qint64 size() const { return  mListDownSize[0].first; }
    bool status() const { return  mStatus; }
    int classifyID() const { return mClassifyID; }
    const QString& tips() const { return  mTips; }
    const QDateTime& createTime() const { return  mCreateTime; }
    const QDateTime& lastTime() const { return mLastTime; }
    const QString& url() const { return mUrl; }
    QList<QPair<qint64,qint64>>& downloadList() { return mListDownSize; }
    QString downloadListToString();
    qint64 oldReceivedSize() const { return mOldReceivedSize; }
    qint64 downloadedSize();
    qint64 leftSize();

    void setStatus(bool status) { mStatus = status; }
    void setClassifyID(int classifyID) { mClassifyID = classifyID; }
    void setTips(const QString& tips) { mTips = tips; }
    void setCreateTime(const QDateTime& time) { mCreateTime = time; }
    void setLastTime(const QDateTime& time) { mCreateTime = time; }
    void setUrl(const QString& strUrl) { mUrl = strUrl; }
    void setDownloadList(const QString& strList);
    void setOldReceivedSize(qint64 oldReceivedSize) { mOldReceivedSize = oldReceivedSize; }
    void setFinishDownList() { // 完成状态只保留完成大小
        mListDownSize.erase(mListDownSize.begin()+1, mListDownSize.end());
    }
private:
    QList<QPair<qint64,qint64>>   mListDownSize;
    bool    mStatus;
    int     mClassifyID;
    QString mUrl;
    QString mTips;
    QDateTime mCreateTime;
    QDateTime mLastTime;
    // 辅助成员
    qint64 mOldReceivedSize = 0; // 上次变动前接收的数据大小
};

#endif // FILEMODEL_H
