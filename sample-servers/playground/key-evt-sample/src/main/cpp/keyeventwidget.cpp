#include "keyeventwidget.h"

#include<iostream>
#include<QKeyEvent>

KeyEventWidget::KeyEventWidget(QWidget *parent, const char *name) {}

bool KeyEventWidget::eventFilter(QObject *obj, QEvent *ev) {

    if(ev->type() == QEvent::KeyPress) {
        QKeyEvent* kev = (QKeyEvent*)ev;
        std::cout << "key: " << kev->key() << std::endl;
    }

    return false;
}