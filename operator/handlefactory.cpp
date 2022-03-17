#include "handlefactory.h"
#include "sqldatahandle.h"

HandleFactory::HandleFactory()
{

}

DataHandle* HandleFactory::getProduct(handleType type)
{
    switch (type) {
    case EHT_XML:

        break;
    case EHT_SQL: {
        static SqlDataHandle instance;
        return  &instance;
    }

    default:
        break;
    }

    return  nullptr;
}


