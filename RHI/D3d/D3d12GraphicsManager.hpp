#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <DXGI1_4.h>
#include "GraphicsManager.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "SceneObject.hpp"
#include "SceneNode.hpp"
#include "GeometryStructure.h"
#include "Core/GraphicsCore.h"


namespace My {
    class D3d12GraphicsManager : public GraphicsManager
    {
    public:
        virtual int Initialize();
        virtual void Finalize();

        virtual void Tick();

        virtual void Clear();
        virtual void Draw();

    public:

    private:
        //bool InitBasicGraphicsParam();
    private:
        bool LoadScene();
        void CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<Vertex>& VertexVec);
        void CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexT>& VertexVec);
        void CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexNT>& VertexVec);
        void CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexNTT>& VertexVec);

    private:
        std::unique_ptr<D3dGraphicsCore::CD3dGraphicsCore> m_pGraphics;
        bool m_bLoadOK = false;
    };
}