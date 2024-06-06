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

#include "V3dFile/V3dFile.h"

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

bool V3dGenerator::loadDocument(const QString &fileName, QVector<Okular::Page *> &pagesVector) { // TODO load document here instead of generatePixmap
    Okular::Page* page = new Okular::Page(0, 1000, 1000, Okular::Rotation0);

    Okular::TextPage* txtPage = new Okular::TextPage{};
    txtPage->append(fileName, new Okular::NormalizedRect(QRect(0.0, 0.2, 0.2, 0.2), 0.2, 0.2 ));
    page->setTextPage(txtPage);

    pagesVector.append(page);

    return true;
}

void V3dGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::string fileName = request->page()->text().toStdString();

    V3dFile file{ fileName };

    std::vector<float> vertices = file.vertices;

    int width = request->width();
    int height = request->height();
    VkSubresourceLayout imageSubresourceLayout;

    std::vector<unsigned int> indices{};
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        indices.push_back(i);
    }

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices, indices);

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

    image = image.mirrored(false, true); // TODO reimplement flipping

    request->page()->setPixmap(request->observer(), new QPixmap(QPixmap::fromImage(image)));

    signalPixmapRequestDone(request);
}

int V3dGenerator::m_V3dGeneratorCount{ 0 };

#include "v3d_generator.moc"