#include "includes.h"
#include <fxfont.h>

// norm255 taken from nvidia-widgets toolkit
#define norm255( i ) ( (float) ( i ) / 255.0 )
extern vec4f veccol4f(int a,int b,int c, int d);

extern FXFont 	font, musicdurationfontfx,
				titlefontfx,clockfontfx, datefontfx,
				sysinfofontfx, notifyfontfx, menuselfontfx,
				settingsfontfx, rssfontfx, counterfontfx, rominfofontfx;

extern std::string itos (int intos); // returns stringified integer
extern std::string ftos (float intos);

void updatefont();
void drawmenuseltext(std::string thistext);
void drawtitletext(std::string thistext);
void drawmenutext(std::string thistext,int showid);
void drawsettingstext(std::string thistext,bool align);
void drawrsstext(std::string thistext);
void listfonts();
void titletextinfo(std::string *thistext);
void setdropshadow();
void setdropshadowstrength(float ds);
void initfontshadow();
void initfontround();
void resizefonts();
void resizemenfont();
void resizemusfont();
void resizetitfont();
void resizedatfont();
void resizeclofont();
void resizesysfont();
void resizenotfont();
void resizerssfont();
void resizeromfont();
void resizerominfofont();
void resizesetfont();
void resizeselfont();
void drawtext(std::string info,std::string thistext,FXFont *thisfonttype);
void textcol(std::string thistext, FXFont *thisfont,int alpha);
void textrot(std::string thistext, std::string drawtext,FXFont *thisfont, int heightcheck);
void switchfont(bool up);
void drawgenerictext(std::string thistext,float depth);
