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

class V3dGenerator : public Okular::Generator {
    Q_OBJECT
    Q_INTERFACES(Okular::Generator)

public:
    V3dGenerator(QObject *parent, const QVariantList &args);
    ~V3dGenerator() override;

    void generatePixmap(Okular::PixmapRequest* request) override;
    bool loadDocument(const QString &fileName, QVector<Okular::Page*> &pages) override;
    bool doCloseDocument() override;

private:
    static int m_V3dGeneratorCount;

    HeadlessRenderer* m_HeadlessRenderer;

    std::unique_ptr<V3dFile> m_File{ nullptr };
};
