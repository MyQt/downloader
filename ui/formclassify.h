#ifndef FORMCLASSIFY_H
#define FORMCLASSIFY_H
#include "operator/sqldatahandle.h"
#include <QWidget>

class ClassifyModel;
namespace Ui {
class FormClassify;
}

class FormClassify : public QWidget
{
    Q_OBJECT

public:
    explicit FormClassify(QWidget *parent = nullptr, ClassifyModel* pModel = nullptr);
    ~FormClassify();
public:
    void initUI();
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_scan_clicked();
signals:
    void classifyChanged();
private:
    Ui::FormClassify *ui;
    ClassifyModel* mModel;
    SqlDataHandle* mDataHandle;
};

#endif // FORMCLASSIFY_H
