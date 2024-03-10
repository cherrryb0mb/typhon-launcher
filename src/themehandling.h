#include "includes.h"

#define TYP 0
#define PS3 1
#define GZ 2
#define BZ2 3
#define JPG 4
#define ZIP 5
#define RAR 6
#define SVNZ 7
#define TYPHON 8

void themepath(std::string themename);
void savetheme();
void readtheme();
void listthemes(std::string path);
void printthemes(std::string path,bool cutsuf);
void changetheme();
void createthemedatadir();
int checkthemetype(std::string thischar);
void copytyphonthemedata();
void removethemesuffix();
void extracttheme();
void themeimport();
void exporttheme();
void startexporttheme();
void createthisthemedir();
void oldthemename();
void nexttyphontheme();
void importtheme();
void themelist(bool up);
void togglethememenu();
void getonlinethemes(bool allowed);
void gettheme();
void printonlinethemes();
