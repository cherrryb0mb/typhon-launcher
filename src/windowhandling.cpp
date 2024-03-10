#include "windowhandling.h"
#include "glmenu.h"
#include "systeminfo.h"
#include "values.h"
#include "videoplayerhandling.h"
#include "editor.h"
#include "mmdhandling.h"
#include "inputhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"

int rootX, rootY;
int oldrootX, oldrootY;

int changex=0;
int changey=0;

int poc=0;	// pickedobjectcount
int ppo1;	// possiblepickedobject1
int ppo2;	// possiblepickedobject2
int poz1=0;	// pickedobject1z
int poz2=0;	// pickedobject2z
unsigned char pixel[3];

int pick(){return rtcfg->getInt("pick");}

// possible active objects are not directly picked, 
// but first checked if their z-position
// is preferable to a 2nd candidate
void pickcandidate(int thisppo, int thispoz)
{
	poc+=1;
	if(poc==1)	{ppo1=thisppo;poz1=thispoz;}
	else 		{ppo2=thisppo;poz2=thispoz;}
}

// don't use (scale/move) active object if it is blocked
// blocked objects (mmb) is coloured blue
void checkpickedobjectblocked()
{
	if(rtcfg->sub("block")->getBool(ppo1)  && ppo2!=0)
	{rtcfg->set("picked",ppo2);}
	
	if(rtcfg->sub("block")->getBool(ppo2))
	{rtcfg->set("picked",ppo1);}	
}

void unblock()
{
	for (int i=0;i<thcfg->sub("objects")->size();i++)
	{rtcfg->sub("block")->setC(thcfg->sub("objects")->getKey(i),0);}
	rtcfg->set("picked",thcfg->sub("objects")->size()+1);
}

void mousepicktog()
{
	if(pick()==2) 	rtcfg->set("pick",0);
	else 			rtcfg->set("pick",2);

	if(pick()==2)
	{
		infopick();
		rtcfg->setBool("mousewas",!tycfg->sub("globalswitches")->getBool("hidemouse"));
		window.setMouseCursorVisible(1);
	}
	else
	{
		unblock();
		infopick();
		window.setMouseCursorVisible(rtcfg->getBool("mousewas"));
		if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==3) vicfg->saveXML(rtcfg->sub("configs")->getString("videoxml"));
	}
}

void setpicktog()
{
	if(pick()==4) 	rtcfg->set("pick",0);
	else 			rtcfg->set("pick",4);
	if(pick()==4)
	{
		rtcfg->set("picked",thcfg->sub("objects")->get("wallpaper")->id); // pick wallpaper per default
		infosetpick();
		rtcfg->setBool("mousewas",!tycfg->sub("globalswitches")->getBool("hidemouse"));
		window.setMouseCursorVisible(1);
	}
	else
	{
		unblock();
		infosetpick();
		window.setMouseCursorVisible(rtcfg->getBool("mousewas"));
	}
}

void colorpicktog()
{
	if(pick()==3) 	rtcfg->set("pick",0);
	else 			rtcfg->set("pick",3);

	if(pick()==3)
	{
		infocolpick();
		rtcfg->setBool("mousewas",!tycfg->sub("globalswitches")->getBool("hidemouse"));
		window.setMouseCursorVisible(1);
	}
	else
	{
		unblock();
		infocolpick();
		window.setMouseCursorVisible(rtcfg->getBool("mousewas"));
	} 
}

void changerotpick()
{
	if(pick()==1) 	rtcfg->set("pick",0);
	else 			rtcfg->set("pick",1);

	if(thcfg->sub("objects")->sub("notifier")->getBool("enabled"))	{inforotpick();}	

	if(pick()==1)	
	{
		rtcfg->set("picked",thcfg->sub("objects")->get("wallpaper")->id); // pick wallpaper per default
		window.setMouseCursorVisible(1);
	}
	else 
	{
		window.setMouseCursorVisible(!tycfg->sub("globalswitches")->getBool("hidemouse"));
		unblock();
		inforotpick();
	}
}

// pick a color	
void getpixelcolor()
{
	if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size())
	{
		rtcfg->setBool("stillpicked",1);
		glReadPixels( rootX, rtcfg->getInt("height")-1-rootY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		if((rtcfg->getInt("picked")==thcfg->sub("objects")->get("menu")->id) && tycfg->sub("globalswitches")->getBool("pickselectioncolor"))
		{
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->sub("menuselfont")->set("R",pixel[0]);
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->sub("menuselfont")->set("G",pixel[1]);
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->sub("menuselfont")->set("B",pixel[2]);	
		}
		else
		{
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("R",pixel[0]);
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("G",pixel[1]);
			thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("B",pixel[2]);
		}
	}
}

// check if a menuentry is a pickcandidate
int pickentries(int pickid, int flop)
{
	int flip=-flop;
	TyphonConfig* mobj =thcfg->sub("objects")->sub("menu");
	TyphonConfig* pobj =thcfg->sub("objects")->sub(pickid);

	switch (thcfg->sub("ints")->getInt("menualign"))
	{
		case 1: 	
		{
			if
			(
				rootX>=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx") &&
				rootX<=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx")+rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("width") &&
				rootY>=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("height") &&
				rootY<=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")
			)
			return 1;break;
		}

		case 2:
		{
			if
			(
				rootX>=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("width")) &&
				rootX<=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx")+rtcfg->sub("objects")->sub("menu")->getInt("maxwidth") &&
				rootY>=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("height") &&
				rootY<=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")
			)
			return 1;break;
		}

		case 3:
		{
			if
			(
				rootX>=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("width"))/2 &&
				rootX<=icwidth *mobj->getInt("posx")+flip*pobj->getInt("posx")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")+rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("width"))/2 &&
				rootY>=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(pickid))->getFloat("height") &&
				rootY<=icheight*mobj->getInt("posy")+flop*pobj->getInt("posy")
			)

			return 1;break;
		}
		default: break;
		break;	
	}
	return 0;
}

float posX(std::string thisobj) {return icwidth *thcfg->sub("objects")->sub(thisobj)->getInt("posx");}
float posY(std::string thisobj) {return icheight*thcfg->sub("objects")->sub(thisobj)->getInt("posy");}
float posW(std::string thisobj) {return rtcfg->sub("objects")->sub(thisobj)->getFloat("width");}
float posH(std::string thisobj) {return rtcfg->sub("objects")->sub(thisobj)->getFloat("height");}

void pickquad(std::string pickquad)
{
	float minX,maxX,minY,maxY;
	
	if (posX(pickquad)<posX(pickquad)+posW(pickquad))
	{minX=posX(pickquad);maxX=posX(pickquad)+posW(pickquad);}
	else
	{minX=posX(pickquad)+posW(pickquad);maxX=posX(pickquad);}

	if (posY(pickquad)<posY(pickquad)+(-posH(pickquad)))
	{minY=posY(pickquad);maxY=posY(pickquad)+(-posH(pickquad));}
	else
	{minY=posY(pickquad)+(-posH(pickquad));maxY=posY(pickquad);}

	if
	(rootX>=minX &&	rootX<=maxX &&	rootY>=minY &&	rootY<=maxY &&
	(thcfg->sub("objects")->sub(pickquad)->getBool("enabled") || (thcfg->sub("objects")->sub("video")->getInt("drawhere")==2 && compare("preview",pickquad)) || pick()==4))
	{pickcandidate(thcfg->sub("objects")->get(pickquad)->id, thcfg->sub("objects")->sub(pickquad)->getInt("posz"));}	
}

void pickenabled(std::string trypick)
{
	if(thcfg->sub("objects")->sub(trypick)->getBool("enabled") || pick()==4)
	{
		if
		(
			rootX>=posX(trypick) &&
			rootX<=posX(trypick)+posW(trypick) &&
			rootY>=posY(trypick)-posH(trypick) &&
			rootY<=posY(trypick)
		)
		{pickcandidate(thcfg->sub("objects")->get(trypick)->id,thcfg->sub("objects")->sub(trypick)->getInt("posz"));}	
	}
}

// the actual picking - mouse pos is compared to objects size and pos -
// if it matches the object is picked

void mousepos(int rootX,int rootY)
{
	poc=0;	poz1=0;	poz2=0;
	ppo2=thcfg->sub("objects")->get("wallpaper")->id;
	ppo2=thcfg->sub("objects")->get("wallpaper")->id;
	
	if (!rtcfg->getBool("stillpicked"))
	{					
		// pick title

		if(thcfg->sub("ints")->getInt("titlealign")==1 || pick()==4)
		{
			if
			(							
			rootX>=posX("title") &&
			rootX<=posX("title")+posW("title") &&
			rootY>=posY("title")-posH("title") &&
			rootY<=posY("title")
			)
			{pickcandidate(thcfg->sub("objects")->get("title")->id,thcfg->sub("objects")->sub("title")->getInt("posz"));}
		}
		
		if(thcfg->sub("ints")->getInt("titlealign")==2 || pick()==4)
		{
			if
			(
				rootX>=posX("title")+(thcfg->sub("objects")->sub("title")->getInt("maxtitlelength")-posW("title")) &&
				rootX<=posX("title")+thcfg->sub("objects")->sub("title")->getInt("maxtitlelength") &&
				rootY>=posY("title")-posH("title") &&
				rootY<=posY("title")
			)
			{pickcandidate(thcfg->sub("objects")->get("title")->id,thcfg->sub("objects")->sub("title")->getInt("posz"));}
		}
		
		if(thcfg->sub("ints")->getInt("titlealign")==3 || pick()==4)
		{
			if
			(
				rootX>=posX("title")+(thcfg->sub("objects")->sub("title")->getInt("maxtitlelength")/2-posW("title")/2) &&
				rootX<=posX("title")+(thcfg->sub("objects")->sub("title")->getInt("maxtitlelength")/2+posW("title")/2) &&
				rootY>=posY("title")-posH("title") &&
				rootY<=posY("title")
			)
			{pickcandidate(thcfg->sub("objects")->get("title")->id,thcfg->sub("objects")->sub("title")->getInt("posz"));}
		}
		// pick title end
		
		pickenabled("entrycur");
		pickenabled("entrycount");
		pickenabled("clock");
		pickenabled("date");
		pickenabled("coop");
		pickenabled("description");
		pickenabled("developer");
		pickenabled("esrb");
		pickenabled("platform");
		pickenabled("publisher");
		pickenabled("reldate");
		pickenabled("romtitle");


		if(thcfg->sub("objects")->sub("rss")->getBool("enabled") && rtcfg->getBool("rssinitialized"))
		{
			if
			(
				rootX>=rtcfg->sub("objects")->sub("rss")->getFloat("posx") &&
				rootX<=rtcfg->sub("objects")->sub("rss")->getFloat("posx")+rssfontfx.getWidth(rtcfg->getString("rss")) &&
				rootY>=posY("rss")-rssfontfx.getMaxHeight() &&
				rootY<=posY("rss")
			)
			{pickcandidate(thcfg->sub("objects")->get("rss")->id,thcfg->sub("objects")->sub("rss")->getInt("posz"));}
		}
		

		if(thcfg->sub("objects")->sub("myip")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("myip") &&
				rootX<=posX("myip")+sysinfofontfx.getWidth(rtcfg->getString("ip")) &&
				rootY>=posY("myip")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("myip")
			)
			{pickcandidate(thcfg->sub("objects")->get("myip")->id,thcfg->sub("objects")->sub("myip")->getInt("posz"));}
		}
		
		if(thcfg->sub("objects")->sub("uptime")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("uptime") &&
				rootX<=posX("uptime")+sysinfofontfx.getWidth(rtcfg->getString("uptime")) &&
				rootY>=posY("uptime")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("uptime")
			)
			{pickcandidate(thcfg->sub("objects")->get("uptime")->id,thcfg->sub("objects")->sub("uptime")->getInt("posz"));}
		}	

		if(thcfg->sub("objects")->sub("freemem")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("freemem") &&
				rootX<=posX("freemem")+sysinfofontfx.getWidth(rtcfg->sub("memusage")->getString("free")) &&
				rootY>=posY("freemem")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("freemem")
			)
			{pickcandidate(thcfg->sub("objects")->get("freemem")->id,thcfg->sub("objects")->sub("freemem")->getInt("posz"));}
		}	

		if(thcfg->sub("objects")->sub("totalmem")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("totalmem") &&
				rootX<=posX("totalmem")+sysinfofontfx.getWidth(rtcfg->sub("memusage")->getString("total")) &&
				rootY>=posY("totalmem")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("totalmem")
			)
			{pickcandidate(thcfg->sub("objects")->get("totalmem")->id,thcfg->sub("objects")->sub("totalmem")->getInt("posz"));}
		}

		if(thcfg->sub("objects")->sub("cpu")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("cpu") &&
				rootX<=posX("cpu")+sysinfofontfx.getWidth(rtcfg->sub("cpuusage")->getString("cpu")) &&
				rootY>=posY("cpu")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("cpu")
			)
			{pickcandidate(thcfg->sub("objects")->get("cpu")->id,thcfg->sub("objects")->sub("cpu")->getInt("posz"));}
		}

		if(thcfg->sub("objects")->sub("disk1")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("disk1") &&
				rootX<=posX("disk1")+sysinfofontfx.getWidth(rtcfg->sub("diskusage")->getString("disk1")) &&
				rootY>=posY("disk1")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("disk1")
			)
			{pickcandidate(thcfg->sub("objects")->get("disk1")->id,thcfg->sub("objects")->sub("disk1")->getInt("posz"));}
		}	

		if(thcfg->sub("objects")->sub("disk2")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("disk2") &&
				rootX<=posX("disk2")+sysinfofontfx.getWidth(rtcfg->sub("diskusage")->getString("disk2")) &&
				rootY>=posY("disk2")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("disk2")
			)
			{pickcandidate(thcfg->sub("objects")->get("disk2")->id,thcfg->sub("objects")->sub("disk2")->getInt("posz"));}
		}	

		if(thcfg->sub("objects")->sub("disk3")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("disk3") &&
				rootX<=posX("disk3")+sysinfofontfx.getWidth(rtcfg->sub("diskusage")->getString("disk3")) &&
				rootY>=posY("disk3")-sysinfofontfx.getMaxHeight() &&
				rootY<=posY("disk3")
			)
			{pickcandidate(thcfg->sub("objects")->get("disk3")->id,thcfg->sub("objects")->sub("disk3")->getInt("posz"));}
		}	

		// pick current selection
		if(thcfg->sub("ints")->getInt("menualign")==1)
		{
			if
			(
				rootX>=posX("menu") &&
				rootX<=posX("menu")+posW("menu") &&
				rootY>=posY("menu")-posH("menu") &&
				rootY<=posY("menu")
			)
			{pickcandidate(thcfg->sub("objects")->get("menu")->id,thcfg->sub("objects")->sub("menu")->getInt("posz"));}
		}

		if(thcfg->sub("ints")->getInt("menualign")==2)
		{
			if
			(
				rootX>=posX("menu")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-posW("menu")) &&
				rootX<=posX("menu")+rtcfg->sub("objects")->sub("menu")->getInt("maxwidth") &&
				rootY>=posY("menu")-posH("menu") &&
				rootY<=posY("menu")
			)
			{pickcandidate(thcfg->sub("objects")->get("menu")->id,thcfg->sub("objects")->sub("menu")->getInt("posz"));}
		}
		
		if(thcfg->sub("ints")->getInt("menualign")==3)
		{
			if
			(
				rootX>=posX("menu")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")/2-posW("menu")/2) &&
				rootX<=posX("menu")+(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")/2+posW("menu")/2) &&
				rootY>=posY("menu")-posH("menu") &&
				rootY<=posY("menu")
			)
			{pickcandidate(thcfg->sub("objects")->get("menu")->id,thcfg->sub("objects")->sub("menu")->getInt("posz"));}
		}

		// pick current selection end
		
		if(thcfg->sub("objects")->sub("notifier")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("notifier") && 
				rootX<=posX("notifier")+rtcfg->getFloat("notifywidth") && 
				rootY>=posY("notifier")-notifyfontfx.getMaxHeight() && 
				rootY<=posY("notifier")
			)
			{pickcandidate(thcfg->sub("objects")->get("notifier")->id,thcfg->sub("objects")->sub("notifier")->getInt("posz"));}
		}
		
		if(thcfg->sub("objects")->sub("sound")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("sound") &&
				rootX<=posX("sound")+rtcfg->getInt("musicwidth") &&
				rootY>=posY("sound")-thcfg->sub("objects")->sub("sound")->getInt("size") &&
				rootY<=posY("sound")
			)
			{pickcandidate(thcfg->sub("objects")->get("sound")->id,thcfg->sub("objects")->sub("sound")->getInt("posz"));}	
		}

		if(thcfg->sub("objects")->sub("musictitle")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("musictitle") &&
				rootX<=posX("musictitle")+rtcfg->getFloat("musicwidth") &&
				rootY>=posY("musictitle")-musicdurationfontfx.getMaxHeight() &&
				rootY<=posY("musictitle")
			)
			{pickcandidate(thcfg->sub("objects")->get("musictitle")->id,thcfg->sub("objects")->sub("musictitle")->getInt("posz"));}
		}	

		for(int z = 0; z <  thcfg->sub("ints")->getInt("menulistmin"); z++)

		{
			int pickid=thcfg->sub("objects")->get("menumin1")->id+z;
			if (pickentries(pickid,-1))
			{pickcandidate(pickid,thcfg->sub("objects")->sub(pickid)->getInt("posz"));}
		}

		for(int z = 0; z <  thcfg->sub("ints")->getInt("menulistmax"); z++)
		{
			int pickid=thcfg->sub("objects")->get("menumax1")->id+z;
			if (pickentries(pickid,1))
			{pickcandidate(pickid,thcfg->sub("objects")->sub(pickid)->getInt("posz"));}
		}

		#ifdef WITHMMD
		if(thcfg->sub("objects")->sub("mmd")->getBool("enabled") || pick()==4)
		{
			if
			(
				rootX>=posX("mmd")-(icwidth*thcfg->sub("objects")->sub("mmd")->getInt("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx")/2) &&
				rootX<=posX("mmd")+(icwidth*thcfg->sub("objects")->sub("mmd")->getInt("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx")/2) &&
				rootY>=posY("mmd")-(icheight*thcfg->sub("objects")->sub("mmd")->getInt("scaley")*rtcfg->sub("objects")->sub("mmd")->getInt("calcy"))  &&
				rootY<=posY("mmd")
			)
			{pickcandidate(thcfg->sub("objects")->get("mmd")->id,thcfg->sub("objects")->sub("mmd")->getInt("posz"));}
		}
		#endif //WITHMMD

		if(rtcfg->getBool("searchmode") || rtcfg->getBool("editmode"))
		{
			if
			(
				rootX>=posX("searcheditbox") &&
				rootX<=posX("searcheditbox")+rtcfg->sub("objects")->sub("searcheditbox")->getFloat("width") &&
				rootY>=posY("searcheditbox")-posH("searcheditbox") &&
				rootY<=posY("searcheditbox")
			)
			{pickcandidate(thcfg->sub("objects")->get("searcheditbox")->id,thcfg->sub("objects")->sub("searcheditbox")->getInt("posz"));}
		}

		if(thcfg->sub("objects")->sub("slide1")->getBool("enabled"))
		{
			if
			(
				rootX>=posX("slide1") &&
				rootX<=posX("slide1")+posW("slide1")&&
				rootY>=posY("slide1") &&
				rootY<=posY("slide1")+posH("slide1")
			)
			{pickcandidate(thcfg->sub("objects")->get("slide1")->id,thcfg->sub("objects")->sub("slide1")->getInt("posz"));}

			if
			(
				rootX>=posX("slide2") &&
				rootX<=posX("slide2")+posW("slide2") &&
				rootY>=posY("slide2") &&
				rootY<=posY("slide2")+posH("slide2")
			)
			{pickcandidate(thcfg->sub("objects")->get("slide2")->id,thcfg->sub("objects")->sub("slide2")->getInt("posz"));}
		}

		pickquad("rectbox");
		pickquad("custom1");
		pickquad("custom2");
		pickquad("custom3");
		pickquad("menuicon");
		pickquad("icon");
		pickquad("stencil");
		if(rtcfg->getBool("showhelpscreen")) pickquad("setbox");
		if (Iem()!=0 &&((thcfg->sub("objects")->sub("rombox")->getBool("enabled") && thcfg->sub("objects")->sub("description")->getBool("enabled")) || pick()==4)) pickquad("rombox");

		if(thcfg->sub("objects")->sub("preview")->getBool("enabled") || (thcfg->sub("objects")->sub("video")->getInt("drawhere")==2) || pick()==4)
		{pickquad("preview");}

		if(thcfg->sub("objects")->sub("previewreflection")->getBool("enabled") || pick()==4)
		{pickquad("previewreflection");}

		if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==1)
		pickquad("video");
	}
	
	if (poc==1){rtcfg->set("picked",ppo1);};
	if (poc==2){if (poz1>=poz2){rtcfg->set("picked",ppo1);}else {rtcfg->set("picked",ppo2);}};
	if (poc==2){checkpickedobjectblocked();}

	changey=(rootY-oldrootY)/icheight;
	changex=(rootX-oldrootX)/icwidth;

	if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size())
	{shownote(lacfg->getString("picked")+thcfg->sub("objects")->getKey(rtcfg->getInt("picked")));}

	if (pick()==2)
	{
		if (rtcfg->getBool("clickedleft"))	{mousemoveobjects();}
		if (rtcfg->getBool("clickedright"))	{mousescaleobjects();}
	}

	if (pick()==3)
	{
		if (rtcfg->getBool("clickedleft"))	{getpixelcolor();}
	}

	if (pick()==4)
	{
		if (rtcfg->getBool("clickedright"))	{mousechangebehaviour();}
	}
	
	if (pick()==1)
	{
		if (rtcfg->getBool("clickedleft"))	{mouserotobjects();}
	}		
}

void mousetogobject()
{
	thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->setBool("enabled",!thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getBool("enabled"));
}

// change z-position of picked object with mouse wheel
void mousezobjects(int thiszchange)
{
	rtcfg->setBool("stillpicked",1);

	if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size() && !rtcfg->sub("block")->getBool(rtcfg->getInt("picked")))
	{
		((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("posz"))->operator+(thiszchange);
		shownote(lacfg->getString("zpos")+itos(thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getInt("posz")));	
	}
}

// change depth of picked object with mouse wheel
void mousedepthobjects(float thiszchange)
{
	rtcfg->setBool("stillpicked",1);

	if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size() && !rtcfg->sub("block")->getBool(rtcfg->getInt("picked")))
	{
		((TConfig<float>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("depth"))->operator+(thiszchange);
		shownote(lacfg->getString("zpos")+ftos(thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getFloat("depth")));	
	}
}

// block current object with middle mouse button
// blocked object can't be moved/resized 
void mouseblockobjects()
{
	rtcfg->setBool("stillpicked",1);
	if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size()){rtcfg->sub("block")->setBool(rtcfg->getInt("picked"),!rtcfg->sub("block")->getBool(rtcfg->getInt("picked")));}
}

void fontborder(int *thisfont,int minfont,int maxfont)
{
	if(*thisfont<minfont)*thisfont=minfont;
	if(*thisfont>maxfont)*thisfont=maxfont;	
}

void scaleblock(std::string thisobj)
{
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get(thisobj)->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get(thisobj)->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("width"))->operator+(changex);
		((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("height"))->operator+(changey);
		rtcfg->sub("objects")->sub(thisobj)->set("width"		,showW(thcfg->sub("objects")->sub(thisobj)->getInt("width")));
		rtcfg->sub("objects")->sub(thisobj)->set("height"		,showH(thcfg->sub("objects")->sub(thisobj)->getInt("height")));
	}
}

// scale active object
void mousescaleobjects()
{
	rtcfg->setBool("stillpicked",1);
	
	scaleblock("preview");
	scaleblock("previewreflection");
	scaleblock("menuicon");
	scaleblock("icon");
	scaleblock("stencil");
	scaleblock("custom1");
	scaleblock("custom2");
	scaleblock("custom3");
	scaleblock("wallpaper");
	scaleblock("slide1");
	scaleblock("slide2");
	scaleblock("setbox");
	scaleblock("rombox");
	scaleblock("rectbox");

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("video")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("video")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("video")->get("width"))->operator+(changex);
		((TConfig<int>*) thcfg->sub("objects")->sub("video")->get("height"))->operator+(changey);
		rtcfg->sub("objects")->sub("video")->set("width"		,showW(thcfg->sub("objects")->sub("video")->getInt("width")));
		rtcfg->sub("objects")->sub("video")->set("height"		,showH(thcfg->sub("objects")->sub("video")->getInt("height")));
	}

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("title")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("title")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("title")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("title")->getIntP("size"),10,150);
		resizetitfont();
	}

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("rss")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("rss")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("rss")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("rss")->getIntP("size"),10,150);
		resizerssfont();
	}

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("clock")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("clock")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("clock")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("clock")->getIntP("size"),10,150);
		resizeclofont();
	}

	if(
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("coop")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("coop")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("description")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("description")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("developer")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("developer")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("esrb")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("esrb")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("romtitle")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("romtitle")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("platform")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("platform")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("publisher")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("publisher")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("reldate")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("reldate")->id)==0)
		)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("platform")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("platform")->getIntP("size"),10,150);
		resizerominfofont();
	}		

	if( 
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("entrycount")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("entrycount")->id)==0) ||
		(rtcfg->getInt("picked")==thcfg->sub("objects")->get("entrycur")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("entrycur")->id)==0)
		)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("entrycount")->get("size"))->operator+(changex);
		((TConfig<int>*) thcfg->sub("objects")->sub("entrycur")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("entrycount")->getIntP("size"),10,150);
		resizeromfont();
	}

	if (
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("myip")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("myip")->id)==0)			||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("uptime")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("uptime")->id)==0 )	||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("freemem")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("freemem")->id)==0 )	||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("totalmem")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("totalmem")->id)==0 )||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("cpu")->id  && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("cpu")->id)==0 )			||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("disk1")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("disk1")->id)==0 )		||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("disk2")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("disk2")->id)==0 )		||
		(	rtcfg->getInt("picked")==thcfg->sub("objects")->get("disk3")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("disk3")->id)==0 )
	   )
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("cpu")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("cpu")->getIntP("size"),10,150);
		resizesysfont();			
	}


	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("menu")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("menu")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("menu")->get("menuselfontsize"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("menu")->getIntP("menuselfontsize"),10,150);
		resizeselfont();	
	}
		
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("notifier")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("notifier")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("notifier")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("notifier")->getIntP("size"),10,150);
		resizenotfont();	
	}
	
	if(	((rtcfg->getInt("picked")==thcfg->sub("objects")->get("sound")->id) && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("sound")->id)==0) ||
		((rtcfg->getInt("picked")==thcfg->sub("objects")->get("musictitle")->id) && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("musictitle")->id)==0))
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("sound")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("sound")->getIntP("size"),10,150);
		resizemusfont();
	}

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("date")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("date")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("date")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("date")->getIntP("size"),10,150);
		resizedatfont();	
	}	

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("searcheditbox")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("searcheditbox")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("searcheditbox")->get("height"))->operator+(changey);
		rtcfg->sub("objects")->sub("searcheditbox")->set("height"		,showH(thcfg->sub("objects")->sub("searcheditbox")->getInt("height")));
	}	

	#ifdef WITHMMD
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("mmd")->id && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("mmd")->id)==0)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("mmd")->get("scalex"))->operator+(changex/rtcfg->sub("objects")->sub("mmd")->getInt("calcx"));
		((TConfig<int>*) thcfg->sub("objects")->sub("mmd")->get("scaley"))->operator-(changey/rtcfg->sub("objects")->sub("mmd")->getInt("calcy"));
		rtcfg->sub("objects")->sub("mmd")->set("scalex"		,showW(thcfg->sub("objects")->sub("mmd")->getInt("scalex")));
		rtcfg->sub("objects")->sub("mmd")->set("scaley"		,-showH(thcfg->sub("objects")->sub("mmd")->getInt("scaley")));
	}
	#endif //WITHMMD

	if(rtcfg->getInt("picked")>=thcfg->sub("objects")->get("menumin1")->id && rtcfg->getInt("picked")<=thcfg->sub("objects")->get("menumax20")->id)
	{
		((TConfig<int>*) thcfg->sub("objects")->sub("menu")->get("size"))->operator+(changex);
		fontborder(thcfg->sub("objects")->sub("menu")->getIntP("size"),10,150);
		resizemenfont();	
	}
}

bool rotstuff(int c1,int c2o)
{
	if(c1==c2o || c1==0) return 0;

	int c2=c2o+tycfg->sub("globalints")->getInt("rotationtolerance");

	if
	(
		(c1>0 && c2>0 && c1> c2) ||
		(c1<0 && c2<0 && c1< c2) ||
		(c1>0 && c2<0 && c1>-c2) ||
		(c1<0 && c2>0 && c1<-c2)
	)	
	return 1;
	else return 0;
}

void mouserotobjects()
{
	rtcfg->setBool("stillpicked",1);

	if (rtcfg->getBool("rotatez"))			// Z
	{((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("rotz"))->operator+(changey);}
	else
	{
		if (rotstuff(changex,changey)) 		// X
		((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("roty"))->operator+(changex/2);

		else if (rotstuff(changey,changex))	// Y
		((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("rotx"))->operator+(changey/2);
	}
}

// move active object

void mousemoveobjects()
{
	rtcfg->setBool("stillpicked",1);
	for(int z = 0; z <  thcfg->sub("objects")->size(); z++)
	{
		std::string thisobj=thcfg->sub("objects")->getKey(z);
		if(rtcfg->getInt("picked")==thcfg->sub("objects")->get(thisobj)->id && !rtcfg->sub("block")->getBool(thcfg->sub("objects")->get(thisobj)->id))
		{
			if(z>=thcfg->sub("objects")->get("menumax1")->id && z<=thcfg->sub("objects")->get("menumax20")->id)
			{
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posx"))->operator-(changex);
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posy"))->operator+(changey);
			}
			else if (z>=thcfg->sub("objects")->get("menumin1")->id && z<=thcfg->sub("objects")->get("menumin10")->id)
			{
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posx"))->operator+(changex);
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posy"))->operator-(changey);
			}
			else
			{
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posx"))->operator+(changex);
				((TConfig<int>*) thcfg->sub("objects")->sub(thisobj)->get("posy"))->operator+(changey);
				rtcfg->sub("objects")->sub(thisobj)->set("posx"	 	,showX(thcfg->sub("objects")->sub(thisobj)->getInt("posx")));
				rtcfg->sub("objects")->sub(thisobj)->set("posy"		,showY(thcfg->sub("objects")->sub(thisobj)->getInt("posy")));

				// if rombox is moved the description should be moved as well:
				if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("rombox")->id)
				{
					((TConfig<int>*) thcfg->sub("objects")->sub("description")->get("posx"))->operator+(changex);
					((TConfig<int>*) thcfg->sub("objects")->sub("description")->get("posy"))->operator+(changey);
					rtcfg->sub("objects")->sub("description")->set("posx",showX(thcfg->sub("objects")->sub("description")->getInt("posx")));
					rtcfg->sub("objects")->sub("description")->set("posy",showY(thcfg->sub("objects")->sub("description")->getInt("posy")));	
				}
			}
		}
	}
}

// change data file of currently picked object using mousewheel in settingspicking mode
void mousechangedata(bool thiszchange)
{
#ifdef WITHMMD
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("mmd")->id) 			updatemodel(thiszchange);
#endif // WITHMMD
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("menuicon")->id) 	changemenuicon(thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("wallpaper")->id) 	changewallpaper(thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("stencil")->id)		changepicture("stencil","stencil", "currentstencil", thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("custom1")->id)		changepicture("custom" ,"custom1", "currentcustom1", thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("custom2")->id)		changepicture("custom" ,"custom2", "currentcustom2", thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("custom3")->id)		changepicture("custom" ,"custom3", "currentcustom3", thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("musictitle")->id) 	switchmusic(thiszchange);
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("icon")->id) 		switchfont(thiszchange);

}

void mousechangebehaviour()
{
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("menuicon")->id) 	changemenuiconspace();
	 printf("mousechangebehaviourtoggleback\n");
	toggleback();
}

void toggleback()
{
	if(thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->exists("back"))
	{
		thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->setBool("back",!thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getBool("back"));
	}
}

void windowpos(int x ,int y)
{
	window.setPosition(sf::Vector2i(x,y));
}

void movewindow(bool up)
{
	std::string mov="windowposx";
	std::string max="width";

	if(up)	{mov="windowposy";	max="height";}
	
	if(tycfg->sub("globalints")->getInt(mov)>=0)
	{
		while (tycfg->sub("globalints")->getInt(mov) > -rtcfg->getInt(max))
		{
			((TConfig<int>*) tycfg->sub("globalints")->get(mov))->operator-(1);
			windowpos(tycfg->sub("globalints")->getInt("windowposx"),tycfg->sub("globalints")->getInt("windowposy"));
		}
	}
	else
	{
		while (tycfg->sub("globalints")->getInt(mov) < 0)
		{
			((TConfig<int>*) tycfg->sub("globalints")->get(mov))->operator+(1);
			windowpos(tycfg->sub("globalints")->getInt("windowposx"),tycfg->sub("globalints")->getInt("windowposy"));
		}		
	}
/*
	if(tycfg->sub("globalints")->getInt(mov) <= -rtcfg->getInt(max))
	rtcfg->setBool("windowhidden",1);
	else
	rtcfg->setBool("windowhidden",0);
*/
}
