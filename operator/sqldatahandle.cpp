#include "sqldatahandle.h"
#include <QVariant>
#include <QDebug>
#include "sqlclassifyadapter.h"
#include "sqlfileadapter.h"

SqlDataHandle::SqlDataHandle()
{
    mSqlQuery = nullptr;
}

SqlDataHandle::~SqlDataHandle()
{
}

bool SqlDataHandle::init(QString &infoName)
{
    mInfoName = infoName;
    if (!initDatabase()) {
        return false;
    }

    return true;
}

bool SqlDataHandle::initDatabase()
{
    mDatabase = QSqlDatabase::addDatabase("QSQLITE");
    mDatabase.setDatabaseName(mInfoName);
    if (!mDatabase.open()) {
        // 创建或打开失败
        return false;
    }
    mSqlQuery = new QSqlQuery(mInfoName); // 连接数据库
    // 下载文件表
    SQLAdapter* pFileAdapter = new SQLFileAdapter;
    mMapAdapter.insert(pFileAdapter->adpterType(), pFileAdapter);
    // 分类配置表
    SQLAdapter* pClassifyAdapter = new SQLClassifyAdapter;
    mMapAdapter.insert(pClassifyAdapter->adpterType(), pClassifyAdapter);
    foreach(auto adapter, mMapAdapter) { // 从数据库加载表数据
        if (!adapter->init(mSqlQuery)) {
            return false;
        }
    }

    return true;
}

void SqlDataHandle::close()
{
    if (mSqlQuery != nullptr) {
        mSqlQuery->clear();
        delete mSqlQuery;
    }
    mDatabase.close();
}
