#include "sqlclassifyadapter.h"
#include <QDebug>
#include "model/classifymodel.h"


SQLClassifyAdapter::SQLClassifyAdapter()
{
    mType = EAT_CLASSIFY;
}

SQLClassifyAdapter::~SQLClassifyAdapter()
{

}

ClassifyModel* SQLClassifyAdapter::findModel(const QString &extName)
{
    foreach(auto model, mMapDataModel) {
        ClassifyModel* pModel = dynamic_cast<ClassifyModel*>(model);
        QStringList strList = pModel->exts().split(",");
        foreach(auto ext, strList) {
            if (ext == extName) {
                return pModel;
            }
        }
    }

    return nullptr;
}

ClassifyModel* SQLClassifyAdapter::defaultModel()
{
    return dynamic_cast<ClassifyModel*>(mMapDataModel.find(2).value());
}



bool SQLClassifyAdapter::init(QSqlQuery *query)
{
    SQLAdapter::init(query);
    mTableName = "Classify";

    select();

    return true;
}

bool SQLClassifyAdapter::select()
{
    QStringList fieldlist;
    QString strClassifyQuery = SQLAdapter::DataSelect(mTableName, fieldlist);
    if (!mSqlQuery->exec(strClassifyQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return false;
    }
    while (mSqlQuery->next()) {
        ClassifyModel* pModel = new ClassifyModel();
        pModel->setID(mSqlQuery->value("id").toInt());
        pModel->setName(mSqlQuery->value("name").toString());
        pModel->setIcon(mSqlQuery->value("icon").toString());
        pModel->setTips(mSqlQuery->value("tips").toString());
        pModel->setSavePath(mSqlQuery->value("savePath").toString());
        pModel->setExts(mSqlQuery->value("exts").toString());

        mMapDataModel.insert(pModel->id(), pModel);
    }

    return true;
}

int SQLClassifyAdapter::insert(const QList<QString> &listData)
{
    QStringList strClassifyList{"name","icon","tips","savePath", "exts"};
    QList<DataType> listType {EDT_STRING,EDT_STRING,EDT_STRING,EDT_STRING,EDT_STRING};
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


