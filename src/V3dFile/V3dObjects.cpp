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

V3dBezierPatch::V3dBezierPatch(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_PATCH } { 
        for (int i = 0; i < 16; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dBezierPatch::getVertices() {
    std::cout << "ERROR: V3dBezierPatch cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierTriangle::V3dBezierTriangle(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE } { 
        for (int i = 0; i < 10; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dBezierTriangle::getVertices() {
    std::cout << "ERROR: V3dBezierTriangle cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierPatchWithCornerColors::V3dBezierPatchWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_PATCH_COLOR } {
        for (int i = 0; i < 16; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    

        for (int i = 0; i < 4; ++i) {
            xdrFile >> cornerColors[i].r;
            xdrFile >> cornerColors[i].g;
            xdrFile >> cornerColors[i].b;
            xdrFile >> cornerColors[i].a;
        }
    }

std::vector<float> V3dBezierPatchWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dBezierPatchWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierTriangleWithCornerColors::V3dBezierTriangleWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::BEZIER_TRIANGLE_COLOR } { 
        for (int i = 0; i < 10; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    

        for (int i = 0; i < 3; ++i) {
            xdrFile >> cornerColors[i].r;
            xdrFile >> cornerColors[i].g;
            xdrFile >> cornerColors[i].b;
            xdrFile >> cornerColors[i].a;
        }    
    }

std::vector<float> V3dBezierTriangleWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dBezierTriangleWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightPlanarQuad::V3dStraightPlanarQuad(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::QUAD } {
        for (int i = 0; i < 4; ++i) {
            xdrFile >> vertices[i].x;
            xdrFile >> vertices[i].y;
            xdrFile >> vertices[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex; 
    }

std::vector<float> V3dStraightPlanarQuad::getVertices() {
    std::cout << "ERROR: V3dStraightPlanarQuad cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightTriangle::V3dStraightTriangle(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLE } { 
        for (int i = 0; i < 3; ++i) {
            xdrFile >> vertices[i].x;
            xdrFile >> vertices[i].y;
            xdrFile >> vertices[i].z;
        }

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
        for (int i = 0; i < 4; ++i) {
            xdrFile >> vertices[i].x;
            xdrFile >> vertices[i].y;
            xdrFile >> vertices[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;     

        for (int i = 0; i < 4; ++i) {
            xdrFile >> cornerColors[i].r;
            xdrFile >> cornerColors[i].g;
            xdrFile >> cornerColors[i].b;
            xdrFile >> cornerColors[i].a;
        }
    }

std::vector<float> V3dStraightPlanarQuadWithCornderColors::getVertices() {
    std::cout << "ERROR: V3dStraightPlanarQuadWithCornderColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dStraightTriangleWithCornerColors::V3dStraightTriangleWithCornerColors(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLE_COLOR } { 
        for (int i = 0; i < 3; ++i) {
            xdrFile >> vertices[i].x;
            xdrFile >> vertices[i].y;
            xdrFile >> vertices[i].z;
        }

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;     

        for (int i = 0; i < 3; ++i) {
            xdrFile >> cornerColors[i].r;
            xdrFile >> cornerColors[i].g;
            xdrFile >> cornerColors[i].b;
            xdrFile >> cornerColors[i].a;
        }
    }

std::vector<float> V3dStraightTriangleWithCornerColors::getVertices() {
    std::cout << "ERROR: V3dStraightTriangleWithCornerColors cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dTriangleGroup::V3dTriangleGroup(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLES } { 
        xdrFile >> nI;

        xdrFile >> nP;
        vertexPositions.resize(nP);
        for (UINT i = 0; i < nP; ++i) {
            xdrFile >> vertexPositions[i].x;
            xdrFile >> vertexPositions[i].y;
            xdrFile >> vertexPositions[i].z;
        }

        xdrFile >> nN;
        vertexNormalArray.resize(nN);
        for (UINT i = 0; i < nN; ++i) {
            xdrFile >> vertexNormalArray[i].x;
            xdrFile >> vertexNormalArray[i].y;
            xdrFile >> vertexNormalArray[i].z;
        }

        xdrFile >> explicitNI;

        xdrFile >> nC;
        if (nC > 0) {
            vertexColorArray.resize(nC);
            for (UINT i = 0; i < nC; ++i) {
                xdrFile >> vertexColorArray[i].r;
                xdrFile >> vertexColorArray[i].g;
                xdrFile >> vertexColorArray[i].b;
                xdrFile >> vertexColorArray[i].a;
            }

            xdrFile >> explicitCI;
        }

        indices.resize(nI);
        for (UINT i = 0; i < nI; ++i) {
            xdrFile >> indices[i].vertexPositionIndices[0];
            xdrFile >> indices[i].vertexPositionIndices[1];
            xdrFile >> indices[i].vertexPositionIndices[2];

            if (explicitNI) {
                xdrFile >> indices[i].vertexNormalIndices[0];
                xdrFile >> indices[i].vertexNormalIndices[1];
                xdrFile >> indices[i].vertexNormalIndices[2];
            } else {
                indices[i].vertexNormalIndices[0] = indices[i].vertexPositionIndices[0];
                indices[i].vertexNormalIndices[1] = indices[i].vertexPositionIndices[1];
                indices[i].vertexNormalIndices[2] = indices[i].vertexPositionIndices[2];
            }

            if (nC > 0 && explicitCI) {
                xdrFile >> indices[i].vertexColorIndices[0];
                xdrFile >> indices[i].vertexColorIndices[1];
                xdrFile >> indices[i].vertexColorIndices[2];
            } else {
                indices[i].vertexColorIndices[0] = indices[i].vertexPositionIndices[0];
                indices[i].vertexColorIndices[1] = indices[i].vertexPositionIndices[1];
                indices[i].vertexColorIndices[2] = indices[i].vertexPositionIndices[2];
            }
        }
        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dTriangleGroup::getVertices() {
    std::cout << "ERROR: V3dTriangleGroup cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dSphere::V3dSphere(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::SPHERE } { 
        xdrFile >> center.x;
        xdrFile >> center.y;
        xdrFile >> center.z;

        xdrFile >> radius;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dSphere::getVertices() {
    std::cout << "ERROR: V3dSphere cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dHemiSphere::V3dHemiSphere(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::HALF_SPHERE } { 
        xdrFile >> center.x;
        xdrFile >> center.y;
        xdrFile >> center.z;

        xdrFile >> radius;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    

        xdrFile >> polarAngle;
        xdrFile >> azimuthalAngle;
    }

std::vector<float> V3dHemiSphere::getVertices() {
    std::cout << "ERROR: V3dHemiSphere cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dDisk::V3dDisk(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::DISK } { 
        xdrFile >> center.x;
        xdrFile >> center.y;
        xdrFile >> center.z;

        xdrFile >> radius;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    

        xdrFile >> polarAngle;
        xdrFile >> azimuthalAngle;
    }

std::vector<float> V3dDisk::getVertices() {
    std::cout << "ERROR: V3dDisk cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dCylinder::V3dCylinder(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::CYLINDER } { 
        xdrFile >> center.x;
        xdrFile >> center.y;
        xdrFile >> center.z;

        xdrFile >> radius;

        xdrFile >> height;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    

        xdrFile >> polarAngle;
        xdrFile >> azimuthalAngle;
    }

std::vector<float> V3dCylinder::getVertices() {
    std::cout << "ERROR: V3dCylinder cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dTube::V3dTube(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TUBE } { 
        for (UINT i = 0; i < 4; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }

        xdrFile >> width;
        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
        xdrFile >> core;
    }

std::vector<float> V3dTube::getVertices() {
    std::cout << "ERROR: V3dTube cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dBezierCurve::V3dBezierCurve(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::CURVE } { 
        for (UINT i = 0; i < 4; ++i) {
            xdrFile >> controlPoints[i].x;
            xdrFile >> controlPoints[i].y;
            xdrFile >> controlPoints[i].z;
        }    

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dBezierCurve::getVertices() {
    std::cout << "ERROR: V3dBezierCurve cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dLineSegment::V3dLineSegment(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::LINE } { 
        for (UINT i = 0; i < 2; ++i) {
            xdrFile >> endpoints[i].x;
            xdrFile >> endpoints[i].y;
            xdrFile >> endpoints[i].z;
        }    

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;    
    }

std::vector<float> V3dLineSegment::getVertices() {
    std::cout << "ERROR: V3dLineSegment cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}


V3dPixel::V3dPixel(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::PIXEL } { 
        xdrFile >> position.x;
        xdrFile >> position.y;
        xdrFile >> position.z;

        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dPixel::getVertices() {
    std::cout << "ERROR: V3dPixel cannot currently give vertices" << std::endl;
    return std::vector<float>{};
}
