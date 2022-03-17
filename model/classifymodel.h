#ifndef CLASSIFYMODEL_H
#define CLASSIFYMODEL_H
#include "datamodel.h"

class ClassifyModel : public DataModel
{
public:
    ClassifyModel() {}
    ClassifyModel(int id, const QString& name, const QString& icon, const QString& tips, const QString& savePath, const QString& exts);
    explicit ClassifyModel(ClassifyModel& other)=delete;
    ClassifyModel& operator=(const ClassifyModel&)=delete;

    const QString& icon() const { return  mIcon; }
    const QString& tips() const { return  mTips; }
    const QString& savePath() const { return  mSavePath; }
    const QString& exts() const { return  mExts; }

    void setIcon(const QString& icon) { mIcon = icon; }
    void setTips(const QString& tips) { mTips = tips; }
    void setSavePath(const QString& path) { mSavePath = path; }
    void setExts(const QString& exts) { mExts = exts; }
private:
    QString mIcon;
    QString mTips;
    QString mSavePath; // 分类文件保存路径
    QString mExts; // 分类扩展名集合(逗号分隔)
};

#endif // CLASSIFYMODEL_H
