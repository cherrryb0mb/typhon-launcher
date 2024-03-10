#include "themehandling.h"
#include "picturehandling.h"
#include "sound.h"
#include "glmenu.h"
#include "emuhandling.h"
#include "confighandling.h"
#include "systeminfo.h"
#include "videoplayerhandling.h"
#include "values.h"
#include "mmdhandling.h"
#include "inputhandling.h"
#include "filehandling.h"
#include "shaderhandling.h"
#include "editor.h"
#include "p3thandling.h"
#include "xmlconfig.h"
#include "networking.h"

extern sf::RenderWindow window;

void changetheme()
{
	setrtdefaults();

	themepath(tycfg->getString("themename"));
	readtheme();
	calculatepositions();
	checkpaths();
	listfonts();
	updatefont();
	clocktimer();
	initaudio();

	if(rtcfg->getInt("themetype")==TYP && isp3t())	rtcfg->setBool("nop3tdata",1);
	else											rtcfg->setBool("nop3tdata",0);
	
	rtcfg->setBool("emptyp3t",0);
	updateimages();
	stopvideo();
	if(thcfg->sub("switches")->getBool("playvideoonstart"))
	{startplayvideo();}
}

void copytyphonthemedata()
{
	// copy the themedata
	std::string srcdir =tycfg->sub("tempdirs")->getString("import")+"/"+tycfg->getString("themename")+"/themedata/"+tycfg->getString("themename")+"/";
	std::string destdir=rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename");
	mkdir(destdir.c_str(), 0755);
	copyrecfromto(srcdir,destdir);

	// copy the theme config
	copyfromto(tycfg->sub("tempdirs")->getString("import")+"/"+tycfg->getString("themename")+"/themes/"+tycfg->getString("themename")+".xml",rtcfg->getString("tythemedir"));
}

void exporttheme()
{
	std::string copyty;
	std::string fexportdir;
	std::string fexportmdir;
	std::string orgdatadir;

// create temp destdir
	fexportmdir=tycfg->sub("tempdirs")->getString("export");
	mkdir(fexportmdir.c_str(), 0755);
	fexportmdir+="/"+rtcfg->getString("exportname");
	mkdir(fexportmdir.c_str(), 0755);
	
// original themedatadir
	if(rtcfg->getInt("themetype")==PS3)
	{orgdatadir=tycfg->sub("tempdirs")->getString("p3text")+"/";}
	else
	{orgdatadir=rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename");}

// create themepreview
	FILE *fcheck = fopen((orgdatadir+"/themepreview.jpg").c_str(),"r");
	if(fcheck)	{fclose(fcheck);}
	else
	{
		sf::Image Screen = window.capture();
		Screen.saveToFile(orgdatadir+"/themepreview.jpg");	
	}
	
	FILE *finddata = fopen(orgdatadir.c_str(),"r");
	if(finddata)
	{

// create themedatadir
		fexportdir=fexportmdir+"/themedata/";
		mkdir(fexportdir.c_str(), 0755);
		fexportdir+=rtcfg->getString("exportname")+"/";
		mkdir(fexportdir.c_str(), 0755);

// copy themedata
		copyrecfromto(orgdatadir,fexportdir);
	}

// copy the theme config
	fexportdir=fexportmdir;
	mkdir(fexportdir.c_str(), 0755);
	fexportdir+="/themes/";
	mkdir(fexportdir.c_str(), 0755);

	rtcfg->sub("configs")->set("themexml",fexportdir+rtcfg->getString("exportname")+".xml");	
// if it's a p3t theme some paths need a reset
	if(rtcfg->getInt("themetype")==PS3)	{resetp3tpaths();}
	
	savetheme();

	rtcfg->setBool("exporting",0);
	
// export as tgz
	std::string exportdir=rtcfg->getString("cfgdir")+"/export/";

	if(!rtcfg->getBool("havethedate")){clocktimer();}

	std::string exportcmd;
	exportcmd=tycfg->sub("tempdirs")->getString("export")+" && tar cfz "+exportdir+rtcfg->getString("exportname")+"-v"+rtcfg->getString("themeversion")+".tgz "+rtcfg->getString("exportname");
	systemstart("cd ",exportcmd,1);
	shownote(lacfg->getString("export")+" "+rtcfg->getString("exportname"));
	removerec(tycfg->sub("tempdirs")->getString("export"));
}

void startexporttheme()
{
	rtcfg->setBool("exporting",1);
	rtcfg->set("inputchanged",2);
	if(rtcfg->getInt("themetype")==PS3)	{rtcfg->set("exportname",getPrefix(tycfg->getString("p3tfile")));}
	else 								{rtcfg->set("exportname",tycfg->getString("themename"));}
	editorinput(rtcfg->getString("exportname"),3);
}

void createthemedatadir()
{
	rtcfg->sub("scanned")->set("themedata",tycfg->sub("paths")->getString("themedata"));
	FILE *pathcheck = fopen(rtcfg->sub("scanned")->getString("themedata").c_str(),"r");

	if(pathcheck)	{fclose(pathcheck);}
	else			{rtcfg->sub("scanned")->set("themedata",setEnvhome(rtcfg->sub("scanned")->getString("themedata")));}
	
	pathcheck = fopen(rtcfg->sub("scanned")->getString("themedata").c_str(),"r");
	
	if(pathcheck)	{fclose(pathcheck);}
	else			{mkdir(rtcfg->sub("scanned")->getString("themedata").c_str(), 0755);}
}

void themepath(std::string themename) {rtcfg->sub("configs")->set("themexml",rtcfg->getString("tythemedir")+getPrefix(themename)+".xml");}

void savetheme()
{
	FILE *wthemefile = fopen(rtcfg->sub("configs")->getString("themexml").c_str(),"w");
	if(wthemefile)
	{
		savethemexmlcfg();
		fclose(wthemefile);
		shownote(lacfg->getString("saved")+tycfg->getString("themename"));
	}
	else
	{
		shownote(lacfg->getString("cantwrite")+rtcfg->sub("configs")->getString("themexml"));
		printf("can't write to %s\n",rtcfg->sub("configs")->getString("themexml").c_str());
	}
}

void readtheme()
{
	FILE *themexmlcfgcheck = fopen(rtcfg->sub("configs")->getString("themexml").c_str(),"r");
	if(themexmlcfgcheck)
	{
		loadthemexmlcfg();
		fclose(themexmlcfgcheck);
	}
	else
	{
		setthemedefaults();
		savethemexmlcfg();
	}
}

void togglethememenu()
{
	if (Ishowmenu()>=1)
	{
		setcs(0);
		if(!Bthememenu())
		{
			tycfg->sub("globalints")->setC("thememenu",Ishowmenu());
			setem(0);
			if(Bp3tmenu())		tycfg->sub("globalints")->set("p3tmenu",-1);
			if(Bvidmenu())		thcfg->sub("objects")->sub("video")->set("videomenu",-1);
		}
		else
		tycfg->sub("globalints")->setC("thememenu",-1);

		updatepreview();
	}
}

void themelist(bool up)
{
	if(Ishowmenu()!=0)
	{
		if(up)
		{
			if(Ics()==0)	{setcs(rtcfg->sub("themes")->size()-1);}
			else 			{setcs(Ics()-1);}
		}
		else
		{
			if(Ics()==rtcfg->sub("themes")->size()-1)	{setcs(0);}
			else 			{setcs(Ics()+1);}
		}
	}		
}

void listthemes(std::string path)
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir(path.c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		int k=0;
		for (int j=0; j<num_entries; j++)
		{
			std::stringstream entry;
			entry << "theme" << k;
			rtcfg->sub("themes")->set(entry.str(),sdit[j]->d_name,k++);
		}
	}
}

void printthemes(std::string path,bool cutsuf)
{
	listthemes(path);
	for (int p=0; p<rtcfg->sub("themes")->size(); p++)
	{
		if(cutsuf)	printf("%s\n",getPrefix(rtcfg->sub("themes")->getString(p)).c_str());
		else 		printf("%s\n",rtcfg->sub("themes")->getString(p).c_str());
	}
	exit(0);
}

void createthisthemedir()
{
	if(rtcfg->getBool("importingtheme") && rtcfg->getInt("themetype")==PS3)
	{rtcfg->set("ithemedatadir",rtcfg->sub("scanned")->getString("themedata")+"/"+getPrefix(tycfg->getString("p3tfile"))+"/");}
	else
	{rtcfg->set("ithemedatadir",rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename")+"/");}
	
	FILE *fcheck = fopen(rtcfg->getString("ithemedatadir").c_str(),"r");
	if(fcheck)
	{fclose (fcheck);}
	else
	{mkdir(rtcfg->getString("ithemedatadir").c_str(), 0755);}	
}

void extractzip()
{
	std::string extargs="-qq -j "+rtcfg->getString("importtheme")+" -d "+tycfg->sub("tempdirs")->getString("import");
	systemstart("unzip", extargs,1);
}

void extract7z()
{
	std::string extargs="e "+rtcfg->getString("importtheme")+" -o"+tycfg->sub("tempdirs")->getString("import");
	systemstart("7za", extargs,1);
}

void extractrar()
{
	std::string extargs="e -ep "+rtcfg->getString("importtheme")+" "+tycfg->sub("tempdirs")->getString("import")+" -inul";
	systemstart("unrar", extargs,1);
}

void extractgz()
{
	std::string extargs="xvfz "+rtcfg->getString("importtheme")+" -C "+tycfg->sub("tempdirs")->getString("import");
	systemstart("tar", extargs,1);
}

void extractbz2()
{
	std::string extargs="xvfj "+rtcfg->getString("importtheme")+" -C "+tycfg->sub("tempdirs")->getString("import");
	systemstart("tar", extargs,1);
}

void extracttheme()
{
	removerec(tycfg->sub("tempdirs")->getString("import"));
	mkdir(tycfg->sub("tempdirs")->getString("import").c_str(), 0755);

	if (rtcfg->getInt("themetype")==ZIP)	{extractzip();}
	if (rtcfg->getInt("themetype")==RAR)	{extractrar();}
	if (rtcfg->getInt("themetype")==SVNZ)	{extract7z();}
	if (rtcfg->getInt("themetype")==GZ)		{extractgz();}
	if (rtcfg->getInt("themetype")==TYPHON)	{extractgz();}
	if (rtcfg->getInt("themetype")==BZ2)	{extractbz2();}
}

int checkthemetype(std::string thischar)
{
	const char *suffix = strrchr(thischar.c_str(), '.');
	if (suffix)
	{
		if (strcasecmp(suffix,".p3t") == 0){return PS3;}
		if (strcasecmp(suffix,".tgz") == 0 && compare(thischar.c_str(),"-v")){return TYPHON;}
		if (strcasecmp(suffix,".jpg") == 0){return JPG;}
		if (strcasecmp(suffix,".gz")  == 0){return GZ;}
		if (strcasecmp(suffix,".bz2") == 0){return BZ2;}
		if (strcasecmp(suffix,".zip") == 0){return ZIP;}	
		if (strcasecmp(suffix,".rar") == 0){return RAR;}
		if (strcasecmp(suffix,".7z")  == 0){return SVNZ;}		
		else printf("no valid theme - exiting!\n");
		exit(0);
		return TYP;
	}
	else printf("no valid theme - exiting!\n");
	exit(0);
	return TYP;
}

void copythistheme()
{
	std::string srcdir =tycfg->sub("tempdirs")->getString("import")+"/";
	std::string destdir=rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename");
	mkdir(destdir.c_str(), 0755);
	copyrecfromto(srcdir,destdir);
}

// very basic generic "theme" importer

void themeimport()
{
	createthemedatadir();
	createthisthemedir();		
	copythistheme();
	setthemedefaults();
	thcfg->sub("paths")->setC("wallpaperpath",rtcfg->getString("ithemedatadir"));
	
	rtcfg->sub("scanned")->set("wallpaperpath",thcfg->sub("paths")->getString("wallpaperpath"));
	listpictures("wallpapers",rtcfg->sub("scanned")->getString("wallpaperpath"));

	sTex("wallpaper",rtcfg->sub("wallpapers")->sub(0)->getString("file"));
	if(rtcfg->sub("wallpapers")->size()>1)		{sTex("stencil",rtcfg->sub("wallpapers")->sub(1)->getString("file"));}
	if(rtcfg->sub("wallpapers")->size()>2)		{sTex("custom1",rtcfg->sub("wallpapers")->sub(2)->getString("file"));}
	if(rtcfg->sub("wallpapers")->size()>3)		{sTex("custom2",rtcfg->sub("wallpapers")->sub(3)->getString("file"));}		
	if(rtcfg->sub("wallpapers")->size()>4)		{sTex("custom3",rtcfg->sub("wallpapers")->sub(4)->getString("file"));}
	else
	{thcfg->sub("objects")->sub("stencil")->setBool("enabled",0);}
	rtcfg->sub("scanned")->set("fontpath",rtcfg->getString("ithemedatadir"));
	listfonts();
	switchfont(0);

	thcfg->sub("paths")->setC("wallpaperpath","\0");
	thcfg->sub("paths")->setC("stencilpicturepath",rtcfg->getString("ithemedatadir"));
	thcfg->sub("paths")->setC("menuiconpath",rtcfg->getString("ithemedatadir"));
	thcfg->sub("paths")->setC("customimagepath",rtcfg->getString("ithemedatadir"));
	themepath(tycfg->getString("themename"));
	savetheme();
	shownote(lacfg->getString("saved")+tycfg->getString("themename"));
}

void nexttyphontheme()
{
	tycfg->set("themename",getPrefix(rtcfg->sub("themes")->getString(Ics())));

	rtcfg->set("themetype",TYP);
	changetheme();
	shownote(lacfg->getString("changetheme")+tycfg->getString("themename"));
}

void importtheme()
{
	cfgpath();
	checkcfg();
	readtyphonxml();

	rtcfg->setBool("importingtheme",1);

	rtcfg->set("themetype",checkthemetype(getFile(rtcfg->getString("importtheme"))));


	FILE *themecheck = fopen(rtcfg->getString("importtheme").c_str(),"r");
	if(themecheck)	{fclose (themecheck);}
	else
	{
		if(rtcfg->getInt("themetype")==PS3)
		{
			rtcfg->sub("scanned")->set("p3tpath",checkpath(tycfg->sub("paths")->getString("p3tpath"),rtcfg->getString("datadir")));
			rtcfg->set("importtheme",rtcfg->sub("scanned")->getString("p3tpath")+"/"+getPrefix(rtcfg->getString("importtheme"))+".p3t");
		}
		if(rtcfg->getInt("themetype")==TYPHON)
		{
			rtcfg->set("importtheme",rtcfg->getString("cfgdir")+"/export/"+getPrefix(rtcfg->getString("importtheme"))+".tgz");
			FILE *themecheck = fopen(rtcfg->getString("importtheme").c_str(),"r");
			if(themecheck)	{fclose (themecheck);}
			else
			{
				getonlinethemes(1);
				for (int t=0; t<oncfg->sub("themes")->size(); t++)
				{
					if (strcmp(getFile(oncfg->sub("themes")->getString(t)).c_str(), getFile(rtcfg->getString("importtheme")).c_str() )==0)
					{
						printf("%s is online - downloading and installing\n",getFile(oncfg->sub("themes")->getString(t)).c_str());
						rtcfg->sub("helpsel")->set("online",t);
						gettheme();
					}
				}
			}
		}
	}

	if(rtcfg->getInt("themetype")==PS3)
	{
		checkp3ttheme();
		tycfg->set("themename","p3t");
		tycfg->set("p3tfile",getFile(rtcfg->getString("importtheme")));
		themepath((char*)tycfg->getString("themename").c_str());
		readtheme();
		extractp3t();
		copyp3t();
		removerec(tycfg->sub("tempdirs")->getString("p3text"));
	}

	else if (rtcfg->getInt("themetype")==TYPHON)
	{
		tycfg->set("themename",getPrefix(getImportname(getFile(rtcfg->getString("importtheme")))));
		extracttheme();
		createthemedatadir();
		copytyphonthemedata();
	}
	else if 
	(
	rtcfg->getInt("themetype")==ZIP  || 
	rtcfg->getInt("themetype")==RAR  || 
	rtcfg->getInt("themetype")==SVNZ ||
	rtcfg->getInt("themetype")==BZ2  ||
	rtcfg->getInt("themetype")==GZ
	)
	{	
		tycfg->set("themename",getPrefix(getFile(rtcfg->getString("importtheme"))));
		extracttheme();
		themeimport();
		removerec(tycfg->sub("tempdirs")->getString("import"));
	}

	else if (rtcfg->getInt("themetype")==JPG)
	{
		tycfg->set("themename",getPrefix(getFile(rtcfg->getString("importtheme"))));
		createthemedatadir();
		createthisthemedir();
		std::string destdir=rtcfg->sub("scanned")->getString("themedata")+"/"+tycfg->getString("themename");
		mkdir(destdir.c_str(), 0755);

		copyfromto(rtcfg->getString("importtheme"),destdir);
		setthemedefaults();
		sTex("wallpaper",getFile(rtcfg->getString("importtheme")));
		thcfg->sub("paths")->setC("wallpaperpath","\0");
		thcfg->sub("paths")->setC("stencilpicturepath","\0");
		thcfg->sub("objects")->sub("stencil")->setBool("enabled",0);
		thcfg->sub("objects")->sub("wallpaper")->set("A" ,255);
		thcfg->sub("paths")->setC("menuiconpath","\0");
		thcfg->sub("paths")->setC("customimagepath","\0");
		themepath(tycfg->getString("themename"));
		savetheme();
	}
			
	printf("import successful\n");
}

void getonlinethemes(bool allowed)
{
	if (tycfg->sub("globalswitches")->getBool("useinternet") || allowed)
	{
		downloadurito
		(
			tycfg->sub("strings")->getString("onlinethemes")+"/"+rtcfg->sub("cfgs")->getString("online"),
			rtcfg->sub("configs")->getString("onlinexml")
		);
		FILE *ocheck = fopen(rtcfg->sub("configs")->getString("onlinexml").c_str(),"r");
		if(ocheck)
		{
			oncfg->loadXML(rtcfg->sub("configs")->getString("onlinexml"));
			fclose(ocheck);
		}
	}
}

void gettheme()
{
	downloadurito
	(
		oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online")),
		rtcfg->getString("cfgdir")+"/export/"+"/"+getFile(oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online")))
	);

	FILE *tcheck = fopen((rtcfg->getString("cfgdir")+"/export/"+"/"+getFile(oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online")))).c_str(),"r");
	if(tcheck)
	{
#ifdef DEBUG
		printf("downloaded theme %s\n",(rtcfg->getString("cfgdir")+"/export/"+"/"+getFile(oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online")))).c_str());
#endif //DEBUG
		rtcfg->set("importtheme",rtcfg->getString("cfgdir")+"/export/"+"/"+getFile(oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online"))));
		importtheme();
		rtcfg->sub("themes")->wipe();
		listthemes(rtcfg->getString("tythemedir"));
/*
		tycfg->set("themename",getPrefix(getFile(oncfg->sub("themes")->getString(rtcfg->sub("helpsel")->getInt("online")))));
		changetheme();
*/
		fclose(tcheck);
	}
}

void printonlinethemes()
{
	getonlinethemes(1);
	for (int t=0; t<oncfg->sub("themes")->size(); t++)
	{printf("%s\n",getFile(oncfg->sub("themes")->getString(t)).c_str());}
	exit(0);
}
