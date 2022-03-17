#include "sqlfileadapter.h"
#include<QDebug>
#include "model/filemodel.h"

SQLFileAdapter::SQLFileAdapter()
{
    mType = EAT_FILE;
}

bool SQLFileAdapter::init(QSqlQuery *query)
{
    SQLAdapter::init(query);
    mTableName = "File";
//    QStringList strList{"name,256", "url,512", "download_list,256","status,0","classify_id, 0","tips,256","createTime,'1970-2-21 00:00::00'","lastTime,'1970-2-21 00:00::00'"};
//    QList<DataType> listType{EDT_STRING, EDT_STRING, EDT_STRING, EDT_BOOL,EDT_INT, EDT_STRING, EDT_DATETIME, EDT_DATETIME};
//    QString strFileQuery = SQLAdapter::DataCreate(mTableName, strList, listType);

//    if (!mSqlQuery->exec(strFileQuery)) {
//        qDebug()<<mSqlQuery->lastError();
//        return false;
//    }
    if (!select()) return false;

    return true;
}

int SQLFileAdapter::insert(const QList<QString>& listData)
{
    QStringList strClassifyList{"name","url","download_list","status","classify_id", "tips", "createTime", "lastTime"};
    QList<DataType> listType {EDT_STRING,EDT_STRING,EDT_STRING, EDT_BOOL, EDT_INT, EDT_STRING,EDT_DATETIME, EDT_DATETIME};
    QString strClassifyQuery = SQLAdapter::DataInsert(mTableName, strClassifyList, listData, listType);
    if (!mSqlQuery->exec(strClassifyQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return 0;
    }
    // 返回刚才插入的记录自增Id
    strClassifyQuery = "select LAST_INSERT_ROWID() from "+mTableName;
    if (!mSqlQuery->exec(strClassifyQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return 0;
    }
    int lastId = mSqlQuery->lastInsertId().toInt();

    return lastId;
}

bool SQLFileAdapter::select()
{
    QStringList fieldlist;
    QString strClassifyQuery = SQLAdapter::DataSelect(mTableName, fieldlist);
    if (!mSqlQuery->exec(strClassifyQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return false;
    }
    while (mSqlQuery->next()) {
        FileModel* pModel = new FileModel();
        pModel->setID(mSqlQuery->value("id").toInt());
        pModel->setName(mSqlQuery->value("name").toString());
        pModel->setUrl(mSqlQuery->value("url").toString());
        pModel->setDownloadList(mSqlQuery->value("download_list").toString());
        pModel->setStatus(mSqlQuery->value("status").toBool());
        pModel->setClassifyID(mSqlQuery->value("classify_id").toInt());
        pModel->setTips(mSqlQuery->value("tips").toString());
        pModel->setCreateTime(mSqlQuery->value("createTime").toDateTime());
        pModel->setLastTime(mSqlQuery->value("lastTime").toDateTime());

        mMapDataModel.insert(pModel->id(), pModel);
    }

    return true;
}

bool SQLFileAdapter::findUrl(const QString &strUrl)
{
    foreach (auto fileModel, mMapDataModel) {
        if (dynamic_cast<FileModel*>(fileModel)->url() == strUrl) {
            return true;
        }
    }

    return false;
}
