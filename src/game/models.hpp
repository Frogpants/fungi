#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../core/essentials.hpp"
#include "../renderer/render.hpp"

struct Model {
    std::vector<vec3> vertices;
    std::vector<Triangle> triangles;
};

inline bool LoadOBJ(
    const std::string& path,
    Model& outModel,
    vec3 position = vec3(0.0f),
    vec3 scale = vec3(1.0f)
) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to load model: " << path << std::endl;
        return false;
    }

    std::vector<vec3> tempVerts;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            tempVerts.emplace_back(x, y, z);
        }

        else if (type == "f") {
            int i1, i2, i3;
            char slash;

            ss >> i1 >> i2 >> i3;

            vec3 v0 = tempVerts[i1 - 1] * scale + position;
            vec3 v1 = tempVerts[i2 - 1] * scale + position;
            vec3 v2 = tempVerts[i3 - 1] * scale + position;

            outModel.triangles.push_back({
                v0, v2, v1
            });
        }
    }

    return true;
}

inline void AppendModelToScene(
    const Model& model,
    std::vector<Triangle>& scene
) {
    for (const Triangle& t : model.triangles)
        scene.push_back(t);
}

#endif
