#pragma once
#include <nlohmann/json.hpp>
#include <Vector3.h>
#include <SRT.h>


//Vector3
inline void to_json(nlohmann::json& j, const Vector3& v)
{
    j = {
        {"x", v.x},
        {"y", v.y},
        {"z", v.z}
    };
}

inline void from_json(const nlohmann::json& j, Vector3& v)
{
    v = {
         j.value("x", 0.0f),
         j.value("y", 0.0f),
         j.value("z", 0.0f),
    };
}

//SRT
inline void to_json(nlohmann::json& j, const SRT& t)
{
    j = {
        {"scale", t.scale},
        {"rotate", t.rotate},
        {"translate", t.translate},
    };
}

inline void from_json(const nlohmann::json& j, SRT& t)
{
    t = {
         j.value("scale", Vector3{}),
         j.value("rotate", Vector3{}),
         j.value("translate", Vector3{}),
    };
}