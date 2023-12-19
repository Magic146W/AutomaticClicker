#include "inputmanager.h"
#include "windowshookmanager.h"
#include "mousemanager.h"
#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QLocalSocket>

InputManager* InputManager::instance = nullptr;

InputManager::InputManager(QObject *parent)
    : QObject(parent)
    ,isProcessRunning(false)
{
    windowsHookManager = new WindowsHookManager(this);
    connect(this, &InputManager::hotkeyChangePassed, windowsHookManager, &WindowsHookManager::updateKeyboardVirtualKeys);
    connect(this, &InputManager::isDialogOpen, windowsHookManager, &WindowsHookManager::setDialogOpen);
    connect(windowsHookManager, &WindowsHookManager::passCursorLocation, this, &InputManager::updateUserCursorLocationSet);
    connect(this, &InputManager::getCursorPosition, windowsHookManager, &WindowsHookManager::prepareToGetCursorLocation);
    windowsHookManager->updateKeyboardVirtualKeys("f6");
    connect(WindowsHookManager::getInstance(), &WindowsHookManager::keyboardEventTriggered, this, &InputManager::updateProcessWithHook);

    mouseManager = new MouseManager(this);
    connect(this, &InputManager::startApplication, mouseManager, &MouseManager::runClickingApplication);
    connect(this, &InputManager::stopApplication,mouseManager, &MouseManager::stopClickingApplication);
    connect(mouseManager, &MouseManager::finished,this, &InputManager::updateProcessWithHook);
}

InputManager* InputManager::getInstance(QObject *parent)
{
    if (!instance) {
        instance = new InputManager(parent);
    }
    return instance;
}

void InputManager::setMainWindowInstance(MainWindow *instance) {
    mainWindowInstance = instance;
    QObject::connect(this, &InputManager::initializeStartProcess, mainWindowInstance, &MainWindow::startApplication);
    QObject::connect(this, &InputManager::hotkeyChangePassed, mainWindowInstance, &MainWindow::updateButtonsText);
    QObject::connect(this, &InputManager::blockUIElements, mainWindowInstance, &MainWindow::blockUIElements);
}

void InputManager::updateProcessWithHook()
{
    updateProcessState(isProcessRunning);
}

void InputManager::changeButton(const QString& buttonType, const QString& value)
{
    radioButtonValues[buttonType] = value;
}

QString InputManager::getUserHotkey()
{
    return userHotkey;
}

void InputManager::updateUserHotkey(const QString pressedKey)
{
    userHotkey = pressedKey;
    emit hotkeyChangePassed(pressedKey);
}

void  InputManager::updateUserCursorLocationSet(int x, int y)
{
    emit returnCursorPosition(x,y);
}

void InputManager::updateProcessState(bool stop)
{
    qDebug() << "update process: " + QString(stop ? "true" : "false");
    if (stop)
    {
        emit stopApplication();
        blockUIElements(true);
        isProcessRunning = false;
    } else
    {
        emit initializeStartProcess();
        blockUIElements(false);
        isProcessRunning = true;
    }   
}

void InputManager::updateUserData(int fixedTime, int randomTime, const int& repeatTimes, const QPoint& xy, const int& area)
{
    int repetitions = repeatTimes;

    if(radioButtonValues["RepetitionMode"].toLower() != "repeat")
    {
        repetitions = 2000000000;
    }

    if(randomTime<0)
    {
        randomTime = 0;
    }

    QString typeName = radioButtonValues["PressType"];
    QChar type = typeName.at(0);

    QString locationName = radioButtonValues["LocationOption"];
    QChar location = locationName.at(0);
    qDebug() << location;

    emit startApplication(fixedTime, randomTime, type, repetitions, location, xy, area);
}
