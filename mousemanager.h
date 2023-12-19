#ifndef MOUSEMANAGER_H
#define MOUSEMANAGER_H

#include "qpoint.h"
#include <QObject>

class MouseManager : public QObject
{
    Q_OBJECT
public:
    explicit MouseManager(QObject *parent = nullptr);
    ~MouseManager();

    void startMouseHook();
    void stopMouseHook();

    bool isRunning;

private:
    QTimer *mouseTimer;
    QPoint getRandomPointWithinCircle(const QPoint& centerOfCircle, int circleRadius);
    void leftClick();
    bool isLocation;
    bool isArea;
    bool shouldStop = false;
    int repetitionCount = 0;

    struct ThreadData {
        QChar clickType;
        int timeToClick;
        int addRandomTime;
        int repetitions;
        int area;
        QPoint location;
        bool isArea;
        bool isLocation;
    };

    ThreadData threadData;
    bool repetitionsCompleted() const;

public slots:
    void runClickingApplication(const int& clickTime, const int& timeBetweenClicks, const QChar& type, const int& repetitions, const QChar& location, const QPoint& xy, const int& area);
    void stopClickingApplication();

private slots:
    void runApplication();

signals:
    void leftMouseClick();
    void mouseMoved(int x, int y);
    void finished();
};

#endif // MOUSEMANAGER_H
