// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

#include <sstream>

Mesh::Mesh(const std::string& fname)
	: m_vertices(), m_faces()
{
	std::string line;

	std::ifstream ifs(fname.c_str());
	if (!ifs) {
		std::cerr << "Failed to open OBJ file: " << fname << std::endl;
		return;
	}

	while (std::getline(ifs, line)) {
		std::istringstream ss(line);
		std::string code;
		ss >> code;

		if (code == "v") {
			double vx, vy, vz;
			ss >> vx >> vy >> vz;
			m_vertices.push_back(glm::vec3(vx, vy, vz));
		}
		else if (code == "f") {
			std::string v_str[3];
			ss >> v_str[0] >> v_str[1] >> v_str[2];

			auto parseIndex = [&](const std::string& token) -> size_t {
				if (token.empty()) return std::numeric_limits<size_t>::max(); // Invalid

				std::istringstream tokenStream(token);
				std::string indexStr;
				std::getline(tokenStream, indexStr, '/'); // Get only the vertex index

				int idx = std::stoi(indexStr);
				if (idx < 0) idx = int(m_vertices.size()) + idx;
				else idx = idx - 1;

				if (idx < 0 || idx >= int(m_vertices.size())) return std::numeric_limits<size_t>::max();
				return size_t(idx);
			};

			size_t i1 = parseIndex(v_str[0]);
			size_t i2 = parseIndex(v_str[1]);
			size_t i3 = parseIndex(v_str[2]);

			if (i1 != std::numeric_limits<size_t>::max() &&
			    i2 != std::numeric_limits<size_t>::max() &&
			    i3 != std::numeric_limits<size_t>::max()) {
				m_faces.push_back(Triangle(i1, i2, i3));
			} else {
				std::cerr << "Skipped malformed face: " << line << std::endl;
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
