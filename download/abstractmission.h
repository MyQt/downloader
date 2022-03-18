#ifndef ABSTRACTMISSION_H
#define ABSTRACTMISSION_H

#include <QUrl>
#include <QDebug>
#include <QObject>

class AbstractMission : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        Running,
        Paused,
        Stopped
    };
    virtual ~AbstractMission() Q_DECL_OVERRIDE = default;
    template<typename T>
    void setUrl(const T& url) {
        mUrl = url;
        if (!mUrl.isValid()) {
            qWarning("AbstractMisiion: url is not valid.");
        }
    }
    QUrl url() const { return mUrl; }
    bool isFinished() const { return mIsFinished; }
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void restart() = 0;
    State state() const { return mState; }
protected:
    explicit AbstractMission(QObject* parent = nullptr) : QObject(parent) {}
    inline void updateState(const State state) {
        mState = state;
        emit stateChanged(state);
    }
    inline void setFinished(const bool isFinished = true) {
        mIsFinished = isFinished;
        if (isFinished) {
            emit finished(-1);
        }
    }
    QUrl mUrl;
    State mState = Stopped;
    bool mIsFinished = false;
signals:
    void finished(int id);
    void stateChanged(const State state);
};

#endif // ABSTRACTMISSION_H
