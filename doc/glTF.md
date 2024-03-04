模型加载顺序为，先加载gltf文件，然后解析依次读取scenes、nodes、meshes、accessors、bufferViews、buffers、materials、textures、images。其中每个mesh包括一个bufferViews和一个materials。每一层的递进都有数组下标来确定。

认为meshes里存的每一个元素就是一个网格数据

scene就是场景，一个gltf通常只有一个场景      std::unordered_map<std::string, std::shared_ptr<SceneObjectGeometry>>   Geometries;

scene里面有node,认为每个node就是一个物体Object      std::shared_ptr<SceneObjectGeometry>

node下有mesh，就是物体的所有网格属性                SceneObjectGeometry里面有std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;



## 一、BufferView

uint32_t buffer
uint32_t byteLength
uint32_t byteOffset
uint32_t byteStride
bool elementArrayBuffer

## 二、Accessor

### 1.ComponentType

enum { kByte, kUnsignedByte,  kShort, kUnsignedShort, kSignedInt, kUnsignedInt, kFloat}	

分别对应ComponentType的值为{5120, 5121, 5122, 5123, 5125, 5126}

### 2.Type

enum {kScalar,  kVec2, kVec3, kVec4, kMat2, kMat3, kMat4}

分别对应各自的tpye名称

### 3.

char* dataPtr
uint32_t stride
uint32_t count
uint16_t componentType
uint16_t type

## 三、Image

std::string path

## 四、Sampler

D3D12_FILTER filter;
D3D12_TEXTURE_ADDRESS_MODE wrapS;
D3D12_TEXTURE_ADDRESS_MODE wrapT;

## 五、Texture

Image* source
Sampler* sampler

## 六、Material

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

## 七、Primitive

### 1.AttribType

enum {kPosition, kNormal, kTangent, kTexcoord0, kTexcoord1, kColor0, kJoints0, kWeights0, kNumAttribs}







# All:

class Parser是完成解析的类，解析入口为函数Parser()。

在GenerateNode()函数中创建对应的节点来描述Mesh，Material等信息，并将其记录到Scene所对应的map中。



