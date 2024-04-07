#include "shadermanager.h"
#include <SFML/Graphics/Shader.hpp>
#include <filesystem>
#include <iostream>
namespace fs =  std::filesystem;

ShaderManager::ShaderManager(int num_shaders)
	:m_shaderContainer(new sf::Shader[num_shaders])
{
	if (m_shaderContainer == NULL){
          std::cerr << "Could not allocate interal shader container!";
	  exit(1);
        }
}

//nothing to do for now; the unique ptr should handle destroying the array itself
ShaderManager::~ShaderManager(){}

bool ShaderManager::loadShaderFile(int id, std::string path, sf::Shader::Type shadertype){
	if (id < 0){
		std::cerr << "Bad shader id:" <<  id;
		return false;
	}
	fs::file_status status =  fs::status(path);
	if(status.type() == fs::file_type::not_found){
		std::cerr << "Shader file for shader " << id << " was not found!";
		return false;
	}
	else{
		//you passed the vibe check...almost.
		if(!m_shaderContainer[id].loadFromFile(path, shadertype)){
			std::cerr << "Shader " << id << " failed to load";
			return false;
		}
		return true;
	}
}
sf::Shader& ShaderManager::getShader(int id){
	return m_shaderContainer[id];
}

