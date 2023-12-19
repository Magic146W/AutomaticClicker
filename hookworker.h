#ifndef HOOKWORKER_H
#define HOOKWORKER_H

#include <QObject>
#include <windows.h>
#include <QMap>

class HookWorker : public QObject
{
    Q_OBJECT
public:
    explicit HookWorker(QObject *parent = nullptr);
    ~HookWorker();

    void processHooks();
    void blockHook(bool block);
    int getCurrentVKCode();
    void setVkCode(int newVKCode);
    void stopHook(bool run);

    static HookWorker* getInstance() {
        return instance;
    }

private:
    HHOOK globalKeyboardHook;
    static HookWorker* instance;
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    bool isRunning;

    int vkCode = 117;
    bool isHookBlocked;

signals:
    void keyboardEventTriggered();

};

#endif // HOOKWORKER_H
