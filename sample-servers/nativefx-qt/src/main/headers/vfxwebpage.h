#pragma once

#include <QEvent>
#include <QChildEvent>
#include <QPointer>
#include <QOpenGLWidget>
#include <QWebEngineView>
#include <QPaintEvent>
#include <QWebEngineSettings>


class VFXWebPage : public QWebEngineView
{
     Q_OBJECT

public:
    VFXWebPage();

    void set_redraw_callback(std::function<void ()> redraw_callback);

    void request_redraw(QImage* img);

    void sendSimulatedEvent(QEvent *evt);

    //@override
   bool event(QEvent * ev);

protected:

    // virtual void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent * event);

    bool eventFilter(QObject *obj, QEvent *ev);

    //@override
    virtual void childEvent(QChildEvent *event);

    private:
        QPointer<QOpenGLWidget> child_;
        QImage* image;
        bool dirty;
        std::function<void ()> redraw_callback;
        QWidget* eventsReciverWidget;

    signals:
        void delegatePaint(QPaintEvent*);
};

