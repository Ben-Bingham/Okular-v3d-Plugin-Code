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
    txtPage->append(fileName, new Okular::NormalizedRect( QRect(0.0, 0.2, 0.2, 0.2), 0.2, 0.2 ));
    page->setTextPage(txtPage);

    pagesVector.append(page);

    return true;
}

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

std::vector<float> parseContent(const std::string& content) {
    // std::vector<float> vertices{
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f, 0.5f, 0.0f
    // };

    // std::vector<float> vertices{
    //     0.199099f,
    //     0.257729f,
    //     -6.70361f,
    //     -0.414842f,
    //     -0.00357958f,
    //     -5.95876f,
    //     0.37451f,
    //     -0.20682f,
    //     -5.37944f
    // };

    std::vector<float> vertices{
        0.199099f,
        0.257729f,
        0.0f,
        -0.414842f,
        -0.00357958f,
        0.0f,
        0.37451f,
        -0.20682f,
        0.0f
    };

    // std::vector<std::string> lines = split(content, std::string{ '\n' });
    // std::cout << "Length: " << lines.size() << std::endl;

    // double version = std::stod(lines[0]);
    // lines.erase(lines.begin());

    // bool doublePrecision = false;
    // if (std::stoi(lines[0])) {
    //     doublePrecision = true;
    // }
    // lines.erase(lines.begin());

    // std::cout << "Length: " << lines.size() << std::endl;


    // for (auto it = lines.begin(); it != lines.end(); ++it) {
    //     std::cout << *it << std::endl;
    //     // switch (std::stoi(*it)) {
    //     // case 65:
    //         double v1x = std::stod(it[0]);
    //         double v1y = std::stod(it[1]);
    //         double v1z = std::stod(it[2]);

    //         double v2x = std::stod(it[3]);
    //         double v2y = std::stod(it[4]);
    //         double v2z = std::stod(it[5]);

    //         double v3x = std::stod(it[6]);
    //         double v3y = std::stod(it[7]);
    //         double v3z = std::stod(it[8]);

    //         vertices.push_back(v1x);
    //         vertices.push_back(v1y);
    //         vertices.push_back(v1z);

    //         vertices.push_back(v2x);
    //         vertices.push_back(v2y);
    //         vertices.push_back(v2z);

    //         vertices.push_back(v3x);
    //         vertices.push_back(v3y);
    //         vertices.push_back(v3z);

    //         // std::cout << std::stoi(it[9]) << std::endl;
    //         // std::cout << std::stoi(it[10]) << std::endl;
            
    //         // unsigned int centerIndex = (unsigned int)std::stoi(it[9]);
    //         // unsigned int materialIndex = (unsigned int)std::stoi(it[10]);

    //         for (int i = 0; i < 11; ++i) {
    //             it = lines.erase(lines.begin());
    //         }
    //         break;
    //     // }
    // }

    // std::cout << vertices.size() << std::endl;
    // for (auto val : vertices) {
    //     std::cout << val << std::endl;
    // }

    return vertices;
}

void V3dGenerator::generatePixmap(Okular::PixmapRequest* request) {
    std::string fileName = request->page()->text().toStdString();

    V3dFile file{fileName};

    std::vector<float> vertices = file.vertices;

    int width = request->width();
    int height = request->height();
    VkSubresourceLayout imageSubresourceLayout;

    unsigned char* imageData = m_HeadlessRenderer->render(width, height, &imageSubresourceLayout, vertices);

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
