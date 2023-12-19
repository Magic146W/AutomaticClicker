#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "mainwindow.h"
#include "mousemanager.h"
#include "windowshookmanager.h"
#include <QLocalSocket>
#include <QObject>
#include <QMap>
#include <QString>
#include <QTimer>

class InputManager : public QObject
{
    Q_OBJECT
public:
    static InputManager* getInstance(QObject *parent = nullptr);

    void setMainWindowInstance(MainWindow *instance);
    void changeButton(const QString& buttonType, const QString& value);
    QString getUserHotkey();
    bool isProcessRunning;

private:
    explicit InputManager(QObject *parent = nullptr);
    static InputManager* instance;
    WindowsHookManager* windowsHookManager;
    MouseManager* mouseManager;
    MainWindow* mainWindowInstance;
    void updateProcessWithHook();

    QMap<QString, QString> radioButtonValues;
    QTimer debounceTimer;
    QString userHotkey;

public slots:
    void updateProcessState(bool stop);
    void updateUserHotkey(const QString pressedKeys);
    void updateUserCursorLocationSet(int x, int y);
    void updateUserData(int fixedTime, int randomTime,const int& repeatTimes,const QPoint& location, const int& area);

signals:
    void hotkeyChangePassed(QString newHotkey);
    void getCursorPosition();
    void returnCursorPosition(int x, int y);
    void startApplication(const int& clickTime, const int& timeBetweenClicks, const QChar& type, const int& repetitions, const QChar& location, const QPoint& xy, const int& area);
    void stopApplication();
    void initializeStartProcess();
    void isDialogOpen(bool isDialog);
    void blockUIElements(bool isBlock);

};

#endif // INPUTMANAGER_H
