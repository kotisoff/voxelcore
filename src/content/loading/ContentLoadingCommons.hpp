#pragma once

#include "data/dv.hpp"

#include <string>

template <typename T>
inline void process_properties(T& def, const std::string& name, const dv::value& root) {
    if (def.properties == nullptr) {
        def.properties = dv::object();
        def.properties["name"] = name;
    }
    for (auto& [key, value] : root.asObject()) {
        auto pos = key.rfind('@');
        if (pos == std::string::npos) {
            def.properties[key] = value;
            continue;
        }
        auto field = key.substr(0, pos);
        auto suffix = key.substr(pos + 1);
        process_method(def.properties, suffix, field, value);
    }
}
