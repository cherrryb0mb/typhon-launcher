#include "timerfunc.h"
#include "picturehandling.h"
#include "glmenu.h"
#include "systeminfo.h"
#include "windowhandling.h"
#include "inputhandling.h"
#include "videoplayerhandling.h"
#include "mmdhandling.h"
#include "editor.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"
#include "networking.h"

sf::Clock Clock;

void checkinputswitch()
{
	switch (rtcfg->getInt("inputchanged"))
	{
		case 1:
		{
			rtcfg->setBool("searchmode",0);
			rtcfg->setBool("editmode",0);
			break;
		}
		case 2:
		{
			rtcfg->setBool("editmode",1);
			break;
		}
		case 3:
		{
			rtcfg->setBool("searchmode",1);
			break;
		}

		default:
		break;
	}
	rtcfg->set("inputchanged",0);
}

// update some stuff every second:
void checktimer()
{
	rtcfg->set("elapsed",(float)Clock.getElapsedTime().asSeconds());
	
	if
	(
		thcfg->sub("objects")->sub("clock")->getBool("enabled") ||
		thcfg->sub("objects")->sub("date")->getBool("enabled") ||
		thcfg->sub("switches")->getBool("usemonthcolors")
	)
	{clocktimer();}
	
	bool sthupdated=0;

// picture,icon,video:

	if(rtcfg->getBool("mwmoved"))
	{
		((TConfig<float>*) rtcfg->get("waitup"))->operator+(rtcfg->getFloat("elapsed"));
		if(rtcfg->getFloat("waitup")>=thcfg->sub("floats")->getFloat("mousewheelwait"))
		{rtcfg->setBool("mwmoved",0);rtcfg->set("waitup",(float)0);rtcfg->setBool("menuisscrolling",0);selchanged();}
	}
	
	if (rtcfg->getBool("selchanged"))
	{
		if (thcfg->sub("switches")->getBool("playvideoonswitch"))	{startplayvideo();}		
		if (Iem()!=0)	getrominfo(IeID()-1,Ics(),0);
		updatepreview();
		loadshadertex();
		sthupdated=1;
	}

	if (thcfg->sub("objects")->sub("icon")->getBool("enabled") && rtcfg->getBool("selchanged") && !Bvidmenu())	
	{
		updateicon();
		sthupdated=1;
	}

// show musicduration if required
	if(	thcfg->sub("objects")->sub("sound")->getBool("enabled") && 
		thcfg->sub("switches")->getBool("playmusic"))
		{getmusicdur();}

// take screenshot

	if( rtcfg->getBool("launchscrot") && tycfg->sub("globalswitches")->getBool("autoscreenshot") )
	{
		((TConfig<float>*) rtcfg->get("scrottimer"))->operator+(rtcfg->getFloat("elapsed"));
		if(rtcfg->getFloat("scrottimer")>=tycfg->sub("globalints")->getInt("scrotwait"))
		{takescreenshot();rtcfg->set("scrottimer",(float)0);rtcfg->setBool("launchscrot",0);}
	}

	checkinputswitch();

	if(sthupdated)	{rtcfg->setBool("selchanged",0);}


// update some system informations

	if 	(
			thcfg->sub("objects")->sub("uptime")->getBool("enabled") || 
			thcfg->sub("objects")->sub("freemem")->getBool("enabled") || 
			thcfg->sub("objects")->sub("cpu")->getBool("enabled")
		)
	{
		((TConfig<float>*) rtcfg->get("sysinfotimer"))->operator+(rtcfg->getFloat("elapsed"));
		if(rtcfg->getFloat("sysinfotimer")>=1.0)
		{
			if (thcfg->sub("objects")->sub("uptime")->getBool("enabled"))  {getuptime();}

			if (thcfg->sub("objects")->sub("freemem")->getBool("enabled")) {getfreeram();}

			if (thcfg->sub("objects")->sub("cpu")->getBool("enabled")) 
			{ 
				if (rtcfg->getBool("cpucheck")) 	{cpucheck1();rtcfg->setBool("cpucheck",0);}
				else 								{cpucheck2();rtcfg->setBool("cpucheck",1);};
			}
			rtcfg->set("sysinfotimer",(float)0);
		}

	}
	
	if (rtcfg->getBool("shownotify"))
	{
		((TConfig<float>*) rtcfg->get("noticount"))->operator+(rtcfg->getFloat("elapsed"));
		if(rtcfg->getFloat("noticount")>=thcfg->sub("ints")->getInt("notifierduration"))
		{rtcfg->setBool("shownotify",0);rtcfg->set("note"," ");rtcfg->set("noticount",(float)0);}
	}

// toggle selection color:
	if (rtcfg->getBool("menuisscrolling"))
	{
		rtcfg->set("tempselr",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("R"));
		rtcfg->set("tempselg",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("G"));
		rtcfg->set("tempselb",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("B"));
	}
	else
	{
		((TConfig<float>*) rtcfg->get("selcolcheck"))->operator+(rtcfg->getFloat("elapsed"));
		if(rtcfg->getFloat("selcolcheck")>=thcfg->sub("floats")->getFloat("menuselcoltime"))
		{
			if (rtcfg->getInt("tempselb")!=thcfg->sub("objects")->sub("menu")->getInt("B"))
			{
				rtcfg->set("tempselr",thcfg->sub("objects")->sub("menu")->getInt("R"));
				rtcfg->set("tempselg",thcfg->sub("objects")->sub("menu")->getInt("G"));
				rtcfg->set("tempselb",thcfg->sub("objects")->sub("menu")->getInt("B"));
			}
			else if (rtcfg->getInt("tempselb")!=thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("B"))
			{
				if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("menu")->id)
				{
					if(rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("menu")->id))
							{rtcfg->set("tempselr",1);rtcfg->set("tempselg",1);rtcfg->set("tempselb",255);}
					else 	{rtcfg->set("tempselr",255);rtcfg->set("tempselg",1);rtcfg->set("tempselb",1);}
				}
				else
				{
					rtcfg->set("tempselr",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("R"));
					rtcfg->set("tempselg",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("G"));
					rtcfg->set("tempselb",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("B"));
				}
			}
			rtcfg->set("selcolcheck",(float)0);
		}
	}	
	
// change menu
	if(	thcfg->sub("objects")->sub("menuicon")->getBool("enabled") && 
		(rtcfg->getFloat("scrollicon")>=rtcfg->getFloat("showmiconspace")-thcfg->sub("floats")->getFloat("menuiconscrollspeed") || 
		 rtcfg->getFloat("scrollicon") <=-rtcfg->getFloat("showmiconspace")+thcfg->sub("floats")->getFloat("menuiconscrollspeed"))
//		&&	(dockstyle==0 || (dockstyle!=0 && !rtcfg->getBool("undock")) )
	)
	{rtcfg->set("scrollicon",(float)0);rtcfg->setBool("scrollisdone",1);}

	if (rtcfg->getBool("changemenu"))		 
	{
		int minmen=1;
		if (rtcfg->getBool("fadeisdone") && (rtcfg->getBool("scrollisdone") || !thcfg->sub("objects")->sub("menuicon")->getBool("enabled")))
		{

			if (tycfg->sub("globalswitches")->getBool("showemulist")){minmen=0;};
			if (rtcfg->getInt("scrollmenu")==1)	
			{
				if (Ishowmenu()==minmen)
				{
					if(thcfg->sub("switches")->getBool("stoponend"))tycfg->set("showmenu",minmen);
					else tycfg->set("showmenu",mecfg->size());
				}
				else{((TConfig<int>*) tycfg->get("showmenu"))->operator-(1);}
			}

			if (rtcfg->getInt("scrollmenu")==2)	
			{
				if (Ishowmenu()==mecfg->size())
				{
					if(thcfg->sub("switches")->getBool("stoponend"))tycfg->set("showmenu",mecfg->size());
					else tycfg->set("showmenu",minmen);
				}
				else{((TConfig<int>*) tycfg->get("showmenu"))->operator+(1);}
			}
			rtcfg->setBool("changemenu",0);
			rtcfg->setBool("scrollisdone",0);
			rtcfg->setBool("selchanged",1);
			if(thcfg->sub("floats")->getFloat("menufadeduration")!=thcfg->sub("floats")->getFloat("menufadespeed"))
			{
				if(thcfg->sub("switches")->getBool("invertfade"))				rtcfg->set("oldfademenu", rtcfg->getFloat("fademenu"));
				else 															rtcfg->set("oldfademenu",-rtcfg->getFloat("fademenu"));
				rtcfg->set("fademenu",(float)0);
				rtcfg->set("menualpha",0);
				rtcfg->setBool("fadeisdone",0);
			}
		}
	}
	
	if (rtcfg->getFloat("oldfademenu") !=0 && thcfg->sub("floats")->getFloat("menufadeduration")!=thcfg->sub("floats")->getFloat("menufadespeed"))
	{

		if (rtcfg->getInt("menualpha")<=255)	((TConfig<int>*) rtcfg->get("menualpha"))->operator+(rtcfg->getInt("alphaspeed"));
		if (rtcfg->getInt("menualpha")>255)		rtcfg->set("menualpha",255);
	}
	if (rtcfg->getBool("undock"))
	{
		if(rtcfg->sub("scaleicon")->getFloat("icon0")>1.0)((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon0"))->operator-(0.03);
		if(rtcfg->sub("scaleicon")->getFloat("icon1")>1.0)((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon1"))->operator-(0.02);
		if(rtcfg->sub("scaleicon")->getFloat("icon2")>1.0)((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon2"))->operator-(0.01);
		if(	rtcfg->sub("scaleicon")->getFloat("icon0")<=1.0 && 
			rtcfg->sub("scaleicon")->getFloat("icon1")<=1.0 && 
			rtcfg->sub("scaleicon")->getFloat("icon2")<=1.0)
		{
			rtcfg->sub("scaleicon")->setC("icon0",(float)1.0);
			rtcfg->sub("scaleicon")->setC("icon1",(float)1.0);
			rtcfg->sub("scaleicon")->setC("icon2",(float)1.0);
			rtcfg->setBool("undock",0);
		}
	}

	((TConfig<float>*) rtcfg->get("autorotate"))->operator+(thcfg->sub("ints")->getInt("autorotatespeed"));
	if(rtcfg->getFloat("autorotate")>=360)rtcfg->set("autorotate",(float)0);

	if(rtcfg->getInt("exportnow")!=0)
	{
		if(rtcfg->getInt("exportnow")>=2)
		{exporttheme();rtcfg->set("exportnow",0);}
		else{((TConfig<int>*) rtcfg->get("exportnow"))->operator+(1);}
	}

	#ifdef WITHMMD
	if(thcfg->sub("objects")->sub("mmd")->getBool("enabled")){idlemodel();}
	#endif // WITHMMD
	Clock.restart();
}
