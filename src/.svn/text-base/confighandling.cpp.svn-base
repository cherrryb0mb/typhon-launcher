#include "confighandling.h"
#include "p3thandling.h"
#include "windowhandling.h"
#include "systeminfo.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"
#include "videoplayerhandling.h"
#include "mmdhandling.h"

// checks if configured path exists in any logical way (absolute path,in home or in themedatadir)
std::string checkpath(std::string orgpath, std::string defaultpath)
{
	std::string wipcheck;
	if(strcmp(orgpath.c_str(),"")!=0)
	wipcheck=orgpath;
	FILE *pathcheck = fopen(wipcheck.c_str(),"r");

	if(pathcheck)	{fclose (pathcheck);}
	else			{wipcheck=setEnvhome(wipcheck);}

	pathcheck = fopen(wipcheck.c_str(),"r");
	if(pathcheck)	{fclose (pathcheck);}
	else
	{
		// try to use as relative path in themes datadir
		wipcheck=rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename")+"/"+orgpath;
	}
	pathcheck = fopen(wipcheck.c_str(),"r");
	if(pathcheck)	{fclose (pathcheck);}
	else
	{
#ifdef DEBUG
		if (!isp3t())
		{
			printf("WARNING - path %s not found! using defaults!\n",orgpath.c_str());
		}
#endif // DEBUG
			wipcheck=defaultpath;	
	}
	return wipcheck;
}

// checks several default paths
void checkpaths()
{
	int q=0;
	TyphonConfig* typa=tycfg->sub("paths");

	for(int p = 0; p < typa->size(); p++)
	{
		std::string path=typa->getKey(p);
		rtcfg->sub("scanned")->set(path,checkpath(typa->getString(path),rtcfg->getString("datadir")),p);
		q=p+1;
	}
	
	TyphonConfig* thpa=thcfg->sub("paths");

	for(int p = 0; p <  thpa->size(); p++)
	{
		std::string path=thpa->getKey(p);
		rtcfg->sub("scanned")->set(path,checkpath(thpa->getString(path),rtcfg->getString("datadir")),p+q);
	}
}

// path to the config
void cfgpath()
{
	rtcfg->set("home",getenv("HOME"));
	rtcfg->set("cfgdir",rtcfg->getString("home") + "/.config/typhon/");
	mkdir(rtcfg->getString("cfgdir").c_str(), 0755);
	rtcfg->set("tythemedir",rtcfg->getString("cfgdir")+"/themes/");
	rtcfg->set("tyexportdir",rtcfg->getString("cfgdir")+"/export/");

	mkdir(rtcfg->getString("tythemedir").c_str(), 0755);

	rtcfg->set("rominfodir",rtcfg->getString("cfgdir")+"/rominfo/");	mkdir(rtcfg->getString("rominfodir").c_str(), 0755);

	TyphonConfig* co=rtcfg->sub("configs");
	TyphonConfig* cf=rtcfg->sub("cfgs");

	co->set("globalxml",rtcfg->getString("cfgdir")+cf->getString("global"));
	co->set("menuxml",rtcfg->getString("cfgdir")+cf->getString("menu"));
	co->set("emuxml",rtcfg->getString("cfgdir")+cf->getString("emu"));
	co->set("romxml",rtcfg->getString("cfgdir")+cf->getString("rom"));
	co->set("videoxml",rtcfg->getString("cfgdir")+cf->getString("video"));
	co->set("p3txml",rtcfg->getString("cfgdir")+cf->getString("p3t"));
	co->set("inputxml",rtcfg->getString("cfgdir")+cf->getString("input"));
	co->set("shaderxml",rtcfg->getString("cfgdir")+cf->getString("shader"));
	co->set("onlinexml",rtcfg->getString("cfgdir")+cf->getString("online"));
}

// checks if configfile and some default paths exist and creates them if needed
void checkcfg()
{
	std::string cfgsubdir;
	
	cfgsubdir=rtcfg->getString("cfgdir")+"/previews/";					mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir+="/programpictures/";										mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/videos/";					mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/music/";						mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/p3ts/";						mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/shaders/";					mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/previews/videopictures/";	mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/previews/videopreviews/";	mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/previews/icons/";			mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/export/";					mkdir(cfgsubdir.c_str(), 0755);
	#ifdef WITHMMD
	cfgsubdir=rtcfg->getString("cfgdir")+"/models/";					mkdir(cfgsubdir.c_str(), 0755);
	cfgsubdir=rtcfg->getString("cfgdir")+"/motion/";					mkdir(cfgsubdir.c_str(), 0755);
	#endif //WITHMMD
}

// reads main config
void readtyphonxml()
{
	FILE *globalxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("globalxml").c_str(),"r");
	if(globalxmlcfgcheck)
	{
		loadglobalxmlcfg();
		fclose(globalxmlcfgcheck);
	}
	else
	{
		setglobaldefaults();
		saveglobalxmlcfg();
	}
}

void readlanguagexml()
{
	std::string langxmlconfig 	=rtcfg->getString("cfgdir")+rtcfg->sub("cfgs")->getString("lang")+tycfg->sub("strings")->getString("language")+".xml";
	
	FILE *xmlcfgcheck = fopen(langxmlconfig.c_str(),"r");
	if(xmlcfgcheck)
	{
#ifdef DEBUG
		printf("found language config in home %s\n",langxmlconfig.c_str());
#endif // DEBUG
		lacfg->loadXML(langxmlconfig);
		fclose(xmlcfgcheck);
	}
	else
	{
		setlanguagedefaults();
		lacfg->saveXML(langxmlconfig);
	}
}

void readmenuxml()
{
	FILE *menuxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("menuxml").c_str(),"r");
	if(menuxmlcfgcheck)
	{
		loadmenuxmlcfg();
		fclose(menuxmlcfgcheck);
	}
	else
	{
		setmenudefaults();
		savemenuxmlcfg();
	}
}

void readinputxml()
{
	FILE *inputxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("inputxml").c_str(),"r");
	if(inputxmlcfgcheck)
	{
		loadinputxmlcfg();
		fclose(inputxmlcfgcheck);
	}
	else 
	{
		setinputdefaults();
		saveinputxmlcfg();
	}
}
