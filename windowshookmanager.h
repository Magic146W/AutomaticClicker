#ifndef WINDOWSHOOKMANAGER_H
#define WINDOWSHOOKMANAGER_H

#include "GlobalMouseHook.h"
#include "hookworker.h"
#include <Windows.h>
#include <QObject>
#include <QMap>

class WindowsHookManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowsHookManager(QObject *parent = nullptr);
    ~WindowsHookManager();
    static WindowsHookManager* getInstance(QObject *parent = nullptr);

private:
    static WindowsHookManager* instance;
    QMap<QString, int> keyMap;
    QThread *hookWorker;
    HookWorker *hookWorkerInstance;
    GlobalMouseHook* mouseHook;
    GlobalMouseHook* mouseHookInstance;
    void getCursorLocationOnScreen();
    bool shouldGrabMouse;

public slots:
    void updateKeyboardVirtualKeys(const QString& keyCombination);
    void setDialogOpen(bool isOpen);
    void prepareToGetCursorLocation();

signals:
    void keyboardEventTriggered();
    void passCursorLocation(int x, int y);
};

#endif // WINDOWSHOOKMANAGER_H
