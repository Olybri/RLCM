#include <QLocale>
#include "MainFrame.hpp"

int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    QApplication app(argc, argv);

    MainFrame frame(argv[0]);
    frame.show();

    return app.exec();
}

