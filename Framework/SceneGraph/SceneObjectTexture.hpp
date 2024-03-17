#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "Image.hpp"
#include "Buffer.hpp"
#include "AssetLoader.hpp"
#include "ImageParserHeader.h"
#include <memory>
#include <string>
#include <ostream>

namespace My {
    class SceneObjectTexture : public BaseSceneObject
    {
    protected:
        std::string m_Name;
        uint32_t m_nTexCoordIndex;
        std::shared_ptr<Image> m_pImage;
        std::vector<Matrix4X4f> m_Transforms;


        //for sampler
        int m_filter;
        int m_wrapS;
        int m_wrapT;

    public:
        SceneObjectTexture() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(0) {};
        SceneObjectTexture(std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_Name(name), m_nTexCoordIndex(0) {};
        SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(coord_index), m_pImage(image) {};
        SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>&& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(coord_index), m_pImage(std::move(image)) {};
        SceneObjectTexture(SceneObjectTexture&) = default;
        SceneObjectTexture(SceneObjectTexture&&) = default;
        void AddTransform(Matrix4X4f& matrix) { m_Transforms.push_back(matrix); };
        void SetName(const std::string& name) { m_Name = name; };
        void SetName(std::string&& name) { m_Name = std::move(name); };
        std::string GetName() { return m_Name; };
        void SetSampler(int filter, int wrapS, int wrapT) { m_filter = filter; m_wrapS = wrapS; m_wrapT = wrapT; };
        int GetSamplerFilter() { return m_filter; };
        int GetSamplerWrapS() { return m_wrapS; };
        int GetSamplerWrapT() { return m_wrapT; };
        void LoadTexture() {
            AssetLoader assetLoader;
            if (!m_pImage)
            {
                // we should lookup if the texture has been loaded already to prevent
                // duplicated load. This could be done in Asset Loader Manager.
                std::string fullpath = m_Name;
                Buffer buf = assetLoader.SyncOpenAndReadBinary(fullpath.c_str());
                std::string ext = m_Name.substr(m_Name.find_last_of("."));
                if (ext == ".jpg" || ext == ".jpeg")
                {
                    JfifParser jfif_parser;
                    m_pImage = std::make_shared<Image>(jfif_parser.Parse(buf));
                }
                else if (ext == ".png")
                {
                    PngParser png_parser;
                    m_pImage = std::make_shared<Image>(png_parser.Parse(buf));
                }
                else if (ext == ".bmp")
                {
                    BmpParser bmp_parser;
                    m_pImage = std::make_shared<Image>(bmp_parser.Parse(buf));
                }
                else if (ext == ".tga")
                {
                    TgaParser tga_parser;
                    m_pImage = std::make_shared<Image>(tga_parser.Parse(buf));
                }
            }
        }

        Image& GetTextureImage()
        {
            if (!m_pImage)
            {
                LoadTexture();
            }

            return *m_pImage;
        };

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectTexture& obj);
    };
}