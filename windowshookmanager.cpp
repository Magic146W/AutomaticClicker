#include "windowshookmanager.h"
#include "hookworker.h"
#include "GlobalMouseHook.h"
#include "qdebug.h"
#include "qthread.h"
#include <QKeyEvent>

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

void WindowsHookManager::prepareToGetCursorLocation()
{
    mouseHookInstance = GlobalMouseHook::getInstance();
    connect(mouseHookInstance, &GlobalMouseHook::leftButtonClicked, this, &WindowsHookManager::getCursorLocationOnScreen);
    shouldGrabMouse = true;
}

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
        } else
        {
            qDebug() << "Failed to get cursor position.";
        }
    }
}
