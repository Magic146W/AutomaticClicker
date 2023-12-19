#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlineedit.h"
#include <QLocalSocket>
#include <QMainWindow>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool eventFilter(QObject *watched, QEvent *event);
    void radioButtonGroupSetUp();
    template<typename... Args>
    void setUpButtonGroup(const QString buttonGroupName, QButtonGroup* buttonGroup, Args... buttons);
    QButtonGroup clickingIntervalData;
    QButtonGroup pressTypeData;
    QButtonGroup repeatClickData;
    QButtonGroup locationOptionsData;
    void handleButtonUpdate(QAbstractButton* button, const QString& option, QButtonGroup* buttonGroup);
    QList<QButtonGroup*> allButtonGroups;
    void connectValidatorForLineEdit();
    QVector<QLineEdit *> lineEditList;
    void loadSelectedRadioButtons();
    int getFixedTime();

public slots:
    void mouseLocationUpdate(int x, int y);
    void updateButtonsText(const QString& newHotkey);
    void startApplication();
    void blockUIElements(bool isBlocked);

private slots:
    void on_PushButton_SetLocation_clicked();
    void on_PushButton_SetHotkey_clicked();
    void on_PushButton_Start_clicked();
    void on_PushButton_Stop_clicked();
    void on_PushButton_Save_clicked();
    void on_PushButton_Load_clicked();
    void updateInputManager(QAbstractButton* button);
    void validateTimeRange();

signals:
    void startCursorPositionGrab();
    void updateApplicationRunProcess(int fixedTime, int randomTime,const int& repeatTimes,const QPoint& location, const int&area);
    void stopApplication(bool stop);

};
#endif // MAINWINDOW_H
