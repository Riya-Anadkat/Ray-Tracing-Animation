// Termm--Fall 2020

#include <glm/ext.hpp>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"
#include <omp.h>

#include <algorithm>

bool reflection_flag = true;

struct BVHNode {
	glm::vec3 bbox_min, bbox_max;
	BVHNode* left = nullptr;
	BVHNode* right = nullptr;
	std::vector<glm::vec3> triangles;

	bool isLeaf() const {
		return left == nullptr && right == nullptr;
	}
};
std::unordered_map<SceneNode*, PhongMaterial*> nodeMaterials;
std::vector<std::pair<SceneNode*, std::pair<glm::vec3, glm::vec3>>> spheres;
std::vector<std::pair<SceneNode*, std::pair<glm::vec3, glm::vec3>>> cubes;
std::vector<std::pair<SceneNode*, std::vector<glm::vec3>>> meshes;
std::vector<std::pair<SceneNode*, BVHNode*>> meshBVHs;
std::unordered_map<SceneNode*, glm::vec3> nodeVelocities;

float epislon = 0.01f;

struct Texture {
	int width, height;
	std::vector<glm::vec3> data;
};

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
	Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {}
};

glm::vec3 sampleTexture(const Texture& tex, float u, float v) {
	u = glm::clamp(u, 0.0f, 1.0f);
	v = glm::clamp(v, 0.0f, 1.0f);
	int x = int(u * (tex.width - 1));
	int y = int(v * (tex.height - 1));
	return tex.data[y * tex.width + x];  // row-major order
}

Texture loadPPM(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open texture file.");
	}

	std::string format;
	file >> format;
	if (format != "P3") {
		throw std::runtime_error("Only ASCII PPM (P3) format is supported.");
	}

	Texture texture;
	int maxval;
	file >> texture.width >> texture.height >> maxval;

	texture.data.resize(texture.width * texture.height);
	for (int i = 0; i < texture.width * texture.height; ++i) {
		int r, g, b;
		file >> r >> g >> b;
		texture.data[i] = glm::vec3(r, g, b) / 255.0f;
	}
	return texture;
}

float clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

bool checkIntresectCube(const glm::vec3& cube_min, const glm::vec3& cube_max, const Ray& ray, float& t_min, glm::vec3& hit_normal) {
	float t_near = -std::numeric_limits<float>::max();
	float t_far = std::numeric_limits<float>::max();
	glm::vec3 allFaceNormals[6] = { {1, 0, 0}, {-1, 0, 0},
	{0, 1, 0}, {0, -1, 0},
	{0, 0, 1}, {0, 0, -1} };

	float t0, t1;
	int hit_face = -1;

	for (int i = 0; i < 3; i++) {
		float invD = 1.0f / ray.direction[i];
		t0 = (cube_min[i] - ray.origin[i]) * invD;
		t1 = (cube_max[i] - ray.origin[i]) * invD;

		if (t0 > t1) std::swap(t0, t1);
		if (t0 > t_near) {
			t_near = t0;
			hit_face = i * 2 + (ray.direction[i] > 0 ? 1 : 0);
		}
		t_far = glm::min(t_far, t1);

		if (t_near > t_far) return false;
	}

	if (t_near < 0) return false;

	t_min = t_near;
	hit_normal = allFaceNormals[hit_face];
	return true;
}

bool checkIntersectTriangle(
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
	const Ray& ray, float& hitTime, glm::vec3& hitNormal)
{
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;

	glm::vec3 surfaceNormal = glm::normalize(glm::cross(edge1, edge2));

	if (glm::dot(surfaceNormal, ray.direction) > 0.0f) {
		return false;
	}
	glm::vec3 h = glm::cross(ray.direction, edge2);
	float determinant = glm::dot(edge1, h);

	if (fabs(determinant) < epislon) return false;

	float inverseDet = 1.0f / determinant;
	glm::vec3 s = ray.origin - v0;
	float u = glm::dot(s, h) * inverseDet;

	if (u < 0.0f || u > 1.0f) return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = glm::dot(ray.direction, q) * inverseDet;

	if (v < 0.0f || u + v > 1.0f) return false;

	hitTime = glm::dot(edge2, q) * inverseDet;

	if (hitTime < epislon) return false;
	hitNormal = surfaceNormal;
	return true;
}

bool checkintersectMesh(const std::vector<glm::vec3>& triangleVertices, const Ray& ray, float& tMin, glm::vec3& normal) {
	bool hit = false;
	tMin = std::numeric_limits<float>::max();

	for (size_t i = 0; i < triangleVertices.size(); i += 3) {
		float t;
		glm::vec3 tempNormal;

		if (checkIntersectTriangle(
			triangleVertices[i],
			triangleVertices[i + 1],
			triangleVertices[i + 2],
			ray, t, tempNormal) && t < tMin) {

			tMin = t;
			normal = tempNormal;
			hit = true;
		}
	}

	return hit;
}

bool checkIntersectSpheres(const glm::vec3& center, const glm::vec3& radii, const Ray& ray, float& t) {
	glm::vec3 oc = ray.origin - center;

	glm::vec3 invRadii = 1.0f / radii;
	glm::vec3 ocScaled = oc * invRadii;
	glm::vec3 dirscaled = ray.direction * invRadii;

	float a = glm::dot(dirscaled, dirscaled);
	float b = 2.0f * glm::dot(ocScaled, dirscaled);
	float c = glm::dot(ocScaled, ocScaled) - 1.0f;

	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return false;

	float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
	float t1 = (-b + sqrt(discriminant)) / (2.0f * a);

	if (t0 > 0) {
		t = t0;
		return true;
	}
	if (t1 > 0) {
		t = t1;
		return true;
	}

	return false;
}

BVHNode* buildBVH(std::vector<glm::vec3>& triangles, int depth = 0, int max_leaf_triangles = 6) {
	BVHNode* node = new BVHNode();

	node->bbox_min = node->bbox_max = triangles[0];
	for (size_t i = 0; i < triangles.size(); ++i) {
		node->bbox_min = glm::min(node->bbox_min, triangles[i]);
		node->bbox_max = glm::max(node->bbox_max, triangles[i]);
	}

	if (triangles.size() <= max_leaf_triangles * 3) {
		node->triangles = triangles;
		return node;
	}

	glm::vec3 extent = node->bbox_max - node->bbox_min;
	int axis = 0;
	if (extent.y > extent.x && extent.y > extent.z) axis = 1;
	else if (extent.z > extent.x && extent.z > extent.y) axis = 2;

	std::vector<std::pair<glm::vec3, size_t>> centers;
	for (size_t i = 0; i < triangles.size(); i += 3) {
		glm::vec3 center = (triangles[i] + triangles[i + 1] + triangles[i + 2]) / 3.0f;
		centers.emplace_back(center, i);
	}

	std::sort(centers.begin(), centers.end(), [axis](const auto& a, const auto& b) {
		return a.first[axis] < b.first[axis];
		});

	std::vector<glm::vec3> left_tris, right_tris;
	size_t mid = centers.size() / 2;
	for (size_t i = 0; i < centers.size(); ++i) {
		size_t idx = centers[i].second;
		glm::vec3 v0 = triangles[idx];
		glm::vec3 v1 = triangles[idx + 1];
		glm::vec3 v2 = triangles[idx + 2];
		if (i < mid) {
			left_tris.push_back(v0);
			left_tris.push_back(v1);
			left_tris.push_back(v2);
		}
		else {
			right_tris.push_back(v0);
			right_tris.push_back(v1);
			right_tris.push_back(v2);
		}
	}

	node->left = buildBVH(left_tris, depth + 1, max_leaf_triangles);
	node->right = buildBVH(right_tris, depth + 1, max_leaf_triangles);
	return node;
}


bool rayIntersectsAABB(const Ray& ray, const glm::vec3& minB, const glm::vec3& maxB) {
	float tmin = (minB.x - ray.origin.x) / ray.direction.x;
	float tmax = (maxB.x - ray.origin.x) / ray.direction.x;
	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (minB.y - ray.origin.y) / ray.direction.y;
	float tymax = (maxB.y - ray.origin.y) / ray.direction.y;
	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax)) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	float tzmin = (minB.z - ray.origin.z) / ray.direction.z;
	float tzmax = (maxB.z - ray.origin.z) / ray.direction.z;
	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	return !(tmin > tzmax || tzmin > tmax);
}

bool intersectBVH(BVHNode* node, const Ray& ray, float& closest_t, glm::vec3& hitNormal, glm::vec3& hitPoint) {
	if (!rayIntersectsAABB(ray, node->bbox_min, node->bbox_max)) return false;

	bool hit = false;
	if (node->isLeaf()) {
		for (size_t i = 0; i < node->triangles.size(); i += 3) {
			float t;
			glm::vec3 normal;
			if (checkIntersectTriangle(node->triangles[i], node->triangles[i + 1], node->triangles[i + 2], ray, t, normal) && t < closest_t) {
				closest_t = t;
				hitNormal = normal;
				hitPoint = ray.origin + t * ray.direction;
				hit = true;
			}
		}
	}
	else {
		glm::vec3 tempNormal, tempPoint;
		if (intersectBVH(node->left, ray, closest_t, hitNormal, hitPoint)) hit = true;
		if (intersectBVH(node->right, ray, closest_t, hitNormal, hitPoint)) hit = true;
	}
	return hit;
}
bool shouldBeInShadow(const glm::vec3& hitpoint, const glm::vec3& lightpos) {
	glm::vec3 lightdir = glm::normalize(lightpos - hitpoint);
	Ray shadowray(hitpoint + epislon * lightdir, lightdir);

	float lightdistance = glm::length(lightpos - hitpoint);

	for (const auto& [node, sphere] : spheres) {
		float t;
		if (checkIntersectSpheres(sphere.first, sphere.second, shadowray, t) && t < lightdistance) {
			return true;
		}
	}

	for (const auto& [node, cube] : cubes) {
		float t;
		glm::vec3 normal;
		if (checkIntresectCube(cube.first, cube.second, shadowray, t, normal) && t < lightdistance) {
			return true;
		}
	}

	for (const auto& [node, triangleVertices] : meshes) {
		float t;
		glm::vec3 normal;
		if (checkintersectMesh(triangleVertices, shadowray, t, normal) && t < lightdistance) {
			return true;
		}
	}
	for (const auto& [node, bvh] : meshBVHs) {
		float t;
		glm::vec3 normal, pt;
		if (intersectBVH(bvh, shadowray, t, normal, pt) && t < lightdistance) {
			return true;
		}
	}
	return false;
}
void getNodes(SceneNode* node, glm::mat4 transform = glm::mat4(1.0f)) {
	// printf("node: %s\n", node->m_name.c_str());
	glm::mat4 currentTransform = transform * node->trans;

	glm::vec3 scale(
		glm::length(glm::vec3(currentTransform[0])),
		glm::length(glm::vec3(currentTransform[1])),
		glm::length(glm::vec3(currentTransform[2]))
	);

	if (node->m_nodeType == NodeType::GeometryNode) {
		GeometryNode* geometryNode = static_cast<GeometryNode*>(node);

		if (auto* sphere = dynamic_cast<NonhierSphere*>(geometryNode->m_primitive)) {
			glm::vec3 worldPos = glm::vec3(currentTransform * glm::vec4(sphere->getPos(), 1.0));
			glm::vec3 worldRadii = sphere->getRadius() * scale;

			spheres.emplace_back(geometryNode, std::make_pair(worldPos, worldRadii));
			nodeMaterials[geometryNode] = static_cast<PhongMaterial*>(geometryNode->m_material);
		}

		if (auto* box = dynamic_cast<NonhierBox*>(geometryNode->m_primitive)) {
			glm::vec3 minCorner = glm::vec3(currentTransform * glm::vec4(box->getPos(), 1.0));
			glm::vec3 maxCorner = minCorner + box->getSize() * scale;

			cubes.emplace_back(geometryNode, std::make_pair(minCorner, maxCorner));
			nodeMaterials[geometryNode] = static_cast<PhongMaterial*>(geometryNode->m_material);
		}

		if (auto* mesh = dynamic_cast<Mesh*>(geometryNode->m_primitive)) {
			std::vector<glm::vec3> transformedTriangles;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(currentTransform)));

			size_t num_vertices = mesh->m_vertices.size();

			for (const auto& triangle : mesh->m_faces) {
		
				if (triangle.v1 >= num_vertices || triangle.v2 >= num_vertices || triangle.v3 >= num_vertices) {
					std::cerr << "Invalid triangle indices in mesh: "
						<< triangle.v1 << ", " << triangle.v2 << ", " << triangle.v3
						<< " (vertex count: " << num_vertices << ")" << std::endl;
					continue; 
				}

				glm::vec3 v0 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v1], 1.0f));
				glm::vec3 v1 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v2], 1.0f));
				glm::vec3 v2 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v3], 1.0f));

				transformedTriangles.push_back(v0);
				transformedTriangles.push_back(v1);
				transformedTriangles.push_back(v2);
			}
			BVHNode* bvh = buildBVH(transformedTriangles);
			if (geometryNode->m_name[0] == 's' || geometryNode->m_name[0] == 'c') {
	
				meshBVHs.emplace_back(geometryNode, bvh);
			}

			else {
				meshes.emplace_back(geometryNode, transformedTriangles);
			}
			nodeMaterials[geometryNode] = static_cast<PhongMaterial*>(geometryNode->m_material);
		}

	}

	for (SceneNode* child : node->children) {
		// printf("currentTransform: %s\n", currentTransform[0]);
		getNodes(child, currentTransform);
	}
}

void A4_Render(
	// What to render  
	SceneNode* root,

	// Image to write to, set to a given width and height  
	Image& image,

	// Viewing parameters  
	const glm::vec3& eye,
	const glm::vec3& view,
	const glm::vec3& up,
	double fovy,

	// Lighting parameters  
	const glm::vec3& ambient,
	const std::list<Light*>& lights
) {
	spheres.clear();
	cubes.clear();
	meshes.clear();
	meshBVHs.clear();
	nodeMaterials.clear();

	Texture* brickTex = new Texture(loadPPM("brick_texture.ppm"));
	Texture* spidermanTex = new Texture(loadPPM("spiderman_suit.ppm"));

	size_t width = image.width();
	size_t height = image.height();
	float aspect = width / (float)height;
	float scale = tan(glm::radians(fovy / 2.0f));

	glm::vec3 w = glm::normalize(eye - view);
	glm::vec3 u = glm::normalize(glm::cross(up, w));
	glm::vec3 v = glm::cross(w, u);

	getNodes(root);

	// for (size_t y = 0; y < height; y++) {
	omp_set_num_threads(100);
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < static_cast<int>(height); y++) {
		int thread_id = omp_get_thread_num();
		// printf("Row %d computed by thread %d\n", y, thread_id);

		for (int x = 0; x < static_cast<int>(width); x++) {

			float px = (2 * (x + 0.5) / (float)width - 1) * aspect * scale;
			float py = (1 - 2 * (y + 0.5) / (float)height) * scale;
			glm::vec3 dir = glm::normalize(px * u + py * v - w);
			Ray ray(eye, dir);

			float closest_t = std::numeric_limits<float>::max();
			glm::vec3 hit_normal, hit_point;
			PhongMaterial* phongMaterial = nullptr;
			glm::vec3 mod_color = glm::vec3(0.0f);
			bool use_colour_flag = false;

			for (const auto& [node, sphere] : spheres) {
				float t;
				if (checkIntersectSpheres(sphere.first, sphere.second, ray, t) && t < closest_t) {
					closest_t = t;
					hit_point = ray.origin + t * ray.direction;
					hit_normal = glm::normalize((hit_point - sphere.first) / sphere.second);
					phongMaterial = nodeMaterials[node];
				}
			}

			for (const auto& [node, cube] : cubes) {
				float t;
				glm::vec3 cube_normal;
				if (checkIntresectCube(cube.first, cube.second, ray, t, cube_normal) && t < closest_t) {
					closest_t = t;
					hit_point = ray.origin + t * ray.direction;
					hit_normal = cube_normal;
					phongMaterial = nodeMaterials[node];
					// printf("cube name: %s\n", node->m_name.c_str());
					if (node->m_name[0] == 'b') {

						use_colour_flag = true;
						mod_color = phongMaterial->m_kd;


						glm::vec3 local_hit = hit_point - cube.first;
						glm::vec3 cube_size = cube.second - cube.first;

						glm::vec3 uv = local_hit / cube_size;
						float u = 0.0f, v = 0.0f;
						auto fract = [](float x) { return x - floor(x); };

						int windows_per_face = 10;
						float window_unit = 1.0f / windows_per_face;
						float window_size = window_unit * 0.6f;

						bool is_window = false;

						int tile_x = 0, tile_y = 0;

						if (glm::abs(hit_normal.x) > 0.9f) {
							u = uv.z;
							v = uv.y;
							tile_x = int(uv.y / window_unit);
							tile_y = int(uv.z / window_unit);
							is_window = fract(uv.y / window_unit) < (window_size / window_unit) &&
								fract(uv.z / window_unit) < (window_size / window_unit);
						}
						else if (glm::abs(hit_normal.y) > 0.9f) {
							u = uv.x;
							v = uv.z;
							tile_x = int(uv.x / window_unit);
							tile_y = int(uv.z / window_unit);
							is_window = fract(uv.x / window_unit) < (window_size / window_unit) &&
								fract(uv.z / window_unit) < (window_size / window_unit);
						}
						else if (glm::abs(hit_normal.z) > 0.9f) {
							u = uv.x;
							v = uv.y;
							tile_x = int(uv.x / window_unit);
							tile_y = int(uv.y / window_unit);
							is_window = fract(uv.x / window_unit) < (window_size / window_unit) &&
								fract(uv.y / window_unit) < (window_size / window_unit);
						}

						int seed = int(cube.first.x + cube.first.y + cube.first.z + tile_x * 13 + tile_y * 17);
						float rand = fract(sin(seed * 91.345f) * 47453.0f);

						bool is_lit = rand < 0.3f;

						

						
						if (is_window) {
							if(reflection_flag){
								glm::vec3 reflect_dir = glm::reflect(ray.direction, hit_normal);
								Ray reflected_ray(hit_point + reflect_dir * epislon, reflect_dir);
						
								float t_reflect = std::numeric_limits<float>::max();
								glm::vec3 n_reflect, p_reflect;
								PhongMaterial* m_reflect = nullptr;
								glm::vec3 reflected_color = glm::vec3(0.0f);
						
								// Find the closest reflection hit
								for (const auto& [node2, sphere] : spheres) {
									float t;
									if (checkIntersectSpheres(sphere.first, sphere.second, reflected_ray, t) && t < t_reflect) {
										t_reflect = t;
										p_reflect = reflected_ray.origin + t * reflected_ray.direction;
										n_reflect = glm::normalize((p_reflect - sphere.first) / sphere.second);
										m_reflect = nodeMaterials[node2];
									}
								}
								for (const auto& [node2, cube2] : cubes) {
									float t;
									glm::vec3 normal2;
									if (checkIntresectCube(cube2.first, cube2.second, reflected_ray, t, normal2) && t < t_reflect) {
										t_reflect = t;
										p_reflect = reflected_ray.origin + t * reflected_ray.direction;
										n_reflect = normal2;
										m_reflect = nodeMaterials[node2];
									}
								}
								for (const auto& [node2, bvh] : meshBVHs) {
									if (intersectBVH(bvh, reflected_ray, t_reflect, n_reflect, p_reflect)) {
										m_reflect = nodeMaterials[node2];
									}
								}
								for (const auto& [node2, tri] : meshes) {
									if (node2->m_name[0] == 's') continue;
									float t;
									glm::vec3 n_temp;
									for (size_t i = 0; i < tri.size(); i += 3) {
										if (checkIntersectTriangle(tri[i], tri[i+1], tri[i+2], reflected_ray, t, n_temp) && t < t_reflect) {
											t_reflect = t;
											p_reflect = reflected_ray.origin + t * reflected_ray.direction;
											n_reflect = n_temp;
											m_reflect = nodeMaterials[node2];
										}
									}
								}
						
								if (m_reflect) {
									// Simple diffuse reflection (ambient only)
									reflected_color = m_reflect->m_kd ;
								} 
								else {
									// Reflect the sky
									float t = 0.5f * reflect_dir.y + 0.5f;
									reflected_color = (1.0f - t) * glm::vec3(1.0, 0.55, 0.25) + t * glm::vec3(0.6, 0.8, 1.0);

								}
								// Blend the reflection with base window color
								mod_color = 0.3f * mod_color + 0.7f * reflected_color;
								//mod_color = 0.1f * mod_color + 0.9f * reflected_color;

								use_colour_flag = true;
	
							}
							else if (is_lit) {
								mod_color = glm::vec3(1.0f, 0.85f, 0.6f);
							}
							else {
								mod_color *= 0.4f;
							}
						}
						else {
							// mod_color *= 0.8f;
							mod_color = 0.5 * mod_color + 0.5 * sampleTexture(*brickTex, u, v);
						}
					
					}
				}
			}
			for (const auto& [node, bvh] : meshBVHs) {
				if (intersectBVH(bvh, ray, closest_t, hit_normal, hit_point)) {
					phongMaterial = nodeMaterials[node];
					// printf("mesh: %s\n", node->m_name.c_str());
					if (node->m_name[0] == 's') {
						mod_color = phongMaterial->m_kd;
						use_colour_flag = true;

						


						// glm::vec3 local_hit = hit_point;
						// glm::vec3 v0 = hit_point - hit_normal * 0.001f;
						// float u = glm::fract(local_hit.x);
						// float v = glm::fract(local_hit.y);
						// u = glm::clamp(u, 0.0f, 1.0f);
						// v = glm::clamp(v, 0.0f, 1.0f);
						// mod_color = sampleTexture(*spidermanTex, u, v);
					}
				}
			}
			for (const auto& [node, triangleVertices] : meshes) {
				// printf("mesh: %s\n", node->m_name.c_str());
				if (node->m_name[0] == 's') {
					continue;
				}
				float t;
				glm::vec3 mesh_normal;

				for (size_t i = 0; i < triangleVertices.size(); i += 3) {
					if (checkIntersectTriangle(
						triangleVertices[i],
						triangleVertices[i + 1],
						triangleVertices[i + 2],
						ray, t, mesh_normal) && t < closest_t) {

						closest_t = t;
						hit_point = ray.origin + t * ray.direction;
						hit_normal = mesh_normal;
						phongMaterial = nodeMaterials[node];
					}
				}
			}
			if (closest_t < std::numeric_limits<float>::max() && phongMaterial) {
				glm::vec3 color = phongMaterial->m_kd * ambient;
				if (use_colour_flag == true) {
					color = mod_color * ambient;
				}
				for (const Light* light : lights) {
					if (!shouldBeInShadow(hit_point, light->position)) {
						glm::vec3 light_dir = glm::normalize(light->position - hit_point);
						float diff = glm::max(glm::dot(hit_normal, light_dir), 0.0f);
						glm::vec3 diffuse = diff * light->colour * phongMaterial->m_kd;

						glm::vec3 view_dir = glm::normalize(eye - hit_point);
						glm::vec3 reflect_dir = glm::reflect(-light_dir, hit_normal);
						float spec = pow(glm::max(glm::dot(view_dir, reflect_dir), 0.0f), phongMaterial->m_shininess);
						glm::vec3 specular = spec * light->colour * phongMaterial->m_ks;

						color += diffuse + specular;
					}
				}

				color = glm::clamp(color, 0.0f, 1.0f);

				image(x, y, 0) = color.r;
				image(x, y, 1) = color.g;
				image(x, y, 2) = color.b;
			}
			else {
				float t = py * 0.5f + 0.5f;

				glm::vec3 sky_top = glm::vec3(1.0, 0.4, 0.0);  // more orange
				glm::vec3 sky_bottom = glm::vec3(0.4, 0.7, 1.0);//blue

				glm::vec3 sky_color = (1.0f - t) * sky_top + t * sky_bottom;

				float cloud = 0.0f;

				cloud += 0.2f * sin(py * 40.0f + 0.5f * sin(px * 10.0f));
				cloud += 0.1f * sin(py * 60.0f + 2.0f * px);
				cloud *= 1.0f - px;
				float verticalGradient = 1.0f + 0.5f * px;
				cloud += 0.1f * sin((py + verticalGradient) * 40.0f + 0.5f * sin(px * 10.0f));
				cloud += 0.05f * sin((py + verticalGradient) * 60.0f + 2.0f * px);

				cloud = clamp(cloud, 0.0f, 1.0f);

				glm::vec3 cloud_color = glm::vec3(1.0f, 1.0f, 1.0f);
				glm::vec3 final_color = mix(sky_color, cloud_color, cloud);

				image(x, y, 0) = final_color.r;
				image(x, y, 1) = final_color.g;
				image(x, y, 2) = final_color.b;
			}
		}
	}
}

