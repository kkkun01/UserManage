#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QtSql/QSqlDatabase>
QString LoadQssFile(const QString &path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        file.close();
        qDebug()<<"file open sucess";
        return style;
    }else
    {
        qDebug()<<"file open file";
    }
    return "";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(LoadQssFile(":/qss/mystyle.qss"));//“a.setStyleSheet(LoadQssFile(":/qss/mystyle.qss"))”
    MainWindow w;
    w.show();

    return a.exec();
}
