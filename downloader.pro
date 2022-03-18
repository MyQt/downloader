QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# 配置file_copies
CONFIG += file_copies

# 配置需要复制的文件或目录(支持通配符)
drivers.files += $$PWD/config
# 配置需要复制的目标目录, $$OUT_PWD为QMake内置变量，含义为程序输出目录
CONFIG(debug, debug|release){
    drivers.path = $$OUT_PWD/debug
} else {
    drivers.path = $$OUT_PWD/release
}

# 配置COPIES
COPIES += drivers
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


FORMS += \
    downloadinfo.ui \
    mainwindow.ui \
    newtask.ui \
    formclassify.ui \
    removetask.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

HEADERS += \
    common/common.h \
    download/abstractmission.h \
    download/downloadmission.h \
    download/multithreaddownloader.h \
    download/multithreadeddownloaderwriter.h \
    download/until.h \
    model/classifymodel.h \
    model/datamodel.h \
    model/filemodel.h \
    operator/DataHandle.h \
    operator/handlefactory.h \
    operator/sqladapter.h \
    operator/sqlclassifyadapter.h \
    operator/sqldatahandle.h \
    operator/sqlfileadapter.h \
    ui/downloadinfo.h \
    ui/formclassify.h \
    ui/mainwindow.h \
    ui/newtask.h \
    ui/removetask.h

SOURCES += \
    download/downloadmission.cpp \
    download/multithreaddownloader.cpp \
    download/multithreadeddownloaderwriter.cpp \
    main.cpp \
    model/classifymodel.cpp \
    model/datamodel.cpp \
    model/filemodel.cpp \
    operator/handlefactory.cpp \
    operator/sqladapter.cpp \
    operator/sqlclassifyadapter.cpp \
    operator/sqldatahandle.cpp \
    operator/sqlfileadapter.cpp \
    ui/downloadinfo.cpp \
    ui/formclassify.cpp \
    ui/mainwindow.cpp \
    ui/newtask.cpp \
    ui/removetask.cpp
