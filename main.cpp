#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QPluginLoader>
#include "mainwindow.h"

int main(int argc,char *argv[])
{
    Q_INIT_RESOURCE(resource);

    QApplication app(argc,argv);

    app.setApplicationName(QString("SqliteGUI"));
    app.setApplicationVersion(QString("v0.04"));
    app.setOrganizationName(QString("ivansoft"));

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    QApplication::addLibraryPath(QString("./plugins"));
    QPluginLoader loader(QString("./plugins/qsqlite4.dll"));
    loader.load();

    MainWindow mainwin;
    mainwin.showMaximized();

    QApplication::setWindowIcon(QIcon(":/images/app.png"));

    return app.exec();
}
