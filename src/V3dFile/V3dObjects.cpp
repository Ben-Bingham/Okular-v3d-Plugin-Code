#include "V3dObjects.h"

#include <iostream>

#include "V3dHeaderEntries.h"

V3dHeader::V3dHeader(xdr::ixstream& xdrFile) 
    : V3dObject{ ObjectTypes::HEADER } {
        xdrFile >> headerEntryCount;
        
        for (UINT i = 0; i < headerEntryCount; ++i) {
            entries.push_back(HeaderEntry{ xdrFile });
        }
    }

std::vector<float> V3dHeader::getVertices() {
    return std::vector<float>{};
}

V3dHeader::HeaderEntry::HeaderEntry(xdr::ixstream& xdrFile) {
    xdrFile >> headerKey;
    xdrFile >> dataLength;

    data.resize(dataLength);

    for (size_t i = 0; i < dataLength; ++i) {
        xdrFile >> data[i];
    }
}


V3dCenters::V3dCenters(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::CENTERS } { 
        xdrFile >> centersLength;

        centers.resize(centersLength);

        for (UINT i = 0; i < centersLength; ++i) {
            xdrFile >> centers[i].x;
            xdrFile >> centers[i].y;
            xdrFile >> centers[i].z;
        }
    }

std::vector<float> V3dCenters::getVertices() {
    return std::vector<float>{};
}


V3dMaterial::V3dMaterial(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::MATERIAL } { 
        xdrFile >> diffuse.r;
        xdrFile >> diffuse.g;
        xdrFile >> diffuse.b;
        xdrFile >> diffuse.a;

        xdrFile >> emissive.r;
        xdrFile >> emissive.g;
        xdrFile >> emissive.b;
        xdrFile >> emissive.a;

        xdrFile >> specular.r;
        xdrFile >> specular.g;
        xdrFile >> specular.b;
        xdrFile >> specular.a;

        xdrFile >> shininess;
        xdrFile >> metallic;
        xdrFile >> fresnel0;
    }

std::vector<float> V3dMaterial::getVertices() {
    return std::vector<float>{};
}


V3dBezierPatch::V3dBezierPatch(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_PATCH } { 
        std::cout << "ERROR: V3dBezierPatch cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dBezierPatch::getVertices() {
    std::cout << "ERROR: V3dBezierPatch cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierTriangle::V3dBezierTriangle(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE } { 
        std::cout << "ERROR: V3dBezierTriangle cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dBezierTriangle::getVertices() {
    std::cout << "ERROR: V3dBezierTriangle cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierPatchWithCornerColors::V3dBezierPatchWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_PATCH_COLOR } {
        std::cout << "ERROR: V3dBezierPatchWithCornerColors cannot currently be constructed" << std::endl;
     }

std::vector<float> V3dBezierPatchWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dBezierPatchWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierTriangleWithCornerColors::V3dBezierTriangleWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE_COLOR } { 
        std::cout << "ERROR: V3dBezierTriangleWithCornerColors cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dBezierTriangleWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dBezierTriangleWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightPlanarQuad::V3dStraightPlanarQuad(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::QUAD } {
        std::cout << "ERROR: V3dStraightPlanarQuad cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dStraightPlanarQuad::getVertices() {
    std::cout << "ERROR: V3dStraightPlanarQuad cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightTriangle::V3dStraightTriangle(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLE } { 
        xdrFile >> vertices[0].x;
        xdrFile >> vertices[0].y;
        xdrFile >> vertices[0].z;

        xdrFile >> vertices[1].x;
        xdrFile >> vertices[1].y;
        xdrFile >> vertices[1].z;

        xdrFile >> vertices[2].x;
        xdrFile >> vertices[2].y;
        xdrFile >> vertices[2].z;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;

    }

std::vector<float> V3dStraightTriangle::getVertices() {
    std::vector<float> out{};

    for (auto& ver : vertices) {
        out.push_back(static_cast<float>(ver.x));
        out.push_back(static_cast<float>(ver.y));
        out.push_back(0.0f);
    }

    return out;
}


V3dStraightPlanarQuadWithCornderColors::V3dStraightPlanarQuadWithCornderColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::QUAD_COLOR } { 
        std::cout << "ERROR: V3dStraightPlanarQuadWithCornderColors cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dStraightPlanarQuadWithCornderColors::getVertices() {
    std::cout << "ERROR: V3dStraightPlanarQuadWithCornderColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightTriangleWithCornerColors::V3dStraightTriangleWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLE_COLOR } { 
        std::cout << "ERROR: V3dStraightTriangleWithCornerColors cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dStraightTriangleWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dStraightTriangleWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dTriangleGroup::V3dTriangleGroup(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLES } { 
        std::cout << "ERROR: V3dTriangleGroup cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dTriangleGroup::getVertices() {
    std::cout << "ERROR: V3dTriangleGroup cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dSphere::V3dSphere(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::SPHERE } { 
        std::cout << "ERROR: V3dSphere cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dSphere::getVertices() {
    std::cout << "ERROR: V3dSphere cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dHemiSphere::V3dHemiSphere(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::HALF_SPHERE } { 
        std::cout << "ERROR: V3dHemiSphere cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dHemiSphere::getVertices() {
    std::cout << "ERROR: V3dHemiSphere cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dDisk::V3dDisk(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::DISK } { 
        std::cout << "ERROR: V3dDisk cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dDisk::getVertices() {
    std::cout << "ERROR: V3dDisk cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dCylinder::V3dCylinder(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::CYLINDER } { 
        std::cout << "ERROR: V3dCylinder cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dCylinder::getVertices() {
    std::cout << "ERROR: V3dCylinder cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dTube::V3dTube(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TUBE } { 
        std::cout << "ERROR: V3dTube cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dTube::getVertices() {
    std::cout << "ERROR: V3dTube cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierCurve::V3dBezierCurve(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::CURVE } { 
        std::cout << "ERROR: V3dBezierCurve cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dBezierCurve::getVertices() {
    std::cout << "ERROR: V3dBezierCurve cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dLineSegment::V3dLineSegment(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::LINE } { 
        std::cout << "ERROR: V3dLineSegment cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dLineSegment::getVertices() {
    std::cout << "ERROR: V3dLineSegment cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dPixel::V3dPixel(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::PIXEL } { 
        std::cout << "ERROR: V3dPixel cannot currently be constructed" << std::endl;
    }

std::vector<float> V3dPixel::getVertices() {
    std::cout << "ERROR: V3dPixel cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}
