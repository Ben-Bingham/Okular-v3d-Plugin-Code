#pragma once

#include <vector>

#include "V3dTypes.h"

class V3dObject {
public:
    V3dObject(UINT objectType);
    virtual ~V3dObject() = default;

    virtual std::vector<float> getVertices() = 0;

    UINT objectType;
};