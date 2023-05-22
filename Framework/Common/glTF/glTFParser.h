#pragma once
#include <iostream>
#include <unordered_map>
#include "portable.hpp"
#include "SceneParser.hpp"

namespace My {
	class glTFParser : implements SceneParser {
	public:
		glTFParser() = default;
		virtual ~glTFParser() = default;

		virtual std::unique_ptr<Scene> Parse(const std::string& buf);
	};
}

