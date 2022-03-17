#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model/classifymodel.h"
#include "model/filemodel.h"
#include "operator/handlefactory.h"
#include "operator/sqlfileadapter.h"
#include "downloadinfo.h"
#include "formclassify.h"
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QToolTip>
#include <QIcon>
#include "download/until.h"

const QString strConfigPath = "/config/";
const QString strInfoName ="info.db";

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

}

bool MainWindow::init()
{
    mMapDownloader.clear();
    mClassifyMenu = nullptr;
    mDataHandle = (SqlDataHandle*)HandleFactory::getInstance()->getProduct(EHT_SQL);
    QString strInfoPath = QApplication::applicationDirPath()+strConfigPath+strInfoName;
    if (!mDataHandle->init(strInfoPath)) {
        return false;
    }
    initToolbar();
    initTreeWidget();
    initTableWidget();
    return true;
}



void MainWindow::initTreeWidget()
{

    ui->treeWidget_task->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_task, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    ui->treeWidget_task->clear();
    QStringList strList{"全部任务","未完成","已完成"};
    QStringList strIconList{":/icons/tasklist_16px.png",":/icons/downloading_16px.png",":/icons/task_completed_16px.png"};
    int nIndex = 0;
    foreach(auto topLevel, strList) {
        QTreeWidgetItem* topItem = new QTreeWidgetItem(ui->treeWidget_task);
        topItem->setData(0, Qt::UserRole, QPoint(0,nIndex));
        topItem->setText(0,topLevel);
        topItem->setIcon(0, QIcon(strIconList.at(nIndex)));
        ui->treeWidget_task->addTopLevelItem(topItem);
        // 添加子项
        SQLAdapter* pAdapter = mDataHandle->sqlAdapter(EAT_CLASSIFY);
        for (auto iter = pAdapter->dataModel().begin(); iter != pAdapter->dataModel().end(); iter++) {
            ClassifyModel* pModel = dynamic_cast<ClassifyModel*>(iter.value());
            QTreeWidgetItem* childItem = new QTreeWidgetItem(topItem);
            childItem->setData(0, Qt::UserRole, QPoint(1, pModel->id()));
            childItem->setText(0,iter.value()->name());
            childItem->setIcon(0, QIcon(":/icons/"+pModel->icon()));
        }

        nIndex++;
    }
    ui->treeWidget_task->expandAll();
    // 创建分类右键菜单
    if (mClassifyMenu == nullptr) {
        mClassifyMenu = new QMenu(ui->treeWidget_task);
        connect(mClassifyMenu, &QMenu::triggered, this, &MainWindow::onClassifyAction);

        nIndex = 0;
        QStringList strActionList{"浏览","属性","添加分类","删除分类"};
        foreach(auto strAction, strActionList) {
            QAction* pAction = new QAction(strAction, mClassifyMenu);
            pAction->setData(nIndex);
            mClassifyMenu->addAction(pAction);
            nIndex++;
        }
    }
}

void MainWindow::insertTableWidgetItem(FileModel* pFileModel, int row)
{
    ui->tableWidget_download_file->horizontalHeader()->setStyleSheet("QHeaderView::up-arrow { subcontrol-position: center right; padding-right: 8px;"
                                                       "image: url(:/icons/sort_asc_16px.png);}"
                                                      "QHeaderView::down-arrow { subcontrol-position: center right; padding-right: 8px;"
                                                       "image: url(:/icons/sort_desc_16px.png);}");

    ui->tableWidget_download_file->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_download_file->setSelectionBehavior(QAbstractItemView::SelectRows);
    SQLAdapter* pAdapter = mDataHandle->sqlAdapter(EAT_CLASSIFY);
    ClassifyModel* pClassifyModel = dynamic_cast<ClassifyModel*>(pAdapter->findDataModel(pFileModel->classifyID()));
    auto iter = mMapDownloader.find(pFileModel->id());
    MultithreadDownloader* pDownloader = nullptr;
    if (iter != mMapDownloader.end()) {
        pDownloader = iter.value();
    }
    ui->tableWidget_download_file->setRowCount(ui->tableWidget_download_file->rowCount()+1);
    for (int column = 0; column < ui->tableWidget_download_file->columnCount(); column++) {
        QTableWidgetItem*pItem = new QTableWidgetItem();
        pItem->setTextAlignment(Qt::AlignHCenter);
        if (column == ETIT_FILENAME) { // 文件名
            pItem->setText(pFileModel->name());
            pItem->setIcon(QIcon(":/icons/" + pClassifyModel->icon()));
            pItem->setData(Qt::UserRole, pFileModel->id());
        } else if (column == ETIT_SIZE){ // 大小
            pItem->setText(Until::readableFileSize(pFileModel->size()));
        } else if (column == ETIT_STATUS) { // 状态
            if (pFileModel->status() == true) {
                pItem->setText("完成");
            } else {
                pItem->setText(Until::downloadProgress(pFileModel->downloadedSize(), pFileModel->size()));
            }
        } else if(column == ETIT_LEFTTIME) { // 剩余时间

            pItem->setText("");
        } else if (column == ETIT_SPEED) { // 下载速度
            pItem->setText("");
        } else if (column == ETIT_CREATETIME) { // 任务创建时间
            pItem->setText(pFileModel->createTime().toString());
        } else if (column == ETIT_TIPS) { // 说明
            pItem->setText(pFileModel->tips());
        }
        ui->tableWidget_download_file->setItem(row, column, pItem);
        if (pFileModel->status() == false && pDownloader == nullptr) {
            pDownloader = new MultithreadDownloader(this);
            pDownloader->setUrl<QString>(pFileModel->url());
            if (!pDownloader->load(pFileModel)) {
                QToolTip::showText(QPoint(this->pos().x()+this->width()/2, this->pos().y()+this->height()/2), "获取下载信息失败", this);
                pDownloader->deleteLater();
                return;
            }
            mMapDownloader.insert(pFileModel->id(), pDownloader);
            initDownloaderSlots(pDownloader);
        }
  }
}

void MainWindow::initTableWidget()
{
//    ui->tableWidget_download_file->clear();
    connect(ui->tableWidget_download_file->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onSectionClicked);
    ui->tableWidget_download_file->horizontalHeader()->setSortIndicatorShown(true);
    // 初始化文件列表
    QMap<int, DataModel*>& mapFileModel = mDataHandle->sqlAdapter(EAT_FILE)->dataModel();
    int nIndex = 0;
    for (auto iter = mapFileModel.begin(); iter != mapFileModel.end(); iter++) {
        FileModel* pFileModel = dynamic_cast<FileModel*>(iter.value());
        insertTableWidgetItem(pFileModel, nIndex);
      nIndex++;

    }
}

void MainWindow::disconnectDownloaderSlots(const MultithreadDownloader *pDownloader)
{
    if (pDownloader == nullptr) {
        return;
    }
    QObject::disconnect(pDownloader, &MultithreadDownloader::finished, this,
                     &MainWindow::onDownloadFinish);

    QObject::disconnect(pDownloader, &MultithreadDownloader::error, this,
                     &MainWindow::onDownloadError);

    QObject::disconnect(pDownloader, &MultithreadDownloader::stateChanged, this,
                     nullptr);

    QObject::disconnect(pDownloader, &MultithreadDownloader::downloadProgress, this,
                     &MainWindow::onDownloadProgress);

}

void MainWindow::initDownloaderSlots(const MultithreadDownloader *pDownloader)
{
    if (pDownloader == nullptr) {
        return;
    }
    QObject::connect(pDownloader, &MultithreadDownloader::finished, this,
                     &MainWindow::onDownloadFinish);

    QObject::connect(pDownloader, &MultithreadDownloader::error, this,
                     &MainWindow::onDownloadError);

    QObject::connect(pDownloader, &MultithreadDownloader::stateChanged, this,
                     [this](MultithreadDownloader::State state){
        switch (state)
        {
        case MultithreadDownloader::Running:
            QToolTip::showText(QPoint(this->pos().x()+width()/2,this->pos().y()+height()/2),tr("下载中"));
            break;
        case MultithreadDownloader::Paused:
            QToolTip::showText(QPoint(this->pos().x()+width()/2,this->pos().y()+height()/2),tr("暂停"));
            break;
        case MultithreadDownloader::Stopped:
            QToolTip::showText(QPoint(this->pos().x()+width()/2,this->pos().y()+height()/2),tr("停止"));
            break;
        }
    });

    QObject::connect(pDownloader, &MultithreadDownloader::downloadProgress, this,
                     &MainWindow::onDownloadProgress);
}

MainWindow::~MainWindow()
{
    delete ui;
    clear();
}

void MainWindow::clear()
{
    foreach(auto downloader, mMapDownloader) {
        downloader->stop();
    }
    mMapDownloader.clear();
}

void MainWindow::initToolbar()
{
    QList<QAction*> pNewTask;
    pNewTask.append(ui->menu_file->actions());
    pNewTask.append(ui->menu_download->actions());
    pNewTask.append(ui->menu_downopt->actions());
    QStringList strList{"action_new", "action_stop","action_start","action_pause_all","action_stop_all",
                       "action_delete","action_restart","action_del_all_complete"};
    foreach(QAction* action, pNewTask) {
        if (strList.indexOf(action->objectName()) != -1) {
            ui->toolBar->addAction(action);
        }
    }
}

void MainWindow::on_action_new_triggered()
{

        NewTask* pNewTask = new NewTask(this);
        connect(pNewTask, &NewTask::addNewTask, this, &MainWindow::onAddNewTask);
        pNewTask->show();
}

void MainWindow::on_action_classify_triggered(bool checked)
{
    if (ui->treeWidget_task->isVisible()) {
        ui->treeWidget_task->hide();
        ui->action_classify->setText(tr("显示分类"));
    } else {
        ui->treeWidget_task->show();
        ui->action_classify->setText(tr("隐藏分类"));
    }
}

void MainWindow::on_action_about_triggered()
{
    QString dlgTitile = "关于我";
    QString strInfo = "一个无聊的开发者";
    QMessageBox::about(this, dlgTitile, strInfo);
}

void MainWindow::on_treeWidget_task_itemClicked(QTreeWidgetItem *item, int column)
{

}

void MainWindow::slotContextMenu(QPoint pos)
{
    const QTreeWidgetItem* pCurItem = ui->treeWidget_task->currentItem();
    bool bSelect = false; // 是否选中有效项
    if (pCurItem != nullptr) {
        QPoint property = pCurItem->data(0, Qt::UserRole).toPoint();
        if (property.x() == 1) {
            bSelect = true;
        }
    }
    QList<QAction*> listAction = mClassifyMenu->actions();
    foreach(auto action, listAction) {
        if (!bSelect) {
            int nIndex = action->data().toInt();
            if (nIndex == 2) {
                action->setDisabled(false);
            } else {
                action->setDisabled(true);
            }
        } else {
            action->setDisabled(false);
        }
    }

    if (ui->treeWidget_task->hasFocus()) {

        mClassifyMenu->exec(QCursor::pos());
    }
}

void MainWindow::onClassifyAction(QAction* pAction)
{
    const QTreeWidgetItem* pCurItem = ui->treeWidget_task->currentItem();
    int nIndex = pAction->data().toInt();

    ClassifyModel* pModel = nullptr;
    int modelId = -1;
    if (pCurItem != nullptr) {
        modelId = pCurItem->data(0,Qt::UserRole).toPoint().y();
        pModel = dynamic_cast<ClassifyModel*>(mDataHandle->sqlAdapter(EAT_CLASSIFY)->findDataModel(modelId));
        if (pModel == nullptr) return;
    }
    if (nIndex == 0) { // 浏览
        // 打开分类文件保存文件夹
        QDesktopServices::openUrl(QUrl(QString("file:///")+pModel->savePath()));
    } else if (nIndex == 1) { // 属性
        // 打开分类属性，可修改
        FormClassify* pFormClassify = new FormClassify(this, pModel);
        connect(pFormClassify, &FormClassify::classifyChanged, this, &MainWindow::onClassifyChanged);
        pFormClassify->show();
    } else if (nIndex == 2) { // 添加分类
        // 打开添加分类窗口
        FormClassify* pFormClassify = new FormClassify(this);
        connect(pFormClassify, &FormClassify::classifyChanged, this, &MainWindow::onClassifyChanged);
        pFormClassify->show();
    } else if (nIndex == 3) { // 删除
        // 删除选定的分类
        mDataHandle->sqlAdapter(EAT_CLASSIFY)->deleteDataModel(modelId);
        // 数据库中删除分类
        QList<int> listId{modelId};
        SQLAdapter::DataDelete(mDataHandle->sqlAdapter(EAT_CLASSIFY)->tableName(), listId);
        // 重新生成treewidget条目
        initTreeWidget();
    }
}

void MainWindow::onClassifyChanged()
{
    initTreeWidget();
}

void MainWindow::onAddNewTask(const QString& strUrl)
{
    if (strUrl.isEmpty()) return;
    SQLFileAdapter* pAdapter = dynamic_cast<SQLFileAdapter*>(mDataHandle->sqlAdapter(EAT_FILE));
    if (pAdapter->findUrl(strUrl)) {
        QToolTip::showText(QPoint(this->pos().x()+this->width()/2, this->pos().y()+this->height()/2), "下载已经存在", this);
        return;
    }
    // 请求下载信息
    MultithreadDownloader* pDownloader = new MultithreadDownloader(this);

    pDownloader->setUrl<QString>(strUrl);
    if (!pDownloader->load()) {
        QToolTip::showText(QPoint(this->pos().x()+this->width()/2, this->pos().y()+this->height()/2), "获取下载信息失败", this);
        pDownloader->deleteLater();
        return;
    }
    initDownloaderSlots(pDownloader);
    mMapDownloader.insert(pDownloader->fileModel()->id(), pDownloader);
    // 弹出新建任务信息窗口
    DownloadInfo* pInfo = new DownloadInfo(this,pDownloader);
    connect(pInfo, &DownloadInfo::cancelDownload, this, &MainWindow::onCancelDownload);
    connect(pInfo, &DownloadInfo::addToDownloadQueue, this, &MainWindow::onAddToDownloadQueue);
    connect(pInfo, &DownloadInfo::startDownload, this, &MainWindow::onStartDownload);

    pInfo->show();
}

void MainWindow::onSectionClicked(int logicalIndex)
{
    Qt::SortOrder order = mSortUpDown ? Qt::AscendingOrder : Qt::DescendingOrder;
    ui->tableWidget_download_file->sortItems(logicalIndex, order);
    mSortUpDown = !mSortUpDown;
}

void MainWindow::onDownloadError(int id, const MultithreadDownloader::Error err)
{
    QMap<int, DataModel*>& mapFileModel = mDataHandle->sqlAdapter(EAT_FILE)->dataModel();
    DataModel* pModel = mapFileModel.find(id).value();
    QString strTips = pModel->name();
    if (err == MultithreadDownloader::OpenFileFailed) {
        strTips += "打开失败";
    } else {
        strTips += "下载失败";
    }
    QToolTip::showText(QPoint(pos().x()+width()/2, pos().y()+height()/2), strTips);
}

void MainWindow::onDownloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal)
{
    QMap<int, DataModel*>& mapFileModel = mDataHandle->sqlAdapter(EAT_FILE)->dataModel();
    for(int i = 0; i < ui->tableWidget_download_file->rowCount(); i++) {
       QTableWidgetItem* pItem = ui->tableWidget_download_file->item(i,0);

       if (pItem != nullptr && pItem->data(Qt::UserRole).toInt() == id) {
           // 更新进度
           FileModel* pModel = dynamic_cast<FileModel*>(mapFileModel.find(id).value());
           if (pModel == nullptr) return;

           qint64 receivedOneTime = bytesReceived - pModel->oldReceivedSize();
           if (receivedOneTime <= 0) return;
           // 下载速度
           QTableWidgetItem* pSpeedItem = ui->tableWidget_download_file->item(i, ETIT_SPEED);
           pSpeedItem->setText(Until::readableFileSize(receivedOneTime)+"/S");
           pModel->setOldReceivedSize(bytesReceived);
           // 剩余时间
           QTableWidgetItem* pLeftTimeItem = ui->tableWidget_download_file->item(i, ETIT_LEFTTIME);
           qint64 totalLeftSize = pModel->leftSize();
           int s = totalLeftSize / receivedOneTime;
           int m = s / 60;
           int h = m / 60;
           pLeftTimeItem->setText(Until::serilizeTime(h,m,s));

           return;
       }
    }
}

void MainWindow::onDownloadFinish(int id)
{
    for(int i = 0; i < ui->tableWidget_download_file->rowCount(); i++) {
       QTableWidgetItem* pItem = ui->tableWidget_download_file->item(i,0);
       int itemId = pItem->data(Qt::UserRole).toInt();
       if (pItem != nullptr && itemId == id) {
           // 修改状态
           QTableWidgetItem* pStatusItem = ui->tableWidget_download_file->item(i, ETIT_STATUS);
           if (pStatusItem != nullptr) {
               pStatusItem->setText("完成");
           }
           QTableWidgetItem* pSLeftItem = ui->tableWidget_download_file->item(i, ETIT_LEFTTIME);
           if (pSLeftItem != nullptr) {
               pSLeftItem->setText("");
           }
           QTableWidgetItem* pSpeedItem = ui->tableWidget_download_file->item(i, ETIT_SPEED);
           if (pSpeedItem != nullptr) {
               pSpeedItem->setText("");
           }
       }
    }
}

void MainWindow::onStartDownload(int id)
{
    MultithreadDownloader* pDownloader = mMapDownloader.find(id).value();
    if (pDownloader == nullptr) return;
    QMap<int, DataModel*>& mapFileModel = mDataHandle->sqlAdapter(EAT_FILE)->dataModel();
    FileModel* pFileModel = dynamic_cast<FileModel*>(mapFileModel.find(id).value());
    if (pFileModel == nullptr) return;
    insertTableWidgetItem(pFileModel, ui->tableWidget_download_file->rowCount());
    pDownloader->start();
}
void MainWindow::onAddToDownloadQueue(int id)
{
    MultithreadDownloader* pDownloader = mMapDownloader.find(id).value();
    if (pDownloader == nullptr) return;
    FileModel* pFileModel = dynamic_cast<FileModel*>(mDataHandle->sqlAdapter(EAT_FILE)->findDataModel(id));
    if (pFileModel == nullptr) return;
    insertTableWidgetItem(pFileModel, ui->tableWidget_download_file->rowCount());

}
void MainWindow::onCancelDownload(int id)
{
    // 删除目标
    MultithreadDownloader* pDownloader = mMapDownloader.find(id).value();
    if (pDownloader == nullptr) return;
    disconnectDownloaderSlots(pDownloader);
    pDownloader->stop();
    pDownloader->deleteLater();
    mMapDownloader.remove(id);
    QMap<int, DataModel*>& mapFileModel = mDataHandle->sqlAdapter(EAT_FILE)->dataModel();
    QMap<int, DataModel*>::iterator fileIter = mapFileModel.find(id);
    if (fileIter != mapFileModel.end()) {
        FileModel* pFileModel = dynamic_cast<FileModel*>(mapFileModel.find(id).value());
        delete pFileModel;
        mapFileModel.erase(fileIter);
    }

    mDataHandle->sqlAdapter(EAT_FILE)->deleteRecord(QList<int>{id});

}

void MainWindow::on_action_start_triggered()
{
    QList<QTableWidgetItem*> pSelectedItems = ui->tableWidget_download_file->selectedItems();
    foreach(auto item, pSelectedItems) {
        if (item->column() == ETIT_FILENAME) {
            int fileId = item->data(Qt::UserRole).toInt();
            MultithreadDownloader* pDownloader = mMapDownloader.find(fileId).value();
            if (pDownloader == nullptr) return;
            pDownloader->start();
        }
    }
}

void MainWindow::on_action_show_task_triggered()
{

}

void MainWindow::on_action_quit_triggered()
{
    this->close();
}

void MainWindow::on_action_stop_triggered()
{

}

void MainWindow::on_action_restart_triggered()
{
    QList<QTableWidgetItem*> pSelectedItems = ui->tableWidget_download_file->selectedItems();
    foreach(auto item, pSelectedItems) {
        if (item->column() == ETIT_FILENAME) {
            int fileId = item->data(Qt::UserRole).toInt();
            MultithreadDownloader* pDownloader = mMapDownloader.find(fileId).value();
            if (pDownloader == nullptr) return;


        }
    }
}

void MainWindow::on_action_delete_triggered()
{

}

void MainWindow::on_action_pause_all_triggered()
{

}

void MainWindow::on_action_stop_all_triggered()
{

}

void MainWindow::on_action_del_all_complete_triggered()
{

}

void MainWindow::on_action_find_triggered()
{

}

