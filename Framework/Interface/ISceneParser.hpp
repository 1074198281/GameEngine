#pragma once
#include <memory>
#include <string>
#include "Interface.hpp"
#include "Scene.hpp"

namespace My {
    __Interface SceneParser
    {
    public:
        virtual std::unique_ptr<Scene> Parse(const std::string& buf, std::string RootNodeName = "") = 0;
    };
}
