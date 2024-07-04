/*
    Orignal Source: txt plugin for Okular:
    SPDX-FileCopyrightText: 2013 Azat Khuzhin <a3at.mail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

    Code has been modified by Benjamin Bingham to become: v3d_generator
*/

#include <iostream>

#include "v3d_generator.h"
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <KAboutData>
#include <KConfigDialog>
#include <KLocalizedString>
#include <core/page.h>
#include <core/utils.h>
#include <vector>
#include <fstream>
#include <cstring>

#include <core/textdocumentgenerator.h>
#include <core/textdocumentgenerator_p.h>
#include <QApplication>
#include <QtWidgets>
#include <QPushButton>

#include <thread>
#include <chrono>

#include <shell/shell.h>

#include "part/pageview.h"

#include <cmath>

#include "Utility/EventFilter.h"

#include "Utility/ProtectedFunctionCaller.h"

#include "Utility/Arcball.h"
#include <glm/gtx/string_cast.hpp>

OKULAR_EXPORT_PLUGIN(V3dGenerator, "libokularGenerator_v3d.json")

V3dGenerator::V3dGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_V3dGeneratorCount == 0) {
        m_HeadlessRenderer = new HeadlessRenderer{ "/home/benjaminb/kde/src/okular/generators/Okular-v3d-Plugin-Code/shaders/" };

        m_PageView = getPageViewWidget();

        m_EventFilter = new EventFilter(m_PageView, this);
        m_PageView->viewport()->installEventFilter(m_EventFilter);

        // TODO deal with resizing window
        // TODO deal with these numbers not being entirly acurate, print out the normalized mouse coords to see,
        //      Everyting needs to be shifted
        m_PageViewDimensions.x = m_PageView->width();
        m_PageViewDimensions.y = m_PageView->height();
    }

    m_V3dGeneratorCount++;
}

V3dGenerator::~V3dGenerator() {
    if (m_V3dGeneratorCount == 1) {
        delete m_HeadlessRenderer;
    }

    m_V3dGeneratorCount--;
}

void V3dGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::vector<float> vertices = m_File->vertices;
    std::vector<unsigned int> indices = m_File->indices;

    int width = request->width();
    int height = request->height();
    VkSubresourceLayout imageSubresourceLayout;

    // Model
    glm::mat4 model = glm::mat4{ 1.0f };

    // Projection
    setProjection();

	glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * model;

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices, indices, mvp);

    unsigned char* imgDatatmp = imageData;

    size_t finalImageSize = width * height * 4;

    std::vector<unsigned char> vectorData;
    vectorData.reserve(finalImageSize);

    int x = 0;
    unsigned int* oldRow;
    bool done = false;
    for (int32_t y = 0; y < height; y++) {
        unsigned int *row = (unsigned int*)imgDatatmp;
        for (int32_t x = 0; x < width; x++) {
            unsigned char* charRow = (unsigned char*)row;
            vectorData.push_back(charRow[2]);
            vectorData.push_back(charRow[1]);
            vectorData.push_back(charRow[0]);
            vectorData.push_back(charRow[3]);

            row++;
        }
        imgDatatmp += imageSubresourceLayout.rowPitch;
    }

    QImage image{ vectorData.data(), request->width(), request->height(), QImage::Format_ARGB32 };

    image = image.mirrored(false, true);

    QPixmap* pixmap = new QPixmap(QPixmap::fromImage(image));
    request->page()->setPixmap(request->observer(), pixmap);

    signalPixmapRequestDone(request);

    delete imageData;
}

bool V3dGenerator::loadDocument(const QString &fileName, QVector<Okular::Page *> &pagesVector) {
    m_File = std::make_unique<V3dFile>(fileName.toStdString());

    initProjection();

    size_t width = m_File->headerInfo.canvasWidth;
    size_t height = m_File->headerInfo.canvasHeight;

    while (!(width > 600)) {
        width *= 2;
        height *= 2;
    }

    Okular::Page* page = new Okular::Page(0, width, height, Okular::Rotation0);

    pagesVector.append(page);

    return true;
}

bool V3dGenerator::doCloseDocument() {
    return true;
}

bool V3dGenerator::mouseMoveEvent(QMouseEvent* event) {
    m_MousePosition.x = event->globalPos().x();
    m_MousePosition.y = event->globalPos().y();

    if (m_MouseDown == false) {
        return true;
    }

    glm::vec2 normalizedMousePosition{ };
    glm::vec2 lastNormalizedMousePosition{ };

    glm::vec2 halfPageViewDimensions = m_PageViewDimensions / 2.0f;

    normalizedMousePosition.x = (float)(m_MousePosition.x - halfPageViewDimensions.x) / halfPageViewDimensions.x;
    normalizedMousePosition.y = (float)(m_MousePosition.y - halfPageViewDimensions.y) / halfPageViewDimensions.y;

    lastNormalizedMousePosition.x = (float)(m_LastMousePosition.x - halfPageViewDimensions.x) / halfPageViewDimensions.x;
    lastNormalizedMousePosition.y = (float)(m_LastMousePosition.y - halfPageViewDimensions.y) / halfPageViewDimensions.y;

    switch (m_DragMode) {
        case V3dGenerator::DragMode::SHIFT: {
            dragModeShift(normalizedMousePosition, lastNormalizedMousePosition);
            break;
        }
        case V3dGenerator::DragMode::ZOOM: {
            dragModeZoom(normalizedMousePosition, lastNormalizedMousePosition);
            break;
        }
        case V3dGenerator::DragMode::PAN: {
            dragModePan(normalizedMousePosition, lastNormalizedMousePosition);
            break;
        }
        case V3dGenerator::DragMode::ROTATE: {
            dragModeRotate(normalizedMousePosition, lastNormalizedMousePosition);
            break;
        }
    }

    m_LastMousePosition.x = m_MousePosition.x;
    m_LastMousePosition.y = m_MousePosition.y;

    setProjection();
    requestPixmapRefresh();

    return true;
}

bool V3dGenerator::mouseButtonPressEvent(QMouseEvent* event) {
    if (m_MouseDown != false) {
        return true;
    }

    m_LastMousePosition.x = m_MousePosition.x;
    m_LastMousePosition.y = m_MousePosition.y;

    m_MouseDown = true;

    bool controlKey = event->modifiers() & Qt::ControlModifier;
    bool shiftKey = event->modifiers() & Qt::ShiftModifier;
    bool altKey = event->modifiers() & Qt::AltModifier;

    if (controlKey && !shiftKey && !altKey) {
        m_DragMode = V3dGenerator::DragMode::SHIFT;
    } else if (!controlKey && shiftKey && !altKey) {
        m_DragMode = V3dGenerator::DragMode::ZOOM;
    } else if (!controlKey && !shiftKey && altKey) {
        m_DragMode = V3dGenerator::DragMode::PAN;
    } else {
        m_DragMode = V3dGenerator::DragMode::ROTATE;
    }

    return true;
}

bool V3dGenerator::mouseButtonReleaseEvent(QMouseEvent* event) {
    if (m_MouseDown != true) {
        return true;
    }

    m_MouseDown = false;

    return true;
}

void V3dGenerator::initProjection() {
    m_H = -std::tan(0.5f * m_File->headerInfo.angleOfView) * m_File->headerInfo.maxBound.z;

    m_Center.x = 0.0f;
    m_Center.y = 0.0f;

    m_Center.z = 0.5f * (m_File->headerInfo.minBound.z + m_File->headerInfo.maxBound.z);

    m_Zoom = m_File->headerInfo.initialZoom;
    m_LastZoom = m_File->headerInfo.initialZoom;

    m_ViewParam.minValues.z = m_File->headerInfo.minBound.z;
    m_ViewParam.maxValues.z = m_File->headerInfo.maxBound.z;

    m_Shift.x = 0.0f;
    m_Shift.y = 0.0f;
}

void V3dGenerator::setProjection() {
    setDimensions(m_PageViewDimensions.x, m_PageViewDimensions.y, m_Shift.x, m_Shift.y);

    m_ProjectionMatrix = glm::frustum(m_ViewParam.minValues.x, m_ViewParam.maxValues.x, m_ViewParam.minValues.y, m_ViewParam.maxValues.y, -m_ViewParam.maxValues.z, -m_ViewParam.minValues.z);
    
    updateViewMatrix();
}

void V3dGenerator::setDimensions(float width, float height, float X, float Y) {
    float Aspect = width / height;

    xShift = (X / width + m_File->headerInfo.viewportShift.x) * m_Zoom;
    yShift = (Y / height + m_File->headerInfo.viewportShift.y) * m_Zoom;

    float zoomInv = 1.0f / m_Zoom;

    float r = m_H * zoomInv;
    float rAspect = r * Aspect;

    float X0 = 2.0f * rAspect * xShift;
    float Y0 = 2 * r * yShift;

    m_ViewParam.minValues.x = -rAspect-X0;
    m_ViewParam.maxValues.x = rAspect-X0;
    m_ViewParam.minValues.y = -r - Y0;
    m_ViewParam.maxValues.y = r - Y0;
}

void V3dGenerator::updateViewMatrix() {
    glm::mat4 temp{ 1.0f };
    temp = glm::translate(temp, m_Center);
    glm::mat4 cjmatInv = glm::inverse(temp);

    m_ViewMatrix = m_RotationMatrix * cjmatInv;
    m_ViewMatrix = temp * m_ViewMatrix;

    m_ViewMatrix = glm::translate(m_ViewMatrix, { m_Center.x, m_Center.y, 0.0f });
}

void V3dGenerator::dragModeShift(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {

}

void V3dGenerator::dragModeZoom(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {
    float diff = lastNormalizedMousePosition.y - normalizedMousePosition.y;

    float stepPower = m_File->headerInfo.zoomStep * (m_PageViewDimensions.y / 2.0f) * diff;
    const float limit = std::log(0.1f * std::numeric_limits<float>::max()) / std::log(m_File->headerInfo.zoomFactor);

    if (std::abs(stepPower) < limit) {
        m_Zoom *= std::pow(m_File->headerInfo.zoomFactor, stepPower);

        float maxZoom = std::sqrt(std::numeric_limits<float>::max());
        float minZoom = 1 / maxZoom;

        if (m_Zoom <= minZoom) {
            m_Zoom = minZoom;
        } else if (m_Zoom >= maxZoom) {
            m_Zoom = maxZoom;
        }
    }
}

void V3dGenerator::dragModePan(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {

}

void V3dGenerator::dragModeRotate(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {
    float arcballFactor = 1.0f;

    if (normalizedMousePosition == lastNormalizedMousePosition) { return; }

    Arcball arcball{ { lastNormalizedMousePosition.x, -lastNormalizedMousePosition.y }, { normalizedMousePosition.x, -normalizedMousePosition.y} };
    float angle = arcball.angle;
    glm::vec3 axis = arcball.axis;

    float angleRadians = 2.0f * angle / m_Zoom * arcballFactor;
    glm::mat4 temp = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
    m_RotationMatrix = temp * m_RotationMatrix;
}

void V3dGenerator::refreshPixmap() {
    static bool shouldZoomIn = true;

    int zoom = 0;
    if (shouldZoomIn) {
        zoom = 1;
        shouldZoomIn = false;
    } else {
        zoom = -1;
        shouldZoomIn = true;
    }

    QWheelEvent* wheelEvent = new QWheelEvent(
        QPointF{},            // pos
        QPointF{},            // globalPos
        QPoint{},             // pixelDelta
        QPoint{ zoom, zoom }, // angleDelta
        0,                    // buttons
        Qt::ControlModifier,  // modifiers
        Qt::NoScrollPhase,    // phase
        false                 // inverted
    );

    QMouseEvent* mouseEvent = new QMouseEvent(
        QEvent::MouseButtonRelease,     // type
        QPointF{ },                     // localPos
        QPointF{ },                     // globalPos
        Qt::MiddleButton,               // button
        0,                              // buttons
        Qt::NoModifier                  // modifiers
    );

    ProtectedFunctionCaller::callWheelEvent(m_PageView, wheelEvent);
    ProtectedFunctionCaller::callMouseReleaseEvent(m_PageView, mouseEvent);
}

void V3dGenerator::requestPixmapRefresh() {
    auto elapsedTime = std::chrono::system_clock::now() - m_LastPixmapRefreshTime;

    auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

    if (elapsedTimeSeconds > m_MinTimeBetweenRefreshes) {
        refreshPixmap();
        m_LastPixmapRefreshTime = std::chrono::system_clock::now();
    }
}

int V3dGenerator::m_V3dGeneratorCount{ 0 };

QAbstractScrollArea* V3dGenerator::getPageViewWidget() {
    QAbstractScrollArea* pageView = nullptr;

    for (QWidget* widget : QApplication::allWidgets()) {
        bool hasScrollArea = false;
        bool parentIsWidget = false;
        bool has8Children = false;
        bool has1QVboxChild = false;
        bool has5QFrameChild = false;

        QAbstractScrollArea* scrollArea = dynamic_cast<QAbstractScrollArea*>(widget);

        if (scrollArea != nullptr) {
            hasScrollArea = true;
        } else {
            continue;
        }

        QWidget* parent = dynamic_cast<QWidget*>(widget->parent());

        if (parent != nullptr) {
            parentIsWidget = true;
        } else {
            continue;
        }

        if (parent->children().size() == 9) {
            has8Children = true;
        } else {
            continue;
        }

        int QBoxLayoutCount = 0;
        for (auto child : parent->children()) {
            QBoxLayout* qBox = dynamic_cast<QBoxLayout*>(child);

            if (qBox != nullptr) {
                QBoxLayoutCount += 1;
            }
        }

        if (QBoxLayoutCount == 1) {
            has1QVboxChild = true;
        } else {
            continue;
        }

        int QFrameCount = 0;
        for (auto child : parent->children()) {
            QFrame* qFrame = dynamic_cast<QFrame*>(child);

            if (qFrame != nullptr) {
                QFrameCount += 1;
            }
        }

        if (QFrameCount == 6) {
            has5QFrameChild = true;
        } else {
            continue;
        }

        if (hasScrollArea && parentIsWidget && has8Children && has1QVboxChild && has5QFrameChild) {
            if (pageView != nullptr) {
                std::cout << "ERROR, multiple pageViews found" << std::endl;
            }

            pageView = dynamic_cast<QAbstractScrollArea*>(widget);
        }
    }

    return pageView;
}

#include "v3d_generator.moc"
