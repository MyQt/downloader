#include "classifymodel.h"

ClassifyModel::ClassifyModel(int id, const QString& name, const QString& icon, const QString& tips, const QString& savePath, const QString& exts)
{
    setID(id);
    setName(name);
    setIcon(icon);
    setTips(tips);
    mSavePath = savePath;
    mExts = exts;
}
