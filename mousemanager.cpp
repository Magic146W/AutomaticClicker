#include "mousemanager.h"
#include "qdebug.h"
#include "qtimer.h"
#include <windows.h>
#include <iostream>
#include <QRandomGenerator>
#include <QPoint>

/**
 * @brief Executes mouse actions based on user instructions received from the InputManager.
 * Responsible for simulating mouse clicks and movements.
 *
 * @note Uses QTimer for controlling click intervals and manages mouse event simulation based on given parameters.
 *
 * @details This class handles various mouse actions, such as left-click simulations and cursor positioning.
 * It utilizes QTimer for controlling click intervals and facilitates left-click simulations and cursor movement
 * based on the provided parameters like click time, time between clicks, click type, repetitions, and location.
 * Also, it includes methods to start, stop, and check repetitions in the mouse click simulations.
 */

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

/**
 * @brief Initiates the mouse clicking application based on the provided parameters.
 *
 * @param clickTime The time interval for mouse clicks.
 * @param timeBetweenClicks The extra time interval between consecutive clicks.
 * @param type The type of mouse click.
 * @param repetitions The number of times to repeat the click action.
 * @param location The location type for mouse action.
 * @param xy The coordinates of the mouse action.
 * @param area The area size for mouse action.
 *
 * @details Configures the mouse clicking application based on provided parameters.
 */

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

/**
 * @brief Stops the mouse clicking application if it's currently active.
 */


void MouseManager::stopClickingApplication()
{
    if(!shouldStop)
    {      
        mouseTimer->stop();
    }

    shouldStop = true;
}

/**
 * @brief Checks if the number of repetitions for the mouse clicking process is completed.
 *
 * @return True if the desired number of repetitions is reached; otherwise, false.
 */

bool MouseManager::repetitionsCompleted() const
{
    return repetitionCount >= threadData.repetitions;
}

/**
 * @brief Executes the mouse clicking application based on defined parameters.
 *
 * @details This function manages the execution of the mouse clicking application.
 * It simulates mouse clicks and movements according to the provided settings until
 * the specified number of repetitions is achieved or the process is stopped.
 */

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
    }

    if (threadData.clickType.toLower() == 'd')
    {
        leftClick();
    }
    leftClick();

    int timeToNextClick = threadData.timeToClick + (rand() % (threadData.addRandomTime + 1));

    mouseTimer->start(timeToNextClick);
}

/**
 * @brief Simulates a left mouse button click.
 *
 * @details This function generates a left mouse button click event by sending
 *          the corresponding input flags (down and up) using the Windows API.
 *          It emulates a left mouse button press and release action.
 */

void MouseManager::leftClick()
{
    INPUT input[2] = {{0}};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, input, sizeof(INPUT));
}

/**
 * @brief Generates a random point within a circle given its center and radius.
 *
 * @param center The center point of the circle.
 * @param radius The radius of the circle.
 * @return Returns a random point within the specified circle.
 *
 * @details This function calculates a random point within a circle using polar coordinates.
 *          It generates a random angle and distance within the range of the circle's radius
 *          and then converts these values to Cartesian coordinates (x, y) relative to the center.
 */

QPoint MouseManager::getRandomPointWithinCircle(const QPoint& center, int radius)
{
    qreal randomAngle = QRandomGenerator::global()->bounded(2 * M_PI);
    qreal randomDistance = QRandomGenerator::global()->bounded(radius);
    int randomX = center.x() + static_cast<int>(randomDistance * qCos(randomAngle));
    int randomY = center.y() + static_cast<int>(randomDistance * qSin(randomAngle));

    return QPoint(randomX, randomY);
}
