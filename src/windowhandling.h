#include "includes.h"

extern sf::RenderWindow window;

extern int rootX, rootY;
extern int oldrootX, oldrootY;
extern int changex;
extern int changey;

void mousepos(int rootX,int rootY);
void mousemoveobjects();
void mousescaleobjects();
void mouseblockobjects();
void mouserotobjects();
void mousezobjects(int thiszchange);
void mousechangedata(bool thiszchange);
void getpixelcolor();
void changecolpick();
void changerotpick();
void unblock();
void mousetogobject();
void mousepicktog();
void colorpicktog();
void setpicktog();
void changepick(bool thisbool);
int pick();
void mousechangebehaviour();
void mousedepthobjects(float thiszchange);
void windowpos(int x,int y);
void movewindow(bool up);
void toggleback();
