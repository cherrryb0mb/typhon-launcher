#include "includes.h"
#include "fonthandling.h"
#include "emuhandling.h"

extern sf::Texture* getTexture(int s);
extern sf::Texture* getTextureS(std::string s);

extern float rfloat(float min, float max);
void updatepreview();
void forcepreviewupdate();
void updatewallpaper(std::string *thiswallpaper,sf::Texture *thisbackID);
void renderpicturevalue();
void drawwallpapers();
void drawpictures();
void togglemenuwallpaper();
void loadmenuicons();
void updateicon();

void drawmenuicons(float x, float y, float z);
void updateimage(std::string thisscannedpath,std::string thisimage, sf::Texture *thisimageID);
void initpictureids();
void updateimages();
void startscrotting( std::string programfile, std::string thisscrotpath);
void changemenuicon(bool up);
void changewallpaper(bool up);
void scaleicons();
void loadmenuwallpapers(int thiswp);
void texcol(std::string thistex);
void listpictures(std::string type,std::string searchpath);
void changepicture(std::string listcat,std::string iconcat, std::string counter, bool up);
void changemenuiconspace();
void takescreenshot();
void preppreview(std::string preppreviewpath);
