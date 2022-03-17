#ifndef NEWTASK_H
#define NEWTASK_H

#include <QWidget>

namespace Ui {
class NewTask;
}

class NewTask : public QWidget
{
    Q_OBJECT

public:
    explicit NewTask(QWidget *parent = nullptr);
    ~NewTask();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();
signals:
    void addNewTask(const QString& strUrl);
private:
    Ui::NewTask *ui;

};

#endif // NEWTASK_H
