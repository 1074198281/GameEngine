#pragma once

#include "SceneObjectTexture.hpp"
#include "geommath.hpp"
#include <memory>

namespace My {
    template <typename T>
    struct ParameterValueMap
    {
        T Value;
        std::shared_ptr<SceneObjectTexture> ValueMap;

        ParameterValueMap() = default;

        ParameterValueMap(const T value) : Value(value) {};
        ParameterValueMap(const std::shared_ptr<SceneObjectTexture>& value) : ValueMap(value) {};

        ParameterValueMap(const ParameterValueMap<T>& rhs) = default;

        ParameterValueMap(ParameterValueMap<T>&& rhs) = default;

        ParameterValueMap& operator=(const ParameterValueMap<T>& rhs) = default;
        ParameterValueMap& operator=(ParameterValueMap<T>&& rhs) = default;
        ParameterValueMap& operator=(const std::shared_ptr<SceneObjectTexture>& rhs)
        {
            ValueMap = rhs;
            return *this;
        };
        ParameterValueMap& operator=(const T& value)
        {
            Value = value;
        }

        ~ParameterValueMap() = default;

        friend std::ostream& operator<<(std::ostream& out, const ParameterValueMap<T>& obj)
        {
            out << "Parameter Value: " << obj.Value << std::endl;
            if (obj.ValueMap) {
                out << "Parameter Map: " << *obj.ValueMap << std::endl;
            }

            return out;
        }
    };

    typedef ParameterValueMap<Vector4f> Color;
    typedef ParameterValueMap<Vector3f> Normal;
    typedef ParameterValueMap<float>    Parameter;
}
