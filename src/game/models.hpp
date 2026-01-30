#ifndef MODELS_HPP
#define MODELS_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../core/essentials.hpp"
#include "../renderer/render.hpp"

struct Model {
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

    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::string line;

    auto parseIndex = [](const std::string& token) -> std::pair<int, int> {
        // Returns {vertexIndex, uvIndex} (both 0-based)
        int vi = -1, ti = -1;
        size_t firstSlash = token.find('/');
        size_t secondSlash = token.find('/', firstSlash + 1);

        if (firstSlash != std::string::npos) {
            vi = std::stoi(token.substr(0, firstSlash)) - 1;

            if (secondSlash != std::string::npos && secondSlash > firstSlash + 1) {
                ti = std::stoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
            } else if (firstSlash + 1 < token.size()) {
                ti = std::stoi(token.substr(firstSlash + 1)) - 1;
            }
        } else {
            vi = std::stoi(token) - 1;
        }

        return {vi, ti};
    };

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        }
        else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            uvs.emplace_back(u, v);
        }
        else if (type == "f") {
            std::vector<std::pair<int, int>> faceIndices;
            std::string token;

            while (ss >> token) {
                faceIndices.push_back(parseIndex(token));
            }

            if (faceIndices.size() < 3) continue;

            // Triangulate polygon faces
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                auto [vi0, ti0] = faceIndices[0];
                auto [vi1, ti1] = faceIndices[i];
                auto [vi2, ti2] = faceIndices[i + 1];

                vec3 v0 = vertices[vi0] * scale + position;
                vec3 v1 = vertices[vi1] * scale + position;
                vec3 v2 = vertices[vi2] * scale + position;

                vec2 uv0 = (ti0 >= 0 && ti0 < uvs.size()) ? uvs[ti0] : vec2(0.0f, 0.0f);
                vec2 uv1 = (ti1 >= 0 && ti1 < uvs.size()) ? uvs[ti1] : vec2(0.0f, 0.0f);
                vec2 uv2 = (ti2 >= 0 && ti2 < uvs.size()) ? uvs[ti2] : vec2(0.0f, 0.0f);

                outModel.triangles.push_back({
                    {v0, uv0},
                    {v1, uv1},
                    {v2, uv2}
                });
            }
        }
    }

    if (outModel.triangles.empty()) {
        std::cerr << "OBJ loaded but produced no triangles: " << path << std::endl;
        return false;
    }

    return true;
}

inline void AppendModelToScene(
    const Model& model,
    std::vector<Triangle>& scene
) {
    scene.insert(scene.end(), model.triangles.begin(), model.triangles.end());
}

#endif