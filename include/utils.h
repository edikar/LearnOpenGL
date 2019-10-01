#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>

typedef enum {
	ERROR = -1,
	OK = 0
}STATUS;

void utilsPrintVec(glm::vec3 vec){
	std::cout << "( " << vec.x << " , " << vec.y << " , " << vec.z << " ) \n";
}

#endif