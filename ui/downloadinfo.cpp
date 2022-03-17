#include "downloadinfo.h"
#include "ui_downloadinfo.h"
#include "download/multithreaddownloader.h"
#include "model/classifymodel.h"
#include "model/filemodel.h"
#include "operator/handlefactory.h"
#include "operator/sqldatahandle.h"
#include "operator/sqlclassifyadapter.h"
#include "download/until.h"
#include <QtNetwork/QNetworkRequest>
#include <QFileInfo>

#pragma execution_character_set("utf-8")

DownloadInfo::DownloadInfo(QWidget *parent, MultithreadDownloader* pDownloader) :
    QWidget(parent),
    ui(new Ui::DownloadInfo),
    mDownloader(pDownloader)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    init();
}

DownloadInfo::~DownloadInfo()
{
    delete ui;
}

void DownloadInfo::init()
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->lineEdit_url->setText(mDownloader->url().toString());

    SqlDataHandle* pHandle = dynamic_cast<SqlDataHandle*>(HandleFactory::getInstance()->getProduct(EHT_SQL));

    SQLClassifyAdapter* pAdapter = dynamic_cast<SQLClassifyAdapter*>(pHandle->sqlAdapter(EAT_CLASSIFY));
    QString splitFileName = mDownloader->fileName();

    QString strExts = splitFileName.split(".").last();
    mClassifyModel = pAdapter->findModel(strExts);
    if (mClassifyModel == nullptr)
        mClassifyModel = pAdapter->defaultModel();
    ui->lineEdit_name->setText(mDownloader->fileName());
    ui->lineEdit_classify->setText(mClassifyModel->name());

    ui->lineEdit_note->setText("说明");
    QPixmap pix(":/icons/"+mClassifyModel->icon());
    ui->label_icon->setPixmap(pix);
    ui->label_size->setText(Until::readableFileSize(mDownloader->fileModel()->size()));
    mDownloader->setFolderName(mClassifyModel->savePath());
}

void DownloadInfo::on_pushButton_cancel_clicked()
{
    // 取消下载
    emit cancelDownload(mDownloader->fileModel()->id());
    close();
}



void DownloadInfo::on_pushButton_start_clicked()
{
    emit startDownload(mDownloader->fileModel()->id());
    close();
}


void DownloadInfo::on_pushButton_add_queue_clicked()
{
    emit addToDownloadQueue(mDownloader->fileModel()->id());
    close();
}
