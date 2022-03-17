#include "multithreadeddownloaderwriter.h"

MultithreadedDownloaderWriter::MultithreadedDownloaderWriter(QObject *parent)
    :QThread(parent)
{

}

MultithreadedDownloaderWriter::~MultithreadedDownloaderWriter()
{
    if (m_downloadFile.isOpen()) {
        m_downloadFile.close();
    }
}

void MultithreadedDownloaderWriter::write(const QByteArray &data, const qint64 seek)
{
    WriteMission newMission = {data, seek};
    m_mutex.lock();
    m_writeList.push_back(newMission);
    m_mutex.unlock();
    if (!this->isRunning()) {
        this->start();
    }
}

void MultithreadedDownloaderWriter::run()
{
    do {
        m_downloadFile.seek(m_writeList.first().second);
        m_downloadFile.write(m_writeList.first().first);
        m_mutex.lock();
        m_writeList.pop_front();
        m_mutex.unlock();
        if (m_writeList.isEmpty()) {
            this->msleep(200); // 休眠200毫秒
        }
    } while (!m_writeList.isEmpty());
}

