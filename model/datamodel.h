#ifndef DATAMODEL_H
#define DATAMODEL_H
#include <QString>

class DataModel
{
public:
    DataModel();
    int id() const { return mID; }
    const QString& name() const { return mName; }

    void setID(const int& id) { mID = id; }
    void setName(const QString& name) { mName = name; }
    virtual ~DataModel() {}
private:
    int     mID;
    QString mName;
};

#endif // DATAMODEL_H
