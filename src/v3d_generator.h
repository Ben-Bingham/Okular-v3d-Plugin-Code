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

#include <chrono>

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

    std::chrono::duration<double> timeBetweenRefreshes{ 1.0 / 100.0 }; // In Seconds

    std::chrono::duration<double> initialPause{ 3.0 }; // In Seconds
    bool haveTakenInitialPause = false;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    bool mouseDown{ false };

    void handleMouseMovement(int mouseXPosition, int mouseYPosition);

    std::unique_ptr<V3dFile> m_File{ nullptr }; // TODO

private:
    static int m_V3dGeneratorCount;

    HeadlessRenderer* m_HeadlessRenderer;


    std::unique_ptr<std::thread> m_UpdateThread{ nullptr };

    QAbstractScrollArea* m_PageView{ nullptr };

    EventFilter* m_EventFilter{ nullptr };

    bool m_ZoomIn{ true };
};

class EventFilter : public QObject {
public:
    EventFilter(QObject* parent, V3dGenerator* generator);
    ~EventFilter() override = default;
    bool eventFilter(QObject *object, QEvent *event);

    V3dGenerator* generator;
};