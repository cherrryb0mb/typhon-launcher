#include "includes.h"
#include "sound.h"
#include "emuhandling.h"
#include "picturehandling.h"
#include "themehandling.h"

extern std::string checkpath(std::string orgpath, std::string defaultpath);
void checkpaths();
void cfgpath();
void saveconfig();
void checkcfg();
void readtyphonxml();
void readmenuxml();
void readinputxml();
void readlanguagexml();
