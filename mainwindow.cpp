#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "InputManager.h"
#include "changehotkeydialog.h"
#include "qscreen.h"
#include <Windows.h>
#include <QButtonGroup>
#include <QProcess>
#include <QPoint>
#include <QLocalSocket>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->LineEdit_X->setReadOnly(true);
    ui->LineEdit_Y->setReadOnly(true);

    qApp->installEventFilter(this);
    connectValidatorForLineEdit();

    InputManager *inputManager = InputManager::getInstance(this);
    inputManager->setMainWindowInstance(this);

    connect(inputManager, &InputManager::returnCursorPosition, this, &MainWindow::mouseLocationUpdate);
    connect(this, &MainWindow::startCursorPositionGrab, inputManager, &InputManager::getCursorPosition);
    connect(this, &MainWindow::updateApplicationRunProcess, inputManager, &InputManager::updateUserData);
    connect(this, &MainWindow::stopApplication, inputManager, &InputManager::updateProcessState);
    radioButtonGroupSetUp();

    ui->PushButton_Stop->setEnabled(false);
    on_PushButton_Load_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::validateTimeRange()
{
    QTime fromTime = QTime::fromString(ui->TimeEdit_From->text(),"mm:ss:zzz");
    QTime tillTime = QTime::fromString(ui->TimeEdit_Till->text(),"mm:ss:zzz");

    bool isValidTillTime = tillTime.isValid();

    if(tillTime.msecsSinceStartOfDay()<10 && fromTime.msecsSinceStartOfDay()<10)
    {
        tillTime = QTime::fromString("00:00:010");
        ui->TimeEdit_From->setTime(tillTime);
    }
    bool isSmallerThanFromTime = tillTime.msecsSinceStartOfDay() < fromTime.msecsSinceStartOfDay();
    qDebug() << isSmallerThanFromTime;

    if (!isValidTillTime || isSmallerThanFromTime)
    {
        ui->TimeEdit_Till->setTime(QTime::fromString(ui->TimeEdit_From->text(),"mm:ss:zzz"));
    }
}

int MainWindow::getFixedTime()
{
    QTime fixedQTime = QTime::fromString(QString("%1:%2:%3")
                                             .arg(ui->LineEdit_Minutes->text().rightJustified(2, '0'))
                                             .arg(ui->LineEdit_Seconds->text().rightJustified(2, '0'))
                                             .arg(ui->LineEdit_Milliseconds->text().rightJustified(3, '0')), "mm:ss:zzz");

    if(fixedQTime.msecsSinceStartOfDay()<10)
    {
        ui->LineEdit_Milliseconds->setText("10");
        return 10;
    } else
    {
        return fixedQTime.msecsSinceStartOfDay();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        QWidget *widget = qobject_cast<QWidget*>(watched);
        if (widget && widget->inherits("QLineEdit"))
        {
            QLineEdit *lineEdit = static_cast<QLineEdit*>(widget);
            QString text = lineEdit->text();
            if(text.isEmpty())
            {
                if(lineEdit->objectName() == ui->LineEdit_Milliseconds->objectName())
                {
                    getFixedTime();
                }
                lineEdit->setText("0");
            } else
            {
                if(lineEdit->objectName() == ui->LineEdit_Milliseconds->objectName())
                {
                    if(lineEdit->objectName() == ui->LineEdit_Milliseconds->objectName())
                    {
                        getFixedTime();
                    }
                }

                const QValidator *validator = lineEdit->validator();
                if (validator && validator->inherits("QIntValidator"))
                {
                    const QIntValidator *intValidator = static_cast<const QIntValidator*>(validator);
                    int value = text.toInt();
                    if (intValidator->validate(text, value) != QValidator::Acceptable)
                    {
                        lineEdit->setText(QString::number(intValidator->bottom()));
                    }
                }
            }
        } else if (widget && widget->inherits("QTimeEdit"))
        {
            validateTimeRange();
        }
        return QMainWindow::eventFilter(watched, event);
    }
    return false;
}

void MainWindow::connectValidatorForLineEdit()
{
    QIntValidator *validatorMin = new QIntValidator(0, 60, this);
    ui->LineEdit_Minutes->setValidator(validatorMin);

    QIntValidator *validatorSec = new QIntValidator(0, 59, this);
    ui->LineEdit_Seconds->setValidator(validatorSec);

    QIntValidator *validatorMsec = new QIntValidator(0, 999, this);
    ui->LineEdit_Milliseconds->setValidator(validatorMsec);

    QIntValidator *validatorRepeat = new QIntValidator(0, 10000, this);
    ui->LineEdit_RepeatTimes->setValidator(validatorRepeat);

    int screenHeight = QApplication::primaryScreen()->geometry().height();
    int maxHeight = screenHeight / 2;
    QIntValidator *validatorArea = new QIntValidator(1, maxHeight, this);
    ui->LineEdit_Area->setValidator(validatorArea);
}

template<typename... Args>
void MainWindow::setUpButtonGroup(const QString buttonGroupName, QButtonGroup* buttonGroup, Args... buttons)
{
    (buttonGroup->addButton(buttons), ...);
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateInputManager(QAbstractButton*)));
    buttonGroup->setObjectName(buttonGroupName);
    allButtonGroups << buttonGroup;
}

void MainWindow::radioButtonGroupSetUp()
{
    setUpButtonGroup("ClickingInterval", &clickingIntervalData, ui->RadioButton_FixedClickInterval, ui->RadioButton_RandomClickInterval);
    setUpButtonGroup("PressType", &pressTypeData, ui->RadioButton_SinglePress, ui->RadioButton_DoublePress);
    setUpButtonGroup("RepetitionMode", &repeatClickData,ui->RadioButton_RepeatTimes,ui->RadioButton_InfiniteClick);
    setUpButtonGroup("LocationOption", &locationOptionsData,ui->RadioButton_LocationAtCursor,ui->RadioButton_ChoosenLocation,ui->RadioButton_RandomWithinArea);

    loadSelectedRadioButtons();
}

void MainWindow::loadSelectedRadioButtons()
{
    foreach (QButtonGroup* buttonGroup, allButtonGroups)
    {
        if(buttonGroup->checkedButton() == nullptr)
        {
            buttonGroup->buttons().first()->setChecked(true);
        }
        InputManager::getInstance()->changeButton(buttonGroup->objectName(), buttonGroup->checkedButton()->text());
    }
}

void MainWindow::handleButtonUpdate(QAbstractButton* button, const QString& option, QButtonGroup* buttonGroup)
{
    if (buttonGroup->buttons().contains(button))
    {
        InputManager::getInstance()->changeButton(option, button->text());
        buttonGroup->setExclusive(button);
    }
}

void MainWindow::updateInputManager(QAbstractButton* button)
{
    if (clickingIntervalData.buttons().contains(button))
    {
        handleButtonUpdate(button, "ClickingInterval", &clickingIntervalData);
    } else if (pressTypeData.buttons().contains(button))
    {
        handleButtonUpdate(button, "PressType", &pressTypeData);
    } else if (repeatClickData.buttons().contains(button))
    {
        handleButtonUpdate(button, "RepetitionMode", &repeatClickData);
    } else if (locationOptionsData.buttons().contains(button))
    {
        handleButtonUpdate(button, "LocationOption", &locationOptionsData);
    }
}

void MainWindow::updateButtonsText(const QString& newHotkey)
{
    ui->PushButton_Start->setText("Start ("+newHotkey+")");
    ui->PushButton_Stop->setText("Stop ("+newHotkey+")");
}

void MainWindow::blockUIElements(bool isBlocked)
{
    ui->LineEdit_Area->setEnabled(isBlocked);
    ui->LineEdit_RepeatTimes->setEnabled(isBlocked);
    ui->LineEdit_Minutes->setEnabled(isBlocked);
    ui->LineEdit_Seconds->setEnabled(isBlocked);
    ui->LineEdit_Milliseconds->setEnabled(isBlocked);
    ui->LineEdit_X->setEnabled(isBlocked);
    ui->LineEdit_Y->setEnabled(isBlocked);

    ui->TimeEdit_From->setEnabled(isBlocked);
    ui->TimeEdit_Till->setEnabled(isBlocked);

    ui->PushButton_Save->setEnabled(isBlocked);
    ui->PushButton_Load->setEnabled(isBlocked);
    ui->PushButton_SetHotkey->setEnabled(isBlocked);
    ui->PushButton_SetLocation->setEnabled(isBlocked);
    ui->PushButton_Start->setEnabled(isBlocked);

    ui->RadioButton_ChoosenLocation->setEnabled(isBlocked);
    ui->RadioButton_DoublePress->setEnabled(isBlocked);
    ui->RadioButton_FixedClickInterval->setEnabled(isBlocked);
    ui->RadioButton_InfiniteClick->setEnabled(isBlocked);
    ui->RadioButton_LocationAtCursor->setEnabled(isBlocked);
    ui->RadioButton_RandomClickInterval->setEnabled(isBlocked);
    ui->RadioButton_RandomWithinArea->setEnabled(isBlocked);
    ui->RadioButton_RepeatTimes->setEnabled(isBlocked);
    ui->RadioButton_SinglePress->setEnabled(isBlocked);

    ui->PushButton_Stop->setEnabled(!isBlocked);
}

void MainWindow::on_PushButton_SetLocation_clicked()
{
    emit startCursorPositionGrab();
}

void MainWindow::mouseLocationUpdate(int x, int y)
{
    ui->LineEdit_X ->setText(QString::number(x));
    ui->LineEdit_Y ->setText(QString::number(y));
}

void MainWindow::on_PushButton_SetHotkey_clicked()
{
    ChangeHotkeyDialog* dialog = new ChangeHotkeyDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::startApplication()
{
    loadSelectedRadioButtons();

    int fromTime, tillTime;
    if(clickingIntervalData.checkedButton()->objectName().toLower() == "radiobutton_randomclickinterval")
    {
        QTime fromQTime = QTime::fromString(ui->TimeEdit_From->text(),"mm:ss:zzz");
        QTime tillQTime = QTime::fromString(ui->TimeEdit_Till->text(),"mm:ss:zzz");
        fromTime = fromQTime.msecsSinceStartOfDay();
        tillTime = fromQTime.msecsTo(tillQTime);
    } else
    {
        fromTime = getFixedTime();
        tillTime = 0;
    }

    int repeatTimes = ui->LineEdit_RepeatTimes->text().toInt();
    QPoint point(ui->LineEdit_X->text().toInt(),ui->LineEdit_Y->text().toInt());
    int area = ui->LineEdit_Area->text().toInt();

    emit updateApplicationRunProcess(fromTime,tillTime,repeatTimes,point,area);
}

void MainWindow::on_PushButton_Start_clicked()
{
    InputManager::getInstance()->updateProcessState(false);
}

void MainWindow::on_PushButton_Stop_clicked()
{
    emit stopApplication(true);
}

void MainWindow::on_PushButton_Save_clicked()
{
    QSettings settings("MyApp", "MyAppSettings");

    settings.setValue("size", size());

    settings.setValue("LineEdit_AreaEnabled", ui->LineEdit_Area->text());
    settings.setValue("LineEdit_RepeatTimes", ui->LineEdit_RepeatTimes->text());
    settings.setValue("LineEdit_Minutes", ui->LineEdit_Minutes->text());
    settings.setValue("LineEdit_Seconds", ui->LineEdit_Seconds->text());
    settings.setValue("LineEdit_Milliseconds", ui->LineEdit_Milliseconds->text());
    settings.setValue("LineEdit_X", ui->LineEdit_X->text());
    settings.setValue("LineEdit_Y", ui->LineEdit_Y->text());

    settings.setValue("TimeEdit_From", ui->TimeEdit_From->time().toString("mm:ss:zzz"));
    settings.setValue("TimeEdit_Till", ui->TimeEdit_Till->time().toString("mm:ss:zzz"));

    settings.setValue("RadioButton_ChoosenLocation", ui->RadioButton_ChoosenLocation->isChecked());
    settings.setValue("RadioButton_DoublePress", ui->RadioButton_DoublePress->isChecked());
    settings.setValue("RadioButton_FixedClickInterval", ui->RadioButton_FixedClickInterval->isChecked());
    settings.setValue("RadioButton_InfiniteClick", ui->RadioButton_InfiniteClick->isChecked());
    settings.setValue("RadioButton_LocationAtCursor", ui->RadioButton_LocationAtCursor->isChecked());
    settings.setValue("RadioButton_RandomClickInterval", ui->RadioButton_RandomClickInterval->isChecked());
    settings.setValue("RadioButton_RandomWithinArea", ui->RadioButton_RandomWithinArea->isChecked());
    settings.setValue("RadioButton_RepeatTimes", ui->RadioButton_RepeatTimes->isChecked());
    settings.setValue("RadioButton_SinglePress", ui->RadioButton_SinglePress->isChecked());

    qDebug() << InputManager::getInstance()->getUserHotkey();

    settings.setValue("vkCode", InputManager::getInstance()->getUserHotkey());
}

void MainWindow::on_PushButton_Load_clicked()
{
    QSettings settings("MyApp", "MyAppSettings");
    if(settings.contains("LineEdit_AreaEnabled"))
    {
        resize(settings.value("size", QSize(800, 600)).toSize());

        ui->LineEdit_Area->setText(settings.value("LineEdit_AreaEnabled").toString());
        ui->LineEdit_RepeatTimes->setText(settings.value("LineEdit_RepeatTimes").toString());
        ui->LineEdit_Minutes->setText(settings.value("LineEdit_Minutes").toString());
        ui->LineEdit_Seconds->setText(settings.value("LineEdit_Seconds").toString());
        ui->LineEdit_Milliseconds->setText(settings.value("LineEdit_Milliseconds").toString());
        ui->LineEdit_X->setText(settings.value("LineEdit_X").toString());
        ui->LineEdit_Y->setText(settings.value("LineEdit_Y").toString());

        ui->TimeEdit_From->setTime(QTime::fromString(settings.value("TimeEdit_From").toString(), "mm:ss:zzz"));
        ui->TimeEdit_Till->setTime(QTime::fromString(settings.value("TimeEdit_Till").toString(), "mm:ss:zzz"));

        ui->RadioButton_ChoosenLocation->setChecked(settings.value("RadioButton_ChoosenLocation").toBool());
        ui->RadioButton_DoublePress->setChecked(settings.value("RadioButton_DoublePress").toBool());
        ui->RadioButton_FixedClickInterval->setChecked(settings.value("RadioButton_FixedClickInterval").toBool());
        ui->RadioButton_InfiniteClick->setChecked(settings.value("RadioButton_InfiniteClick").toBool());
        ui->RadioButton_LocationAtCursor->setChecked(settings.value("RadioButton_LocationAtCursor").toBool());
        ui->RadioButton_RandomClickInterval->setChecked(settings.value("RadioButton_RandomClickInterval").toBool());
        ui->RadioButton_RandomWithinArea->setChecked(settings.value("RadioButton_RandomWithinArea").toBool());
        ui->RadioButton_RepeatTimes->setChecked(settings.value("RadioButton_RepeatTimes").toBool());
        ui->RadioButton_SinglePress->setChecked(settings.value("RadioButton_SinglePress").toBool());

        InputManager::getInstance()->updateUserHotkey(settings.value("vkCode").toString());
    }
}


