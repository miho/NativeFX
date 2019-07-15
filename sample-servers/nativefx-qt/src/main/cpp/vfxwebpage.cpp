#include <iostream>
#include <QApplication>

#include "vfxwebpage.h"


    VFXWebPage::VFXWebPage(): QWebEngineView(nullptr), child_(nullptr), image(nullptr), eventsReciverWidget(nullptr) {
    }

    void VFXWebPage::request_redraw(QImage* img) {
        this->image = img;
    }

    void VFXWebPage::set_redraw_callback(std::function<void ()> redraw_callback) {
        this->redraw_callback = redraw_callback;
    }

    void VFXWebPage::sendSimulatedEvent(QEvent *evt) {

        if(eventsReciverWidget==nullptr) {
            // Find the first child widget of QWebEngineView. It can accept user input events.
            for(QObject* obj : this->children())
            {
                QWidget* wgt = qobject_cast<QWidget*>(obj);
                if (wgt)
                {
                    eventsReciverWidget = wgt;
                    break;
                }
            }
        }

        QApplication::sendEvent(eventsReciverWidget, evt);
    }

    //@override
   bool VFXWebPage::event(QEvent * ev) {
        // based on https://stackoverflow.com/questions/30566101/how-can-i-get-paint-events-with-qtwebengine/33576854
        if (ev->type() == QEvent::ChildAdded) {
            QChildEvent *child_ev = static_cast<QChildEvent*>(ev);

            // there is also QObject child that should be ignored here;
            // use only QOpenGLWidget child
            QOpenGLWidget *w = qobject_cast<QOpenGLWidget*>(child_ev->child());
            if (w) {
                child_ = w;
                w->installEventFilter(this);
            }
        }

        return QWebEngineView::event(ev);
    }

    void VFXWebPage::paintEvent(QPaintEvent * event) {
        qDebug() << "paintEvent(" << event << ")";
        QWebEngineView::paintEvent(event);

        qDebug() << event->rect();
        qDebug() << event->region();
    }

    bool VFXWebPage::eventFilter(QObject *obj, QEvent *ev)
    {
        // emit delegatePaint on paint event of the last added QOpenGLWidget child
        if (obj == child_ && ev->type() == QEvent::Paint) {
            QPaintEvent *pe = static_cast<QPaintEvent*>(ev);
            // do something with paint event
            paintEvent(pe);
            // and emit signal to notify other objects
            emit delegatePaint(pe);
        }

        // qDebug() << "EVTYPE: " << ev->type();

        if (ev->type() == QEvent::UpdateRequest) {

                if(image!=nullptr) {
                    // qDebug() << "begin";
                    QPainter painter(image);
                    this->page()->view()->render(&painter);
                    painter.end();
                    // qDebug() << "end";
                } else {
                    qDebug() << "img null";
                } 

                redraw_callback();
        }

        return QWebEngineView::eventFilter(obj, ev);
    }

    //@override
    void VFXWebPage::childEvent(QChildEvent *event) {
         qDebug() << "childEvent(" << event << ")";
    }

