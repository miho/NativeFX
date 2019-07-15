#pragma once

#include <QWidget>

class KeyEventWidget : public QWidget {
    Q_OBJECT

    public:
        KeyEventWidget( QWidget *parent = 0, const char *name = 0 );
        ~KeyEventWidget() {}
        bool eventFilter(QObject *obj, QEvent *ev);
};