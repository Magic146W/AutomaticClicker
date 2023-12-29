#include "inputmanager.h"
#include "windowshookmanager.h"
#include "mousemanager.h"
#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QLocalSocket>

/**
 * @brief The InputManager class manages user inputs, interacts with the Windows hook system, and serves as a central control hub.
 *
 * @details This class coordinates interactions between the user interface, various hook-related operations,
 * handling key sequences, mouse movements, and application state changes. It connects user actions
 * to Windows hook manager and mouse manager instances for processing.
 */

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

/**
 * @brief Sets the MainWindow instance and establishes connections between InputManager signals and MainWindow slots.
 *
 * @param instance A pointer to the MainWindow instance to be managed.
 */

void InputManager::setMainWindowInstance(MainWindow *instance) {
    mainWindowInstance = instance;
    QObject::connect(this, &InputManager::initializeStartProcess, mainWindowInstance, &MainWindow::startApplication);
    QObject::connect(this, &InputManager::hotkeyChangePassed, mainWindowInstance, &MainWindow::updateButtonsText);
    QObject::connect(this, &InputManager::blockUIElements, mainWindowInstance, &MainWindow::blockUIElements);
}

/**
 * @brief Updates the current process state based on the hook's running status.
 */


void InputManager::updateProcessWithHook()
{
    updateProcessState(isProcessRunning);
}

/**
 * @brief Updates the value of a specific button type from mainwindow class.
 *
 * @param buttonType The type of button to update.
 * @param value The new value to set for the button.
 */


void InputManager::changeButton(const QString& buttonType, const QString& value)
{
    radioButtonValues[buttonType] = value;
}

/**
 * @brief Retrieves the current user-defined hotkey.
 */


QString InputManager::getUserHotkey()
{
    return userHotkey;
}

/**
 * @brief Updates and sets the user-defined hotkey, emitting a signal upon change.
 *
 * @param pressedKey The newly pressed hotkey by the user.
 */


void InputManager::updateUserHotkey(const QString pressedKey)
{
    userHotkey = pressedKey;
    emit hotkeyChangePassed(pressedKey);
}

/**
 * @brief Emits a signal to return the updated cursor position.
 *
 * @param x The X-coordinate of the cursor.
 * @param y The Y-coordinate of the cursor.
 */

void  InputManager::updateUserCursorLocationSet(int x, int y)
{
    emit returnCursorPosition(x,y);
}

/**
 * @brief Updates the state of the process based on the given boolean value.
 *
 * @param stop A boolean indicating whether to stop (true) or start (false) the process.
 */


void InputManager::updateProcessState(bool stop)
{
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

/**
 * @brief Updates user data based on the provided parameters, then triggers the application start accordingly.
 *
 * @param fixedTime The fixed time interval for the process.
 * @param randomTime The random time interval for the process.
 * @param repeatTimes The number of repetitions for the process.
 * @param xy The coordinates for the process location.
 * @param area The area size for the process.
 *
 * @details Modifies input parameters based on button values, then emits them further.
 */

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

    emit startApplication(fixedTime, randomTime, type, repetitions, location, xy, area);
}
