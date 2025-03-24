// Termm--Fall 2020

#include <glm/ext.hpp>
#include <vector>
#include <unordered_map>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"

std::unordered_map<SceneNode*, PhongMaterial*> nodeMaterials;
std::vector<std::pair<SceneNode*, std::pair<glm::vec3, glm::vec3>>> spheres;
std::vector<std::pair<SceneNode*, std::pair<glm::vec3, glm::vec3>>> cubes;
std::vector<std::pair<SceneNode*, std::vector<glm::vec3>>> meshes;
float epislon = 0.01f; 

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
	Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {}
};
// Optional: Helper function if you don't have one
float simplexNoise(float x, float y) {
    // This is a placeholder for a proper noise function
    // You'll need to implement a proper noise function or use a library
    
    // Simple placeholder that creates some variation
    float dx = sin(x * 0.1f) * cos(y * 0.1f) * 10.0f;
    float dy = cos(x * 0.1f) * sin(y * 0.1f) * 10.0f;
    
    return 0.5f + 0.5f * sin(x + dx) * cos(y + dy);
}

// Clamp helper function if needed
float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
float smoothstep(float edge0, float edge1, float x) {
    // Clamp the input to [0,1]
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Apply the smoothstep formula
    return t * t * (3.0f - 2.0f * t);
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

	return false; 
}

void getNodes(SceneNode* node, glm::mat4 transform = glm::mat4(1.0f)) {
	// printf("node: %s\n", node->m_name.c_str());
	glm::mat4 currentTransform = transform * node->trans;

	glm::vec3 scale(
		glm::length(glm::vec3( currentTransform[0])),
		glm::length(glm::vec3( currentTransform[1])),
		glm::length(glm::vec3( currentTransform[2]))
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
			// printf("mesh: %s\n", mesh->m_name.c_str());
			std::vector<glm::vec3> transformedTriangles;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(currentTransform)));

			for (const auto& triangle : mesh->m_faces) {
				glm::vec3 v0 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v1], 1.0f));
				glm::vec3 v1 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v2], 1.0f));
				glm::vec3 v2 = glm::vec3(currentTransform * glm::vec4(mesh->m_vertices[triangle.v3], 1.0f));

				transformedTriangles.push_back(v0);
				transformedTriangles.push_back(v1);
				transformedTriangles.push_back(v2);
			}

			meshes.emplace_back(geometryNode, transformedTriangles);
			nodeMaterials[geometryNode] =  static_cast<PhongMaterial*>(geometryNode->m_material);
		}
	}

	for (SceneNode* child : node->children) {
		// printf("currentTransform: %s\n", currentTransform[0]);
		getNodes(child, currentTransform);
	}
}

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here... 
	size_t width = image.width();
	size_t height = image.height();
	float aspect = width / (float)height;
	float scale = tan(glm::radians(fovy / 2.0f));

	glm::vec3 w = glm::normalize(eye - view);
	glm::vec3 u = glm::normalize(glm::cross(up, w));
	glm::vec3 v = glm::cross(w, u);

	getNodes(root);

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			float px = (2 * (x + 0.5) / (float)width - 1) * aspect * scale;
			float py = (1 - 2 * (y + 0.5) / (float)height) * scale;
			glm::vec3 dir = glm::normalize(px * u + py * v - w);
			Ray ray(eye, dir);

			float closest_t = std::numeric_limits<float>::max();
			glm::vec3 hit_normal, hit_point;
			PhongMaterial* phongMaterial = nullptr;

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
				}
			}
			for (const auto& [node, triangleVertices] : meshes) {
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

				for (const Light* light : lights) {
					// printf("light: %s\n", light->name.c_str());
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
				// float t = py * 0.5f + 0.5f;

				// glm::vec3 sunset_top = glm::vec3(1.0, 0.4, 0.6);
				// glm::vec3 sunset_middle = glm::vec3(1.0, 0.6, 0.2);
				// glm::vec3 sunset_bottom = glm::vec3(1.0, 1.0, 0.4);

				// if (t < 0.5) {
				// 	image(x, y, 0) = (1 - t * 2) * sunset_top.r + (t * 2) * sunset_middle.r;
				// 	image(x, y, 1) = (1 - t * 2) * sunset_top.g + (t * 2) * sunset_middle.g;
				// 	image(x, y, 2) = (1 - t * 2) * sunset_top.b + (t * 2) * sunset_middle.b;
				// }
				// else {
				// 	t = (t - 0.5) * 2;
				// 	image(x, y, 0) = (1 - t) * sunset_middle.r + t * sunset_bottom.r;
				// 	image(x, y, 1) = (1 - t) * sunset_middle.g + t * sunset_bottom.g;
				// 	image(x, y, 2) = (1 - t) * sunset_middle.b + t * sunset_bottom.b;
				// }

// Light blue sky with clouds
float t = py * 0.5f + 0.5f;

// Sky colors - from lighter blue at top to slightly darker at bottom
glm::vec3 sky_top = glm::vec3(0.5, 0.7, 1.0);      // Light blue
glm::vec3 sky_bottom = glm::vec3(0.7, 0.8, 1.0);   // Even lighter blue near horizon

// Base sky gradient
glm::vec3 sky_color = (1.0f - t) * sky_top + t * sky_bottom;

// Cloud function - using a simple noise function
float cloud_density = 0.0f;

// Simple noise-based clouds
// px and py should be normalized screen coordinates
float noise_scale = 8.0f;
float cloud_x = px * noise_scale;
float cloud_y = py * noise_scale;

// Simplex noise function (you'll need to implement or include a noise library)
// This is a placeholder - replace with your actual noise function
float noise_val = simplexNoise(cloud_x, cloud_y);
float noise_val2 = simplexNoise(cloud_x * 2.0f + 1.3f, cloud_y * 2.0f + 2.4f) * 0.5f;

// Combine different noise frequencies for more natural clouds
cloud_density = noise_val + noise_val2;

// Make clouds appear only in upper part of sky and fade out near horizon
float cloud_height_mask = 1.0f - (py * 1.5f + 0.2f);
cloud_height_mask = clamp(cloud_height_mask, 0.0f, 1.0f);

// Cloud shape threshold and softness
float cloud_threshold = 0.55f;
float cloud_softness = 0.1f;
float cloud_amount = smoothstep(cloud_threshold, cloud_threshold + cloud_softness, 
                               cloud_density * cloud_height_mask);

// Cloud color (white with slight blueish tint)
glm::vec3 cloud_color = glm::vec3(1.0f, 1.0f, 1.0f);

// Blend sky and clouds
glm::vec3 final_color = mix(sky_color, cloud_color, cloud_amount);

// Write to image
image(x, y, 0) = final_color.r;
image(x, y, 1) = final_color.g;
image(x, y, 2) = final_color.b;
			}
		}
	}
}

