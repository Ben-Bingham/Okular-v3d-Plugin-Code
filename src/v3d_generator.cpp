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

// #include <part/part.h>

OKULAR_EXPORT_PLUGIN(V3dGenerator, "libokularGenerator_v3d.json")

EventFilter::EventFilter(QObject* parent, V3dGenerator* generator) 
    : QObject(parent), generator(generator) { }

bool EventFilter::eventFilter(QObject *object, QEvent *event) {
    if (generator == nullptr) {
        return false;
    }

    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseMove = dynamic_cast<QMouseEvent*>(event);

        if (mouseMove != nullptr) {
            std::cout << "X: " << mouseMove->globalPos().x() << ", Y: " << mouseMove->globalPos().y() << std::endl;

            float value = (((float)mouseMove->globalPos().y() / 2.0f) / 250.0f);

            if (value > 1.0f) {
                value = 1.0f;
            }
            else if (value < 0.0f) {
                value = 0.0f;
            }

            std::cout << value << std::endl;

            generator->clearColor = { value, value, value, 1.0f };
            generator->refreshPixmap();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mousePress = dynamic_cast<QMouseEvent*>(event);

        if (mousePress != nullptr) {
            // std::cout << "Pressed the button: " << mousePress->button() << std::endl;
            // generator->refreshPixmap();
            return true;
        } 
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseRelease = dynamic_cast<QMouseEvent*>(event);

        if (mouseRelease != nullptr) {
            // std::cout << "Released the button: " << mouseRelease->button() << std::endl;
            return true;
        } 
    } else {
        // std::cout << event->type() << std::endl;
    }

    return false;
}

void error_callback(int error, const char* description) {
    std::cout << "GLFW Error[" << error << "]:" << description << std::endl;
}

V3dGenerator::V3dGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_V3dGeneratorCount == 0) {
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

        m_EventFilter = std::make_unique<EventFilter>(m_PageView, this);
        m_PageView->viewport()->installEventFilter(m_EventFilter.get());
    }

    m_V3dGeneratorCount++;
}

V3dGenerator::~V3dGenerator() {
    if (m_V3dGeneratorCount == 1) {
        delete m_HeadlessRenderer;

        m_EventFilter->generator = nullptr;
    }

    m_V3dGeneratorCount--;
}

bool V3dGenerator::doCloseDocument() {
    return true;
}

void V3dGenerator::refreshPixmap() {
    QWheelEvent* wheelEvent = new QWheelEvent(
        QPointF{},            // pos
        QPointF{},            // globalPos
        QPoint{},             // pixelDelta
        QPoint{ 1, 1 },   // angleDelta
        0,                    // buttons
        Qt::ControlModifier,  // modifiers
        Qt::NoScrollPhase,    // phase
        false                 // inverted
    );

    QMouseEvent* mouseEvent = new QMouseEvent(
        QEvent::MouseButtonRelease,         // type
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

bool ran = false;
void V3dGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::cout << "Generating pixmap" << std::endl;

    std::vector<float> vertices = m_File->vertices;
    std::vector<unsigned int> indices = m_File->indices;

    int width = request->width();
    int height = request->height();
    VkSubresourceLayout imageSubresourceLayout;

    glm::mat4 model = glm::mat4{ 1.0f };

	glm::mat4 view = glm::mat4{ 1.0f };
    view = glm::translate(view, { 0.0f, 0.0f, -10.0f });

	glm::mat4 projection = glm::perspective(m_File->headerInfo.angleOfView, (float)width / (float)height, 0.1f, 10000.0f);

	glm::mat4 mvp = projection * model * view;

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices, indices, mvp, clearColor);

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
