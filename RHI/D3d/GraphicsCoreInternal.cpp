#include "GraphicsCore.h"
#include "Core/Resource/DDSTextureLoader.h"
#include "GraphicsStructure.h"
#include "Core/Utility.h"
#include <filesystem>
#include <algorithm>
#include <math.h>

void D3dGraphicsCore::CD3dGraphicsCore::LoadIBLTextures()
{
	m_IBLResource = std::make_unique<IBLImageResource>();

	std::vector<std::string> IBLFiles;
	std::string IBLLoadDirectory = _IBL_RESOURCE_DIRECTORY;
	std::string specular_suffix = "_specularIBL.dds";
	size_t specular_offset = specular_suffix.size();
	std::string diffuse_suffix = "_diffuseIBL.dds";
	size_t diffuse_offset = diffuse_suffix.size();

	int fileCount = std::count_if(std::filesystem::directory_iterator(IBLLoadDirectory), std::filesystem::directory_iterator{}, (bool (*)(const std::filesystem::path&))std::filesystem::is_regular_file);
	for (auto dir : std::filesystem::recursive_directory_iterator(IBLLoadDirectory)) {
		if (!dir.is_directory()) {
			if (dir.path().extension().string() == ".dds") {
				std::string path = dir.path().string();
				if (path.substr(path.size() - specular_offset, specular_offset) == specular_suffix) {
					LoadIBLDDSImage(path, specular_suffix);
				} else if (path.substr(path.size() - diffuse_offset, diffuse_offset) == diffuse_suffix) {
					LoadIBLDDSImage(path, diffuse_suffix);
				}
			}
		}
	}
	m_IBLResource->IBLImageCount = fileCount;
}

void D3dGraphicsCore::CD3dGraphicsCore::LoadIBLDDSImage(std::string ImagePath, std::string suffix)
{
	std::string specular_suffix = "_specularIBL.dds";
	std::string diffuse_suffix = "_diffuseIBL.dds";

	size_t suffix_offset = suffix.size();
	size_t pos = std::max(ImagePath.find_last_of('/'), ImagePath.find_last_of('\\'));
	std::string imageName = ImagePath.substr(pos + 1);
	imageName = imageName.substr(0, imageName.size() - suffix_offset);

	Texture *pSpecularTex, *pDiffuseTex;

	pSpecularTex = new Texture();
	pDiffuseTex = new Texture();

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t pitch = 0;
	uint64_t size = 0;
	int HeapIndex;
	std::string specularImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + specular_suffix;
	std::string diffuseImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + diffuse_suffix;

	D3D12_CPU_DESCRIPTOR_HANDLE HeapHandle = AllocateFromDescriptorHeap(2, HeapIndex);
	HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(specularImage).c_str(), size, false,
		pSpecularTex->GetAddressOf(), HeapHandle);
	if (FAILED(hr)) {
		ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
	}
	if (m_IBLResource->HeapIndex == -1) {
		m_IBLResource->HeapIndex = HeapIndex;
	}
	ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

	OffsetDescriptorHandle(HeapHandle, 1);
	hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(diffuseImage).c_str(), size, false,
		pDiffuseTex->GetAddressOf(), HeapHandle);
	if (FAILED(hr)) {
		ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
	}
	ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

	std::unique_ptr<IBLImageMap> map = std::make_unique<IBLImageMap>();
	map->pSpecular = pSpecularTex;
	map->pDiffuse = pDiffuseTex;
	m_IBLResource->IBLImages.emplace(imageName, std::move(map));
}