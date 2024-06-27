#pragma once

#include <qobject.h>

class V3dGenerator;

struct Global {
    static inline bool firstMove = true;

    static inline float Zoom = 1.0f;
    static inline float lastZoom = Zoom;
};

class EventFilter : public QObject {
public:
    EventFilter(QObject* parent, V3dGenerator* generator);
    ~EventFilter() override = default;

    bool eventFilter(QObject *object, QEvent *event);

    V3dGenerator* generator;
};