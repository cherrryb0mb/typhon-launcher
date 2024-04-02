#include "includes.h"
#include "picturehandling.h"
#include "glmenu.h"
#include "shadermanager.h"

void display(ShaderManager &shaderman);
void rotateme(std::string obj);
void setpos(std::string obj);
void drawcube(float cubex, float cubey,float cubez,std::string obj,std::string bobj);
void drawquad(float quadx, float quady);
void cuboquad(std::string obj,std::string bobj);
//void resizescreen(int x, int y);
void initgl();
