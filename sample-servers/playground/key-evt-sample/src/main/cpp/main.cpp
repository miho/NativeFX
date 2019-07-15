#include <QApplication>

#include <QPushButton>

#include "keyeventwidget.h"


int main(int argc, char *argv[])
{
    // create qapplication before touching any other qobject/class
    QApplication app(argc, argv, false);
    
    KeyEventWidget window;
    window.setFixedSize(120, 50);

    window.installEventFilter(&window);

    QPushButton *button = new QPushButton("Hello World", &window);
    button->setGeometry(10, 10, 100, 30);

    window.show();
 
    // qapplication does its magic...
    int exec_result = app.exec();

    return exec_result;
}