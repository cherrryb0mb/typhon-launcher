#include "TConfig.h"

extern TyphonConfig *tycfg;
extern TyphonConfig *emcfg;
extern TyphonConfig *incfg;
extern TyphonConfig *thcfg;
extern TyphonConfig *rtcfg;
extern TyphonConfig *rmcfg;
extern TyphonConfig *mecfg;
extern TyphonConfig *shcfg;
extern TyphonConfig *vicfg;
extern TyphonConfig *p3cfg;
extern TyphonConfig *lacfg;
extern TyphonConfig *oncfg;
extern TyphonConfig *prcfg;

void inittyconf();
extern int setglobaldefaults();
extern int setmenudefaults();
extern int setinputdefaults();
extern int setemudefaults();
extern int setthemedefaults();
extern int setsetdefaults();
extern int setshaderdefaults();

extern int setlanguagedefaults();

void setrtdefaults();
void loadthemexmlcfg();
void savethemexmlcfg();

void loadmenuxmlcfg();
void savemenuxmlcfg();
void loadglobalxmlcfg();
void saveglobalxmlcfg();
void loadinputxmlcfg();
void saveinputxmlcfg();
void loademuxmlcfg();
void saveemuxmlcfg();
void saveglobalcfgs();
void loadromxmlcfg();

void setcs(int thiscs);
int Ishowmenu();
int Ics();
int Iem();											// emumenu
void setem(int thisem);
void incem(int bythis);								// emumenu
int Ime();											// menuentries 
int IeID();											// emulator X of 21

std::string rompath(int thisrom);
std::string emutitle(int thisemu);
std::string romname(int thisemu);
std::string emuargs(int thisrom);
std::string emulator(int thisemu);
std::string romsuffix(int thisemu);
std::string rompicturespath(int thisemu);
std::string mentry(int menu,int count);
std::string margs(int menu,int count);
std::string title(int menu);						// title
std::string gTex(std::string texof);				// return a texture

void sTex(std::string texof,std::string thistex);	// set texture

bool useShader();
int useBackShader();
bool Bvidmenu();
bool Bp3tmenu();
bool Bthememenu();
