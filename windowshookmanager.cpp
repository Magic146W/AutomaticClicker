#include "windowshookmanager.h"
#include "hookworker.h"
#include "GlobalMouseHook.h"
#include "qdebug.h"
#include "qthread.h"
#include <QKeyEvent>

/**
 * @brief Coordinates and manages all hook-related classes, including keyboard and mouse hooks.
 *
 * @details Acts as a centralized manager for hook-based functionalities, overseeing keyboard and mouse hook operations.
 * Initializes and manages instances of HookWorker and GlobalMouseHook, handles keyboard mapping, and grabs cursor locations.
 */

WindowsHookManager* WindowsHookManager::instance = nullptr;

WindowsHookManager* WindowsHookManager::getInstance(QObject* parent)
{
    if (!instance)
    {
        instance = new WindowsHookManager(parent);
    }
    return instance;
}

WindowsHookManager::WindowsHookManager(QObject *parent) : QObject(parent),
    hookWorker(new QThread(this)),
    hookWorkerInstance(new HookWorker()),
    shouldGrabMouse(false)
{
    keyMap =
        {
            {"f1", VK_F1}, {"f2", VK_F2}, {"f3", VK_F3}, {"f4", VK_F4}, {"f5", VK_F5},
            {"f6", VK_F6}, {"f7", VK_F7}, {"f8", VK_F8}, {"f9", VK_F9}, {"f10", VK_F10},
            {"f11", VK_F11}, {"f12", VK_F12}
        };

    moveToThread(hookWorker);
    hookWorkerInstance = HookWorker::getInstance();
    connect(hookWorkerInstance, &HookWorker::keyboardEventTriggered, this, &WindowsHookManager::keyboardEventTriggered);

    hookWorker->start();
}

WindowsHookManager::~WindowsHookManager()
{
    if (hookWorker->isRunning())
    {
        hookWorker->quit();
        hookWorker->wait();
    }

    delete hookWorkerInstance;
    delete hookWorker;
    hookWorkerInstance = nullptr;
    hookWorker = nullptr;
}

/**
 * @brief Controls the hook execution status based on the hotkey dialog state.
 *
 * @param isOpen Indicates if a dialog window is open or closed.
 */

void WindowsHookManager::setDialogOpen(bool isOpen)
{
    if (isOpen)
    {
        hookWorkerInstance->stopHook(false);
    } else
    {
        hookWorkerInstance->stopHook(true);
    }
}

/**
 * @brief Updates the virtual key code corresponding to the provided key string.
 *
 * @param key The string representation of the key to update the virtual key code.
 */

void WindowsHookManager::updateKeyboardVirtualKeys(const QString& key = nullptr)
{
    int vkCode = 0;

    if (key.compare("ctrl", Qt::CaseInsensitive) == 0)
    {
        vkCode = VK_CONTROL;
    } else if (key.compare("alt", Qt::CaseInsensitive) == 0)
    {
        vkCode = VK_MENU;
    } else if (key.compare("shift", Qt::CaseInsensitive) == 0)
    {
        vkCode = VK_SHIFT;
    } else if (key.length() == 1)
    {
        vkCode = MapVirtualKeyA(key.at(0).unicode(), MAPVK_VK_TO_VSC);
    } else
    {
        if (keyMap.contains(key.toLower()))
        {
            vkCode = keyMap.value(key.toLower());
        }
    }

    if(vkCode != 0)
    {
        hookWorkerInstance->setVkCode(vkCode);
    }
}

/**
 * @brief Prepares to retrieve the cursor's location on the screen.
 */

void WindowsHookManager::prepareToGetCursorLocation()
{
    mouseHookInstance = GlobalMouseHook::getInstance();
    connect(mouseHookInstance, &GlobalMouseHook::leftButtonClicked, this, &WindowsHookManager::getCursorLocationOnScreen);
    shouldGrabMouse = true;
}

/**
 * @brief Retrieves the cursor's location on the screen in response to a left button click.
 */

void WindowsHookManager::getCursorLocationOnScreen()
{
    if (shouldGrabMouse)
    {
        shouldGrabMouse = false;
        disconnect(mouseHookInstance, &GlobalMouseHook::leftButtonClicked, this, &WindowsHookManager::getCursorLocationOnScreen);

        POINT cursorPos;
        if (GetCursorPos(&cursorPos))
        {
            emit passCursorLocation(cursorPos.x, cursorPos.y);
        }
    }
}
