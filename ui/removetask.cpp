#include "removetask.h"
#include "ui_removetask.h"

RemoveTask::RemoveTask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoveTask)
{
    ui->setupUi(this);
}

RemoveTask::~RemoveTask()
{
    delete ui;
}

void RemoveTask::on_pushButton_ok_clicked()
{
    emit removeTasks(ui->checkBox_delete->isChecked());
}

void RemoveTask::on_pushButton_cancel_clicked()
{
    this->close();
}
