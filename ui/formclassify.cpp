#include "formclassify.h"
#include "ui_formclassify.h"
#include "model/classifymodel.h"
#include "operator/handlefactory.h"
#include "operator/sqlclassifyadapter.h"
#include <QStandardPaths>
#include <QResource>
#include <QFileDialog>
#include <QApplication>

#pragma execution_character_set("utf-8")

FormClassify::FormClassify(QWidget *parent, ClassifyModel* pModel) :
    QWidget(parent),
    ui(new Ui::FormClassify)
{
    ui->setupUi(this);
    mModel = pModel;
    initUI();
}

FormClassify::~FormClassify()
{
    delete ui;
}

void FormClassify::initUI()
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    mDataHandle = (SqlDataHandle*)HandleFactory::getInstance()->getProduct(EHT_SQL);
    if (mModel == nullptr) { // 添加分类
        setWindowTitle("添加一个新分类到分类列表中");
        ui->lineEdit_save_path->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    } else { // 修改分类
        setWindowTitle("编辑分类属性");
        ui->lineEdit_name->setText(mModel->name());
        ui->lineEdit_types->setText(mModel->exts());
        ui->lineEdit_save_path->setText(mModel->savePath());
        ui->lineEdit_icon->setText(mModel->icon());
        ui->lineEdit_tips->setText(mModel->tips());
    }
}

void FormClassify::on_pushButton_ok_clicked()
{
    if (ui->lineEdit_name->text().isEmpty() || ui->lineEdit_types->text().isEmpty()
            || ui->lineEdit_save_path->text().isEmpty()) {
        return;
    }
    SQLClassifyAdapter* pAdapter = dynamic_cast<SQLClassifyAdapter*>(mDataHandle->sqlAdapter(EAT_CLASSIFY));
    ClassifyModel* pModel = nullptr;
    QList<QString> strDataList;

    strDataList.push_back(ui->lineEdit_name->text());
    strDataList.push_back(ui->lineEdit_icon->text());
    strDataList.push_back(ui->lineEdit_tips->text());
    strDataList.push_back(ui->lineEdit_save_path->text());
    strDataList.push_back(ui->lineEdit_types->text());
    // 保存到内存与表中
    if (mModel == nullptr) { // 添加分类

        int id = pAdapter->insert(strDataList);

        pModel = new ClassifyModel(id, strDataList[0], strDataList[1],strDataList[2],strDataList[3],strDataList[4]);
        // 插入内存map中
        pAdapter->insertModel(pModel);

    } else { // 修改分类
        mModel->setName(ui->lineEdit_name->text());
        mModel->setIcon(ui->lineEdit_icon->text());
        mModel->setTips(ui->lineEdit_tips->text());
        mModel->setSavePath(ui->lineEdit_save_path->text());
        mModel->setExts(ui->lineEdit_types->text());
        // 存入数据库中
        QStringList strFieldList{"name","icon","tips","savePath","exts"};
        QList<DataType> listType{EDT_STRING,EDT_STRING,EDT_STRING,EDT_STRING,EDT_STRING};
        pAdapter->update(strFieldList, strDataList, listType, mModel->id());
    }
    // 刷新树型列表
    emit classifyChanged();
    this->close();
}

void FormClassify::on_pushButton_cancel_clicked()
{
    this->close();
}

void FormClassify::on_pushButton_scan_clicked()
{
    // 浏览添加分类保存目录
    QString selectDir = ui->lineEdit_save_path->text();
    if (selectDir.isEmpty()) {
        selectDir = QApplication::applicationDirPath();
    }
    QString strDir = QFileDialog::getExistingDirectory(this, "选择一个文件夹", selectDir);
    ui->lineEdit_save_path->setText(strDir);
}
