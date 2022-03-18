#ifndef REMOVETASK_H
#define REMOVETASK_H

#include <QWidget>

namespace Ui {
class RemoveTask;
}

class RemoveTask : public QWidget
{
    Q_OBJECT

public:
    explicit RemoveTask(QWidget *parent = nullptr);
    ~RemoveTask();
signals:
    void removeTasks(bool bDelete);
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::RemoveTask *ui;
};

#endif // REMOVETASK_H
