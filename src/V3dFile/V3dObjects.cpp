#include "V3dObjects.h"

#include <iostream>

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

std::vector<unsigned int> V3dBezierPatch::getIndices() {
    std::cout << "ERROR: V3dBezierPatch cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dBezierTriangle::getIndices() {
    std::cout << "ERROR: V3dBezierTriangle cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dBezierPatchWithCornerColors::getIndices() {
    std::cout << "ERROR: V3dBezierPatchWithCornerColors cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dBezierTriangleWithCornerColors::getIndices() {
    std::cout << "ERROR: V3dBezierTriangleWithCornerColors cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dStraightPlanarQuad::getIndices() {
    std::cout << "ERROR: V3dStraightPlanarQuad cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dStraightTriangle::getIndices() {
    std::vector<unsigned int> out {
        0, 1, 2
    };

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

std::vector<unsigned int> V3dStraightPlanarQuadWithCornderColors::getIndices() {
    std::cout << "ERROR: V3dStraightPlanarQuadWithCornderColors cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dStraightTriangleWithCornerColors::getIndices() {
    std::cout << "ERROR: V3dStraightTriangleWithCornerColors cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
}


V3dTriangleGroup::V3dTriangleGroup(xdr::ixstream& xdrFile)
    : V3dObject{ ObjectTypes::TRIANGLES } { 
        nI = 0;
        xdrFile >> nI;

        nP = 0;
        xdrFile >> nP;
        vertexPositions.resize(nP);
        for (UINT i = 0; i < nP; ++i) {
            xdrFile >> vertexPositions[i].x;
            xdrFile >> vertexPositions[i].y;
            xdrFile >> vertexPositions[i].z;
        }

        nN = 0;
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

        positionIndices.resize(nI);
        normalIndices.resize(nI);
        colorIndices.resize(nI);

        for (UINT i = 0; i < nI; ++i) {
            xdrFile >> positionIndices[i][0];
            xdrFile >> positionIndices[i][1];
            xdrFile >> positionIndices[i][2];

            if (explicitNI) {
                xdrFile >> normalIndices[i][0];
                xdrFile >> normalIndices[i][1];
                xdrFile >> normalIndices[i][2];
            } else {
                normalIndices[i] = positionIndices[i];
            }

            if (nC > 0 && explicitCI) {
                xdrFile >> colorIndices[i][0];
                xdrFile >> colorIndices[i][1];
                xdrFile >> colorIndices[i][2];
            } else {
                colorIndices[i] = positionIndices[i];
            }
        }
        xdrFile >> centerIndex;
        xdrFile >> materialIndex;
    }

std::vector<float> V3dTriangleGroup::getVertices() {
    std::vector<float> out;

    std::vector<TRIPLE> vertices;
    vertices.resize(nP);

    // std::cout << "==================================" << std::endl;
    for(size_t i = 0; i < nI; ++i) {
        std::array<unsigned int, 3> PI = positionIndices[i];
        // const uint32_t *PI=PP[i];
        uint32_t PI0 = PI[0];
        uint32_t PI1 = PI[1];
        uint32_t PI2 = PI[2];
        TRIPLE P0 = vertexPositions[PI0];
        TRIPLE P1 = vertexPositions[PI1];
        TRIPLE P2 = vertexPositions[PI2];

        // std::cout << "0: " << PI0 << ", 1: " << PI1 << ", 2: " << PI2 << std::endl;

        // data.Vertices[PI0]=VertexData(P0,N[NI[0]]);
        // data.Vertices[PI1]=VertexData(P1,N[NI[1]]);
        // data.Vertices[PI2]=VertexData(P2,N[NI[2]]);

        vertices[PI0] = P0;
        vertices[PI1] = P1;
        vertices[PI2] = P2;

        // triple Q[]={P0,P1,P2};
        // if(!offscreen(3,Q)) {
        //     size_t i3=3*i;
        //     data.indices[i3]=PI0;
        //     data.indices[i3+1]=PI1;
        //     data.indices[i3+2]=PI2;
        // }
    }

    for (auto vertex : vertices) {
        out.push_back(vertex.x);
        out.push_back(vertex.y);
        out.push_back(vertex.z);
    }

    return out;
}

std::vector<unsigned int> V3dTriangleGroup::getIndices() {
    std::vector<unsigned int> out;

    out.resize(nI * 3);

    // int j = 0;
    // for (int i = 0; i < nI * 3; i += 3) {
    //     std::array<unsigned int, 3> currentIndices = positionIndices[j];
    //     UINT index0 = currentIndices[0];
    //     UINT index1 = currentIndices[1];
    //     UINT index2 = currentIndices[2];

    //     out[i + 0] = index0;
    //     out[i + 1] = index1;
    //     out[i + 2] = index2;
    //     j++;
    // }


    for(size_t i = 0; i < nI; ++i) {
        std::array<unsigned int, 3> PI = positionIndices[i];
        // const uint32_t *PI=PP[i];
        uint32_t PI0 = PI[0];
        uint32_t PI1 = PI[1];
        uint32_t PI2 = PI[2];
        // TRIPLE P0 = vertexPositions[PI0];
        // TRIPLE P1 = vertexPositions[PI1];
        // TRIPLE P2 = vertexPositions[PI2];

        // data.Vertices[PI0]=VertexData(P0,N[NI[0]]);
        // data.Vertices[PI1]=VertexData(P1,N[NI[1]]);
        // data.Vertices[PI2]=VertexData(P2,N[NI[2]]);

        // vertices[PI0] = P0;
        // vertices[PI1] = P1;
        // vertices[PI2] = P2;

        // triple Q[]={P0,P1,P2};
        // if(!offscreen(3,Q)) {
        size_t i3=3*i;
        out[i3 + 0] = PI0;
        out[i3 + 1] = PI1;
        out[i3 + 2] = PI2;
        //     data.indices[i3]=PI0;
        //     data.indices[i3+1]=PI1;
        //     data.indices[i3+2]=PI2;
        // }
    }

    return out;
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

std::vector<unsigned int> V3dSphere::getIndices() {
    std::cout << "ERROR: V3dSphere cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dHemiSphere::getIndices() {
    std::cout << "ERROR: V3dHemiSphere cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dDisk::getIndices() {
    std::cout << "ERROR: V3dDisk cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dCylinder::getIndices() {
    std::cout << "ERROR: V3dCylinder cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dTube::getIndices() {
    std::cout << "ERROR: V3dTube cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dBezierCurve::getIndices() {
    std::cout << "ERROR: V3dBezierCurve cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dLineSegment::getIndices() {
    std::cout << "ERROR: V3dLineSegment cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
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

std::vector<unsigned int> V3dPixel::getIndices() {
    std::cout << "ERROR: V3dPixel cannot currently give indices" << std::endl;
    return std::vector<unsigned int>{};
}
