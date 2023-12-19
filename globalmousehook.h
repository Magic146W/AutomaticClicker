#ifndef GLOBALMOUSEHOOK_H
#define GLOBALMOUSEHOOK_H

#include <QObject>
#include <QThread>
#include <Windows.h>

class GlobalMouseHook : public QObject {
    Q_OBJECT

public:
    explicit GlobalMouseHook(QObject *parent = nullptr);
    ~GlobalMouseHook();
    static GlobalMouseHook* getInstance();

signals:
    void mouseMoved(int x, int y);
    void leftButtonClicked();
    void rightButtonClicked();
    void middleButtonClicked();

private:
    static HHOOK mouseHook;
    static GlobalMouseHook* instance;
    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static GlobalMouseHook* mouseHookInstance;
};

#endif // GLOBALMOUSEHOOK_H
