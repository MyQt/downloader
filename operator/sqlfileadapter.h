#ifndef FILEADAPTER_H
#define FILEADAPTER_H
#include "sqladapter.h"
#include <QMap>
class FileModel;
class SQLFileAdapter : public SQLAdapter
{
public:
    SQLFileAdapter();
    bool init(QSqlQuery* query);
    bool select();
    int insert(const QList<QString>& listData); // 插入数据，返回ID
    bool findUrl(const QString& strUrl);
};

#endif // FILEADAPTER_H
