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

OKULAR_EXPORT_PLUGIN(V3dGenerator, "libokularGenerator_v3d.json")

int halfCanvasWidth = 0;
int halfCanvasHeight = 0;

// int Width = 0;
// int Height = 0;

bool firstMove = true;

EventFilter::EventFilter(QObject* parent, V3dGenerator* generator) 
    : QObject(parent), generator(generator) { }

enum class MouseMode {
    ROTATE,
    ZOOM,
    PAN,
    NONE
};
MouseMode mouseMode;

float Zoom = 1.0f;
float lastZoom = Zoom;

bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    // if (!generator->haveTakenInitialPause) {
    //     auto elapsedTime = std::chrono::system_clock::now() - generator->startTime;

    //     auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

    //     // std::cout << elapsedTimeSeconds.count() << std::endl;

    //     if (elapsedTimeSeconds >= generator->initialPause) {
    //         generator->haveTakenInitialPause = true;
    //     } else {
    //         return false;
    //     }
    // }
    // std::cout << "Event" << std::endl;

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
            mouseMode = MouseMode::NONE;
        } else if (mousePress->modifiers() & Qt::ShiftModifier) {
            mouseMode = MouseMode::ZOOM;
        } else if (mousePress->modifiers() & Qt::ControlModifier || mousePress->modifiers() & Qt::AltModifier) {
            mouseMode = MouseMode::PAN;
        } else {
            mouseMode = MouseMode::ROTATE;
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

        firstMove = true;

        if (mouseRelease != nullptr) {
            if (generator->mouseDown == true) {
                generator->refreshPixmap();
            }
            generator->mouseDown = false;
            return true;
        } 
    } 
    
    // else if (event->type() == QEvent::Wheel) {
    //     QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);



    //     // if (lastZoom != Zoom) {
    //     //     auto elapsedTime = std::chrono::system_clock::now() - generator->startTime;

    //     //     auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

    //     //     if (elapsedTimeSeconds > generator->timeBetweenRefreshes) {
    //     //         generator->refreshPixmap();
    //     //         generator->startTime = std::chrono::system_clock::now();
    //     //     }

    //     //     lastZoom = Zoom;
    //     // }
        
    //     if ((wheelEvent->angleDelta().x() == 1 && wheelEvent->angleDelta().y() == 1) ||
    //         (wheelEvent->angleDelta().x() == -1 && wheelEvent->angleDelta().y() == -1)) {
    //         return false;
    //     } else {
    //         if (wheelEvent->angleDelta().y() > 0) {
    //             Zoom *= generator->m_File->headerInfo.zoomFactor;
    //         } else {
    //             Zoom /= generator->m_File->headerInfo.zoomFactor;
    //         }

    //         // return true;
    //     }
    // }

    return false;
}

int lastMouseX = halfCanvasWidth;
int lastMouseY = halfCanvasHeight;

struct Arcball {
    float angle;
    glm::vec3 axis;

    Arcball(float oldMouseX, float oldMouseY, float newMouseX, float newMouseY) {
        glm::vec3 v0 = norm(oldMouseX, oldMouseY);
        glm::vec3 v1 = norm(newMouseX, newMouseY);
        float Dot = glm::dot(v0, v1);
        angle = Dot > 1.0f ? 0.0f : (Dot < -1.0f ? glm::pi<float>() : std::acos(Dot));

        axis = glm::normalize(glm::cross(v0, v1));
    }

    glm::vec3 norm(float x, float y) {
        float norm = std::hypot(x, y);
        if (norm > 1.0) {
            float denom = 1.0f/norm;
            x *= denom;
            y *= denom;
        }
        return glm::vec3{ x, y, std::sqrt(std::max(1.0f - x * x - y * y, 0.0f))};
    }
};

glm::mat4 rotMat{ 1.0f };
glm::mat4 viewMat{ 1.0f };

float ArcballFactor = 1.0f;

void V3dGenerator::handleMouseMovement(int mouseXPosition, int mouseYPosition) {
    if (firstMove) {
        lastMouseX = mouseXPosition;
        lastMouseY = mouseYPosition;

        firstMove = false;
    }

    float lastX = (float)(lastMouseX -     halfCanvasWidth)  / (float)halfCanvasWidth;
    float lastY = (float)(lastMouseY -     halfCanvasHeight) / (float)halfCanvasHeight;
    float rawX  = (float)(mouseXPosition - halfCanvasWidth)  / (float)halfCanvasWidth;
    float rawY  = (float)(mouseYPosition - halfCanvasHeight) / (float)halfCanvasHeight;

    switch(mouseMode) {
        case MouseMode::NONE: {
            break;
        }

        case MouseMode::ROTATE: {
            int factor = 1;

            if (!(lastX == rawX && lastY == rawY)) {
                Arcball arcball{(float)lastX, (float)-lastY, (float)rawX, (float)-rawY};
                float angle = arcball.angle;
                glm::vec3 axis = arcball.axis;

                // rotMat = glm::rotate<double>(glm::dvec3(axis.x, axis.y, axis.z), (double)(2 * arcball.angle / Zoom * ArcballFactor)) * rotMat;
                // float angleRadians = 2.0f * factor * ArcballFactor * angle / Zoom;
                float angleRadians = 2.0f * angle / Zoom * ArcballFactor;
                glm::mat4 Temp = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
                rotMat = Temp * rotMat;
            } 
            break;
        }

        case MouseMode::PAN: {
            std::cout << "Panning" << std::endl;
            break;
        }

        case MouseMode::ZOOM: {
            float diff = lastY - rawY;
            float stepPower = m_File->headerInfo.zoomStep * halfCanvasHeight * diff;
            const float limit = std::log(0.1*std::numeric_limits<float>::max()) / std::log(m_File->headerInfo.zoomFactor);

            if (std::abs(stepPower) < limit) {
                Zoom *= std::pow(m_File->headerInfo.zoomFactor, stepPower);
                float maxZoom = std::sqrt(std::numeric_limits<float>::max());
                float minZoom = 1 / maxZoom;
                if (Zoom <= minZoom) {
                    Zoom = minZoom;
                } else if (Zoom >= maxZoom) {
                    Zoom = maxZoom;
                }
            }
            break;
        }
    }

    lastMouseX = mouseXPosition;
    lastMouseY = mouseYPosition;
}

void error_callback(int error, const char* description) {
    std::cout << "GLFW Error[" << error << "]:" << description << std::endl;
}

V3dGenerator::V3dGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_V3dGeneratorCount == 0) {
        viewMat = glm::mat4{ 1.0f };
        rotMat = glm::mat4{ 1.0f };

        halfCanvasWidth = 0;
        halfCanvasHeight = 0;

        // Width = 0;
        // Height = 0;

        firstMove = true;

        ArcballFactor = 1.0f;

        m_HeadlessRenderer = new HeadlessRenderer{ "/home/benjaminb/kde/src/okular/generators/Okular-v3d-Plugin-Code/shaders/" };

        int i = 0;
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
                i++;

                if (m_PageView != nullptr) {
                    std::cout << "ERROR, multiple pageViews found" << std::endl;
                }
                m_PageView = dynamic_cast<QAbstractScrollArea*>(widget);
            }
        }

        m_EventFilter = new EventFilter(m_PageView, this);
        m_PageView->viewport()->installEventFilter(m_EventFilter);

        halfCanvasWidth = m_PageView->width() / 2;
        halfCanvasHeight = m_PageView->height() / 2;

        // Width = m_PageView->width();
        // Height = m_PageView->height();

        lastMouseX = halfCanvasWidth;
        lastMouseY = halfCanvasHeight;
    }

    m_V3dGeneratorCount++;
}

V3dGenerator::~V3dGenerator() {
    if (m_V3dGeneratorCount == 1) {
        delete m_HeadlessRenderer;
    }

    m_V3dGeneratorCount--;
}

bool V3dGenerator::doCloseDocument() {
    return true;
}

void V3dGenerator::refreshPixmap() {
    int zoom = 0;
    if (m_ZoomIn) {
        zoom = 1;
        m_ZoomIn = false;
    } else {
        zoom = -1;
        m_ZoomIn = true;
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

    m_Helper->callWheelEvent(m_PageView, wheelEvent);
    m_Helper->callMouseReleaseEvent(m_PageView, mouseEvent);
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

    viewMat = rotMat * cjmatInv;
    viewMat = Temp * viewMat;

    float Xfactor = 1.0f;
    float Yfactor = 1.0f;

    float X = 0.0f;
    float Y = 0.0f;

    float Width = m_PageView->width();
    float Height = m_PageView->height();

    float Aspect = (float)Width / (float)Height;
    float xshift = (X / (float)Width + m_File->headerInfo.viewportShift.x * Xfactor) * Zoom;
    float yshift = (Y / (float)Height + m_File->headerInfo.viewportShift.y * Yfactor) * Zoom;
    float Zoominv = 1.0f / Zoom;

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

	glm::mat4 mvp = projection * model * viewMat;

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

int V3dGenerator::m_V3dGeneratorCount{ 0 };

#include "v3d_generator.moc"
