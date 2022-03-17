#ifndef DATAHANDLE_H
#define DATAHANDLE_H
#include "common/common.h"

class DataHandle
{
protected:
    DataHandle(){};
public:
    virtual ~DataHandle() {};
public:
    virtual bool init(QString& infoName) = 0;
    virtual void close() {};
protected:
    QString mInfoName;
};

#endif // DATAHANDLE_H
