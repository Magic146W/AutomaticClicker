#include "GlobalMouseHook.h"

/**
 * @brief The GlobalMouseHook class manages mouse events globally through a Windows hook.
 *
 * This class sets up a low-level mouse hook (WH_MOUSE_LL) to capture and handle mouse events,
 * including movements, left and right button clicks.
 *
 * @details It utilizes SetWindowsHookEx to establish the hook and emits signals for specific
 * mouse events, allowing external handling.
 */

HHOOK GlobalMouseHook::mouseHook = nullptr;
GlobalMouseHook* GlobalMouseHook::instance = nullptr;

GlobalMouseHook* GlobalMouseHook::getInstance()
{
    if (!instance)
    {
        instance = new GlobalMouseHook();
    }
    return instance;
}

GlobalMouseHook::GlobalMouseHook(QObject *parent) : QObject(parent)
{
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(NULL), 0);
}

GlobalMouseHook::~GlobalMouseHook()
{
    if (mouseHook != nullptr)
    {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
    }
}

/**
 * @brief Callback function for processing low-level mouse events.
 *
 * @param nCode A hook code. If less than zero, the function must return the result of
 * calling the next hook procedure in the hook chain.
 * @param wParam The identifier of the mouse message.
 * @param lParam A pointer to an MSLLHOOKSTRUCT structure that contains details of the event.
 *
 * @details This function is called when a low-level mouse event occurs. It interprets
 * and handles different mouse events such as mouse movements, left button clicks, and right button clicks.
 * Upon receiving a particular mouse event, it emits corresponding signals to notify the connected slots.
 *
 * @return The result of calling the next hook procedure in the hook chain.
 */

LRESULT CALLBACK GlobalMouseHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        MSLLHOOKSTRUCT *pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        switch (wParam)
        {
        case WM_MOUSEMOVE:
            emit getInstance()->mouseMoved(pMouseStruct->pt.x, pMouseStruct->pt.y);
            break;
        case WM_LBUTTONDOWN:
            getInstance()-> emit leftButtonClicked();
            break;
        case WM_RBUTTONDOWN:
            emit getInstance()->rightButtonClicked();
            break;
        default:
            break;
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
