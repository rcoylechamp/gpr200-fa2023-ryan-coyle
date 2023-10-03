//texture.h
#pragma once
unsigned int loadTexture(const char* filePath);

//New function header in texture.h
unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode);