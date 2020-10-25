#ifndef TEXTURE_CUBE_H
#define TEXTURE_CUBE_H

#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class CubemapTexture {
public:
    unsigned int textureID;
    int width;
    int height;
    int channels;

    CubemapTexture(std::vector<std::string> faces)
    {
        
        // TODO: fill in constructor code.
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for (int i = 0; i < faces.size(); i++) {
			unsigned char* texData = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

			if (texData) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			}
			else {
				std::cout << "Failed to load cubemp texture " << faces[i] << std::endl;
			}
			stbi_image_free(texData);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};
#endif