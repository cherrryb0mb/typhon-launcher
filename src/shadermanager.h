#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
class ShaderManager{
	public:
		ShaderManager(int shadercount);
		~ShaderManager();
		sf::Shader& getShader(int id);
		bool loadShaderFile(int id, std::string path, sf::Shader::Type shadertype);
		void refreshShaders(int num_shaders); 
	private:
		std::unique_ptr<sf::Shader[]> m_shaderContainer;
};
