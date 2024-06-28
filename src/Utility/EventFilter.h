#pragma once

#include <qobject.h>

class V3dGenerator;

class EventFilter : public QObject {
public:
    EventFilter(QObject* parent, V3dGenerator* generator);
    ~EventFilter() override = default;

    bool eventFilter(QObject *object, QEvent *event);

    V3dGenerator* generator;
};