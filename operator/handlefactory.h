#ifndef HANDLEFACTORY_H
#define HANDLEFACTORY_H
#include "DataHandle.h"

class HandleFactory
{
public:
    HandleFactory();
    static HandleFactory* getInstance()
    {
        static HandleFactory instance;

        return &instance;
    }
    static DataHandle* getProduct(handleType type);

};

#endif // HANDLEFACTORY_H
