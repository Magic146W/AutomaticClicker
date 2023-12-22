#include "mousemanager.h"
#include "qdebug.h"
#include "qtimer.h"
#include <windows.h>
#include <iostream>
#include <QRandomGenerator>
#include <QPoint>

MouseManager::MouseManager(QObject *parent)
    : QObject{parent}
    ,isLocation(false)
    ,isArea(false)
{
    mouseTimer = new QTimer(this);
    connect(mouseTimer, &QTimer::timeout, this, &MouseManager::runApplication);
}

MouseManager::~MouseManager()
{
    mouseTimer->stop();
    delete mouseTimer;
}

void MouseManager::runClickingApplication(const int& clickTime, const int& timeBetweenClicks, const QChar& type, const int& repetitions, const QChar& location, const QPoint& xy, const int& area)
{
    isLocation = false;
    isArea = false;

    if (location.toLower() == 'c')
    {
        isLocation = true;
    } else if (location.toLower() == 'r')
    {
        isArea = true;
    }

    threadData.timeToClick = clickTime;
    threadData.addRandomTime = timeBetweenClicks;
    threadData.clickType = type.toLower();
    threadData.repetitions = repetitions;
    threadData.location = xy;
    threadData.area = area;
    threadData.isArea = isArea;
    threadData.isLocation = isLocation;

    shouldStop = false;
    repetitionCount = 0;
    mouseTimer->start(clickTime);
}

void MouseManager::stopClickingApplication()
{
    if(!shouldStop)
    {      
        mouseTimer->stop();
    }

    shouldStop = true;
}

bool MouseManager::repetitionsCompleted() const
{
    return repetitionCount >= threadData.repetitions;
}

void MouseManager::runApplication()
{
    if (repetitionsCompleted() || shouldStop)
    {
        emit finished();
        repetitionCount = 0;
        stopClickingApplication();
        return;
    }

    ++repetitionCount;

    if (isLocation)
    {
        SetCursorPos(threadData.location.x(), threadData.location.y());
    }
    else if (isArea)
    {
        QPoint centerOfCircle(threadData.location.x(), threadData.location.y());
        int circleRadius = threadData.area;

        QPoint randomPoint = getRandomPointWithinCircle(centerOfCircle, circleRadius);
        SetCursorPos(randomPoint.x(), randomPoint.y());
        qDebug() << "Cursor pos X: " << randomPoint.x() << ", " << randomPoint.y();
    }

    if (threadData.clickType.toLower() == 'd')
    {
        leftClick();
    }
    leftClick();

    int timeToNextClick = threadData.timeToClick + (rand() % (threadData.addRandomTime + 1));

    qDebug() << timeToNextClick;
    mouseTimer->start(timeToNextClick);
}

void MouseManager::leftClick()
{
    INPUT input[2] = {{0}};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, input, sizeof(INPUT));
}

QPoint MouseManager::getRandomPointWithinCircle(const QPoint& center, int radius)
{
    qreal randomAngle = QRandomGenerator::global()->bounded(2 * M_PI);
    qreal randomDistance = QRandomGenerator::global()->bounded(radius);
    int randomX = center.x() + static_cast<int>(randomDistance * qCos(randomAngle));
    int randomY = center.y() + static_cast<int>(randomDistance * qSin(randomAngle));

    return QPoint(randomX, randomY);
}
