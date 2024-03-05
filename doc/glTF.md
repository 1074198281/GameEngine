# GLTF

[TOC]

## 1.GLTF Parse

### 1.1 BufferView

```
uint32_t buffer
uint32_t byteLength
uint32_t byteOffset
uint32_t byteStride
bool elementArrayBuffer
```

### 1.2 Accessor

```
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
```

### 1.3 Image

```
std::string path
```

### 1.4 Sampler

```
D3D12_FILTER filter;
D3D12_TEXTURE_ADDRESS_MODE wrapS;
D3D12_TEXTURE_ADDRESS_MODE wrapT;
```

### 1.5 Texture

```
Image* source
Sampler* sampler
```

### 1.6 Material

    std::string name;
    union {
    	struct {
    		float baseColorFactor[4]
    		float metallicFactor
    		float roughnessFactor 
    	}
    }
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
    enum { kBaseColor, kMetallicRoughness, kOcclusion, kEmissive, kNormal, kNumTextures };
    Texture* textures[kNumTextures];
    uint32_t index;

### 1.7 Primitive

```
enum eAttribType { kPosition, kNormal, kTangent, kTexcoord0, kTexcoord1, kColor0, 			kJoints0, kWeights0, kNumAttribs };
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
```

### 1.8 Mesh

```
std::string name;
std::vector<Primitive> primitives;
int32_t skin;
```

### 1.9 Camera

```
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
```

### 1.10 Node

```
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
```

### 1.11 Skin

```
Accessor* inverseBindMatrices;  // An array of IBMs that match the order of joint nodes
Node* skeleton; // root node (if non-null, this node should be in the list of joints too)
std::vector<Node*> joints;
```

### 1.12 Scene

```
std::vector<Node*> nodes;
```

### 1.13 AnimSampler

```
Accessor* m_input;  // key frame time stamps
Accessor* m_output; // key frame values
enum eInterpolation { kLinear, kStep, kCatmullRomSpline, kCubicSpline };
eInterpolation m_interpolation;
```

### 1.14 AnimChannel

```
AnimSampler* m_sampler;
Node* m_target;
enum ePath { kTranslation, kRotation, kScale, kWeights };
ePath m_path;
```

### 1.15 Animation

```
std::vector<AnimChannel> m_channels;
std::vector<AnimSampler> m_samplers;
```

### 1.16 Parsing Progress

解析gltf文件主要由ParseGLTFFile(const std::wstring& filepath)接口完成，解析完成后会将所有gltf文件中的格式转化为上述的各结构体的成员变量中。在将成员变量转为场景格式时，以树的结构，通过ProcessCurrentNode()递归完成解析，最终将mesh等资源存入当前的scene中并返回。



