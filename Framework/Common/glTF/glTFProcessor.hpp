#pragma once
#include "glTFStructures.h"

namespace My {
	namespace glTF {
		class CGLTFProcessor {
		public:
			CGLTFProcessor();
			~CGLTFProcessor() {};
		public:
			bool ProcessText(const std::string& buf);

		private:
			glTF_Accessors m_Accessors;
			glTF_Asset m_Asset;
			glTF_BufferViews m_BufferViews;
			glTF_Buffers m_Buffers;
			glTF_Images m_Images;
			glTF_Materials m_Materials;
			glTF_Meshes m_Meshes;
			glTF_Nodes m_Nodes;
			glTF_Samplers m_Samplers;
			glTF_Scenes m_Scenes;
			glTF_Textures m_Textures;
		};

		CGLTFProcessor::CGLTFProcessor()
		{
			m_BufferViews.bufferIndex = -1;
			m_BufferViews.byteLength = -1;
			m_BufferViews.byteOffset = -1;
			m_BufferViews.targetType = glTF_BufferView_Target::glTF_BufferView_NONE;

			m_Textures.samplerIndex = -1;
			m_Textures.sourceIndex = -1;
		}

		bool CGLTFProcessor::ProcessText(const std::string& buf)
		{
			
			return true;
		}
	}
}
