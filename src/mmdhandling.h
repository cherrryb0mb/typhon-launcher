
#ifdef WITHMMD

#include "timerfunc.h"
#include <mmdconfig.h>
#include <PMDModel.h>
#include <VMDMotion.h>
#include <BulletPhysics.h>

void listmmd(std::string searchdir,std::string suffix,std::string cat1,std::string cat2);
void initmodelgl();
void quitmodel();
int openmodel();
void openmotion();
void idlemodel();
void displaymodel();
void updatemodel(bool up);
void updatemotion(bool up);
void togglephysics();
void togglemotionloop();
#endif // WITHMMD
