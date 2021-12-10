
#include "pch.h"
#include "obj_loader.h"

#include <fstream>
#include <sstream>

namespace platz {
	Vertexbuffer* OBJLoader::load(const std::string& path) {
		std::ifstream file(path);
		std::string line;

		struct Indices {
			int vertex;
			int uv;
			int normal;
		};

		struct Face {
			Indices indices[3];
		};

		std::vector<zmath::Vector3> rawVertices;
		std::vector<zmath::Vector2> rawUvs;
		std::vector<zmath::Vector3> rawNormals;
		std::vector<Face> faces;
		while (std::getline(file, line)) {

			if (line.at(0) == '#') {
				continue;
			}

			std::istringstream iss(line);
			std::string command;
			iss >> command;

			if (command == "v") {
				float x, y, z;
				iss >> x >> y >> z;
				rawVertices.push_back({ x, y , z });

			} else if (command == "vt") {
				float u, v;
				iss >> u >> v;
				rawUvs.push_back({ u, v });

			} else if (command == "vn") {
				float x, y, z;
				iss >> x >> y >> z;
				rawNormals.push_back({ x, y , z });

			} else if (command == "f") {

				std::string vertices[3];
				iss >> vertices[0] >> vertices[1] >> vertices[2];
				Indices indices[3];
				for (int i = 0; i < 3; i++) {
					std::istringstream vIss(vertices[i]);
					std::string p, t, n;
					std::getline(vIss, p, '/');
					std::getline(vIss, t, '/');
					std::getline(vIss, n, '/');
					indices[i] = { std::stoi(p) - 1, std::stoi(t) - 1, std::stoi(n) - 1};
				}
				faces.push_back({ indices[0], indices[1], indices[2] });
			}
		}		

		std::vector<Vertex> vertices;
		for (auto& face : faces) {
			for (int i = 0; i < 3; ++i) {
				auto position = rawVertices[face.indices[i].vertex];
				auto uv = rawUvs[face.indices[i].uv];
				auto normal = rawNormals[face.indices[i].normal];
				Vertex v = {
					{ position, 1 },
					uv,
					normal,
					{ 1, 1, 1, 1}
				};
				vertices.push_back(v);
			}
		}
		return new Vertexbuffer(vertices);
	}
}

