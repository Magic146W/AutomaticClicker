#include "hookworker.h"
#include "qdebug.h"

/**
 * @brief The HookWorker class manages a Windows low-level keyboard hook to monitor and respond to keyboard events globally.
 *
 * This class sets up and controls a low-level keyboard hook (WH_KEYBOARD_LL) for capturing keyboard events like key presses and releases.
 * It enables the detection and handling of specific key sequences in the system.
 *
 * @details The HookWorker class initializes and manages a single instance of a global keyboard hook.
 * It listens to keyboard events (key down and key up) and triggers specific actions based on a defined virtual key code (VKCode).
 * The class also provides functionalities to stop and resume the keyboard hook.
 */

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
        }
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

/**
 * @brief Controls the status of the keyboard hook, either stopping or resuming it.
 * @param running A boolean flag indicating whether the hook should be active or inactive.
 */

void HookWorker::stopHook(bool running)
{
    isRunning = running;
}

/**
 * @brief Triggers the keyboard event signal to indicate the detection of a specific key sequence.
 */

void HookWorker::processHooks()
{
    emit instance->keyboardEventTriggered();
}

/**
 * @brief Sets a new Virtual Key (VK) code to monitor a specific key.
 * @param newVKCode The new Virtual Key (VK) code to be set for monitoring.
 */

void HookWorker::setVkCode(int newVKCode)
{
    vkCode = newVKCode;
}

/**
 * @brief Retrieves the currently set Virtual Key (VK) code being monitored.
 * @return An integer representing the currently monitored Virtual Key (VK) code.
 */

int HookWorker::getCurrentVKCode()
{
    return vkCode;
}

/**
 * @brief Blocks or unblocks the keyboard hook based on the provided boolean parameter.
 * @param block A boolean flag to block or unblock the keyboard hook.
 */

void HookWorker::blockHook(bool block)
{
    isHookBlocked = block;
}

/**
 * @brief Handles low-level keyboard events through a Windows hook, monitoring specific key sequences
 * and controlling hook blocking until the required key sequence is released.
 *
 * @details This function captures keyboard events, allowing identification of when a user presses down and releases a specific key.
 * It blocks the hook until the required key sequence is released during WM_KEYUP,
 * ensuring accurate detection of key presses and releases.
 *
 * @param nCode The hook code indicating the action that should be taken.
 * @param wParam The type of keyboard message (e.g., WM_KEYDOWN, WM_KEYUP).
 * @param lParam A pointer to a KBDLLHOOKSTRUCT structure containing details about the keyboard event.
 * @return Returns the result of the next hook in the chain.
 */

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
