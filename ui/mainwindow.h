#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newtask.h"
#include <QTreeWidgetItem>
#include "operator/sqldatahandle.h"
#include "download/multithreaddownloader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DataSet;
class
        MainWindow : public QMainWindow
{
    Q_OBJECT
    enum tabItemType {
        ETIT_FILENAME = 0, // 文件名
        ETIT_SIZE, // 大小
        ETIT_STATUS, // 状态
        ETIT_LEFTTIME, // 剩余时间
        ETIT_SPEED, // 速度
        ETIT_CREATETIME, // 创建时间
        ETIT_TIPS // 描述
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_new_triggered();

    void on_action_classify_triggered(bool checked);

    void on_action_about_triggered();
    void on_treeWidget_task_itemClicked(QTreeWidgetItem *item, int column);
    // 右键菜单响应函数
    void slotContextMenu(QPoint pos);
    void onClassifyAction(QAction*);
    void onClassifyChanged();
    void onAddNewTask(const QString& strUrl);
    void onSectionClicked(int logicalIndex);
    // 下载相关
    void onDownloadFinish(int id);
    void onDownloadError(int id, const MultithreadDownloader::Error err);
    void onDownloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);
    void onCancelDownload(int id);
    void onStartDownload(int id);
    void onAddToDownloadQueue(int id);
    void on_action_start_triggered();

    void on_action_show_task_triggered();

    void on_action_quit_triggered();

    void on_action_stop_triggered();

    void on_action_restart_triggered();

    void on_action_delete_triggered();

    void on_action_pause_all_triggered();

    void on_action_stop_all_triggered();

    void on_action_del_all_complete_triggered();

    void on_action_find_triggered();

private:
    bool init();
    void initToolbar();
    void initTreeWidget();
    void initTableWidget();
    void initDownloaderSlots(const MultithreadDownloader* pDownloader);
    void disconnectDownloaderSlots(const MultithreadDownloader* pDownloader);
    void clear();
    void insertTableWidgetItem(FileModel* pFileModel, int row);
private:
    Ui::MainWindow *ui;
    QMenu* mClassifyMenu;
    SqlDataHandle* mDataHandle;
    QMap<int, MultithreadDownloader*> mMapDownloader;
    bool mSortUpDown = true;
};
#endif // MAINWINDOW_H
