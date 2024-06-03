#pragma once

#include "V3dObjects.h"

class V3dFile {
public:
    V3dFile(const std::string& fileName);

    UINT versionNumber;
    BOOL doublePrecisionFlag;

    std::vector<std::unique_ptr<V3dObject>> m_Objects;

    std::vector<float> vertices;
};