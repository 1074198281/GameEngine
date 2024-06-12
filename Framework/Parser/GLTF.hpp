//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author(s):   James Stanard
//

#pragma once

#include "ISceneParser.hpp"
#include "SceneNode.hpp"
#include "SceneObject.hpp"
#include "Scene.hpp"
#include "utility.hpp"
#include "geommath.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <xutility>
#include <algorithm>

//
//#ifndef _WIN32
//#define _WIN32
//#endif
#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter
#include "json.hpp"
#pragma warning(pop)


namespace glTF
{
    using json = nlohmann::json;
    typedef std::shared_ptr<std::vector<unsigned char> > ByteArray;

    struct BufferView
    {
        uint32_t buffer;
        uint32_t byteLength;
        uint32_t byteOffset;
        uint16_t byteStride;
        bool     elementArrayBuffer;
    };

    struct Accessor
    {
        enum // componentType
        {
            kByte,
            kUnsignedByte,
            kShort,
            kUnsignedShort,
            kSignedInt, // won't happen
            kUnsignedInt,
            kFloat
        };

        enum // type
        {
            kScalar,
            kVec2,
            kVec3,
            kVec4,
            kMat2,
            kMat3,
            kMat4
        };

        //BufferView* bufferView;
        //uint32_t byteOffset; // offset from start of buffer view
        char* dataPtr;
        uint32_t stride;
        uint32_t count; // number of elements
        uint16_t componentType;
        uint16_t type;

        // Nobody is doing this in the samples.  Seems dumb.
        //uint32_t sparseCount;   // Number of sparse elements
        //uint32_t sparseIndices; // Buffer view for indices into sparse data
        //uint32_t sparseValues;  // Buffer view for packed array of sparse values
    };

    struct Image
    {
        std::string path; // UTF8
    };

    struct Sampler
    {
        //D3D12_FILTER filter;
        int filter;
        //D3D12_TEXTURE_ADDRESS_MODE wrapS;
        int wrapS;
        //D3D12_TEXTURE_ADDRESS_MODE wrapT;
        int wrapT;
    };

    struct Texture
    {
        Image* source;
        Sampler* sampler;
    };

    struct Material
    {
        std::string name;
        union
        {
            struct
            {
                float baseColorFactor[4]; // default=[1,1,1,1]
                float metallicFactor; // default=1
                float roughnessFactor; // default=1
            };
        };
        union
        {
            uint32_t flags;
            struct
            {
                uint32_t baseColorUV : 1;
                uint32_t metallicRoughnessUV : 1;
                uint32_t occlusionUV : 1;
                uint32_t emissiveUV : 1;
                uint32_t normalUV : 1;
                uint32_t twoSided : 1;
                uint32_t alphaTest : 1;
                uint32_t alphaBlend : 1;
                uint32_t _pad : 8;
                uint32_t alphaCutoff : 16; // FP16
            };
        };
        float emissiveFactor[3]; // default=[0,0,0]
        float normalTextureScale; // default=1
        enum eTextureType { kBaseColor, kMetallicRoughness, kOcclusion, kEmissive, kNormal, kNumTextures };
        Texture* textures[kNumTextures];
        uint32_t index;

        float TextureTransform[5 * kNumTextures];
    };

    struct Primitive
    {
        enum eAttribType { kPosition, kNormal, kTangent, kTexcoord0, kTexcoord1, kColor0, kJoints0, kWeights0, kNumAttribs };
        Accessor* attributes[kNumAttribs];
        Accessor* indices;
        Material* material;
        uint16_t attribMask;
        uint16_t mode; // D3D_PRIMITIVE_TOPOLOGY
        __declspec(align(16)) float minPos[3];
        __declspec(align(16)) float maxPos[3];
        uint32_t minIndex;
        uint32_t maxIndex;
        //std::vector<uint32_t> targets;
    };

    struct Mesh
    {
        std::string name;
        std::vector<Primitive> primitives;
        int32_t skin;
        enum eCollisionType { kNone, kSphere, kBox, kPlane = 15, kCollisionType };
        eCollisionType collisionType;
        float Bounding[3];
    };

    struct Camera
    {
        enum eType { kPerspective, kOrthographic } type;
        union
        {
            struct
            {
                float aspectRatio;
                float yfov;
            };
            struct
            {
                float xmag;
                float ymag;
            };
        };
        float znear;
        float zfar;
    };

    struct Node
    {
        std::string name;
        union
        {
            uint8_t flags;
            struct
            {
                bool pointsToCamera : 1;
                bool hasMatrix : 1;
                bool skeletonRoot : 1;
            };
        };
        union
        {
            Mesh* mesh;
            Camera* camera;
        };
        std::vector<Node*> children;
        union
        {
            __declspec(align(16)) float matrix[16];
            struct
            {
                __declspec(align(16)) float scale[3];
                __declspec(align(16)) float rotation[4];
                __declspec(align(16)) float translation[3];
            };
        };
        int32_t linearIdx; // Assists with mapping scene nodes to flat arrays
        int32_t ParentIdx;
    };

    struct Skin
    {
        Accessor* inverseBindMatrices;  // An array of IBMs that match the order of joint nodes
        Node* skeleton; // root node (if non-null, this node should be in the list of joints too)
        std::vector<Node*> joints;
    };

    struct Scene
    {
        std::vector<Node*> nodes;
    };

    struct AnimSampler
    {
        Accessor* m_input;  // key frame time stamps
        Accessor* m_output; // key frame values
        enum eInterpolation { kLinear, kStep, kCatmullRomSpline, kCubicSpline };
        eInterpolation m_interpolation;
    };

    struct AnimChannel
    {
        AnimSampler* m_sampler;
        Node* m_target;
        enum ePath { kTranslation, kRotation, kScale, kWeights };
        ePath m_path;
    };

    struct Animation
    {
        std::vector<AnimChannel> m_channels;
        std::vector<AnimSampler> m_samplers;
    };

    struct Light
    {
        enum eLightType { kPoint, kDirectional, kSpot, kLightType };
        std::string m_name;
        float m_Color[3];
        float m_Intensity;
        eLightType m_Type;
        union {
            struct SpotProperties {
                float innerConnangle;
                float outerConnAngle;
            };
        };
    };

    struct Light_Punctual {
        std::vector<Light> m_Lights;
    };

    struct KHR_Lights
    {
        Light_Punctual m_punctualLights;
    };

    struct Extensions {
        KHR_Lights m_extLights;
    };

    uint16_t TypeToEnum(const char type[])
    {
        if (strncmp(type, "VEC", 3) == 0)
            return Accessor::kVec2 + type[3] - '2';
        else if (strncmp(type, "MAT", 3) == 0)
            return Accessor::kMat2 + type[3] - '2';
        else
            return Accessor::kScalar;
    }

    inline uint32_t floatToHalf(float f)
    {
        const float kF32toF16 = (1.0 / (1ull << 56)) * (1.0 / (1ull << 56)); // 2^-112
        union { float f; uint32_t u; } x;
        x.f = std::clamp(f, 0.0f, 1.0f) * kF32toF16;
        return x.u >> 13;
    }

    void ReadFloats(json& list, float flt_array[])
    {
        uint32_t i = 0;
        for (auto& flt : list)
            flt_array[i++] = flt;
    }

    My::VertexDataType GetVertexDataType(int type)
    {
        My::VertexDataType vertexDataType;
        switch (type) {
        case 0:
            vertexDataType = My::kVertexDataTypeFloat1;
            break;
        case 1:
            vertexDataType = My::kVertexDataTypeFloat2;
            break;
        case 2:
            vertexDataType = My::kVertexDataTypeFloat3;
            break;
        case 3:
            vertexDataType = My::kVertexDataTypeFloat4;
            break;
        case 4:
            vertexDataType = My::kVertexDataTypeMatrix2;
            break;
        case 5:
            vertexDataType = My::kVertexDataTypeMatrix2;
            break;
        case 6:
            vertexDataType = My::kVertexDataTypeMatrix2;
            break;
        default:
            assert(false);
            printf("Vertex Type Parse Error!");
            break;
        }
        return vertexDataType;
    }

    My::IndexDataType GetIndexDataType(int type)
    {
        My::IndexDataType indexDataType;
        switch (type) {
        case 0:
            indexDataType = My::kIndexDataTypeInt8;
            break;
        case 1:
            indexDataType = My::kIndexDataTypeInt8;
            break;
        case 2:
            indexDataType = My::kIndexDataTypeInt16;
            break;
        case 3:
            indexDataType = My::kIndexDataTypeInt16;
            break;
        case 4:
            indexDataType = My::kIndexDataTypeInt32;
            break;
        case 5:
            indexDataType = My::kIndexDataTypeInt32;
            break;
        case 6:
            indexDataType = My::kIndexDataTypeInt32;
            break;
        default:
            assert(false);
            printf("Parse Index Data Type Error!");
            break;
        }
        return indexDataType;
    }

    int GetComponentDataSize(int componentType)
    {
        int dataSize;
        switch (componentType) {
        case 0:
            dataSize = 1;
            break;
        case 1:
            dataSize = 1;
            break;
        case 2:
            dataSize = 2;
            break;
        case 3:
            dataSize = 2;
            break;
        case 4:
            assert(false);
            printf("ComponentType Signed Int Should Not Happen!");
            dataSize = 4;
            break;
        case 5:
            dataSize = 4;
            break;
        case 6:
            dataSize = 4;
            break;
        default:
            assert(false);
            printf("Parse ComponentType Error");
            break;
        }
        return dataSize;
    }

    int GetPerElementCount(My::VertexDataType vtType)
    {
        int count = 0;
        switch (vtType) {
        case My::kVertexDataTypeFloat1:
            count = 1;
            break;
        case My::kVertexDataTypeFloat2:
            count = 2;
            break;
        case My::kVertexDataTypeFloat3:
            count = 3;
            break;
        case My::kVertexDataTypeFloat4:
            count = 4;
            break;
        case My::kVertexDataTypeDouble1:
            count = 1;
            break;
        case My::kVertexDataTypeDouble2:
            count = 2;
            break;
        case My::kVertexDataTypeDouble3:
            count = 3;
            break;
        case My::kVertexDataTypeDouble4:
            count = 4;
            break;
        }
        return count;
    }

    std::string GetTextureType(Material::eTextureType texType)
    {
        std::string stringType;
        switch (texType)
        {
        case Material::eTextureType::kBaseColor:
            stringType = "pbrdiffuse";
            break;
        case Material::eTextureType::kMetallicRoughness:
            stringType = "pbrmetallicroughness";
            break;
        case Material::eTextureType::kOcclusion:
            stringType = "pbrocclusion";
            break;
        case Material::eTextureType::kEmissive:
            stringType = "pbremissive";
            break;
        case Material::eTextureType::kNormal:
            stringType = "pbrnormal";
            break;
        default:
            assert(false);
            printf("Parse Material Texture Type Error!");
            break;
        }
        return stringType;
    }

    int32_t GLtoD3DTextureAddressMode(int32_t glWrapMode)
    {
        switch (glWrapMode)
        {
        default: assert("Unexpected sampler wrap mode");
        //case 33071: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case 33071: return 3;
        //case 33648: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case 33648: return 2;
        //case 10497: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case 10497: return 1;
        }
    }

    class GLTFParser : __implements My::SceneParser
    {
    public:
        GLTFParser() : m_scene(nullptr) {}
        ~GLTFParser() { m_meshes.clear(); }

        Scene* m_scene;
        std::string m_basePath;
        std::string m_AssetPath;
        std::vector<Scene> m_scenes;
        std::vector<Node> m_nodes;
        std::vector<Camera> m_cameras;
        std::vector<Mesh> m_meshes;
        std::vector<Image> m_images;
        std::vector<Sampler> m_samplers;
        std::vector<Texture> m_textures;
        std::vector<Accessor> m_accessors;
        std::vector<Skin> m_skins;
        std::vector<Material> m_materials;
        std::vector<ByteArray> m_buffers;
        std::vector<BufferView> m_bufferViews;
        std::vector<Animation> m_animations;

        Extensions m_extensions;

    private:
        void ProcessLightPunctual(json& lightPunctual)
        {
            KHR_Lights KHRlights;

            if (lightPunctual.find("lights") != lightPunctual.end())
            {
                Light_Punctual pLights;
                json& lights = lightPunctual.at("lights");
                for (json::iterator itLight = lights.begin(); itLight != lights.end(); ++itLight)
                {
                    Light l;
                    json& lightPro = itLight.value();
                    l.m_name = lightPro.at("name");
                    ReadFloats(lightPro.at("color"), l.m_Color);
                    l.m_Intensity = lightPro.at("intensity");
                    std::string type = lightPro.at("type");
                    if (type == "spot") {
                        l.m_Type = Light::eLightType::kSpot;
                    }
                    else if (type == "directional") {
                        l.m_Type = Light::eLightType::kDirectional;
                    }
                    else if (type == "point") {
                        l.m_Type = Light::eLightType::kPoint;
                    }

                    pLights.m_Lights.push_back(l);
                }

                KHRlights.m_punctualLights = pLights;
            }
            m_extensions.m_extLights = KHRlights;
        }

        void ProcessExtensions(json& extensions)
        {
            for (json::iterator it = extensions.begin(); it != extensions.end(); ++it)
            {
                std::string itName = it.key();
                json& itProperty = it.value();
                if (itName == "KHR_lights_punctual");
                {
                    ProcessLightPunctual(itProperty);
                }
            }
        }

        void ProcessBuffers(json& buffers, ByteArray chunk1bin)
        {
            m_buffers.reserve(buffers.size());

            for (json::iterator it = buffers.begin(); it != buffers.end(); ++it)
            {
                json& thisBuffer = it.value();

                if (thisBuffer.find("uri") != thisBuffer.end())
                {
                    const std::string& uri = thisBuffer.at("uri");
                    std::string filepath = m_basePath + std::string(uri.begin(), uri.end());

                    ByteArray ba = My::ReadFileSyncDirectly(filepath.c_str(), true);
                    assert(ba->size() > 0);
                    printf("Missing bin file %s\n", filepath.c_str());
                    m_buffers.push_back(ba);
                }
                else
                {
                    //ASSERT(it == buffers.begin(), "Only the 1st buffer allowed to be internal");
                    assert(it == buffers.begin());
                    printf("Only the 1st buffer allowed to be internal");
                    assert(chunk1bin->size() > 0);
                    printf("GLB chunk1 missing data or not a GLB file");
                    m_buffers.push_back(chunk1bin);
                }
            }
        }

        void ProcessBufferViews(json& bufferViews)
        {
            m_bufferViews.reserve(bufferViews.size());

            for (json::iterator it = bufferViews.begin(); it != bufferViews.end(); ++it)
            {
                glTF::BufferView bufferView;
                json& thisBufferView = it.value();

                bufferView.buffer = thisBufferView.at("buffer");
                bufferView.byteLength = thisBufferView.at("byteLength");
                bufferView.byteOffset = 0;
                bufferView.byteStride = 0;
                bufferView.elementArrayBuffer = false;

                if (thisBufferView.find("byteOffset") != thisBufferView.end())
                    bufferView.byteOffset = thisBufferView.at("byteOffset");

                if (thisBufferView.find("byteStride") != thisBufferView.end())
                    bufferView.byteStride = thisBufferView.at("byteStride");

                // 34962 = ARRAY_BUFFER;  34963 = ELEMENT_ARRAY_BUFFER
                if (thisBufferView.find("target") != thisBufferView.end() && thisBufferView.at("target") == 34963)
                    bufferView.elementArrayBuffer = true;

                m_bufferViews.push_back(bufferView);
            }
        }

        void ProcessAccessors(json& accessors)
        {
            m_accessors.reserve(accessors.size());

            for (json::iterator it = accessors.begin(); it != accessors.end(); ++it)
            {
                glTF::Accessor accessor;
                json& thisAccessor = it.value();

                glTF::BufferView& bufferView = m_bufferViews[thisAccessor.at("bufferView")];
                accessor.dataPtr = reinterpret_cast<char*>(m_buffers[bufferView.buffer]->data()) + bufferView.byteOffset;
                accessor.stride = bufferView.byteStride;
                if (thisAccessor.find("byteOffset") != thisAccessor.end())
                    accessor.dataPtr += thisAccessor.at("byteOffset");
                accessor.count = thisAccessor.at("count");
                accessor.componentType = thisAccessor.at("componentType").get<uint16_t>() - 5120;

                char type[8];
                strcpy_s(type, thisAccessor.at("type").get<std::string>().c_str());

                accessor.type = TypeToEnum(type);

                m_accessors.push_back(accessor);
            }
        }

        void ProcessMaterials(json& materials)
        {
            m_materials.reserve(materials.size());

            uint32_t materialIdx = 0;

            for (json::iterator it = materials.begin(); it != materials.end(); ++it)
            {
                glTF::Material material;
                json& thisMaterial = it.value();

                material.index = materialIdx++;
                material.flags = 0;
                material.alphaCutoff = floatToHalf(0.5f);
                material.normalTextureScale = 1.0f;
                memset(&material.TextureTransform, 0, sizeof(float) * 5 * 5);

                if (thisMaterial.find("name") != thisMaterial.end()) {
                    material.name = thisMaterial.at("name");
                }

                if (thisMaterial.find("alphaMode") != thisMaterial.end())
                {
                    std::string alphaMode = thisMaterial.at("alphaMode");
                    if (alphaMode == "BLEND")
                        material.alphaBlend = true;
                    else if (alphaMode == "MASK")
                        material.alphaTest = true;
                }

                if (thisMaterial.find("alphaCutoff") != thisMaterial.end())
                {
                    material.alphaCutoff = floatToHalf(thisMaterial.at("alphaCutoff"));
                    //material.alphaTest = true;  // Should we alpha test and alpha blend?
                }

                if (thisMaterial.find("pbrMetallicRoughness") != thisMaterial.end())
                {
                    json& metallicRoughness = thisMaterial.at("pbrMetallicRoughness");

                    material.baseColorFactor[0] = 1.0f;
                    material.baseColorFactor[1] = 1.0f;
                    material.baseColorFactor[2] = 1.0f;
                    material.baseColorFactor[3] = 1.0f;
                    material.metallicFactor = 1.0f;
                    material.roughnessFactor = 1.0f;
                    for (uint32_t i = 0; i < Material::kNumTextures; ++i)
                        material.textures[i] = nullptr;

                    if (metallicRoughness.find("baseColorFactor") != metallicRoughness.end())
                        ReadFloats(metallicRoughness.at("baseColorFactor"), material.baseColorFactor);

                    if (metallicRoughness.find("metallicFactor") != metallicRoughness.end())
                        ReadFloats(metallicRoughness.at("metallicFactor"), &material.metallicFactor);

                    if (metallicRoughness.find("roughnessFactor") != metallicRoughness.end())
                        ReadFloats(metallicRoughness.at("roughnessFactor"), &material.roughnessFactor);

                    if (metallicRoughness.find("baseColorTexture") != metallicRoughness.end())
                        material.baseColorUV = ReadTextureInfo(metallicRoughness.at("baseColorTexture"),
                            Material::kBaseColor, material);

                    if (metallicRoughness.find("metallicRoughnessTexture") != metallicRoughness.end())
                        material.metallicRoughnessUV = ReadTextureInfo(metallicRoughness.at("metallicRoughnessTexture"),
                            Material::kMetallicRoughness, material);
                }

                if (thisMaterial.find("doubleSided") != thisMaterial.end())
                    material.twoSided = thisMaterial.at("doubleSided");

                if (thisMaterial.find("normalTextureScale") != thisMaterial.end())
                    material.normalTextureScale = thisMaterial.at("normalTextureScale");

                if (thisMaterial.find("emissiveFactor") != thisMaterial.end())
                    ReadFloats(thisMaterial.at("emissiveFactor"), material.emissiveFactor);

                if (thisMaterial.find("occlusionTexture") != thisMaterial.end())
                    material.occlusionUV = ReadTextureInfo(thisMaterial.at("occlusionTexture"),
                        Material::kOcclusion, material);

                if (thisMaterial.find("emissiveTexture") != thisMaterial.end())
                    material.emissiveUV = ReadTextureInfo(thisMaterial.at("emissiveTexture"),
                        Material::kEmissive, material);

                if (thisMaterial.find("normalTexture") != thisMaterial.end())
                    material.normalUV = ReadTextureInfo(thisMaterial.at("normalTexture"),
                        Material::kNormal, material);

                m_materials.push_back(material);
            }
        }

        void ProcessTextures(json& textures)
        {
            m_textures.resize(textures.size());

            uint32_t texIdx = 0;

            for (json::iterator it = textures.begin(); it != textures.end(); ++it)
            {
                glTF::Texture& texture = m_textures[texIdx++];
                json& thisTexture = it.value();

                texture.source = nullptr;
                texture.sampler = nullptr;

                if (thisTexture.find("source") != thisTexture.end())
                    texture.source = &m_images[thisTexture.at("source")];

                if (thisTexture.find("sampler") != thisTexture.end())
                    texture.sampler = &m_samplers[thisTexture.at("sampler")];
            }
        }

        void ProcessSamplers(json& samplers)
        {
            m_samplers.resize(samplers.size());

            uint32_t samplerIdx = 0;

            for (json::iterator it = samplers.begin(); it != samplers.end(); ++it)
            {
                json& thisSampler = it.value();

                glTF::Sampler& sampler = m_samplers[samplerIdx++];
                //sampler.filter = D3D12_FILTER_ANISOTROPIC;
                sampler.filter = 0x55;
                //sampler.wrapS = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                sampler.wrapS = 1;
                //sampler.wrapT = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                sampler.wrapT = 1;

                /*
                // Who cares what is provided?  It's about what you can afford, generally
                // speaking about materials.  If you want anisotropic filtering, why let
                // the asset dictate that.  And AF isn't represented in WebGL, so blech.
                int32_t magFilter = 9729;
                int32_t minFilter = 9987;
                if (thisSampler.find("magFilter") != thisSampler.end())
                    magFilter = thisSampler.at("magFilter");
                if (thisSampler.find("minFilter") != thisSampler.end())
                    minFilter = thisSampler.at("minFilter");
                sampler.filter = GLtoD3DTextureFilterMode(magFilter, minFilter);
                */

                // But these could matter for correctness.  Though, where is border mode?
                if (thisSampler.find("wrapS") != thisSampler.end())
                    sampler.wrapS = GLtoD3DTextureAddressMode(thisSampler.at("wrapS"));
                if (thisSampler.find("wrapT") != thisSampler.end())
                    sampler.wrapT = GLtoD3DTextureAddressMode(thisSampler.at("wrapT"));
            }
        }

        void ProcessImages(json& images)
        {
            m_images.resize(images.size());

            uint32_t imageIdx = 0;

            for (json::iterator it = images.begin(); it != images.end(); ++it)
            {
                json& thisImage = it.value();
                if (thisImage.find("uri") != thisImage.end())
                {
                    m_images[imageIdx++].path = thisImage.at("uri").get<std::string>();
                }
                else if (thisImage.find("bufferView") != thisImage.end())
                {
                    //assert(false);
                    printf("GLB image at buffer view %d with mime type %s\n", thisImage.at("bufferView").get<uint32_t>(), thisImage.at("mimeType").get<std::string>().c_str());
                    //Utility::Printf("GLB image at buffer view %d with mime type %s\n", thisImage.at("bufferView").get<uint32_t>(), thisImage.at("mimeType").get<std::string>().c_str());
                }
                else
                {
                    assert(false);
                    //ASSERT(0);
                }
            }
        }

        void ProcessSkins(json& skins)
        {
            uint32_t skinIdx = 0;

            for (json::iterator it = skins.begin(); it != skins.end(); ++it)
            {
                glTF::Skin& skin = m_skins[skinIdx++];

                json& thisSkin = it.value();

                skin.inverseBindMatrices = nullptr;
                skin.skeleton = nullptr;

                if (thisSkin.find("inverseBindMatrices") != thisSkin.end())
                    skin.inverseBindMatrices = &m_accessors[thisSkin.at("inverseBindMatrices")];

                if (thisSkin.find("skeleton") != thisSkin.end())
                {
                    skin.skeleton = &m_nodes[thisSkin.at("skeleton")];
                    skin.skeleton->skeletonRoot = true;
                }

                json& joints = thisSkin.at("joints");
                skin.joints.reserve(joints.size());
                for (auto& joint : joints)
                    skin.joints.push_back(&m_nodes[joint]);
            }
        }

        void ProcessMeshes(json& meshes, json& accessors)
        {
            m_meshes.resize(meshes.size());

            uint32_t curMesh = 0;
            for (json::iterator meshIt = meshes.begin(); meshIt != meshes.end(); ++meshIt, ++curMesh)
            {
                json& thisMesh = meshIt.value();
                json& primitives = thisMesh.at("primitives");

                m_meshes[curMesh].primitives.resize(primitives.size());
                m_meshes[curMesh].skin = -1;

                if (thisMesh.find("name") != thisMesh.end()) {
                    m_meshes[curMesh].name = thisMesh.at("name");
                }

                uint32_t curSubMesh = 0;
                for (json::iterator primIt = primitives.begin(); primIt != primitives.end(); ++primIt, ++curSubMesh)
                {
                    glTF::Primitive& prim = m_meshes[curMesh].primitives[curSubMesh];
                    json& thisPrim = primIt.value();

                    prim.attribMask = 0;
                    json& attributes = thisPrim.at("attributes");

                    FindAttribute(prim, attributes, Primitive::kPosition, "POSITION");
                    FindAttribute(prim, attributes, Primitive::kNormal, "NORMAL");
                    FindAttribute(prim, attributes, Primitive::kTangent, "TANGENT");
                    FindAttribute(prim, attributes, Primitive::kTexcoord0, "TEXCOORD_0");
                    FindAttribute(prim, attributes, Primitive::kTexcoord1, "TEXCOORD_1");
                    FindAttribute(prim, attributes, Primitive::kColor0, "COLOR_0");
                    FindAttribute(prim, attributes, Primitive::kJoints0, "JOINTS_0");
                    FindAttribute(prim, attributes, Primitive::kWeights0, "WEIGHTS_0");

                    // Read position AABB
                    json& positionAccessor = accessors[attributes.at("POSITION").get<uint32_t>()];
                    ReadFloats(positionAccessor.at("min"), prim.minPos);
                    ReadFloats(positionAccessor.at("max"), prim.maxPos);

                    //prim.mode = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                    prim.mode = 4;
                    prim.indices = nullptr;
                    prim.material = nullptr;
                    prim.minIndex = 0;
                    prim.maxIndex = 0;
                    prim.mode = 4;

                    if (thisPrim.find("mode") != thisPrim.end())
                        prim.mode = thisPrim.at("mode");

                    if (thisPrim.find("indices") != thisPrim.end())
                    {
                        uint32_t accessorIndex = thisPrim.at("indices");
                        json& indicesAccessor = accessors[accessorIndex];
                        prim.indices = &m_accessors[accessorIndex];
                        if (indicesAccessor.find("max") != indicesAccessor.end())
                            prim.maxIndex = indicesAccessor.at("max")[0];
                        if (indicesAccessor.find("min") != indicesAccessor.end())
                            prim.minIndex = indicesAccessor.at("min")[0];
                    }

                    if (thisPrim.find("material") != thisPrim.end())
                        prim.material = &m_materials[thisPrim.at("material")];

                    // TODO:  Add morph targets
                    //if (thisPrim.find("targets") != thisPrim.end())
                }

                if (thisMesh.find("extras") != thisMesh.end()) {
                    if (thisMesh.at("extras").find("CollisionType") != thisMesh.at("extras").end()) {
                        uint8_t type = thisMesh.at("extras").at("CollisionType");
                        m_meshes[curMesh].collisionType = (Mesh::eCollisionType)type;
                        ReadFloats(thisMesh.at("extras").at("Bounding"), m_meshes[curMesh].Bounding);
                    }
                }
            }
        }

        void ProcessNodes(json& nodes)
        {
            m_nodes.resize(nodes.size());

            uint32_t nodeIdx = 0;

            for (json::iterator it = nodes.begin(); it != nodes.end(); ++it)
            {
                glTF::Node& node = m_nodes[nodeIdx++];
                json& thisNode = it.value();

                node.flags = 0;
                node.mesh = nullptr;
                node.linearIdx = -1;
                node.ParentIdx = -1;

                if (thisNode.find("name") != thisNode.end()) {
                    node.name = thisNode.at("name");
                }

                if (thisNode.find("camera") != thisNode.end())
                {
                    node.camera = &m_cameras[thisNode.at("camera")];
                    node.pointsToCamera = true;
                }
                else if (thisNode.find("mesh") != thisNode.end())
                {
                    node.mesh = &m_meshes[thisNode.at("mesh")];
                }

                if (thisNode.find("skin") != thisNode.end())
                {
                    assert(node.mesh != nullptr);
                    node.mesh->skin = thisNode.at("skin");
                }

                if (thisNode.find("children") != thisNode.end())
                {
                    json& children = thisNode["children"];
                    node.children.reserve(children.size());
                    for (auto& child : children) {
                        m_nodes[child].ParentIdx = nodeIdx - 1;
                        node.children.push_back(&m_nodes[child]);
                    }
                        
                }

                if (thisNode.find("matrix") != thisNode.end())
                {
                    // TODO:  Should check for negative determinant to reverse triangle winding
                    ReadFloats(thisNode["matrix"], node.matrix);
                    node.hasMatrix = true;
                }
                else
                {
                    // TODO:  Should check scale for 1 or 3 negative values to reverse triangle winding
                    json::iterator scale = thisNode.find("scale");
                    if (scale != thisNode.end())
                    {
                        ReadFloats(scale.value(), node.scale);
                    }
                    else
                    {
                        node.scale[0] = 1.0f;
                        node.scale[1] = 1.0f;
                        node.scale[2] = 1.0f;
                    }

                    json::iterator rotation = thisNode.find("rotation");
                    if (rotation != thisNode.end())
                    {
                        ReadFloats(rotation.value(), node.rotation);
                    }
                    else
                    {
                        node.rotation[0] = 0.0f;
                        node.rotation[1] = 0.0f;
                        node.rotation[2] = 0.0f;
                        node.rotation[3] = 1.0f;
                    }

                    json::iterator translation = thisNode.find("translation");
                    if (translation != thisNode.end())
                    {
                        ReadFloats(translation.value(), node.translation);
                    }
                    else
                    {
                        node.translation[0] = 0.0f;
                        node.translation[1] = 0.0f;
                        node.translation[2] = 0.0f;
                    }
                }
            }
        }

        void ProcessAnimations(json& animations)
        {
            m_animations.resize(animations.size());
            uint32_t animIdx = 0;

            // Process all animations
            for (json::iterator it = animations.begin(); it != animations.end(); ++it)
            {
                json& thisAnimation = it.value();
                glTF::Animation& animation = m_animations[animIdx++];

                // Process this animation's samplers
                json& samplers = thisAnimation.at("samplers");
                animation.m_samplers.resize(samplers.size());
                uint32_t samplerIdx = 0;

                for (json::iterator it2 = samplers.begin(); it2 != samplers.end(); ++it2)
                {
                    json& thisSampler = it2.value();
                    glTF::AnimSampler& sampler = animation.m_samplers[samplerIdx++];
                    sampler.m_input = &m_accessors[thisSampler.at("input")];
                    sampler.m_output = &m_accessors[thisSampler.at("output")];
                    sampler.m_interpolation = AnimSampler::kLinear;
                    if (thisSampler.find("interpolation") != thisSampler.end())
                    {
                        const std::string& interpolation = thisSampler.at("interpolation");
                        if (interpolation == "LINEAR")
                            sampler.m_interpolation = AnimSampler::kLinear;
                        else if (interpolation == "STEP")
                            sampler.m_interpolation = AnimSampler::kStep;
                        else if (interpolation == "CATMULLROMSPLINE")
                            sampler.m_interpolation = AnimSampler::kCatmullRomSpline;
                        else if (interpolation == "CUBICSPLINE")
                            sampler.m_interpolation = AnimSampler::kCubicSpline;
                    }
                }

                // Process this animation's channels
                json& channels = thisAnimation.at("channels");
                animation.m_channels.resize(channels.size());
                uint32_t channelIdx = 0;

                for (json::iterator it2 = channels.begin(); it2 != channels.end(); ++it2)
                {
                    json& thisChannel = it2.value();
                    glTF::AnimChannel& channel = animation.m_channels[channelIdx++];
                    channel.m_sampler = &animation.m_samplers[thisChannel.at("sampler")];
                    json& thisTarget = thisChannel.at("target");
                    channel.m_target = &m_nodes[thisTarget.at("node")];
                    const std::string& path = thisTarget.at("path");
                    if (path == "translation")
                        channel.m_path = AnimChannel::kTranslation;
                    else if (path == "rotation")
                        channel.m_path = AnimChannel::kRotation;
                    else if (path == "scale")
                        channel.m_path = AnimChannel::kScale;
                    else if (path == "weights")
                        channel.m_path = AnimChannel::kWeights;
                }
            }
        }

        void ProcessCameras(json& cameras)
        {
            m_cameras.reserve(cameras.size());

            for (json::iterator it = cameras.begin(); it != cameras.end(); ++it)
            {
                glTF::Camera camera;
                json& thisCamera = it.value();

                if (thisCamera["type"] == "perspective")
                {
                    json& perspective = thisCamera["perspective"];
                    camera.type = Camera::kPerspective;
                    camera.aspectRatio = 0.0f;
                    if (perspective.find("aspectRatio") != perspective.end())
                        camera.aspectRatio = perspective.at("aspectRatio");
                    camera.yfov = perspective["yfov"];
                    camera.znear = perspective["znear"];
                    camera.zfar = 0.0f;
                    if (perspective.find("zfar") != perspective.end())
                        camera.zfar = perspective.at("zfar");
                }
                else
                {
                    camera.type = Camera::kOrthographic;
                    json& orthographic = thisCamera["orthographic"];
                    camera.xmag = orthographic["xmag"];
                    camera.ymag = orthographic["ymag"];
                    camera.znear = orthographic["znear"];
                    camera.zfar = orthographic["zfar"];
                    assert(camera.zfar > camera.znear);
                }

                m_cameras.push_back(camera);
            }
        }

        void ProcessScenes(json& scenes)
        {
            m_scenes.reserve(scenes.size());

            for (json::iterator it = scenes.begin(); it != scenes.end(); ++it)
            {
                glTF::Scene scene;
                json& thisScene = it.value();

                if (thisScene.find("nodes") != thisScene.end())
                {
                    json& nodes = thisScene["nodes"];
                    scene.nodes.reserve(nodes.size());
                    for (auto& node : nodes)
                        scene.nodes.push_back(&m_nodes[node]);
                }

                m_scenes.push_back(scene);
            }
        }

        void FindAttribute(Primitive& prim, json& attributes, Primitive::eAttribType type, const std::string& name)
        {
            json::iterator attrib = attributes.find(name);
            if (attrib != attributes.end())
            {
                prim.attribMask |= 1 << type;
                prim.attributes[type] = &m_accessors[attrib.value()];
            }
            else
            {
                prim.attributes[type] = nullptr;
            }
        }

        uint32_t ReadTextureInfo(json& info_json, glTF::Material::eTextureType Type, glTF::Material& material)
        {
            material.textures[Type] = nullptr;

            if (info_json.find("index") != info_json.end()) {
                material.textures[Type] = &m_textures[info_json.at("index")];
            }

            material.TextureTransform[0 + 5 * Type] = 0;
            material.TextureTransform[1 + 5 * Type] = 0;
            material.TextureTransform[2 + 5 * Type] = 1;
            material.TextureTransform[3 + 5 * Type] = 1;
            material.TextureTransform[4 + 5 * Type] = 0;

            if (info_json.find("extensions") != info_json.end()) {
                json& extensions = info_json.at("extensions");
                if (extensions.find("KHR_texture_transform") != extensions.end()) {
                    json& textureTransform = extensions.at("KHR_texture_transform");
                    if (textureTransform.find("offset") != textureTransform.end()) {
                        ReadFloats(textureTransform.at("offset"), &material.TextureTransform[0 + 5 * Type]);
                    }
                    if (textureTransform.find("scale") != textureTransform.end()) {
                        ReadFloats(textureTransform.at("scale"), &material.TextureTransform[2 + 5 * Type]);
                    }
                    if (textureTransform.find("rotation") != textureTransform.end()) {
                        ReadFloats(textureTransform.at("rotation"), &material.TextureTransform[4 + 5 * Type]);
                    }
                }
            }

            if (info_json.find("texCoord") != info_json.end())
                return info_json.at("texCoord");
            else
                return 0;
        }


    public:
        void ParseGLTFFile(const std::string& filepath)
        {
            // TODO:  add GLB support by extracting JSON section and BIN sections
            //https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#glb-file-format-specification

            ByteArray gltfFile;
            ByteArray chunk1Bin;

            std::string fileExt = My::ToLower(My::GetFileExtension(filepath));

            if (fileExt == "glb")
            {
                std::ifstream glbFile(filepath, std::ios::in | std::ios::binary);
                struct GLBHeader
                {
                    char magic[4];
                    uint32_t version;
                    uint32_t length;
                } header;
                glbFile.read((char*)&header, sizeof(GLBHeader));
                if (strncmp(header.magic, "glTF", 4) != 0)
                {
                    //Utility::Printf("Error:  Invalid glTF binary format\n");
                    printf("Error:  Invalid glTF binary format\n");
                    return;
                }
                if (header.version != 2)
                {
                    //Utility::Printf("Error:  Only glTF 2.0 is supported\n");
                    printf("Error:  Only glTF 2.0 is supported\n");
                    return;
                }

                uint32_t chunk0Length;
                char chunk0Type[4];
                glbFile.read((char*)&chunk0Length, 4);
                glbFile.read((char*)&chunk0Type, 4);
                if (strncmp(chunk0Type, "JSON", 4) != 0)
                {
                    //Utility::Printf("Error: Expected chunk0 to contain JSON\n");
                    printf("Error: Expected chunk0 to contain JSON\n");
                    return;
                }
                gltfFile = std::make_shared<std::vector<unsigned char>>(chunk0Length + 1);
                glbFile.read((char*)gltfFile->data(), chunk0Length);
                (*gltfFile)[chunk0Length] = '\0';

                uint32_t chunk1Length;
                char chunk1Type[4];
                glbFile.read((char*)&chunk1Length, 4);
                glbFile.read((char*)&chunk1Type, 4);
                if (strncmp(chunk1Type, "BIN", 3) != 0)
                {
                    //Utility::Printf("Error: Expected chunk1 to contain BIN\n");
                    printf("Error: Expected chunk1 to contain BIN\n");
                    return;
                }

                chunk1Bin = std::make_shared<std::vector<unsigned char>>(chunk1Length);
                glbFile.read((char*)chunk1Bin->data(), chunk1Length);
            }
            else
            {
                assert(fileExt == "gltf");

                // Null terminate the string (just in case)
                gltfFile = My::ReadFileSyncDirectly(filepath.c_str());
                if (gltfFile->size() == 0)
                    return;

                gltfFile->push_back('\0');
                chunk1Bin = std::make_shared<std::vector<unsigned char>>(0);
            }

            json root = json::parse((const char*)gltfFile->data());
            if (!root.is_object())
            {
                //Utility::Printf("Invalid glTF file: %s\n", filepath.c_str());
                printf("Invalid glTF file: %s\n", filepath.c_str());
                return;
            }

            // Strip off file name to get root path to other related files
            m_basePath = My::GetBasePath(filepath);
            size_t offset = m_basePath.find("/Asset/");
            m_AssetPath = m_basePath.substr(offset + 7);

            // Parse all state
            if (root.find("extensions") != root.end())
                ProcessExtensions(root.at("extensions"));
            if (root.find("buffers") != root.end())
                ProcessBuffers(root.at("buffers"), chunk1Bin);
            if (root.find("bufferViews") != root.end())
                ProcessBufferViews(root.at("bufferViews"));
            if (root.find("accessors") != root.end())
                ProcessAccessors(root.at("accessors"));
            if (root.find("images") != root.end())
                ProcessImages(root.at("images"));
            if (root.find("samplers") != root.end())
                ProcessSamplers(root.at("samplers"));
            if (root.find("textures") != root.end())
                ProcessTextures(root.at("textures"));
            if (root.find("materials") != root.end())
                ProcessMaterials(root.at("materials"));
            if (root.find("meshes") != root.end())
                ProcessMeshes(root.at("meshes"), root.at("accessors"));
            if (root.find("cameras") != root.end())
                ProcessCameras(root.at("cameras"));
            if (root.find("skins") != root.end())
                m_skins.resize(root.at("skins").size());
            if (root.find("nodes") != root.end())
                ProcessNodes(root.at("nodes"));
            if (root.find("skins") != root.end())
                ProcessSkins(root.at("skins"));
            if (root.find("scenes") != root.end())
                ProcessScenes(root.at("scenes"));
            if (root.find("animations") != root.end())
                ProcessAnimations(root.at("animations"));
            if (root.find("scene") != root.end())
                m_scene = &m_scenes[root.at("scene")];
        }

        std::shared_ptr<My::BaseSceneNode> ProcessNodeMesh(My::Scene& Scene, glTF::Node* pCurrNode)
        {
            std::shared_ptr<My::SceneGeometryNode> GeoNode;
            std::shared_ptr<My::SceneObjectGeometry> GeoObject;
            auto& mesh = pCurrNode->mesh;
            GeoNode = std::make_shared<My::SceneGeometryNode>(pCurrNode->name);
            GeoObject = std::make_shared<My::SceneObjectGeometry>();

            for (auto meshIt = mesh->primitives.begin(); meshIt != mesh->primitives.end(); meshIt++) {
                std::shared_ptr<My::SceneObjectMesh> GeoMesh;
                GeoMesh = std::make_shared<My::SceneObjectMesh>();


                //primitive type
                switch (meshIt->mode) {
                case 0:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypePointList);
                    break;
                case 1:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeLineStrip);
                    break;
                case 2:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeLineLoop);
                    break;
                case 3:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeLineList);
                    break;
                case 4:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeTriList);
                    break;
                case 5:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeTriStrip);
                    break;
                case 6:
                    GeoMesh->SetPrimitiveType(My::PrimitiveType::kPrimitiveTypeTriFan);
                    break;
                default:
                    assert(false);
                    printf("GLTF Mesh Type Parse Error!");
                    break;
                }

                //attributes   indice/vertex array
                //vertex data
                int morph_index = 0;
                My::VertexDataType vertexDataType;
                int componentSize;
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kPosition)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kPosition]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kPosition]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kPosition]->count * GetPerElementCount(vertexDataType)];
                    float* _debug_read_data = reinterpret_cast<float*>(meshIt->attributes[Primitive::eAttribType::kPosition]->dataPtr);
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kPosition]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kPosition]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("POSITION", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kPosition]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kNormal)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kNormal]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kNormal]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kNormal]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kNormal]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kNormal]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("NORMAL", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kNormal]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kTangent)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kTangent]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kTangent]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kTangent]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kTangent]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kTangent]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("TANGENT", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kTangent]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kTexcoord0)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kTexcoord0]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kTexcoord0]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kTexcoord0]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kTexcoord0]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kTexcoord0]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("TEXCOORD0", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kTexcoord0]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kTexcoord1)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kTexcoord1]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kTexcoord1]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kTexcoord1]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kTexcoord1]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kTexcoord1]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("TEXCOORD1", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kTexcoord1]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kColor0)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kColor0]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kColor0]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kColor0]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kColor0]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kColor0]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("COLOR0", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kColor0]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kJoints0)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kJoints0]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kJoints0]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kJoints0]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kJoints0]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kJoints0]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("JOINTS0", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kJoints0]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                if (TEST_BIT(meshIt->attribMask, Primitive::eAttribType::kWeights0)) {
                    vertexDataType = GetVertexDataType(meshIt->attributes[Primitive::eAttribType::kWeights0]->type);
                    componentSize = GetComponentDataSize(meshIt->attributes[Primitive::eAttribType::kWeights0]->componentType);
                    void* pData = new float[meshIt->attributes[Primitive::eAttribType::kWeights0]->count * GetPerElementCount(vertexDataType)];
                    memcpy(pData, meshIt->attributes[Primitive::eAttribType::kWeights0]->dataPtr, sizeof(float) * meshIt->attributes[Primitive::eAttribType::kWeights0]->count * GetPerElementCount(vertexDataType));
                    My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("WEIGHTS0", morph_index, vertexDataType
                        , pData
                        , meshIt->attributes[Primitive::eAttribType::kWeights0]->count * GetPerElementCount(vertexDataType));
                    GeoMesh->AddVertexArray(std::move(_v_array));
                }
                //indice data
                size_t restart_index = 0;
                uint32_t material_index = meshIt->material ? meshIt->material->index : -1;
                componentSize = GetComponentDataSize(meshIt->indices->componentType);
                My::IndexDataType index_type = GetIndexDataType(meshIt->indices->componentType);
                void* pData = nullptr;
                switch (index_type) {
                case My::kIndexDataTypeInt8:
                    pData = new uint8_t[meshIt->indices->count];
                    memcpy(pData, meshIt->indices->dataPtr, sizeof(uint8_t) * meshIt->indices->count);
                    break;
                case My::kIndexDataTypeInt16:
                    pData = new uint16_t[meshIt->indices->count];
                    memcpy(pData, meshIt->indices->dataPtr, sizeof(uint16_t) * meshIt->indices->count);
                    break;
                case My::kIndexDataTypeInt32:
                    pData = new uint32_t[meshIt->indices->count];
                    memcpy(pData, meshIt->indices->dataPtr, sizeof(uint32_t) * meshIt->indices->count);
                    break;
                case My::kIndexDataTypeInt64:
                    pData = new uint64_t[meshIt->indices->count];
                    memcpy(pData, meshIt->indices->dataPtr, sizeof(uint64_t) * meshIt->indices->count);
                    break;
                }
                My::SceneObjectIndexArray& _i_array = *new My::SceneObjectIndexArray(material_index, restart_index, index_type
                    , pData
                    , meshIt->indices->count);
                GeoMesh->AddIndexArray(std::move(_i_array));

                GeoObject->AddMesh(GeoMesh);

                //Material
                if (meshIt->material) {
                    GeoNode->AddMaterialRef(meshIt->material->name);
                    std::shared_ptr<My::SceneObjectMaterial> GeoMaterial = std::make_shared<My::SceneObjectMaterial>(meshIt->material->name);

                    for (int type = 0; type < Material::eTextureType::kNumTextures; type++) {
                        std::string name;
                        std::string pbrname;
                        switch (type)
                        {
                        case Material::eTextureType::kBaseColor:
                        {
                            name = "diffuse";
                            pbrname = "pbrdiffuse";
                            My::Vector4f BaseColorFactor = My::Vector4f(meshIt->material->baseColorFactor[0], meshIt->material->baseColorFactor[1]
                                , meshIt->material->baseColorFactor[2], meshIt->material->baseColorFactor[3]);
                            GeoMaterial->SetColor(name, BaseColorFactor);
                        }
                        break;
                        case Material::eTextureType::kMetallicRoughness:
                        {
                            name = "metallic";
                            GeoMaterial->SetParam(name, meshIt->material->metallicFactor);
                            name = "roughness";
                            GeoMaterial->SetParam(name, meshIt->material->roughnessFactor);
                            pbrname = "pbrmetallicroughness";
                        }
                        break;
                        case Material::eTextureType::kOcclusion:
                        {
                            pbrname = "pbrocclusion";
                        }
                        break;
                        case Material::eTextureType::kEmissive:
                        {
                            name = "emission";
                            pbrname = "pbremissive";
                            My::Vector4f EmissiveFactor = My::Vector4f(meshIt->material->emissiveFactor[0], meshIt->material->emissiveFactor[1],
                                meshIt->material->emissiveFactor[2], meshIt->material->emissiveFactor[3]);
                            GeoMaterial->SetColor(name, EmissiveFactor);
                        }
                        break;
                        case Material::eTextureType::kNormal:
                        {
                            name = "normal";
                            pbrname = "pbrnormal";
                            GeoMaterial->SetParam(name, meshIt->material->normalTextureScale);
                        }
                        break;
                        default:
                            break;
                        }

                        My::TextureTransform trans;
                        memset(&trans, 0, sizeof(My::TextureTransform));
                        trans.offset[0] = meshIt->material->TextureTransform[0 + type * 5];
                        trans.offset[1] = meshIt->material->TextureTransform[1 + type * 5];
                        trans.scale[0] = meshIt->material->TextureTransform[2 + type * 5];
                        trans.scale[1] = meshIt->material->TextureTransform[3 + type * 5];
                        trans.rotation = meshIt->material->TextureTransform[4 + type * 5];
                        GeoMaterial->SetTextureTransform(pbrname, trans);

                        if (!meshIt->material->textures[type]) {
                            continue;
                        }
                        std::string textureType = GetTextureType(Material::eTextureType(type));

                        std::string texPath = m_AssetPath + meshIt->material->textures[type]->source->path;
                        GeoMaterial->SetTexture(textureType, texPath);

                        if (meshIt->material->textures[type]->sampler) {
                            GeoMaterial->SetSampler(textureType, meshIt->material->textures[type]->sampler->filter,
                                meshIt->material->textures[type]->sampler->wrapS, meshIt->material->textures[type]->sampler->wrapT);
                        }
                    }

                    Scene.Materials[meshIt->material->name] = GeoMaterial;
                }
            }

            //Transform
            if (pCurrNode->hasMatrix) {
                My::Matrix4X4f mat;
                memcpy(mat, pCurrNode->matrix, 16 * sizeof(float));
                std::shared_ptr<My::SceneObjectTransform> trans;
                trans = std::make_shared<My::SceneObjectTransform>(mat);
                GeoNode->AppendChild(std::move(trans));
            }
            else {
                My::Matrix4X4f mat, quaternion;
                std::shared_ptr<My::SceneObjectTransform> trans;
                auto scale = My::BuildScaleMatrix(pCurrNode->scale[0], pCurrNode->scale[1], pCurrNode->scale[2], 1.0f);
                My::MatrixRotationQuaternion(quaternion, My::Quaternion(pCurrNode->rotation[0], pCurrNode->rotation[1], pCurrNode->rotation[2], pCurrNode->rotation[3]));
                auto translation = My::BuildTranslationMatrix(pCurrNode->translation[0], pCurrNode->translation[1], pCurrNode->translation[2], 1.0f);
                mat = translation * quaternion * scale;
                trans = std::make_shared<My::SceneObjectTransform>(mat);
                GeoNode->AppendChild(std::move(trans));
            }

            // CollisionType
            switch (mesh->collisionType) {
            case Mesh::eCollisionType::kSphere:
            {
                GeoObject->SetCollisionType(My::kSceneObjectCollisionTypeSphere);
            }
            break;
            case Mesh::eCollisionType::kBox:
            {
                GeoObject->SetCollisionType(My::kSceneObjectCollisionTypeBox);               
            }
            break;
            case Mesh::eCollisionType::kPlane:
            {
                GeoObject->SetCollisionType(My::kSceneObjectCollisionTypePlane);
            }
            break;
            default:
                break;
            }
            GeoObject->SetBounding(My::Vector3f(mesh->Bounding[0], mesh->Bounding[1], mesh->Bounding[2]));

            GeoNode->AddSceneObjectRef(mesh->name);
            Scene.LUT_Name_GeometryNode.emplace(mesh->name, GeoNode);
            Scene.GeometryNodes.emplace(pCurrNode->name, GeoNode);
            Scene.Geometries[mesh->name] = GeoObject;

            return GeoNode;
        }

        std::shared_ptr<My::BaseSceneNode> ProcessNodeCamera(My::Scene& Scene, glTF::Node* pNode)
        {
            std::shared_ptr<My::SceneCameraNode> CameraNode = std::make_shared<My::SceneCameraNode>();
            std::shared_ptr<My::SceneObjectCamera> CameraObject;

            std::string attribNear("near");
            std::string attribFar("far");
            std::string attribAspect("aspect");
            switch (pNode->camera->type)
            {
            case Camera::eType::kOrthographic:
            {
                CameraObject = std::make_shared<My::SceneObjectOrthogonalCamera>();
            }
            break;
            case Camera::eType::kPerspective:
            {
                CameraObject = std::make_shared<My::SceneObjectPerspectiveCamera>();
                std::string fov("fov");
                CameraObject->SetParam(fov, pNode->camera->yfov);
            }
            break;
            default:
                break;
            }
            CameraObject->SetParam(attribNear, pNode->camera->znear);
            CameraObject->SetParam(attribFar, pNode->camera->zfar);

            My::Vector3f translation;
            My::Quaternion ratation;
            My::Vector3f scale;
            translation = My::Vector3f(pNode->translation[0], pNode->translation[1], pNode->translation[2]);
            ratation = My::Quaternion(pNode->rotation[0], pNode->rotation[1], pNode->rotation[2], pNode->rotation[3]);
            scale = My::Vector3f(pNode->scale[0], pNode->scale[1], pNode->scale[2]);
            CameraNode->SetTranslation(translation);
            CameraNode->SetRotation(ratation);
            CameraNode->SetScale(scale);

            CameraNode->AddSceneObjectRef(pNode->name);
            Scene.CameraNodes.emplace(pNode->name, CameraNode);
            Scene.Cameras.emplace(pNode->name, CameraObject);
            return CameraNode;
        }

        void ProcessCurrentNode(std::shared_ptr<My::BaseSceneNode>& node, My::Scene& Scene, glTF::Node* pNode)
        {
            std::shared_ptr<My::BaseSceneNode> newNode;
            if (pNode->pointsToCamera) {
                // TODO: Multi Camera Support
                newNode = ProcessNodeCamera(Scene, pNode);;
            }
            else {
                newNode = ProcessNodeMesh(Scene, pNode);
            }

            if (pNode->children.size()) {
                for (auto child : pNode->children) {
                    ProcessCurrentNode(newNode, Scene, child);
                }
            }

            node->AppendChild(std::move(newNode));
        }

        void ProcessRootParentNode(std::shared_ptr<My::BaseSceneNode>& node, My::Scene& Scene, glTF::Node* pNode)
        {
            assert(!pNode->camera && !pNode->mesh && pNode->children.size());

            std::shared_ptr<My::BaseSceneNode> pRootNode = std::make_shared<My::BaseSceneNode>(pNode->name);

            for (auto childIt = pNode->children.begin(); childIt != pNode->children.end(); childIt++) {
                ProcessCurrentNode(pRootNode, Scene, *childIt);
            }
            node->AppendChild(std::move(pRootNode));
        }

        virtual std::unique_ptr<My::Scene> Parse(const std::string& buf, std::string RootNodeName)
        {
            std::unique_ptr<My::Scene> pScene(new My::Scene("GLTF Scene"));
            std::string gltfpath = _WORKING_DIRECTORY + std::string("/Asset/") + std::string(buf);
            pScene->m_AssetPath = gltfpath.substr(0, gltfpath.find_last_of('/') + 1);
            ParseGLTFFile(gltfpath);

            std::shared_ptr<My::BaseSceneNode> base_node = pScene->SceneGraph;

            assert(m_scenes.size() == 1);

            std::shared_ptr<My::BaseSceneNode> StructureNode;
            StructureNode = std::make_shared<My::SceneEmptyNode>(RootNodeName);
            
            for (auto nodeIt = m_nodes.begin(); nodeIt != m_nodes.end(); nodeIt++) {
                if (nodeIt->ParentIdx >= 0) {
                    //子节点由顶层父节点递归更新值
                    continue;
                }
                if (!nodeIt->mesh && !nodeIt->camera) {
                    if (!nodeIt->children.size()) {
                        std::cout << "Blank Node, Nothing In It! ERROR!" << "Node Name: " << nodeIt->name << std::endl;
                        //__debugbreak();
                        continue;
                    }
                    ProcessRootParentNode(StructureNode, *pScene, &*nodeIt);
                    continue;
                }

                ProcessCurrentNode(StructureNode, *pScene, &*nodeIt);
            }

            base_node->AppendChild(std::move(StructureNode));

            return pScene;
        }
    };


} // namespace glTF
