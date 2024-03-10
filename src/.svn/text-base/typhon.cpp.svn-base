#include "includes.h"
#include "confighandling.h"
#include "themehandling.h"
#include "fonthandling.h"
#include "inputhandling.h"
#include "glhandling.h"
#include "timerfunc.h"
#include "windowhandling.h"
#include "systeminfo.h"
#include "videoplayerhandling.h"
#include "values.h"
#include "p3thandling.h"
#include "mmdhandling.h"
#include "editor.h"
#include "filehandling.h"
#include "shaderhandling.h"
#include "cmdlinehandling.h"
#include "networking.h"
#include "rsshandling.h"

sf::RenderWindow window;

int main (int argc, char *argv[])
{
    inittyconf();

	std::string pkgdatadir=PKGDATADIR;
	rtcfg->set("datadir",pkgdatadir+"/themedata/typhon/");
	rtcfg->set("systemshaderpath",pkgdatadir+"/generic/backshader/");
	rtcfg->set("systemconfigdir",pkgdatadir+"/themes/");
	thcfg->sub("paths")->set("fontpath",pkgdatadir+"/generic/fonts/");
	
	setrtdefaults();
	rtcfg->sub("shaders")->set( "allowed",sf::Shader::isAvailable());

	typhonCLI* cli=new typhonCLI(argc, argv);

	cfgpath();
	checkcfg();
	readtyphonxml();
	if (cli->exists("-lang"))			{tycfg->sub("strings")->setC("language",cli->getString("-lang"));}

	readlanguagexml();
	
//	readmenuxml();
	getonlinethemes(1); // TODO check internet allowed?
	
	if (cli->getBool("ar") || cli->getBool("-autores"))		{tycfg->sub("globalswitches")->setBoolC("autoresolution",!tycfg->sub("globalswitches")->getBool("autoresolution"));}
	if (cli->getBool("h") || cli->getBool("-help"))			{
																for(int u = 0; u <  lacfg->sub("usage")->size(); u++)
																printf ("%s\n",lacfg->sub("usage")->getString(u).c_str());
																exit(1);
															}
	if (cli->getBool("l") || cli->getBool("-listthemes"))	{cfgpath();printthemes(rtcfg->getString("tythemedir"),1);}
	if (cli->getBool("lp") || cli->getBool("-listp3t")) 	{cfgpath();checkcfg();readtyphonxml();printp3ts();}
	if (cli->exists("i"))									{rtcfg->set("importtheme",cli->getString("i"));importtheme();exit(1);}
	if (cli->exists("-import"))								{rtcfg->set("importtheme",cli->getString("-import"));importtheme();exit(1);}

	if (cli->getBool("le") || cli->getBool("-listexported")){cfgpath();printthemes(rtcfg->getString("tyexportdir"),0);}
	if (cli->getBool("lo") || cli->getBool("-listonline")) 	{cfgpath();checkcfg();readtyphonxml();printonlinethemes();}
	if (cli->exists("fm") || cli->exists("-fillmenu")) 		{
																if (!cli->exists("m"))
																{printf ("%s\n",lacfg->getString("missingarg").c_str());exit(0);}
																else
																{
																	if(cli->getInt("m")<1 || cli->getInt("m")>7)
																	{printf ("%s\n",lacfg->getString("menurange").c_str());exit(0);}
																	else
																	{
																		cfgpath();checkcfg();readtyphonxml();
																		if (cli->exists("fm")) 				addentries(cli->getInt("m")-1,cli->getString("fm"));
																		else if (cli->exists("-fillmenu"))	addentries(cli->getInt("m")-1,cli->getString("-fillmenu"));
																		savemenuxmlcfg();
																		exit(0);
																	}
																}
															}
	if (cli->exists("s") || cli->exists("-scrape"))			{
																if (cli->exists("s"))		rtcfg->set("scrape",cli->getString("s"));
																if (cli->exists("-scrape"))	rtcfg->set("scrape",cli->getString("-scrape"));
																cfgpath();checkcfg();readtyphonxml();reademucfg();listroms();scrapeallroms();
															}

	createthemedatadir();

	if (cli->exists("t") || cli->exists("-theme"))
	{tycfg->set("themename",getPrefix(cli->getString("t")));}

	rtcfg->set("menucounter",mecfg->size()+1);
	
	listp3ts();
	checkp3ttheme();
	
	if (cli->exists("p") || cli->exists("-p3t"))
	{
		tycfg->set("themename","p3t");
		tycfg->set("p3tfile",cli->getString("p"));
		extractp3t();
	}

	themepath(tycfg->getString("themename"));
	readtheme();
	checkpaths();
	readmenuxml();
	readinputxml();
	listfonts();
	reademucfg();
	
	int showmenucfg=Ishowmenu();

	listroms();

	tycfg->set("showmenu",showmenucfg);

	if(!tycfg->sub("globalswitches")->getBool("showemulist") && Ishowmenu()==0) {printf("should never happen\n");tycfg->set("showmenu",1);}
	
#ifdef WITHMMD
	listmmd(rtcfg->sub("scanned")->getString("modeldir"),".pmd","model","models");
	listmmd(rtcfg->sub("scanned")->getString("motiondir"),".vmd","motion","motion");
#endif //WITHMMD

	listaudiofiles("music","musicpath");
	listaudiofiles("sfx","soundpath");

	initaudio();

	if (tycfg->sub("globalswitches")->getBool("autoresolution") && !(cli->getInt("x")))
	{
		sf::VideoMode vm = sf::VideoMode::getDesktopMode(); 
		window.create( vm, "typhon", sf::Style::None);
		rtcfg->set("width",(int)vm.width);
		rtcfg->set("height",(int)vm.height);
	}
	else
	{
		rtcfg->set("width"		,(int)tycfg->sub("globalints")->getInt("width"));
		rtcfg->set("height"		,(int)tycfg->sub("globalints")->getInt("height"));
		if (cli->getInt("x") && cli->getInt("y"))	
		{rtcfg->set("width",cli->getInt("x"));rtcfg->set("height",cli->getInt("y"));}
		
		window.create( sf::VideoMode(rtcfg->getInt("width"),rtcfg->getInt("height"),32), "typhon", sf::Style::None);
	}

	windowpos(tycfg->sub("globalints")->getInt("windowposx"),tycfg->sub("globalints")->getInt("windowposy"));
	calculatepositions();
	updatefont();
	listthemes(rtcfg->getString("tythemedir"));
	if (Bvidmenu())	(setem(0));
	listvideos();
	readsysinfos();
	unblock();
	
	initgl();
	initshaders();
	
#ifdef WITHMMD
	initmodelgl();
#endif //WITHMMD
	initpictureids();
	updateimages();

	window.setMouseCursorVisible(!tycfg->sub("globalswitches")->getBool("hidemouse"));
	rtcfg->setBool("initdone",1);

// main loop

	loadrss();
	
//	scaperoms(5);
//	exit(0);
	
	while(window.isOpen())		
	{
		input();
		checktimer();
		display();
	}
// shutdown program

#ifdef DEBUG
	rtcfg->saveXML("/tmp/typhon-runtime.xml");
	thcfg->saveXML("/tmp/typhon-theme.xml"); 
	tycfg->saveXML("/tmp/typhonrc.xml");
	mecfg->saveXML("/tmp/typhon-menu.xml");
	vicfg->saveXML("/tmp/typhon-videos.xml");
	p3cfg->saveXML("/tmp/typhon-p3ts.xml");
	oncfg->saveXML("/tmp/typhon-onlinethemes.xml");

	cli->getTConfig()->saveXML("/tmp/typhon-cli.xml");
#endif //DEBUG

//	if(tycfg->sub("globalswitches")->getBool("useinputdaemon"))		closesocket();
	if (tycfg->sub("globalswitches")->getBool("saveconfigsonexit"))		{saveglobalcfgs();}
	if (thcfg->sub("objects")->sub("video")->getInt("drawhere")!=0)		{stopvideo();}

	removerec(tycfg->sub("tempdirs")->getString("p3text"));

#ifdef WITHMMD
	quitmodel();
#endif //WITHMMD

	return 0;
}
