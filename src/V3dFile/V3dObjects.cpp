#include "V3dObjects.h"

V3dHeader::V3dHeader() 
    : V3dObject{ ObjectTypes::HEADER } { }

std::vector<float> V3dHeader::getVertices() {
    return std::vector<float>{};
}


V3dCenter::V3dCenter()
    : V3dObject{ ObjectTypes::CENTERS } { }

std::vector<float> V3dCenter::getVertices() {
    return std::vector<float>{};
}


V3dMaterial::V3dMaterial()
    : V3dObject{ ObjectTypes::MATERIAL } { }

std::vector<float> V3dMaterial::getVertices() {
    return std::vector<float>{};
}


V3dBezierPatch::V3dBezierPatch()
    : V3dObject{ ObjectTypes::BEZIER_PATCH } { }

std::vector<float> V3dBezierPatch::getVertices() {
    return std::vector<float>{};
}


V3dBezierTriangle::V3dBezierTriangle()
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE } { }

std::vector<float> V3dBezierTriangle::getVertices() {
    return std::vector<float>{};
}


V3dBezierPatchWithCornerColors::V3dBezierPatchWithCornerColors()
    : V3dObject{ ObjectTypes::BEZIER_PATCH_COLOR } { }

std::vector<float> V3dBezierPatchWithCornerColors::getVertices() {
    return std::vector<float>{};
}


V3dBezierTriangleWithCornerColors::V3dBezierTriangleWithCornerColors()
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE_COLOR } { }

std::vector<float> V3dBezierTriangleWithCornerColors::getVertices() {
    return std::vector<float>{};
}


V3dStraightPlanarQuad::V3dStraightPlanarQuad()
    : V3dObject{ ObjectTypes::QUAD } { }

std::vector<float> V3dStraightPlanarQuad::getVertices() {
    return std::vector<float>{};
}


V3dStraightTriangle::V3dStraightTriangle()
    : V3dObject{ ObjectTypes::TRIANGLE } { }

std::vector<float> V3dStraightTriangle::getVertices() {
    std::vector<float> out{};

    for (auto& ver : vertices) {
        out.push_back(static_cast<float>(ver.x));
        out.push_back(static_cast<float>(ver.y));
        out.push_back(0.0f);
    }

    return out;
}


V3dStraightPlanarQuadWithCornderColors::V3dStraightPlanarQuadWithCornderColors()
    : V3dObject{ ObjectTypes::QUAD_COLOR } { }

std::vector<float> V3dStraightPlanarQuadWithCornderColors::getVertices() {
    return std::vector<float>{};
}


V3dStraightTriangleWithCornderColors::V3dStraightTriangleWithCornderColors()
    : V3dObject{ ObjectTypes::TRIANGLE_COLOR } { }

std::vector<float> V3dStraightTriangleWithCornderColors::getVertices() {
    return std::vector<float>{};
}


V3dTriangleGroup::V3dTriangleGroup()
    : V3dObject{ ObjectTypes::TRIANGLES } { }

std::vector<float> V3dTriangleGroup::getVertices() {
    return std::vector<float>{};
}


V3dSphere::V3dSphere()
    : V3dObject{ ObjectTypes::SPHERE } { }

std::vector<float> V3dSphere::getVertices() {
    return std::vector<float>{};
}


V3dHemiSphere::V3dHemiSphere()
    : V3dObject{ ObjectTypes::HALF_SPHERE } { }

std::vector<float> V3dHemiSphere::getVertices() {
    return std::vector<float>{};
}


V3dDisk::V3dDisk()
    : V3dObject{ ObjectTypes::DISK } { }

std::vector<float> V3dDisk::getVertices() {
    return std::vector<float>{};
}


V3dCylinder::V3dCylinder()
    : V3dObject{ ObjectTypes::CYLINDER } { }

std::vector<float> V3dCylinder::getVertices() {
    return std::vector<float>{};
}


V3dTube::V3dTube()
    : V3dObject{ ObjectTypes::TUBE } { }

std::vector<float> V3dTube::getVertices() {
    return std::vector<float>{};
}


V3dBezierCurve::V3dBezierCurve()
    : V3dObject{ ObjectTypes::CURVE } { }

std::vector<float> V3dBezierCurve::getVertices() {
    return std::vector<float>{};
}


V3dLineSegment::V3dLineSegment()
    : V3dObject{ ObjectTypes::LINE } { }

std::vector<float> V3dLineSegment::getVertices() {
    return std::vector<float>{};
}


V3dPixel::V3dPixel()
    : V3dObject{ ObjectTypes::PIXEL } { }

std::vector<float> V3dPixel::getVertices() {
    return std::vector<float>{};
}
