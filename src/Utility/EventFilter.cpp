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
            if (generator->mouseDown) {
                generator->handleMouseMovement(mouseMove->globalPos().x(), mouseMove->globalPos().y());
            }

            auto elapsedTime = std::chrono::system_clock::now() - generator->startTime;

            auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

            if (generator->mouseDown) {
                if (elapsedTimeSeconds > generator->timeBetweenRefreshes) {
                    generator->refreshPixmap();
                    generator->startTime = std::chrono::system_clock::now();
                }
            }

            return true;
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mousePress = dynamic_cast<QMouseEvent*>(event);

        if ((mousePress->modifiers() & Qt::ControlModifier && mousePress->modifiers() & Qt::ShiftModifier) || (mousePress->modifiers() & Qt::AltModifier && mousePress->modifiers() & Qt::ShiftModifier)) {
            Global::mouseMode = MouseMode::NONE;
        } else if (mousePress->modifiers() & Qt::ShiftModifier) {
            Global::mouseMode = MouseMode::ZOOM;
        } else if (mousePress->modifiers() & Qt::ControlModifier || mousePress->modifiers() & Qt::AltModifier) {
            Global::mouseMode = MouseMode::PAN;
        } else {
            Global::mouseMode = MouseMode::ROTATE;
        }

        if (mousePress != nullptr) {
            if (generator->mouseDown == false) {
                generator->startTime = std::chrono::system_clock::now();
            }

            generator->mouseDown = true;
            return true;
        } 
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseRelease = dynamic_cast<QMouseEvent*>(event);

        Global::firstMove = true;

        if (mouseRelease != nullptr) {
            if (generator->mouseDown == true) {
                generator->refreshPixmap();
            }
            generator->mouseDown = false;
            return true;
        } 
    }

    return false;
}