#ifndef SETTINGEDITOR_H
#define SETTINGEDITOR_H

#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class SettingEditor;
}

class SettingEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SettingEditor(QWidget *parent = 0);
    ~SettingEditor();

private slots:
    void on_confirmBox_accepted();

    void on_languageBox_currentIndexChanged(int index);

private:
    Ui::SettingEditor *ui;
    bool initized = false;
};

#endif // SETTINGEDITOR_H
