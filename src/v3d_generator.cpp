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

OKULAR_EXPORT_PLUGIN(V3dGenerator, "libokularGenerator_v3d.json")

V3dGenerator::V3dGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_V3dGeneratorCount == 0) {
        Global::firstMove = true;

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

    glm::mat4 model = glm::mat4{ 1.0f };

    glm::mat4 Temp{ 1.0f };

    glm::vec3 center{ 0.0f };
    center.z = 0.5 * (m_File->headerInfo.minBound.z + m_File->headerInfo.maxBound.z);
    Temp = glm::translate(Temp, center);
    glm::mat4 cjmatInv = glm::inverse(Temp);

    m_ViewMatrix = m_RotationMatrix * cjmatInv;
    m_ViewMatrix = Temp * m_ViewMatrix;

    float Xfactor = 1.0f;
    float Yfactor = 1.0f;

    float X = 0.0f;
    float Y = 0.0f;

    float Width = m_PageView->width();
    float Height = m_PageView->height();

    float Aspect = (float)Width / (float)Height;
    float xshift = (X / (float)Width + m_File->headerInfo.viewportShift.x * Xfactor) * Global::Zoom;
    float yshift = (Y / (float)Height + m_File->headerInfo.viewportShift.y * Yfactor) * Global::Zoom;
    float Zoominv = 1.0f / Global::Zoom;

    float zMin = m_File->headerInfo.minBound.z;
    float zMax = m_File->headerInfo.maxBound.z;

    float H = -std::tan(0.5f * m_File->headerInfo.angleOfView) * zMax; 

    float r = H * Zoominv;
    float rAspect = r * Aspect;
    float X0 = 2.0f * rAspect * xshift;
    float Y0 = 2.0f * r * yshift;
    float xMin = -rAspect - X0;
    float xMax = rAspect - X0;
    float yMin = -r - Y0;
    float yMax = r - Y0;
    // TODO issue with loading in when using this projection function
    // glm::mat4 projection = glm::frustum(xMin, xMax, yMin, yMax, -zMax, -zMin);
	glm::mat4 projection = glm::perspective(m_File->headerInfo.angleOfView, (float)width / (float)height, 0.1f, 10000.0f);

	glm::mat4 mvp = projection * model * m_ViewMatrix;

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

// int halfCanvasWidth = 0;
// int halfCanvasHeight = 0;

// int Width = 0;
// int Height = 0;

// int lastMouseX = halfCanvasWidth;
// int lastMouseY = halfCanvasHeight;

struct Arcball {
    float angle;
    glm::vec3 axis;

    Arcball(const glm::vec2& lastMousePosition, const glm::vec2& mousePosition) {
        glm::vec3 v0 = norm(lastMousePosition);
        glm::vec3 v1 = norm(mousePosition);
        float Dot = glm::dot(v0, v1);
        angle = Dot > 1.0f ? 0.0f : (Dot < -1.0f ? glm::pi<float>() : std::acos(Dot));

        axis = glm::normalize(glm::cross(v0, v1));
    }

    glm::vec3 norm(glm::vec2 v) {
        float norm = std::hypot(v.x, v.y);
        if (norm > 1.0) {
            float denom = 1.0f/norm;
            v.x *= denom;
            v.y *= denom;
        }
        return glm::vec3{ v.x, v.y, std::sqrt(std::max(1.0f - v.x * v.x - v.y * v.y, 0.0f))};
    }
};

float ArcballFactor = 1.0f;

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

void V3dGenerator::dragModeShift(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {

}

void V3dGenerator::dragModeZoom(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {

}

void V3dGenerator::dragModePan(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {

}

void V3dGenerator::dragModeRotate(const glm::vec2& normalizedMousePosition, const glm::vec2& lastNormalizedMousePosition) {
    int factor = 1;

    if (normalizedMousePosition == lastNormalizedMousePosition) { return; }

    Arcball arcball{ { lastNormalizedMousePosition.x, -lastNormalizedMousePosition.y }, { normalizedMousePosition.x, -normalizedMousePosition.y} };
    float angle = arcball.angle;
    glm::vec3 axis = arcball.axis;

    float angleRadians = 2.0f * angle / Global::Zoom * ArcballFactor;
    glm::mat4 temp = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
    m_RotationMatrix = temp * m_RotationMatrix;
}

void V3dGenerator::handleMouseMovement(int mouseXPosition, int mouseYPosition) {
    // if (Global::firstMove) {
    //     lastMouseX = mouseXPosition;
    //     lastMouseY = mouseYPosition;

    //     Global::firstMove = false;
    // }

    // float lastX = (float)(lastMouseX -     halfCanvasWidth)  / (float)halfCanvasWidth;
    // float lastY = (float)(lastMouseY -     halfCanvasHeight) / (float)halfCanvasHeight;
    // float rawX  = (float)(mouseXPosition - halfCanvasWidth)  / (float)halfCanvasWidth;
    // float rawY  = (float)(mouseYPosition - halfCanvasHeight) / (float)halfCanvasHeight;

    // switch(Global::mouseMode) {
    //     case MouseMode::NONE: {
    //         break;
    //     }

    //     case MouseMode::ROTATE: {
    //         int factor = 1;

    //         if (!(lastX == rawX && lastY == rawY)) {
    //             Arcball arcball{(float)lastX, (float)-lastY, (float)rawX, (float)-rawY};
    //             float angle = arcball.angle;
    //             glm::vec3 axis = arcball.axis;
    //             float angleRadians = 2.0f * angle / Global::Zoom * ArcballFactor;
    //             glm::mat4 Temp = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
    //             rotMat = Temp * rotMat;
    //         } 
    //         break;
    //     }

    //     case MouseMode::PAN: {
    //         std::cout << "Panning" << std::endl;
    //         break;
    //     }

    //     case MouseMode::ZOOM: {
    //         float diff = lastY - rawY;
    //         float stepPower = m_File->headerInfo.zoomStep * halfCanvasHeight * diff;
    //         const float limit = std::log(0.1*std::numeric_limits<float>::max()) / std::log(m_File->headerInfo.zoomFactor);

    //         if (std::abs(stepPower) < limit) {
    //             Global::Zoom *= std::pow(m_File->headerInfo.zoomFactor, stepPower);
    //             float maxZoom = std::sqrt(std::numeric_limits<float>::max());
    //             float minZoom = 1 / maxZoom;
    //             if (Global::Zoom <= minZoom) {
    //                 Global::Zoom = minZoom;
    //             } else if (Global::Zoom >= maxZoom) {
    //                 Global::Zoom = maxZoom;
    //             }
    //         }
    //         break;
    //     }
    // }

    // lastMouseX = mouseXPosition;
    // lastMouseY = mouseYPosition;
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

void error_callback(int error, const char* description) {
    std::cout << "GLFW Error[" << error << "]:" << description << std::endl;
}

int V3dGenerator::m_V3dGeneratorCount{ 0 };

QAbstractScrollArea* V3dGenerator::getPageViewWidget() {
    QAbstractScrollArea* pageView;

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
