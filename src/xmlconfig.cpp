#include "xmlconfig.h"
#include "emuhandling.h"
#include "confighandling.h"
#include "inputhandling.h"
#include "shaderhandling.h"
#include "systeminfo.h"
#include "p3thandling.h"
#include "values.h"
#include "mmdhandling.h"
#include "videoplayerhandling.h"
#include "picturehandling.h"
#include "filehandling.h"

TyphonConfig *tycfg;
TyphonConfig *emcfg;
TyphonConfig *mecfg;
TyphonConfig *incfg;
TyphonConfig *thcfg;
TyphonConfig *rmcfg;
TyphonConfig *shcfg;
TyphonConfig *vicfg;
TyphonConfig *p3cfg;
TyphonConfig *lacfg;
TyphonConfig *oncfg;
TyphonConfig *prcfg;

TyphonConfig *rtcfg;

void inittyconf()
{
	tycfg = new TyphonConfig();
	emcfg = new TyphonConfig();
	mecfg = new TyphonConfig();
	incfg = new TyphonConfig();
	thcfg = new TyphonConfig();
	rmcfg = new TyphonConfig();
	shcfg = new TyphonConfig();
	vicfg = new TyphonConfig();
	p3cfg = new TyphonConfig();
	lacfg = new TyphonConfig();
	rtcfg = new TyphonConfig();
	prcfg = new TyphonConfig();

	oncfg = new TyphonConfig();
}

// runtime defaults

void setrtdefaults()
{	
	std::string defrtconfig=rtcfg->getString("systemconfigdir")+"typhon-defaultruntime.xml";
	FILE *rtcheck = fopen(defrtconfig.c_str(),"r");
	if(rtcheck)
	{
		rtcfg->loadXML(defrtconfig);
		fclose(rtcheck);
	}
	else
	{
		printf("ERROR - runtimeconfig %s not found - exiting!\n",defrtconfig.c_str());
		exit(1);
	}
}

int setlanguagedefaults()
{
	std::string deflangxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("lang")+tycfg->sub("strings")->getString("language")+".xml";
	FILE *xmlcfgcheck = fopen(deflangxmlconfig.c_str(),"r");
	if(xmlcfgcheck)
	{
		lacfg->loadXML(deflangxmlconfig);
		fclose(xmlcfgcheck);
	}
	else
	{
		std::string enlangxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("lang")+"en.xml";
		lacfg->loadXML(enlangxmlconfig);
	}
	return 0;
}

// menu config

int setmenudefaults()
{
	std::string defmenuxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("menu");
	FILE *defcheck = fopen(defmenuxmlconfig.c_str(),"r");
	if(defcheck)
	{
		mecfg->loadXML(defmenuxmlconfig);
		fclose(defcheck);
	}
	
	parsedesktopfiles();
	return 0;
}

// global config

int setglobaldefaults()
{
	std::string defglobalxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("global");

	FILE *defcheck = fopen(defglobalxmlconfig.c_str(),"r");
	if(defcheck)
	{
		tycfg->loadXML(defglobalxmlconfig);
		fclose(defcheck);
	}
	return 0;
}

// input config

int setinputdefaults()
{
	std::string definputxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("input");
	FILE *defcheck = fopen(definputxmlconfig.c_str(),"r");
	if(defcheck)
	{
		incfg->loadXML(definputxmlconfig);
		fclose(defcheck);
	}
	return 0;
}

// emulator config

int setemudefaults()
{
	std::string defemuxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("emu");
	FILE *defcheck = fopen(defemuxmlconfig.c_str(),"r");
	if(defcheck)
	{
		emcfg->loadXML(defemuxmlconfig);	
		fclose(defcheck);
	}
	return 0;
}

// shader config

int setshaderdefaults()
{
	std::string defshaderxmlconfig=rtcfg->getString("systemconfigdir")+rtcfg->sub("cfgs")->getString("shader");
	FILE *defcheck = fopen(defshaderxmlconfig.c_str(),"r");
	if(defcheck)
	{
		shcfg->loadXML(defshaderxmlconfig);
		fclose(defcheck);
	}
	return 0;
}

// theme config

int setthemedefaults()
{
	std::string defthemexmlconfig=rtcfg->getString("systemconfigdir")+"typhon.xml";

	FILE *defcheck = fopen(defthemexmlconfig.c_str(),"r");
	if(defcheck)
	{
		thcfg->loadXML(defthemexmlconfig);
		fclose(defcheck);
	}
	return 0;
}

// save/load
void loadmenuxmlcfg()	{mecfg->loadXML(rtcfg->sub("configs")->getString("menuxml"));}
void savemenuxmlcfg()	{
							for(int m = 1; m <  mecfg->size()+1; m++)
							{mecfg->sub(m-1)->set("savedentry",rtcfg->sub("curSel")->get(m));}
							mecfg->saveXML(rtcfg->sub("configs")->getString("menuxml"));
						}
void loadglobalxmlcfg()	{tycfg->loadXML(rtcfg->sub("configs")->getString("globalxml"));}
void saveglobalxmlcfg()	{tycfg->saveXML(rtcfg->sub("configs")->getString("globalxml"));}
void loadinputxmlcfg()	{incfg->loadXML(rtcfg->sub("configs")->getString("inputxml"));}
void saveinputxmlcfg()	{incfg->saveXML(rtcfg->sub("configs")->getString("inputxml"));}
void loademuxmlcfg()	{emcfg->loadXML(rtcfg->sub("configs")->getString("emuxml"));}
void saveemuxmlcfg()	{emcfg->saveXML(rtcfg->sub("configs")->getString("emuxml"));}
void loadromxmlcfg()	{rmcfg->loadXML(rtcfg->sub("configs")->getString("romxml"));}
void loadthemexmlcfg()	{thcfg->loadXML(rtcfg->sub("configs")->getString("themexml"));}
void savethemexmlcfg()	{thcfg->saveXML(rtcfg->sub("configs")->getString("themexml"));}

void saveglobalcfgs()
{
	saveglobalxmlcfg();
	savemenuxmlcfg();
	saveemuxmlcfg();
	saveinputxmlcfg();
	shownote(lacfg->getString("savedall"));
}

int Ishowmenu()
{
	return tycfg->getInt("showmenu");	
}

int Ics() //FYI: Ics() < 0 means you're in an empty menu.
{
	return rtcfg->sub("curSel")->getInt(Ishowmenu());
}

bool Bvidmenu()
{
	if (thcfg->sub("objects")->sub("video")->getInt("videomenu")==Ishowmenu()) return 1;
	else return 0;
}

bool Bp3tmenu()
{
	if (tycfg->sub("globalints")->getInt("p3tmenu")==Ishowmenu() && p3cfg->sub("ps3themes")->size()>0) return 1;
	else return 0;
}

bool Bthememenu()
{
	if (tycfg->sub("globalints")->getInt("thememenu")==Ishowmenu() && rtcfg->sub("themes")->size()>0) return 1;
	else return 0;
}

bool Bextramenu()
{
	if
	(
		tycfg->sub("globalints")->getInt("thememenu")==Ishowmenu()	||
		tycfg->sub("globalints")->getInt("p3tmenu")==Ishowmenu()	||
		Iem()!=0													||
		thcfg->sub("objects")->sub("video")->getInt("videomenu")==Ishowmenu()
	) return 1;
	
	else return 0;
}

int Iem()
{
	if(Ishowmenu()==0) return 0;
	return mecfg->sub(Ishowmenu()-1)->getInt("emumenu");
}

int Ime()
{
	if (Ishowmenu()==0)		return emcfg->size();
	else if(Iem()!=0)
	{
		if(rmcfg->sub("Roms")->sub(IeID()-1)==0) return 0;
		else return rmcfg->sub("Roms")->sub(IeID()-1)->size();
	}
	else if (Bp3tmenu()) 	return p3cfg->sub("ps3themes")->size();
	else if (Bthememenu()) 	return rtcfg->sub("themes")->size();
	else if (Bvidmenu())	return vicfg->sub("videos")->size();
	else 					return mecfg->sub(Ishowmenu()-1)->sub("entry")->size();
}

void setcs(int thiscs)
{
	if (Ics()>Ime())rtcfg->sub("curSel")->set(Ishowmenu(),Ime()-1);
	else 			rtcfg->sub("curSel")->set(Ishowmenu(),thiscs);
}

void setem(int thisem)
{
	mecfg->sub(Ishowmenu()-1)->set("emumenu",thisem);
}

void incem(int bythis)
{
	((TConfig<int>*) mecfg->sub(Ishowmenu()-1)->get("emumenu"))->operator+(bythis);
}

int IeID()
{
	if(Iem()!=0) return Ishowmenu()+((Iem()-1)*7);
	else return 0;
}

std::string rompath(int thisrom)
{
	return emcfg->sub(thisrom)->getString("rompath");
}

std::string emuargs(int thisrom)
{
	if(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->exists("args")) return rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getString("args");
	else return emcfg->sub(thisrom)->getString("emuargs");
}

std::string emutitle(int thisemu)
{
	return emcfg->sub(thisemu)->getString("emutitle");
}

std::string emulator(int thisemu)
{
	return emcfg->sub(thisemu)->getString("emulator");
}

std::string rompicturespath(int thisemu)
{
	return emcfg->sub(thisemu)->getString("rompicturespath");
}

std::string romsuffix(int thisemu)
{
	return emcfg->sub(thisemu)->getString("romsuffix");
}

std::string romname(int thisemu)
{
	return rmcfg->sub("Roms")->sub(thisemu)->sub(Ics())->getString("file");
}

std::string mentry(int menu,int count)
{
	if (menu==-1)
	{
		if(thcfg->sub("switches")->getBool("useemutitles")) 	return emutitle(count);
		else 		 											return emulator(count);
	}
	else
	{
		if (Iem()!=0)					return getPrefix(rmcfg->sub("Roms")->sub(IeID()-1)->sub(count)->getString("file"));
		else if (Bvidmenu())				return getPrefix(vicfg->sub("videos")->sub(count)->getString("file"));
		else if (Bp3tmenu())				return getPrefix(p3cfg->sub("ps3themes")->getString(count));
		else if (Bthememenu())				return getPrefix(rtcfg->sub("themes")->getString(count));
		else								
		{
			if(!mecfg->sub(menu)){
				std::cout << "\n\nbad menu id: " << std::to_string(count);
				std::cout << "\nmake sure typhon-menu.xml has valid menu entries!" << "\n\n";
				exit(-1);
			}
			if(!mecfg->sub(menu)->sub("entry")->sub(count)){
				return "Empty Menu";	
			}	
			if(tycfg->sub("globalswitches")->getBool("entrynames"))
				return mecfg->sub(menu)->sub("entry")->sub(count)->getString("name");
			else
				return mecfg->sub(menu)->sub("entry")->sub(count)->getString("binary");
		}
	}
}

std::string margs(int menu,int count)
{
	return mecfg->sub(menu)->sub("entry")->sub(count)->getString("args");
}

std::string title(int menu)
{
	if(menu==-1)					return lacfg->getString("emulisttitle");
	else if (Bvidmenu())			return lacfg->getString("videolisttitle");
	else if (Bp3tmenu())			return lacfg->getString("p3tlisttitle");
	else if (Bthememenu())			return lacfg->getString("themelisttitle");
	else 							return mecfg->sub(menu)->getString("title");
}

std::string gTex(std::string texof)
{
	return thcfg->sub("textures")->getString(texof);
}

void sTex(std::string texof,std::string thistex)
{
	thcfg->sub("textures")->setC(texof,thistex);
}

bool useShader()
{
	return thcfg->sub("shader")->getBool("useshader");
}

int useBackShader()
{
	return thcfg->sub("shader")->getInt("usebackshader");
}
