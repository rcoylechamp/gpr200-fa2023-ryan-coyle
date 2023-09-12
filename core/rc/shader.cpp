#include "shader.h"

//shader.cpp

namespace rc{
std::string loadShaderSourceFromFile(const std::string& filePath)
{
	std::ifstream fstream(filePath);
	if (!fstream.is_open()) {
		printf("Failed to load file %s", filePath);
		return {};
	}
	std::stringstream buffer;
	buffer << fstream.rdbuf();
	return buffer.str();
}
}
