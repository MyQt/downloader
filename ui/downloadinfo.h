#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QWidget>
#include <QUrl>

namespace Ui {
class DownloadInfo;
}

class MultithreadDownloader;
class ClassifyModel;
class DownloadInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadInfo(QWidget *parent = nullptr, MultithreadDownloader* pDownloader = nullptr);
    ~DownloadInfo();
    void init();
signals:
    void cancelDownload(int id);
    void startDownload(int id);
    void addToDownloadQueue(int id);
private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_add_queue_clicked();

private:
    Ui::DownloadInfo *ui;
    MultithreadDownloader* mDownloader;
    const ClassifyModel* mClassifyModel = nullptr;
};

#endif // DOWNLOADINFO_H
