#include "changehotkeydialog.h"
#include "inputmanager.h"
#include "ui_changehotkeydialog.h"
#include <QKeyEvent>
#include <QMessageBox>

/**
 * @brief The ChangeHotkeyDialog class handles the secondary dialog responsible for changing hotkeys.
 *
 * @details This dialog captures keyboard input to set new hotkeys and displays a confirmation message
 * to accept the newly set hotkey. The dialog comprises buttons to start capturing a new hotkey,
 * cancel the operation, and accept the captured hotkey as the new input.
 */

ChangeHotkeyDialog::ChangeHotkeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeHotkeyDialog)
    ,capturingKeys(false)
{
    ui->setupUi(this);

    InputManager* inputManager = InputManager::getInstance();
    connect(this, &ChangeHotkeyDialog::updateHotkey, inputManager, &InputManager::updateUserHotkey);
    connect(this, &ChangeHotkeyDialog::isDialogOpen, inputManager, &InputManager::isDialogOpen);
    emit isDialogOpen(true);

    ui->LineEdit_New->setReadOnly(true);
    ui->LineEdit_New->setPlaceholderText("Press start...");
}

ChangeHotkeyDialog::~ChangeHotkeyDialog()
{
    emit isDialogOpen(false);
    delete ui;
}

/**
 * @brief Closes the dialog window without changing hotkey.
 */

void ChangeHotkeyDialog::on_PushButton_Cancel_clicked()
{
    close();
}

/**
 * @brief Updates the hotkey and closes the dialog window.
 */

void ChangeHotkeyDialog::on_PushButton_Accept_clicked()
{
    if (!pressedKey.isEmpty())
    {
        emit updateHotkey(pressedKey);        
        close();
    }
}

/**
 * @brief Initiates or resets the hotkey capturing process.
 */

void ChangeHotkeyDialog::on_PushButton_Start_clicked()
{
    if(ui->PushButton_Start->text() != "...")
    {
        if(capturingKeys)
        {
            ui->PushButton_Start ->setText("reset");
            ui->LineEdit_New->setPlaceholderText("Press start...");
            capturingKeys = false;

        } else if(!capturingKeys)
        {
            ui->PushButton_Start ->setText("...");
            ui->LineEdit_New->setPlaceholderText("");
            pressedKey = "";
            ui->LineEdit_New->clear();
            capturingKeys = true;
        }
    }
}

/**
 * @brief Processes key events to capture the user input for setting a new hotkey.
 *
 * @param event The key event to be processed.
 */

void ChangeHotkeyDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

    if(capturingKeys)
    {
        QString keyText = event->text();


        if (keyText.isEmpty())
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                pressedKey = "Ctrl";
            } else if (event->modifiers() & Qt::AltModifier)
            {
                pressedKey = "Alt";
            } else if (event->modifiers() & Qt::ShiftModifier)
            {
                pressedKey = "Shift";
            } else if (event->key() == Qt::Key_Space)
            {
                pressedKey = "Space";
            } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
            {
                pressedKey = "Enter";
            } else if(event->nativeModifiers())
            {
                switch (event->nativeScanCode())
                {
                case 0x3B:
                    pressedKey = "F1";
                    break;
                case 0x3C:
                    pressedKey = "F2";
                    break;
                case 0x3D:
                    pressedKey = "F3";
                    break;
                case 0x3E:
                    pressedKey = "F4";
                    break;
                case 0x3F:
                    pressedKey = "F5";
                    break;
                case 0x40:
                    pressedKey = "F6";
                    break;
                case 0x41:
                    pressedKey = "F7";
                    break;
                case 0x42:
                    pressedKey = "F8";
                    break;
                case 0x43:
                    pressedKey = "F9";
                    break;
                case 0x44:
                    pressedKey = "F10";
                    break;
                case 0x57:
                    pressedKey = "F11";
                    break;
                case 0x58:
                    pressedKey = "F12";
                    break;
                }
            }
        } else
        {
            if (!event->text().startsWith("\\"))
            {
                pressedKey = QKeySequence(event->key()).toString();
            }
        }

        ui->LineEdit_New->setText(pressedKey);
        ui->PushButton_Start ->setText("reset");
        on_PushButton_Start_clicked();
        QDialog::keyPressEvent(event);
    }
}

/**
 * @brief Displays a help message box providing guidance on changing hotkeys.
 */

void ChangeHotkeyDialog::on_pushButton_clicked()
{
    QString helpMessage = "<font color='black' face='Arial' size='4'>"
                          "1. 'CANCEL' button cancels the hotkey change.<br>"
                          "2. 'START' button initiates the recording process for a new hotkey.<br>"
                          "- When it displays '...', the program is waiting for input.<br>"
                          "- Pressing 'RESET', it will reset the hotkey and allow new input.<br>"
                          "3. 'ACCEPT' button changes the current hotkey to a new one.<br><br></font>";

    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(helpMessage);
    msgBox.setWindowTitle(tr("Help Message"));
    msgBox.exec();
}
