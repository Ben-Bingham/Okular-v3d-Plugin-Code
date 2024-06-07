#include <iostream>
#include <fstream>
#include <queue>
#include <memory>

#include "V3dFile.h"

#include "xstream.h"

V3dFile::V3dFile(const std::string& fileName) {
    xdr::ixstream xdrFile{ fileName.c_str() };

    xdrFile >> versionNumber;
    xdrFile >> doublePrecisionFlag;

    // std::cout << "==========================================" << std::endl;
    // UINT data;
    // while (xdrFile >> data) {
    //     std::cout << data << std::endl;
    // }
    // std::cout << "==========================================" << std::endl;

    std::cout << "Filename: " << fileName << std::endl;

    UINT objectType;
    while (xdrFile >> objectType) {
        std::cout << "ObjectType: " << objectType << std::endl;
        switch (objectType) {
        default:
            std::cout << "UNKNOWN TYPE: " << objectType << std::endl;
            break;

        case ObjectTypes::MATERIAL:
            V3dMaterial material;
            xdrFile >> material.diffuse.r;
            xdrFile >> material.diffuse.g;
            xdrFile >> material.diffuse.b;
            xdrFile >> material.diffuse.a;

            xdrFile >> material.emissive.r;
            xdrFile >> material.emissive.g;
            xdrFile >> material.emissive.b;
            xdrFile >> material.emissive.a;

            xdrFile >> material.specular.r;
            xdrFile >> material.specular.g;
            xdrFile >> material.specular.b;
            xdrFile >> material.specular.a;

            xdrFile >> material.shininess;
            xdrFile >> material.metallic;
            xdrFile >> material.fresnel0;

            materials.push_back(material);
            break;

        case ObjectTypes::TRANSFORM:
            std::cout << "ERROR: No current way to store v3d object: TRANSFORM" << std::endl;
            break;

        case ObjectTypes::ELEMENT:
            std::cout << "ERROR: No current way to store v3d object: ELEMENT" << std::endl;
            break;

        case ObjectTypes::CENTERS:
            UINT centersLength;
            xdrFile >> centersLength;

            if (centersLength > 0) {
                centers.resize(centersLength);

                for (UINT i = 0; i < centersLength; ++i) {
                    xdrFile >> centers[i].x;
                    xdrFile >> centers[i].y;
                    xdrFile >> centers[i].z;
                }
            }

            break;

        case ObjectTypes::HEADER:
            m_Objects.push_back(std::move(std::make_unique<V3dHeader>(xdrFile)));
            break;

        case ObjectTypes::LINE:
            m_Objects.push_back(std::move(std::make_unique<V3dLineSegment>(xdrFile)));
            break;

        case ObjectTypes::TRIANGLE:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightTriangle>(xdrFile)));
            break;

        case ObjectTypes::QUAD:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightPlanarQuad>(xdrFile)));
            break;

        case ObjectTypes::CURVE:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierCurve>(xdrFile)));
            break;

        case ObjectTypes::BEZIER_TRIANGLE:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierTriangle>(xdrFile)));
            break;

        case ObjectTypes::BEZIER_PATCH:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierPatch>(xdrFile)));
            break;

        case ObjectTypes::LINE_COLOR:
            std::cout << "ERROR: No current way to store v3d object: LINE_COLOR" << std::endl;
            break;

        case ObjectTypes::TRIANGLE_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightTriangleWithCornerColors>(xdrFile)));
            break;

        case ObjectTypes::QUAD_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightPlanarQuadWithCornderColors>(xdrFile)));
            break;

        case ObjectTypes::CURVE_COLOR:
            std::cout << "ERROR: No current way to store v3d object: CURVE_COLOR" << std::endl;
            break;

        case ObjectTypes::BEZIER_TRIANGLE_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierTriangleWithCornerColors>(xdrFile)));
            break;

        case ObjectTypes::BEZIER_PATCH_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierPatchWithCornerColors>(xdrFile)));
            break;

        case ObjectTypes::TRIANGLES:
            m_Objects.push_back(std::move(std::make_unique<V3dTriangleGroup>(xdrFile)));
            break;

        case ObjectTypes::DISK:
            m_Objects.push_back(std::move(std::make_unique<V3dDisk>(xdrFile)));
            break;

        case ObjectTypes::CYLINDER:
            m_Objects.push_back(std::move(std::make_unique<V3dCylinder>(xdrFile)));
            break;

        case ObjectTypes::TUBE:
            m_Objects.push_back(std::move(std::make_unique<V3dTube>(xdrFile)));
            break;

        case ObjectTypes::SPHERE:
            m_Objects.push_back(std::move(std::make_unique<V3dSphere>(xdrFile)));
            break;

        case ObjectTypes::HALF_SPHERE:
            m_Objects.push_back(std::move(std::make_unique<V3dHemiSphere>(xdrFile)));
            break;

        case ObjectTypes::ANIMATION:
            std::cout << "ERROR: No current way to store v3d object: ANIMATION" << std::endl;
            break;

        case ObjectTypes::PIXEL:
            m_Objects.push_back(std::move(std::make_unique<V3dPixel>(xdrFile)));
            break;
        }
    }

    xdrFile.close();

    std::vector<float> vertices;

    for (auto& object : m_Objects) {
        std::vector<float> vert = object->getVertices();

        for (auto& value : vert) {
            vertices.push_back(value);
        }
    }

    this->vertices = vertices;
}
