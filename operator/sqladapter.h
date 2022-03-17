#ifndef SQLADAPTER_H
#define SQLADAPTER_H
#include <QStringList>
#include <QList>
#include <QMap>
#include <QSqlQuery>
#include <QSqlError>
#include "model/datamodel.h"

enum DataOperator
{
    EDO_CREATE=0, // 建表
    EDO_SELECT, // 查找
    EDO_ADD, // 加列
    EDO_INSERT, // 插入数据
    EDO_UPDATE, // 更新数据
    EDO_DELETE, // 删除数据
    EDO_TRUNCATE, // 清空表
    EDO_END
};

enum DataType
{
    EDT_INT=0, // 整形
    EDT_BOOL, // 布尔型
    EDT_STRING, // 字符串型
    EDT_DATETIME, // 日期时间
    EDT_END
};

enum AdapterType {
    EAT_UNKNOWN,
    EAT_FILE,
    EAT_CLASSIFY,
    EAT_END
};

class SQLAdapter
{
public:
    SQLAdapter();
    ~SQLAdapter();
    virtual bool init(QSqlQuery* query) { mSqlQuery = query; return true; }
    QMap<int, DataModel*>& dataModel() { return mMapDataModel; }
    DataModel* findDataModel(int key) { return  mMapDataModel.find(key).value(); }
    void insertModel(DataModel* pModel) { mMapDataModel.insert(pModel->id(), pModel); }
    void deleteDataModel(int key);
    AdapterType adpterType() { return mType; }
    const QString& tableName() { return  mTableName; }
    virtual bool select() { return true; }
    virtual int insert(const QList<QString>& listData){ return true; } // 插入数据，返回ID
    virtual bool update(const QStringList& strFieldList, const QList<QString>& listData, const QList<DataType> &listType, int id);
    virtual bool deleteRecord(const QList<int>& listId);
    int maxId();
public:
    static QString DataCreate(const QString& strTable,const QStringList& strField, const QList<DataType>& listType);
    static QString DataSelect(const QString& strTable,const QStringList& strField);
    static QString DataAddColumn(const QString& strTable, const QString& strField, DataType type);
    static QString DataInsert(const QString& strTable,const QStringList& strField, const QList<QString>& listData, const QList<DataType> &listType);
    static QString DataUpdate(const QString& strTable,const QStringList& strField, const QList<QString>& listData, const QList<DataType> &listType, int id);
    static QString DataDelete(const QString& strTable, const QList<int>& listId);
    static QString DataTruncate(const QString& strTable);
    static QString getDataType(DataType type);
protected:
    QSqlQuery* mSqlQuery;
    QMap<int, class DataModel*> mMapDataModel;
    QString mTableName;
    AdapterType mType;


};



#endif // SQLADAPTER_H
