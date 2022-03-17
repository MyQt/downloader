#include "newtask.h"
#include "ui_newtask.h"
#include <QToolTip>
#include <QUrl>

NewTask::NewTask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewTask)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    ui->lineEdit_url->setFocus();
    setAttribute(Qt::WA_DeleteOnClose);
}

NewTask::~NewTask()
{
    delete ui;
}

void NewTask::on_pushButton_ok_clicked()
{
    QUrl url(ui->lineEdit_url->text());
    if (!url.isValid()) {
        QToolTip::showText(ui->pushButton_ok->pos(), "url非法", this);
        return;
    }

    emit addNewTask(url.toString());

    // 关闭新建任务窗口
    this->close();
}

void NewTask::on_pushButton_cancel_clicked()
{
    this->close();
}
