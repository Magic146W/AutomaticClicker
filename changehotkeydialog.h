#ifndef CHANGEHOTKEYDIALOG_H
#define CHANGEHOTKEYDIALOG_H

#include "qlineedit.h"
#include <QDialog>

namespace Ui {
class ChangeHotkeyDialog;
}

class ChangeHotkeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeHotkeyDialog(QWidget *parent = nullptr);
    ~ChangeHotkeyDialog();

private slots:
    void on_PushButton_Cancel_clicked();
    void on_PushButton_Accept_clicked();
    void on_PushButton_Start_clicked();
    void on_pushButton_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::ChangeHotkeyDialog *ui;

    QLineEdit *lineEdit;
    QString pressedKey;
    bool capturingKeys;

signals:
    void updateHotkey(const QString& pressedKeys);
    void isDialogOpen(bool isOpen);
};

#endif // CHANGEHOTKEYDIALOG_H
