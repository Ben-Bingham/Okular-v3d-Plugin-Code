/*
    Orignal Source: txt plugin for Okular:
    SPDX-FileCopyrightText: 2013 Azat Khuzhin <a3at.mail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

    Code has been modified by Benjamin Bingham to become: v3d_generator
*/
#pragma once

#include <memory>

#include <core/textdocumentgenerator.h>

#include "renderheadless.h"
#include "V3dFile/V3dFile.h"

#include <thread>
#include "part/pageview.h"

#include <core/generator.h>

class Helper : public QAbstractScrollArea {
public:
    void callWheelEvent(QAbstractScrollArea* obj, QWheelEvent* event) {
        void (QAbstractScrollArea::*funcPtr)(QWheelEvent*) = &Helper::wheelEvent;

        (obj->*funcPtr)(event);
    }

    void callMouseReleaseEvent(QAbstractScrollArea* obj, QMouseEvent* event) {
        void (QAbstractScrollArea::*funcPtr)(QMouseEvent*) = &Helper::mouseReleaseEvent;

        (obj->*funcPtr)(event);
    }
};

class EventFilter;

class V3dGenerator : public Okular::Generator {
    Q_OBJECT
    Q_INTERFACES(Okular::Generator)

public:
    V3dGenerator(QObject *parent, const QVariantList &args);
    ~V3dGenerator() override;

    void generatePixmap(Okular::PixmapRequest* request) override;
    bool loadDocument(const QString &fileName, QVector<Okular::Page*> &pages) override;
    bool doCloseDocument() override;

    void refreshPixmap();
    glm::vec4 clearColor{ 1.0f, 0.0f, 0.0f, 1.0f };

private:
    static int m_V3dGeneratorCount;

    HeadlessRenderer* m_HeadlessRenderer;

    std::unique_ptr<V3dFile> m_File{ nullptr };

    std::unique_ptr<std::thread> m_UpdateThread{ nullptr };

    QAbstractScrollArea* m_PageView{ nullptr };

    std::unique_ptr<Helper> m_Helper{ nullptr };

    std::unique_ptr<EventFilter> m_EventFilter{ nullptr };
};

class EventFilter : public QObject {
public:
    EventFilter(QObject* parent, V3dGenerator* generator);

    bool eventFilter(QObject *object, QEvent *event);

    V3dGenerator* generator;
};