#include "glTFParser.h"
#include "glTFProcessor.hpp"

namespace My {
	std::unique_ptr<Scene> glTFParser::Parse(const std::string& buf)
	{
		std::unique_ptr<Scene> pScene(new Scene("glTF Scene"));
		glTF::CGLTFProcessor glTFProcessor;
		bool result = glTFProcessor.ProcessText(buf);

		return pScene;
	}
}