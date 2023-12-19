#include "hookworker.h"
#include "qdebug.h"

HookWorker* HookWorker::instance = nullptr;

HookWorker::HookWorker(QObject *parent) : QObject(parent),
    globalKeyboardHook(nullptr),
    isRunning(true),
    vkCode(117),
    isHookBlocked(false)
{
    if(!instance)
    {
        instance = this;

        if (!globalKeyboardHook)
        {
            globalKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
            if (globalKeyboardHook == NULL)
            {
                qDebug() << "Failed to install the global keyboard hook!";
            }
        }
    }
    else
    {
        qDebug() << "Only one instance of HookWorker allowed!";
    }
}

HookWorker::~HookWorker()
{
    if (globalKeyboardHook != NULL)
    {
        UnhookWindowsHookEx(globalKeyboardHook);
        globalKeyboardHook = NULL;
    }

    instance = nullptr;
}

void HookWorker::stopHook(bool running)
{
    isRunning = running;
}

void HookWorker::processHooks()
{
    qDebug() << "trigger sent!";
    emit instance->keyboardEventTriggered();
}

void HookWorker::setVkCode(int newVKCode)
{
    vkCode = newVKCode;
}

int HookWorker::getCurrentVKCode()
{
    return vkCode;
}

void HookWorker::blockHook(bool block)
{
    isHookBlocked = block;
}

LRESULT CALLBACK HookWorker::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    HookWorker* instance = getInstance();

    if (instance == nullptr) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if(instance->isRunning)
    {
        if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_KEYUP))
        {
            int vkCode = instance->getCurrentVKCode();

            if (wParam == WM_KEYDOWN && !instance->isHookBlocked)
            {
                KBDLLHOOKSTRUCT* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
                int pressedKey = pKeyBoard->vkCode;

                if (pressedKey == vkCode)
                {
                    qDebug() << vkCode;
                    instance->blockHook(true);
                    emit instance->processHooks();
                }
            } else if (wParam == WM_KEYUP)
            {
                KBDLLHOOKSTRUCT* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
                int releasedKey = pKeyBoard->vkCode;

                if (releasedKey == vkCode)
                {
                    instance->blockHook(false);
                } else
                {
                    return CallNextHookEx(nullptr, nCode, wParam, lParam);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
