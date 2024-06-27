#include "EventFilter.h"
#include "v3d_generator.h"

#include <qcoreevent.h>
#include <qevent.h>

EventFilter::EventFilter(QObject* parent, V3dGenerator* generator) 
    : QObject(parent), generator(generator) { }

bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    if (generator == nullptr) {
        return false;
    }

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseMove = dynamic_cast<QMouseEvent*>(event);

        if (mouseMove != nullptr) {
            return generator->mouseMoveEvent(mouseMove);
        }

    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mousePress = dynamic_cast<QMouseEvent*>(event);

        if (mousePress != nullptr) {
            return generator->mouseButtonPressEvent(mousePress);
        }

    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseRelease = dynamic_cast<QMouseEvent*>(event);

        if (mouseRelease != nullptr) {
            return generator->mouseButtonReleaseEvent(mouseRelease);
        }
    }

    return false;
}