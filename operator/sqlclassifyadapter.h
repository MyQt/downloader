#ifndef CLASSIFYADAPTER_H
#define CLASSIFYADAPTER_H
#include <QMap>
#include "sqladapter.h"

class ClassifyModel;

class SQLClassifyAdapter : public SQLAdapter
{
public:
    SQLClassifyAdapter();
    ~SQLClassifyAdapter();
    bool init(QSqlQuery* query);
    ClassifyModel* findModel(const QString& extName);
    ClassifyModel* defaultModel();
    bool select();
    int insert(const QList<QString>& listData); // 插入数据，返回ID
};

#endif // CLASSIFYADAPTER_H
