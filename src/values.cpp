#include "values.h"
#include "videoplayerhandling.h"
#include "systeminfo.h"
#include "themehandling.h"
#include "mmdhandling.h"
#include "sound.h"
#include "xmlconfig.h"
#include "windowhandling.h"
#include "networking.h"

// calculate positions

float icwidth;
float icheight;

float hdw=1920.0;
float hdh=1080.0;

float showW(int width)	{ return roundf((icwidth*width));}
float showH(int height)	{ return roundf(-(icheight*height));}
float showX(int posx)	{ return roundf(icwidth*posx-(rtcfg->getInt("width")/2));}
float showY(int posy)	{ return roundf(-(icheight*posy-(rtcfg->getInt("height")/2)));}

void setX(std::string obj)	{rtcfg->sub("objects")->sub(obj)->set("posx"  ,showX(thcfg->sub("objects")->sub(obj)->getInt("posx")));}
void setY(std::string obj)	{rtcfg->sub("objects")->sub(obj)->set("posy"  ,showY(thcfg->sub("objects")->sub(obj)->getInt("posy")));}
void setW(std::string obj)	{rtcfg->sub("objects")->sub(obj)->set("width" ,showW(thcfg->sub("objects")->sub(obj)->getInt("width")));}
void setH(std::string obj)	{rtcfg->sub("objects")->sub(obj)->set("height",showH(thcfg->sub("objects")->sub(obj)->getInt("height")));}
void setXY(std::string obj)	{setX(obj);setY(obj);}
void setA (std::string obj)	{setXY(obj);setW(obj);setH(obj);}

void proportions()
{
	icwidth=(window.getSize().x/hdw);
	icheight=(window.getSize().y/hdh);
}

void calculatepositions()
{
	proportions();

	setA("preview");
	setA("previewreflection");
	setA("stencil");
	setA("rectbox");
	setA("custom1");
	setA("custom2");
	setA("custom3");
	setA("menuicon");
	setA("icon");
	setA("wallpaper");
	setA("video");
	setA("setbox");
	setA("rombox");

	setXY("title");
	setXY("clock");
	setXY("date");
	setXY("menu");
	setXY("mmd");
	setXY("searcheditbox");
	setXY("freemem");
	setXY("totalmem");
	setXY("disk1");
	setXY("disk2");
	setXY("disk3");
	setXY("cpu");
	setXY("myip");
	setXY("uptime");
	setXY("notifier");
	setXY("entrycount");
	setXY("entrycur");
	setXY("sound");
	setXY("rss");
	setXY("musictitle");

	setXY("coop");
	setXY("description");
	setXY("developer");
	setXY("esrb");
	setXY("platform");
	setXY("publisher");
	setXY("reldate");
	setXY("romtitle");
	
	setH("searcheditbox");

	rtcfg->sub("objects")->sub("menu")->set("maxwidth"		,(int)showW(thcfg->sub("ints")->getInt("menumaxwidth")));
	rtcfg->sub("objects")->sub("mmd")->set("scalex"	 		,showW(thcfg->sub("objects")->sub("mmd")->getInt("scalex")));
	rtcfg->sub("objects")->sub("mmd")->set("scaley"			,-showH(thcfg->sub("objects")->sub("mmd")->getInt("scaley")));
	rtcfg->set("showmiconspace",thcfg->sub("floats")->getFloat("menuiconspace")*icwidth);
	rtcfg->sub("objects")->sub("mmd")->set("calcx",12);
	rtcfg->sub("objects")->sub("mmd")->set("calcy",21);
	for(int m = 1; m <  mecfg->size()+1; m++)
	{
		if(mecfg->sub(m-1)->getInt("savedentry") <= mecfg->sub(m-1)->sub("entry")->size())
		rtcfg->sub("curSel")->set(m,mecfg->sub(m-1)->getInt("savedentry"));
		else
		rtcfg->sub("curSel")->set(m,mecfg->sub(m-1)->sub("entry")->size()-1);
	}
	if (Iem()!=0)	getrominfo(IeID()-1,Ics(),0);
}
