#include "glTFParser.h"

namespace My {
	std::unique_ptr<Scene> glTFParser::Parse(const std::string& buf)
	{
		std::unique_ptr<Scene> pScene(new Scene("glTF Scene"));

	}
}