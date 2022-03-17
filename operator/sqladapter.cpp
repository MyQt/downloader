#include "sqladapter.h"
#include<QDebug>

SQLAdapter::SQLAdapter()
{
    mSqlQuery = nullptr;
    mMapDataModel.clear();
    mType = EAT_UNKNOWN;
}

SQLAdapter::~SQLAdapter()
{
    for(auto iter = mMapDataModel.begin(); iter != mMapDataModel.end(); iter++) {
        delete iter.value();
    }
    mMapDataModel.clear();
}

void SQLAdapter::deleteDataModel(int key)
{
    auto iter = mMapDataModel.find(key);
    if (iter != mMapDataModel.end()) {
        delete iter.value();
    }
}

QString SQLAdapter::getDataType(DataType type)
{
    switch (type) {
        case EDT_INT:
            return "INTEGER";
        break;
        case EDT_BOOL:
            return "BOOLEAN";
        break;
        case EDT_STRING:
            return "VARCHAR";
        break;
        case EDT_DATETIME:
            return "DATETIME";
        break;
        default:
        break;
    }

    return "";
}

QString SQLAdapter::DataCreate(const QString &strTable, const QStringList &strField, const QList<DataType> &listType)
{
    QString strQuery = QString("CREATE TABLE IF NOT EXISTS %1("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,").arg(strTable);
    int nIndex = 0;
    foreach(auto field, strField) {
        QString strType = SQLAdapter::getDataType(listType.at(nIndex));
        QStringList strSplit = field.split(",");
        QString strSplitVal;
        if (strType == "VARCHAR") {
            strSplitVal = QString("%1 %2(%3)").arg(strSplit[0],strType,strSplit[1]);
        } else {
            strSplitVal = QString("%1 %2 NOT NULL default %3").arg(strSplit[0], strType, strSplit[1]);
        }
        strQuery += strSplitVal;
        if (nIndex < strField.size()-1) {
            strQuery += ",";
        }
        nIndex++;
    }
    strQuery += ")";

    return strQuery;
}

QString SQLAdapter::DataSelect(const QString &strTable, const QStringList &strField)
{
    QString strQuery = QString("select ");
    foreach(auto field, strField) {
        strQuery += QString("%1 ").arg(field);
    }
    if (strField.isEmpty()) {
        strQuery += "*";
    }
    strQuery +=QString(" from %1").arg(strTable);

    return strQuery;
}

QString SQLAdapter::DataAddColumn(const QString &strTable, const QString& strField, DataType type)
{
    QString strQuery = QString("ALTER TABLE %1 ADD COLUMN ").arg(strTable);

    QString strType = SQLAdapter::getDataType(type);
    QStringList strSplit = strField.split(",");
    if (strType == "VARCHAR") {
        strQuery += QString("%1 %2(%3)").arg(strSplit[0],strType,strSplit[1]);
    } else {
        if (type == EDT_DATETIME)
            strQuery += QString("%1 %2 NOT NULL default '%3'").arg(strSplit[0], strType, strSplit[1]);
        else
            strQuery += QString("%1 %2 NOT NULL default %3").arg(strSplit[0], strType, strSplit[1]);
    }

    return strQuery;
}

QString SQLAdapter::DataInsert(const QString &strTable, const QStringList &strField, const QList<QString>& listData, const QList<DataType> &listType)
{
    QString strQuery = QString("insert into %1(").arg(strTable);
    int nIndex = 0;
    foreach(auto field, strField) {
        strQuery += QString("%1").arg(field);
        if (nIndex < strField.size()-1) {
            strQuery +=",";
        }
        nIndex++;
    }
    strQuery += ") VALUES(";
    nIndex = 0;
    foreach(auto val, listData) {
        if (listType[nIndex] == EDT_STRING || listType[nIndex] == EDT_DATETIME)
            strQuery += QString("'%1'").arg(val);
        else
            strQuery += QString("%1").arg(val);
        if (nIndex < listData.size()-1) {
            strQuery +=",";
        }
        nIndex++;
    }
    strQuery += ")";


    return strQuery;
}

QString SQLAdapter::DataUpdate(const QString &strTable, const QStringList &strField, const QList<QString> &listData, const QList<DataType> &listType, int id)
{
    QString strQuery = QString("update %1 set ").arg(strTable);
    int nIndex = 0;
    foreach(auto field, strField) {
        if (listType[nIndex] == EDT_STRING || listType[nIndex] == EDT_DATETIME) {
            strQuery+= QString("%1='%2'").arg(field, listData.at(nIndex));
        } else {
            strQuery+= QString("%1=%2").arg(field, listData.at(nIndex));
        }

        if (nIndex < strField.size()-1) {
            strQuery+=",";
        }
        nIndex++;
    }
    strQuery += QString(" where id = %1").arg(id);

    return strQuery;
}

QString SQLAdapter::DataDelete(const QString &strTable, const QList<int>& listId)
{
    QString strQuery = QString("delete from %1 where id in(").arg(strTable);
    int nIndex = 0;
    foreach(auto id, listId) {
        strQuery+=QString("%1").arg(id);
        if (nIndex < listId.size()-1) {
            strQuery+=",";
        }
        nIndex++;
    }
    strQuery+=")";

    return strQuery;
}

QString SQLAdapter::DataTruncate(const QString &strTable)
{
    QString strQuery = QString("delete from %1").arg(strTable);

    return strQuery;
}

bool SQLAdapter::update(const QStringList &strFieldList, const QList<QString> &listData, const QList<DataType> &listType, int id)
{
    if (strFieldList.isEmpty()) {
        return false;
    }
    QString strQuery = SQLAdapter::DataUpdate(mTableName, strFieldList, listData, listType, id);
    if (!mSqlQuery->exec(strQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return false;
    }

    return true;
}

bool SQLAdapter::deleteRecord(const QList<int> &listId)
{
    if (listId.isEmpty()) {
        return false;
    }
    QString strQuery = SQLAdapter::DataDelete(mTableName, listId);
    if (!mSqlQuery->exec(strQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return false;
    }

    return true;
}

int SQLAdapter::maxId()
{
    QString strQuery = "select max(id) from " + mTableName + "as maxid";
    if (!mSqlQuery->exec(strQuery)) {
        qDebug()<<mSqlQuery->lastError();
        return -1;
    }
    mSqlQuery->next(); // next函数里面有个指针，不执行的话那个指针是野指针，执行后指针指向结果集合中第一条数据或下一条数据。

    return mSqlQuery->value(0).toInt();


}
