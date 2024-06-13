#include <iostream>
#include <fstream>
#include <queue>
#include <memory>

#include "V3dFile.h"

#include "xstream.h"

#include "V3dUtil.h"

// append array b onto array a with offset
void appendOffset(std::vector<UINT>& a, const std::vector<UINT>& b, size_t offset) {
    size_t n=a.size();
    size_t m=b.size();
    a.resize(n+m);
    for(size_t i=0; i < m; ++i)
        a[n+i]=b[i]+offset;
}

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

    UINT objectType;
    while (xdrFile >> objectType) {
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
                    centers[i].x = readReal(xdrFile, doublePrecisionFlag);
                    centers[i].y = readReal(xdrFile, doublePrecisionFlag);
                    centers[i].z = readReal(xdrFile, doublePrecisionFlag);
                }
            }

            break;

        case ObjectTypes::HEADER:
            UINT headerEntryCount;
            xdrFile >> headerEntryCount;

            for (UINT i = 0; i < headerEntryCount; ++i) {
                UINT headerKey;
                xdrFile >> headerKey;

                UINT length;
                xdrFile >> length;

                switch (headerKey) {
                    case CANVAS_WIDTH:
                        xdrFile >> headerInfo.canvasWidth;   
                        break;     

                    case CANVAS_HEIGHT:
                        xdrFile >> headerInfo.canvasHeight; 
                        break;    

                    case ABSOLUTE:
                        xdrFile >> headerInfo.absolute;   
                        break;     

                    case MIN_BOUND:
                        headerInfo.minBound.x = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.minBound.y = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.minBound.z = readReal(xdrFile, doublePrecisionFlag);
                        break; 

                    case MAX_BOUND:
                        headerInfo.maxBound.x = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.maxBound.y = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.maxBound.z = readReal(xdrFile, doublePrecisionFlag);
                        break;       

                    case ORTHOGRAPHIC:
                        xdrFile >> headerInfo.orthographic;   
                        break;     

                    case ANGLE_OF_VIEW:
                        headerInfo.angleOfView = readReal(xdrFile, doublePrecisionFlag);
                        break;     

                    case INITIAL_ZOOM:
                        headerInfo.initialZoom = readReal(xdrFile, doublePrecisionFlag);
                        break;     

                    case VIEWPORT_SHIFT:
                        headerInfo.viewportShift.x = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.viewportShift.y = readReal(xdrFile, doublePrecisionFlag);
                        break;    

                    case VIEWPORT_MARGIN:
                        headerInfo.viewportMargin.x = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.viewportMargin.y = readReal(xdrFile, doublePrecisionFlag); 
                        break;

                    case LIGHT:
                        headerInfo.light.direction.x = readReal(xdrFile, doublePrecisionFlag);
                        headerInfo.light.direction.y = readReal(xdrFile, doublePrecisionFlag); 
                        headerInfo.light.direction.z = readReal(xdrFile, doublePrecisionFlag);

                        xdrFile >> headerInfo.light.color.r;
                        xdrFile >> headerInfo.light.color.g;       
                        xdrFile >> headerInfo.light.color.b;              
                        break;       

                    case BACKGROUND:
                        xdrFile >> headerInfo.background.r;
                        xdrFile >> headerInfo.background.g;
                        xdrFile >> headerInfo.background.b;
                        xdrFile >> headerInfo.background.a; 
                        break;      

                    case ZOOM_FACTOR:
                        headerInfo.zoomFactor = readReal(xdrFile, doublePrecisionFlag);   
                        break;    

                    case ZOOM_PINCH_FACTOR:
                        headerInfo.zoomPinchFactor = readReal(xdrFile, doublePrecisionFlag);   
                        break;

                    case ZOOM_PINCH_CAP:
                        headerInfo.zoomPinchCap = readReal(xdrFile, doublePrecisionFlag);   
                        break;

                    case ZOOM_STEP:
                        headerInfo.zoomStep = readReal(xdrFile, doublePrecisionFlag);            
                        break;

                    case SHIFT_HOLD_DISTANCE:
                        headerInfo.shiftHoldDistance = readReal(xdrFile, doublePrecisionFlag);   
                        break;

                    case SHIFT_WAIT_TIME:
                        headerInfo.shiftWaitTime = readReal(xdrFile, doublePrecisionFlag);   
                        break;
                        
                    case VIBRATE_TIME:
                        // xdrFile >> headerInfo.vibrateTime;   
                        headerInfo.vibrateTime = readReal(xdrFile, doublePrecisionFlag);    
                        break;    
                }
            }
            break;

        case ObjectTypes::LINE:
            m_Objects.push_back(std::move(std::make_unique<V3dLineSegment>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::TRIANGLE:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightTriangle>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::QUAD:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightPlanarQuad>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::CURVE:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierCurve>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::BEZIER_TRIANGLE:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierTriangle>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::BEZIER_PATCH:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierPatch>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::LINE_COLOR:
            std::cout << "ERROR: No current way to store v3d object: LINE_COLOR" << std::endl;
            break;

        case ObjectTypes::TRIANGLE_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightTriangleWithCornerColors>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::QUAD_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dStraightPlanarQuadWithCornderColors>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::CURVE_COLOR:
            std::cout << "ERROR: No current way to store v3d object: CURVE_COLOR" << std::endl;
            break;

        case ObjectTypes::BEZIER_TRIANGLE_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierTriangleWithCornerColors>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::BEZIER_PATCH_COLOR:
            m_Objects.push_back(std::move(std::make_unique<V3dBezierPatchWithCornerColors>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::TRIANGLES:
            m_Objects.push_back(std::move(std::make_unique<V3dTriangleGroup>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::DISK:
            m_Objects.push_back(std::move(std::make_unique<V3dDisk>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::CYLINDER:
            m_Objects.push_back(std::move(std::make_unique<V3dCylinder>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::TUBE:
            m_Objects.push_back(std::move(std::make_unique<V3dTube>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::SPHERE:
            m_Objects.push_back(std::move(std::make_unique<V3dSphere>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::HALF_SPHERE:
            m_Objects.push_back(std::move(std::make_unique<V3dHemiSphere>(xdrFile, doublePrecisionFlag)));
            break;

        case ObjectTypes::ANIMATION:
            std::cout << "ERROR: No current way to store v3d object: ANIMATION" << std::endl;
            break;

        case ObjectTypes::PIXEL:
            m_Objects.push_back(std::move(std::make_unique<V3dPixel>(xdrFile, doublePrecisionFlag)));
            break;
        }
    }

    xdrFile.close();

    for (auto& object : m_Objects) {
        std::vector<float> vert = object->getVertexData();
        std::vector<unsigned int> ind = object->getIndices();

        appendOffset(indices, ind, vertices.size() / 6);
        vertices.insert(vertices.end(), vert.begin(), vert.end());
    }
}
