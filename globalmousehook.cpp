#include "GlobalMouseHook.h"

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
