#include <QApplication>
#include "MainWindow.h"


int main(int argc, char** argv)
{   
    QApplication::setDesktopSettingsAware(true);

    QApplication app(argc, argv);

	MainWindow* mw = new MainWindow();
    mw->show();
    return app.exec();
}
