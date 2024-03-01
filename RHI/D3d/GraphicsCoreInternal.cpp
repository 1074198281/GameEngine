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

	std::unordered_map<std::string, int> ImageName;

	int fileCount = std::count_if(std::filesystem::directory_iterator(IBLLoadDirectory), std::filesystem::directory_iterator{}, (bool (*)(const std::filesystem::path&))std::filesystem::is_regular_file);

	// initialize heap
	m_IBLResource->IBLDescriptorHeap.Create(L"IBLDescriptorHeap", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, fileCount + 1);

	for (auto dir : std::filesystem::recursive_directory_iterator(IBLLoadDirectory)) {
		if (!dir.is_directory()) {
			if (dir.path().extension().string() == ".dds") {
				std::string path = dir.path().string();
				if (path.substr(path.size() - specular_offset, specular_offset) == specular_suffix) {
					LoadIBLDDSImage(path, specular_suffix, ImageName);
				} else if (path.substr(path.size() - diffuse_offset, diffuse_offset) == diffuse_suffix) {
					LoadIBLDDSImage(path, diffuse_suffix, ImageName);
				}
			}
		}
	}
	m_IBLResource->IBLImageCount = fileCount;


	// Load BRDF_LUT Image
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t pitch = 0;
	uint64_t size = 0;
	std::string BRDF_LUT_Name = std::string(_IBL_RESOURCE_DIRECTORY) + "/BRDF/" + "BRDF_LUT.dds";
	m_IBLResource->BRDF_LUT_Image = std::make_unique<Texture>();
	m_IBLResource->BRDF_LUT_Handle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
	HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(BRDF_LUT_Name).c_str(), size, false,
		m_IBLResource->BRDF_LUT_Image->GetAddressOf(), m_IBLResource->BRDF_LUT_Handle);
	if (FAILED(hr)) {
		ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
	}
	m_IBLResource->BRDF_offset = m_IBLResource->IBLImageCount;

	// dispaly first cube map

	if (m_IBLResource->CurrentCubemapIndex == -1) {
		m_IBLResource->CurrentCubemapIndex = 0;
		m_IBLResource->LastCubemapIndex = 0;
		DescriptorHandle Handle, prevHandle;
		int HeapIdx = -1;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> image_handle_vec;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> brdf_handle_vec;
		prevHandle = m_IBLResource->IBLFirstHandle;
		image_handle_vec.push_back(prevHandle);
		OffsetDescriptorHandle(prevHandle);
		image_handle_vec.push_back(prevHandle);
		Handle = AllocateFromDescriptorHeap(2, HeapIdx);
		m_IBLResource->FirstHandle = Handle;
		m_IBLResource->HeapIndex = HeapIdx;
		CopyDescriptors(Handle, image_handle_vec, 2);
		Handle = AllocateFromDescriptorHeap(1, HeapIdx);
		brdf_handle_vec.push_back(m_IBLResource->BRDF_LUT_Handle);
		CopyDescriptors(Handle, brdf_handle_vec, 1);
	}
}

void D3dGraphicsCore::CD3dGraphicsCore::LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName)
{
	std::string specular_suffix = "_specularIBL.dds";
	std::string diffuse_suffix = "_diffuseIBL.dds";

	size_t suffix_offset = suffix.size();
	size_t pos = std::max(ImagePath.find_last_of('/'), ImagePath.find_last_of('\\'));
	std::string imageName = ImagePath.substr(pos + 1);
	imageName = imageName.substr(0, imageName.size() - suffix_offset);

	if (ImageName.find(imageName) != ImageName.end()) {
		return;
	}

	std::unique_ptr<Texture> pSpecularTex, pDiffuseTex;

	pSpecularTex = std::make_unique<Texture>();
	pDiffuseTex = std::make_unique<Texture>();

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t pitch = 0;
	uint64_t size = 0;
	int HeapIndex;
	std::string specularImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + specular_suffix;
	std::string diffuseImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + diffuse_suffix;

	DescriptorHandle HeapHandle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
	if (m_IBLResource->IBLFirstHandle.IsNull()) {
		m_IBLResource->IBLFirstHandle = HeapHandle;
	}
	HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(specularImage).c_str(), size, false,
		pSpecularTex->GetAddressOf(), HeapHandle);
	if (FAILED(hr)) {
		ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
	}
	//if (m_IBLResource->HeapIndex == -1) {
	//	m_IBLResource->HeapIndex = HeapIndex;
	//}
	//ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

	HeapHandle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
	hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(diffuseImage).c_str(), size, false,
		pDiffuseTex->GetAddressOf(), HeapHandle);
	if (FAILED(hr)) {
		ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
	}
	//ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

	std::unique_ptr<IBLImageMap> map = std::make_unique<IBLImageMap>();
	map->name = imageName;
	map->pSpecular = std::move(pSpecularTex);
	map->pDiffuse = std::move(pDiffuseTex);
	m_IBLResource->IBLImages.emplace(m_IBLResource->IBLImages.size(), std::move(map));

	ImageName.emplace(imageName, m_IBLResource->IBLImages.size());
}