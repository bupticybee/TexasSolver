#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QInputDialog>
#include <string>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList languages;
    languages << "English" << QString::fromLocal8Bit("简体中文");
    QString lang = QInputDialog::getItem(NULL,"select language","language",languages,0,false);

    QTranslator trans;
    if(lang == "English"){
        trans.load(":/lang_en.qm");
    }else if(lang == QString::fromLocal8Bit("简体中文")){
        trans.load(":/lang_cn.qm");
    }
    a.installTranslator(&trans);
    MainWindow w;
    w.show();

    return a.exec();
}
