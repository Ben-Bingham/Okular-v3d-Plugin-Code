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

    bool mouseMoveEvent(QMouseEvent* event);
    bool mouseButtonPressEvent(QMouseEvent* event);
    bool mouseButtonReleaseEvent(QMouseEvent* event);

private:
    bool m_MouseDown{ false };

    enum class DragMode {
        SHIFT,
        ZOOM,
        PAN,
        ROTATE
    };
    DragMode m_DragMode{ DragMode::ROTATE };

    glm::ivec2 m_MousePosition;
    glm::ivec2 m_LastMousePosition;

    float m_Zoom = 1.0f;
    float m_LastZoom{ };

    float xShift;
    float yShift;

    glm::vec2 m_PageViewDimensions;

    glm::mat4 m_RotationMatrix{ 1.0f };
    glm::mat4 m_ViewMatrix{ 1.0f };
    glm::mat4 m_ProjectionMatrix{ 1.0f };

    float m_H{ };
    glm::vec3 m_Center{ };
    glm::vec2 m_Shift{ };

    struct ViewParam {
        glm::vec3 minValues{ };
        glm::vec3 maxValues{ };
    };

    ViewParam m_ViewParam;

    void initProjection();
    void setProjection();

    void setDimensions(float width, float height, float X, float Y);
    void updateViewMatrix();

    void dragModeShift  (const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition);
    void dragModeZoom   (const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition);
    void dragModePan    (const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition);
    void dragModeRotate (const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition);

    void requestPixmapRefresh();

    void refreshPixmap();

    QAbstractScrollArea* getPageViewWidget();

    static int m_V3dGeneratorCount;

    std::unique_ptr<V3dFile> m_File{ nullptr };

    std::chrono::duration<double> m_MinTimeBetweenRefreshes{ 1.0 / 100.0 }; // In Seconds
    std::chrono::time_point<std::chrono::system_clock> m_LastPixmapRefreshTime;

    HeadlessRenderer* m_HeadlessRenderer;

    QAbstractScrollArea* m_PageView{ nullptr };

    EventFilter* m_EventFilter{ nullptr };
};