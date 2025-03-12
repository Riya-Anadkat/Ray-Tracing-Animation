// Termm--Fall 2020

#include <glm/ext.hpp>
#include <vector>

#include "A4.hpp"
#include "GeometryNode.hpp"

void getNodes(const SceneNode &root, glm::mat4 transform = glm::mat4(1.0f)) {
	glm::mat4 currentTransform = transform * root.trans;
	if (root.m_nodeType == NodeType::GeometryNode){
		const GeometryNode * geometryNode = dynamic_cast<const GeometryNode *>(&root);
		if(dynamic_cast<NonhierSphere*>(geometryNode->m_primitive)!=nullptr){
			printf("Found a sphere\n");
			printf("name: %s\n", geometryNode->m_name.c_str());
		}

		
	}


	for (const SceneNode * node : root.children) {
		getNodes(*node);
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

	std::vector<std::pair<glm::vec3, float>> spheres;
	getNodes(*root);

//   std::cout << "F20: Calling A4_Render(\n" <<
// 		  "\t" << *root <<
//           "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
//           "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
// 		  "\t" << "view: " << glm::to_string(view) << std::endl <<
// 		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
// 		  "\t" << "fovy: " << fovy << std::endl <<
//           "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
// 		  "\t" << "lights{" << std::endl;

// 	for(const Light * light : lights) {
// 		std::cout << "\t\t" <<  *light << std::endl;
// 	}
// 	std::cout << "\t}" << std::endl;
// 	std:: cout <<")" << std::endl;

// 	size_t h = image.height();
// 	size_t w = image.width();

// 	for (uint y = 0; y < h; ++y) {
// 		for (uint x = 0; x < w; ++x) {
// 			// Red: 
// 			image(x, y, 0) = (double)1.0;
// 			// Green: 
// 			image(x, y, 1) = (double)1.0;
// 			// Blue: 
// 			image(x, y, 2) = (double)1.0;
// 		}
// 	}

}
