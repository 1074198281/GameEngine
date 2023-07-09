模型加载顺序为，先加载gltf文件，然后解析依次读取scenes、nodes、meshes、accessors、bufferViews、buffers、materials、textures、images。其中每个mesh包括一个bufferViews和一个materials。每一层的递进都有数组下标来确定。

认为meshes里存的每一个元素就是一个网格数据

scene就是场景，一个gltf通常只有一个场景      std::unordered_map<std::string, std::shared_ptr<SceneObjectGeometry>>   Geometries;

scene里面有node,认为每个node就是一个物体Object      std::shared_ptr<SceneObjectGeometry>

node下有mesh，就是物体的所有网格属性                SceneObjectGeometry里面有std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;