#include <SFML/Graphics.hpp>
#include "includes.h"
#include "shadermanager.h"


extern sf::Texture shadtexture;
void loadshadertex();
void initshaders(ShaderManager &shaderman);
void drawshadebackground(ShaderManager &shaderman);
void toggleshaders();
void updateshaders(ShaderManager &shaderman);
void loadShaderConfig();
