#include "filemodel.h"

void FileModel::setDownloadList(const QString &strList)
{
    QStringList downSizeList = strList.split(",");
    for (int i = 0; i < downSizeList.size(); i+=2) {
        mListDownSize.push_back(QPair<qint64, qint64>(downSizeList[i].toLongLong(),downSizeList[i+1].toLongLong()));
    }
}

QString FileModel::downloadListToString()
{
    QString strResult;
    for (int i = 0; i < mListDownSize.size(); i++) {
        strResult += QString("%1").arg(mListDownSize[i].first);
        strResult += ",";
        strResult += QString("%1").arg(mListDownSize[i].second);
        if ( i < mListDownSize.size()-1) {
            strResult+=",";
        }
    }

    return strResult;
}

qint64 FileModel::leftSize()
{
    qint64 leftSize = 0.0;
    for (int i = 0; i < mListDownSize.size(); i++) {
        if (i > 0) {
            leftSize += mListDownSize[i].second - mListDownSize[i].first;
        }
    }
    return  leftSize;
}

qint64 FileModel::downloadedSize()
{
    return  mListDownSize[0].first - leftSize();
}
