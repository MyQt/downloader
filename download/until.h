#ifndef UNTIL_H
#define UNTIL_H

#include <QString>
#include <QTime>
namespace Until {
    /*
     * 链接重定义问题，因为命名空间中的函数，默认不是内联的，所以包含该.h文件时，会出现重定义情况
     * 解决方法：
     * 1.将函数实现所入cpp文件中，这样只生成了一份该文件的obj代码
     * 2.将函数声明为内联函数，编译阶段直接拆分成obj代码，不再去入口找
    */
    /*
     * brief 获取可读文件大小
     *
    */
    inline QString readableFileSize(const qint64 value, int precision = 2)
    {
        qint64 kbSize = value / 1024;
        if (kbSize > 1024) { // 大于1mb
            qreal mbRet = (qreal)(kbSize)/1024.0;
            if (mbRet - 1024.0 > 0.000001) { // 大于1GB
                qreal gbRet = mbRet / 1024.0;
                return QString::number(gbRet, 'f', precision) + "GB";
            } else {
                return QString::number(mbRet, 'f', precision) + "MB";
            }
        } else if (kbSize > 0) {
            return QString::number(kbSize) + "KB";
        }

        return QString::number(value) + "B";
    }
    /**
     * @brief 获取下载百分比
     * @param recvSize 已经下载的大小
     * @param totalSize 总大小
     * @return 百分比字符串
    */
    inline QString downloadProgress(qint64 recvSize, qint64 totalSize, int precision = 2)
    {
        qreal percent = recvSize / totalSize * 100.0;

        return QString::number(percent, 'f', precision) + "%";
    }
    /**
     * @brief 序列化时间为HH:MM:SS
     * @param h:时 m:分 s:秒
     * @return 序列化时间字符串
    */
    inline QString serilizeTime(int h, int m, int s) {
        QTime time;
        time.setHMS(h,m,s);
        return time.toString();
    }
    /**
     * @brief 合法化文件名
     * @param 字符串
     * @return 合法字符串
    */
    inline QString validateString(QString str)
    {
        return  str.replace(QRegExp("[《》:*?\"<>|&#$\\s\\/\\\\]"), "");
    }
};

#endif
