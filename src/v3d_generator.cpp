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

OKULAR_EXPORT_PLUGIN(V3dGenerator, "libokularGenerator_v3d.json")

void error_callback(int error, const char* description) {
    std::cout << "GLFW Error[" << error << "]:" << description << std::endl;
}

V3dGenerator::V3dGenerator(QObject *parent, const QVariantList &args) {
    Q_UNUSED(parent);
    Q_UNUSED(args);

    if (m_V3dGeneratorCount == 0) {
        m_HeadlessRenderer = new HeadlessRenderer{ "/home/benjaminb/kde/src/okular/generators/Okular-v3d-Plugin-Code/shaders/" };
    }

    m_V3dGeneratorCount++;

    int i = 0;
    for (QWidget* widget : QApplication::allWidgets()) {
        if (widget->inherits("QMainWindow")) {
            // ++i;
            // if (stackedWidget != nullptr) {
            //     // ++i;
            //     // std::cout << "Widget has: " << stackedWidget->count() << " Children" << std::endl;
            // }

            // This is the shell
            QMainWindow* mainWindowWidget = dynamic_cast<QMainWindow*>(widget);

            // Shell* shellWidget = dynamic_cast<Shell*>(widget);
            // Shell* shell = qobject_cast<Shell*>(widget);
            // if (shell != nullptr) {
            //     std::cout << "FOUND SHELL WIDGET" << std::endl;
            // }

            if (mainWindowWidget != nullptr) {
                std::cout << "FOUND mainWindowWiget" << std::endl;
                // mainWindowWidget->widget();

                std::cout << "Found : " << mainWindowWidget->children().size() <<  " Children" << std::endl;
                for (auto obj : mainWindowWidget->children()) {
                    // if (obj->inherits("")) {

                    // }

                    Part* part = qobject_cast<Part*>(obj);

                    if (part != nullptr) {
                        i++;
                    }
                }



                

                // QWidget* centralWidget = mainWindowWiget->centralWidget();

                // QStackedWidget* stackedWidget = dynamic_cast<QStackedWidget*>(centralWidget);
                // if (stackedWidget != nullptr) {
                //     std::cout << "FOUND CENTRAL WIDGEt" << std::endl;
                //     // ++i;
                //     std::cout << "Widget has: " << stackedWidget->count() << " Children" << std::endl;
                //     // stackedWidget->addWidget(this);
                }
            // }


        }

        // QStackedWidget* stackedWidget = dynamic_cast<QStackedWidget*>(widget);
        // if (stackedWidget != nullptr) {
        //     ++i;
        //     std::cout << "Widget has: " << stackedWidget->count() << " Children" << std::endl;
        // }
    }

    std::cout << "FOUND : " << i << "WIDGETS" << std::endl;
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

void V3dGenerator::rotationChanged(Okular::Rotation orientation, Okular::Rotation oldOrientation) {
    std::cout << "Rotation Changed ffffffffffffffffffffffffffffffffffffffffffffffffffffff" << std::endl;
}


bool V3dGenerator::event(QEvent* e) {
    std::cout << "GOt an eventffffffffffaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaffffffff" << std::endl;
    if (e->type() == QEvent::MouseButtonPress) {
        std::cout << "BuToNon PreSe sfjsd;lkdfj" << std::endl;
        return true;
    }

    return Okular::Generator::event(e);
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

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices, indices, mvp);

    auto imgDatatmp = imageData;

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

    // for (int i = 0; i < 5; ++i) {
    signalPixmapRequestDone(request);

    // using namespace std::this_thread; // sleep_for, sleep_until
    // using namespace std::chrono; // nanoseconds, system_clock, seconds

    // sleep_for(5s);
    // std::cout << "Done sleeping" << std::endl;

    // std::thread t1{[](){
    //     std::cout << "On a new thread" << std::endl;
    // }};

    // t1.join();


                // signalPixmapRequestDone(request);
    // }
    
    // if (!ran) {
    //     generatePixmap(request);
    //     ran = true;
    // }
}

int V3dGenerator::m_V3dGeneratorCount{ 0 };

#include "v3d_generator.moc"
