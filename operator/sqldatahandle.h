#ifndef SQLDATAHANDLE_H
#define SQLDATAHANDLE_H
#include "DataHandle.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QMap>
#include "sqladapter.h"

class SqlDataHandle : public DataHandle
{
public:
    SqlDataHandle();
    ~SqlDataHandle();
    virtual bool init(QString& infoName) override;
    SQLAdapter* sqlAdapter(AdapterType type) { return mMapAdapter.find(type).value(); }
    virtual void close() override;
    bool initDatabase();

private:
    QSqlDatabase mDatabase;
    QSqlQuery*   mSqlQuery;
    QMap<AdapterType, SQLAdapter*> mMapAdapter;

};

#endif // SQLDATAHANDLE_H
