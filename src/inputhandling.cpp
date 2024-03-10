#include "inputhandling.h"
#include "windowhandling.h"
#include "systeminfo.h"
#include "videoplayerhandling.h"
#include "editor.h"
#include "p3thandling.h"
#include "mmdhandling.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"
#include "networking.h"

sf::Event event;

void startprog()
{
	togentryname(1);
	if ((Iem()!=0 && !expathcheck(emulator(IeID()-1).c_str())) || (Iem()==0 && !expathcheck(mentry(Ishowmenu()-1,Ics()).c_str())))
	{
		if (tycfg->sub("globalswitches")->getBool("stopsoundonlaunch"))
		{
			if (thcfg->sub("switches")->getBool("playsound"))		{rtcfg->setBool("soundwason",1);stopsound();}
			if (thcfg->sub("switches")->getBool("playmusic"))		{rtcfg->setBool("musicwason",1);stopmusic();}
		}

		if (thcfg->sub("switches")->getBool("stopvideoonlaunch") && thcfg->sub("objects")->sub("video")->getInt("drawhere")!=0)
		rtcfg->setBool("videowason",1);stopvideo();
			
		if (tycfg->sub("globalswitches")->getBool("stopjoypadonlaunch") && rtcfg->getBool("usejoypad"))
		{rtcfg->setBool("joypadwason",1);;rtcfg->setBool("usejoypad",0);}
			
		if (tycfg->sub("globalswitches")->getBool("stopshaderonlaunch"))
		{if(useShader())rtcfg->setBool("shaderwas",useShader());thcfg->sub("shader")->setBool("useshader",0);}

		if (Iem()!=0)
		{
			std::string romargs=emuargs(IeID()-1)+" \""+rompath(IeID()-1)+"/"+romname(IeID()-1)+"\"";
			systemstart(emulator(IeID()-1), romargs,0);
	#ifdef DEBUG
			printf ("INFO    - commandline for running rom is %s %s\n",emulator(IeID()-1).c_str(),romargs.c_str()); 
	#endif //DEBUG
			if (tycfg->sub("globalswitches")->getBool("autoscreenshot")){startscrotting(getPrefix(romname(IeID()-1)),rompicturespath(IeID()-1));}
		}
		else
		{
			systemstart(mentry(Ishowmenu()-1,Ics()), margs(Ishowmenu()-1,Ics()),0);
	#ifdef DEBUG
			printf("INFO    - commandline for program is %s %s\n",mentry(Ishowmenu()-1,Ics()).c_str(), margs(Ishowmenu()-1,Ics()).c_str());
	#endif //DEBUG
			if (tycfg->sub("globalswitches")->getBool("autoscreenshot")){startscrotting(mentry(Ishowmenu()-1,Ics()),rtcfg->sub("scanned")->getString("previewpath"));}
		}
		rtcfg->setBool("programwasstarted",1);
	}
#ifdef DEBUG
	else printf("program doesn't exist - doing nothing\n");
#endif //DEBUG
	togentryname(0);
}

void toggleglobalswitch()
{
	int curswitch=rtcfg->sub("helpsel")->getInt("globalswitches");
	tycfg->sub("globalswitches")->setBool(curswitch,!tycfg->sub("globalswitches")->getBool(curswitch));

	if (strcmp(tycfg->sub("globalswitches")->getKey(curswitch).c_str(), "hidemouse")==0)	
	{window.setMouseCursorVisible(!tycfg->sub("globalswitches")->getBool("hidemouse"));}

	if (strcmp(tycfg->sub("globalswitches")->getKey(curswitch).c_str(), "useinternet")==0)
	{getonlinethemes(1);}
}

void togglethswitch()
{
	int curswitch=rtcfg->sub("helpsel")->getInt("switches");
	thcfg->sub("switches")->setBool(curswitch,!thcfg->sub("switches")->getBool(curswitch));

//	if (strcmp(thcfg->sub("switches")->getKey(curswitch).c_str(), "hidemouse")==0)	{window.ShowMouseCursor(!thcfg->sub("switches")->getBool(curswitch));}
}

int helpid(std::string help){return lacfg->sub("helpscreens")->get(help)->id;}

void selchanged()
{
	if (!rtcfg->getBool("showhelpscreen") /*&& pick()==0*/ && !rtcfg->getBool("menuisscrolling")) rtcfg->setBool("selchanged",1);
}

void sethelp(int a)
{
	if(rtcfg->getInt("showhelp")==a)
	{rtcfg->setBool("showhelpscreen",0);rtcfg->set("showhelp", helpid("helpscreenlist"));}
	else 
	rtcfg->set("showhelp",a);
}

void changeobjvalue(int ch)
{
	int k=rtcfg->sub("helpsel")->getInt("objects");
	std::string keyname=rtcfg->sub("object")->getKey(k);
	std::string object=thcfg->sub("objects")->getKey(rtcfg->getInt("edobject"));

	TyphonConfig* val =thcfg->sub("objects")->sub(rtcfg->getInt("edobject"));

	if(val->exists(keyname))
	{
		if(k==0 || k==15|| k==16)	// bools
		{
			val->setBool(keyname,!val->getBool(keyname));
		}
		else if(k==4)					// float
		{
			((TConfig<float>*) val->get(keyname))->operator+(ch*(float)tycfg->sub("globalints")->getInt("changevaluespeed"));
		}
		else 						// ints
		{
			((TConfig<int>*) val->get(keyname))->operator+(ch*tycfg->sub("globalints")->getInt("changevaluespeed"));
			if(k==1 || k==2)
			{
				rtcfg->sub("objects")->sub(object)->set("width"		,showW(thcfg->sub("objects")->sub(rtcfg->getInt("edobject"))->getInt("width")));
				rtcfg->sub("objects")->sub(object)->set("height"		,showH(thcfg->sub("objects")->sub(rtcfg->getInt("edobject"))->getInt("height")));
			}
			if(k==5 || k==6)
			{
				rtcfg->sub("objects")->sub(object)->set("posx"	 	,showX(thcfg->sub("objects")->sub(rtcfg->getInt("edobject"))->getInt("posx")));
				rtcfg->sub("objects")->sub(object)->set("posy"		,showY(thcfg->sub("objects")->sub(rtcfg->getInt("edobject"))->getInt("posy")));				
			}
		}		
	}
}

void startselection()
{
	if(rtcfg->getBool("showhelpscreen"))
	{
		switch(rtcfg->getInt("showhelp"))
		{
			case 0: {sethelp(rtcfg->sub("helpsel")->getInt("helplist"));break;}
			case 1:	{rtcfg->setBool("editkeys",1);break;}
			case 2:	{rtcfg->setBool("waitforjoypad",1);break;}
			case 3:	{rtcfg->set("inputchanged",2);editorinput(thcfg->sub("paths")->getString(rtcfg->sub("helpsel")->getInt("paths")),4);break;}
			case 4:	{togglethswitch();break;}
			case 5:	{toggleglobalswitch();break;}
			case 6:	{rtcfg->set("inputchanged",2);editorinput(emcfg->sub(rtcfg->getInt("edemu"))->getString(rtcfg->sub("helpsel")->getInt("emu")),6);break;}
			case 11:{rtcfg->set("inputchanged",2);editorinput(tycfg->sub("strings")->getString(rtcfg->sub("helpsel")->getInt("stringedit")),9);break;}
			case 12:{rtcfg->set("inputchanged",2);editorinput(tycfg->sub("paths")->getString(rtcfg->sub("helpsel")->getInt("globalpaths")),5);break;}
			case 13:{gettheme();break;}
			case 14:{changeobjvalue(1);break;}

			default: break;
		}	
	}

	else if(rtcfg->getInt("edmode")!=0)
	{
		if(rtcfg->getInt("edmode")==1)
		{
			if(Ishowmenu()!=0 && !Bvidmenu() && !Bp3tmenu() && !Bthememenu() && Iem()==0)
			{
				rtcfg->set("inputchanged",2);
				editorinput(mentry(Ishowmenu()-1,Ics()),1);
			}
		}
		else if(rtcfg->getInt("edmode")==2 && Iem()==0)
		{
			if(Ishowmenu()!=0 && !Bvidmenu() && !Bp3tmenu() && !Bthememenu())
			{
				rtcfg->set("inputchanged",2);
				editorinput(margs(Ishowmenu()-1,Ics()),2);
			}
		}		
		else if(rtcfg->getInt("edmode")==3)
		{
			if (Ishowmenu()>=0 && Iem()==0)
			{rtcfg->set("inputchanged",2);editorinput(title(Ishowmenu()-1),7);}
			if (Ishowmenu()>=1 && !Bvidmenu() && !Bp3tmenu() && !Bthememenu() && Iem()!=0 && thcfg->sub("switches")->getBool("useemutitles"))
			{rtcfg->set("inputchanged",2);editorinput(emutitle(IeID()-1),8);}
		}
	}

	else
	{
		if(Ishowmenu()==0)					{openemulist();}
		else if(Ishowmenu()>=1)
		{
			if (!Bvidmenu() && !Bp3tmenu() && !Bthememenu())	{startprog();}
			if (Iem()==0 && Bvidmenu())							{startplayvideo();}
			if (Iem()==0 && Bp3tmenu())							{changep3t();}
			if (Iem()==0 && Bthememenu()) 						{nexttyphontheme();}
		}
	}
}
	
// is called on cursorkeys and restarts media if required after a programlaunch
void restartmedia()
{
	if(rtcfg->getBool("programwasstarted"))
	{
		if(rtcfg->getBool("soundwason") ||rtcfg->getBool("musicwason"))	{initaudio();};
		if(rtcfg->getBool("soundwason"))								{togglesound();rtcfg->setBool("soundwason",0);};
		if(rtcfg->getBool("musicwason"))								{togglemusic();rtcfg->setBool("musicwason",0);};
		if(rtcfg->getBool("videowason"))								{startplayvideo();rtcfg->setBool("videowason",0);};
		if(rtcfg->getBool("joypadwason"))								{rtcfg->setBool("usejoypad",1);rtcfg->setBool("joypadwason",0);};	
		if(rtcfg->getBool("shaderwas")){thcfg->sub("shader")->setBool("useshader",rtcfg->getBool("shaderwas"));loadshadertex();rtcfg->setBool("shaderwas",0);};
		rtcfg->setBool("programwasstarted",0);
	}
	rtcfg->setBool("mwmoved",1);
	selchanged();
}

void edchangevalue(int cone)
{
	if(rtcfg->getBool("menueditmode"))
	{
		if(Ics()+(cone*-1)<0 || Ics()+(cone*-1)>=mecfg->sub(Ishowmenu()-1)->sub("entry")->size()-1){}
		else
		{
			mecfg->sub(Ishowmenu()-1)->sub("entry")->move(Ics(),Ics()+(cone*-1));	
			if ((cone*-1) >0)dokeydown();
			else dokeyup();
		}
	}
	else
	{
		((TConfig<int>*) tycfg->sub("globalints")->get("changevaluespeed"))->operator+(cone);
		shownote(lacfg->getString("valuespeed")+itos(tycfg->sub("globalints")->getInt("changevaluespeed")));
	}
}

void domenuchange(int here)
{
	if(rtcfg->getBool("showhelpscreen"))
	{
		if(rtcfg->getInt("showhelp")==helpid("themefiles"))
		{
			bool ch=1;
			if(here==2) ch=0;

			if(rtcfg->sub("helpsel")->getInt("datafile")==thcfg->sub("files")->get("fontfile")->id)			switchfont(ch);
			if(rtcfg->sub("helpsel")->getInt("datafile")==thcfg->sub("files")->get("musicfile")->id)		switchmusic(ch);
			if(rtcfg->sub("helpsel")->getInt("datafile")==thcfg->sub("files")->get("soundfile")->id)		switchsfx(ch);
#ifdef WITHMMD
			if(rtcfg->sub("helpsel")->getInt("datafile")==thcfg->sub("files")->get("mmdmodelfile")->id)		updatemodel(ch);
			if(rtcfg->sub("helpsel")->getInt("datafile")==thcfg->sub("files")->get("mmdmotionfile")->id)	updatemotion(ch);
#endif // WITHMMD
		}
		else {}
	}
	else
	{
		restartmedia();
		scaleicons();
		rtcfg->setBool("changemenu",1);

		if (thcfg->sub("switches")->getBool("randomwallpaper") && !thcfg->sub("switches")->getBool("usemenuwallpaper"))
		{
			rtcfg->set("currentwp",rand() %rtcfg->sub("wallpapers")->size());
			sTex("wallpaper",rtcfg->sub("wallpapers")->sub(rtcfg->getInt("currentwp"))->getString("file"));
			updatewallpaper(thcfg->sub("textures")->getStringP("wallpaper"),getTexture(rtcfg->sub("textureids")->sub("wallpaper")->id));
		}

		if (thcfg->sub("switches")->getBool("randomwallpaper") && thcfg->sub("switches")->getBool("usemenuwallpaper"))
		{loadmenuwallpapers(Ishowmenu());}
		if(thcfg->sub("switches")->getBool("usemenuwallpaper")){rtcfg->set("blendwall",(float)1);rtcfg->setBool("wallscroll",here-1);}
		rtcfg->set("scrollmenu",here);
	}
}

void dokeyleft()	
{
	playsfx();
	if(rtcfg->getBool("showhelpscreen"))
	{
		if (rtcfg->getInt("showhelp")==helpid("hotkeylist"))
		{
			playsfx();
			((TConfig<int>*) rtcfg->sub("helpsel")->get("key"))->operator-(incfg->sub("key")->size()/3);
			if(rtcfg->sub("helpsel")->getInt("key")<0)rtcfg->sub("helpsel")->setC("key",incfg->sub("key")->size()-1);
		}
		else if(rtcfg->getInt("showhelp")==helpid("emulatorsettings"))
		{
			((TConfig<int>*) rtcfg->get("edemu"))->operator-(1);
			if(rtcfg->getInt("edemu")<0)rtcfg->set("edemu",emcfg->size()-1);
		}	
		else if (rtcfg->getInt("showhelp")==helpid("themeints"))
		{
			((TConfig<int>*) rtcfg->sub("helpsel")->get("thint"))->operator-(thcfg->sub("ints")->size()/3);
			if(rtcfg->sub("helpsel")->getInt("thint")<0)rtcfg->sub("helpsel")->setC("thint",thcfg->sub("ints")->size()-1);
		}
		else if (rtcfg->getInt("showhelp")==helpid("objects"))
		{
			((TConfig<int>*) rtcfg->get("edobject"))->operator-(1);
			if(rtcfg->getInt("edobject")<0)rtcfg->set("edobject",thcfg->sub("objects")->size()-1);
		}
	}
	else
	{domenuchange(1);}
}

void dokeyright()
{
	playsfx();

	if(rtcfg->getBool("showhelpscreen"))
	{
		if (rtcfg->getInt("showhelp")==helpid("hotkeylist"))
		{
			((TConfig<int>*) rtcfg->sub("helpsel")->get("key"))->operator+(incfg->sub("key")->size()/3);
			if(rtcfg->sub("helpsel")->getInt("key")>incfg->sub("key")->size()-1)rtcfg->sub("helpsel")->setC("key",0);
		}
		else if(rtcfg->getInt("showhelp")==helpid("emulatorsettings"))
		{
			((TConfig<int>*) rtcfg->get("edemu"))->operator+(1);
			if(rtcfg->getInt("edemu")>emcfg->size()-1)rtcfg->set("edemu",0);
		}
		else if (rtcfg->getInt("showhelp")==helpid("themeints"))
		{
			((TConfig<int>*) rtcfg->sub("helpsel")->get("thint"))->operator+(thcfg->sub("ints")->size()/3);
			if(rtcfg->sub("helpsel")->getInt("thint")>thcfg->sub("ints")->size()-1)rtcfg->sub("helpsel")->setC("thint",0);
		}
		else if (rtcfg->getInt("showhelp")==helpid("objects"))
		{
			((TConfig<int>*) rtcfg->get("edobject"))->operator+(1);
			if(rtcfg->getInt("edobject")>thcfg->sub("objects")->size()-1)rtcfg->set("edobject",0);
		}
	}
	else
	{domenuchange(2);}
}

void chthint(int ch)
{
	int curthint=rtcfg->sub("helpsel")->getInt("thint");
	int min=rtcfg->sub("thintbarrier")->sub(curthint)->getInt("min");
	int max=rtcfg->sub("thintbarrier")->sub(curthint)->getInt("max");
	((TConfig<int>*) thcfg->sub("ints")->get(curthint))->operator+(ch);
	     if(thcfg->sub("ints")->getInt(curthint)>max){thcfg->sub("ints")->set(curthint,min);}
	else if(thcfg->sub("ints")->getInt(curthint)<min){thcfg->sub("ints")->set(curthint,max);}

	     if (strcmp(thcfg->sub("ints")->getKey(curthint).c_str(), "dateformat")==0)		{rtcfg->setBool("havethedate",0);}
	else if (strcmp(thcfg->sub("ints")->getKey(curthint).c_str(), "menucurve")==0)		{domenucurve();}
	else if (strcmp(thcfg->sub("ints")->getKey(curthint).c_str(), "videovolume")==0)	{setvideovol(thcfg->sub("ints")->getInt("videovolume"));}
	else if (strcmp(thcfg->sub("ints")->getKey(curthint).c_str(), "musicvolume")==0)	{musicvol(thcfg->sub("ints")->getInt("musicvolume"));}
}

void chglobint(int ch)
{
	TyphonConfig* gi=tycfg->sub("globalints");
	int curglobint=rtcfg->sub("helpsel")->getInt("globalints");
	int min=rtcfg->sub("globintbarrier")->sub(curglobint)->getInt("min");
	int max=rtcfg->sub("globintbarrier")->sub(curglobint)->getInt("max");
	((TConfig<int>*) tycfg->sub("globalints")->get(curglobint))->operator+(ch);
	     if(gi->getInt(curglobint)>max){gi->set(curglobint,min);}
	else if(gi->getInt(curglobint)<min){gi->set(curglobint,max);}
/*
		 if (strcmp(gi->getKey(curglobint).c_str(), "width")==0 || 
			 strcmp(gi->getKey(curglobint).c_str(), "height")==0)
			{resizescreen(gi->getInt("width"), gi->getInt("height"));}
*/
		 if (strcmp(gi->getKey(curglobint).c_str(), "windowposx")==0)	{windowpos(gi->getInt("windowposx"),gi->getInt("windowposy"));}
	else if (strcmp(gi->getKey(curglobint).c_str(), "windowposy")==0)	{windowpos(gi->getInt("windowposx"),gi->getInt("windowposy"));}
	else if (strcmp(gi->getKey(curglobint).c_str(), "thememenu")==0)	{if(Bthememenu()) setcs(0);}
	else if (strcmp(gi->getKey(curglobint).c_str(), "p3tmenu")==0)		{if(Bp3tmenu()) setcs(0);}
}

void chthfloat(float ch)
{
	int curthfloat=rtcfg->sub("helpsel")->getInt("thfloat");
	float min=rtcfg->sub("thfloatbarrier")->sub(curthfloat)->getFloat("min");
	float max=rtcfg->sub("thfloatbarrier")->sub(curthfloat)->getFloat("max");
	((TConfig<float>*) thcfg->sub("floats")->get(curthfloat))->operator+(ch*(max/100));
	     if(thcfg->sub("floats")->getFloat(curthfloat)>max){thcfg->sub("floats")->set(curthfloat,min);}
	else if(thcfg->sub("floats")->getFloat(curthfloat)<min){thcfg->sub("floats")->set(curthfloat,max);}

	     if (strcmp(thcfg->sub("floats")->getKey(curthfloat).c_str(), "menuiconspace")==0)		{rtcfg->set("showmiconspace",thcfg->sub("floats")->getFloat("menuiconspace")*icwidth);}
	else if (strcmp(thcfg->sub("floats")->getKey(curthfloat).c_str(), "dropshadowstrength")==0)	{setdropshadowstrength(thcfg->sub("floats")->getFloat("dropshadowstrength"));}
}

void dokeychangeval(bool up)
{
// in-decrease value
	playsfx();
	int ch;
	
	if(up)ch=1;
	else ch=-1;
	
	if(rtcfg->getBool("menueditmode"))
	{
		if(up)	
		{
			addentry(Ishowmenu()-1,"startme"+itos(mecfg->sub(Ishowmenu()-1)->sub("entry")->size()),Ics(),"");
		}
		else 
		{	delentry(Ishowmenu()-1,Ics());}
	}
	
	else
	{
		if(rtcfg->getBool("showhelpscreen"))
		{
			if(rtcfg->getInt("showhelp")==helpid("themeints"))	{chthint(ch);}
			if(rtcfg->getInt("showhelp")==helpid("globalints"))	{chglobint(ch);}
			if(rtcfg->getInt("showhelp")==helpid("themefloats")){chthfloat((float)ch);}
			if(rtcfg->getInt("showhelp")==helpid("objects"))	{changeobjvalue(ch);}
		}
		else if(rtcfg->getBool("choosemicon") && !rtcfg->getBool("editkeys") && !rtcfg->getBool("choosewp"))
		{changemenuicon(up);}
		else if(rtcfg->getBool("choosewp") && !rtcfg->getBool("editkeys") && !rtcfg->getBool("choosemicon"))
		{changewallpaper(up);loadshadertex();}
	}
}

void menupageup()
{
	playsfx();

	if (Bvidmenu())			{changevideo(1);}
	else if (Bp3tmenu())	{switchp3t(1);}
	else if (Bthememenu())	{themelist(1);}

	else
	{
		if(Ics() <= tycfg->sub("globalints")->getInt("changevaluespeed"))	dokeyup();
		else
		{setcs((Ics() - tycfg->sub("globalints")->getInt("changevaluespeed")));selchanged();}
	}
}

void menupagedown()
{
	playsfx();
	
	if (Bvidmenu())			{changevideo(0);}
	else if (Bp3tmenu())	{switchp3t(0);}
	else if (Bthememenu())	{themelist(0);}

	else
	{
		if (Ics()>=Ime()-tycfg->sub("globalints")->getInt("changevaluespeed"))
		dokeydown();
		else
		{setcs(((Ics()+tycfg->sub("globalints")->getInt("changevaluespeed"))%Ime()));selchanged();}
	}
}

void dokeyup()
{
	playsfx();
	if(!rtcfg->getBool("showhelpscreen"))
	{
		if (Bvidmenu())			changevideo(1);
		else if (Bp3tmenu())	switchp3t(1);
		else if (Bthememenu())	themelist(1);

		else
		{
			rtcfg->setBool("menuisscrolling",1);
			if (Ics()==0)	setcs(Ime()-1);
			else			setcs((Ics()-1));
		}
		restartmedia();
	}
	else
	{
		TyphonConfig* hs=rtcfg->sub("helpsel");
		
		switch(rtcfg->getInt("showhelp"))
		{
			case 0:	
			{
				((TConfig<int>*) hs->get("helplist"))->operator-(1);
				if(hs->getInt("helplist")<0)hs->setC("helplist",lacfg->sub("helpscreens")->size()-1);
				break;
			}
			case 1:	
			{
				((TConfig<int>*) hs->get("key"))->operator-(1);
				if(hs->getInt("key")<0)hs->setC("key",incfg->sub("key")->size()-1);
				break;
			}
			case 2:	
			{
				((TConfig<int>*) hs->get("joypad"))->operator-(1);
				if(hs->getInt("joypad")<0)hs->setC("joypad",incfg->sub("joypad")->size()-1);
				break;
			}
			case 3:	
			{
				((TConfig<int>*) hs->get("paths"))->operator-(1);
				if(hs->getInt("paths")<0)hs->setC("paths",thcfg->sub("paths")->size()-1);
				break;
			}
			case 4:	
			{
				((TConfig<int>*) hs->get("switches"))->operator-(1);
				if(hs->getInt("switches")<0)hs->setC("switches",thcfg->sub("switches")->size()-1);		
				break;
			}
			case 5:	
			{
				((TConfig<int>*) hs->get("globalswitches"))->operator-(1);
				if(hs->getInt("globalswitches")<0)hs->setC("globalswitches",tycfg->sub("globalswitches")->size()-1);
				break;
			}
			case 6:	
			{
				((TConfig<int>*) hs->get("emu"))->operator-(1);
				if(hs->getInt("emu")<0)hs->setC("emu",emcfg->sub(rtcfg->getInt("edemu"))->size()-1);		
				break;
			}
			case 7:	
			{
				((TConfig<int>*) hs->get("thint"))->operator-(1);
				if(hs->getInt("thint")<0)hs->setC("thint",thcfg->sub("ints")->size()-1);		
				break;
			}
			case 8:	
			{
				((TConfig<int>*) hs->get("globalints"))->operator-(1);
				if(hs->getInt("globalints")<0)hs->setC("globalints",tycfg->sub("globalints")->size()-1);		
				break;
			}
			case 9:	
			{
				((TConfig<int>*) hs->get("thfloat"))->operator-(1);
				if(hs->getInt("thfloat")<0)hs->setC("thfloat",thcfg->sub("floats")->size()-1);		
				break;
			}		
			case 10:	
			{
				((TConfig<int>*) hs->get("datafile"))->operator-(1);
				if(hs->getInt("datafile")<0)hs->setC("datafile",thcfg->sub("files")->size()-1);		
				break;
			}
			case 11:	
			{
				((TConfig<int>*) hs->get("stringedit"))->operator-(1);
				if(hs->getInt("stringedit")<0)hs->setC("stringedit",tycfg->sub("strings")->size()-1);		
				break;
			}	
			case 12:	
			{
				((TConfig<int>*) hs->get("globalpaths"))->operator-(1);
				if(hs->getInt("globalpaths")<0)hs->setC("globalpaths",tycfg->sub("paths")->size()-1);
				break;
			}
			case 13:	
			{
				((TConfig<int>*) hs->get("online"))->operator-(1);
				if(hs->getInt("online")<0)hs->setC("online",oncfg->sub("themes")->size()-1);		
				break;
			}
			case 14:	
			{
				((TConfig<int>*) hs->get("objects"))->operator-(1);
				if(hs->getInt("objects")<0)hs->setC("objects",rtcfg->sub("object")->size()-1);		
				break;
			}
			default: break;
		}
	}
}

void dokeydown()
{
	playsfx();
	if(!rtcfg->getBool("showhelpscreen"))
	{
		if (Bvidmenu())			changevideo(0);
		else if (Bp3tmenu())	switchp3t(0);
		else if (Bthememenu())	themelist(0);

		else 
		{
			rtcfg->setBool("menuisscrolling",1);
			setcs((Ics()+1)%Ime());
		}
		restartmedia();
	}
	else
	{
		TyphonConfig* hs=rtcfg->sub("helpsel");
		switch(rtcfg->getInt("showhelp"))
		{
			case 0:	
			{
				((TConfig<int>*) hs->get("helplist"))->operator+(1);
				if(hs->getInt("helplist")>lacfg->sub("helpscreens")->size()-1)hs->setC("helplist",0);
				break;
			}
			case 1:	
			{
				((TConfig<int>*) hs->get("key"))->operator+(1);
				if(hs->getInt("key")>incfg->sub("key")->size()-1)hs->setC("key",0);
				break;
			}
			case 2:	
			{
				((TConfig<int>*) hs->get("joypad"))->operator+(1);
				if(hs->getInt("joypad")>incfg->sub("joypad")->size()-1)hs->setC("joypad",0);
				break;
			}
			case 3:	
			{
				((TConfig<int>*) hs->get("paths"))->operator+(1);
				if(hs->getInt("paths")>thcfg->sub("paths")->size()-1)hs->setC("paths",0);
				break;
			}
			case 4:	
			{
				((TConfig<int>*) hs->get("switches"))->operator+(1);
				if(hs->getInt("switches")>thcfg->sub("switches")->size()-1)hs->setC("switches",0);
				break;
			}
			case 5:	
			{
				((TConfig<int>*) hs->get("globalswitches"))->operator+(1);
				if(hs->getInt("globalswitches")>tycfg->sub("globalswitches")->size()-1)hs->setC("globalswitches",0);
				break;
			}
			case 6:	
			{
				((TConfig<int>*) hs->get("emu"))->operator+(1);
				if(hs->getInt("emu")>emcfg->sub(rtcfg->getInt("edemu"))->size()-1)hs->setC("emu",0);
				break;
			}
			case 7:	
			{
				((TConfig<int>*) hs->get("thint"))->operator+(1);
				if(hs->getInt("thint")>thcfg->sub("ints")->size()-1)hs->setC("thint",0);
				break;
			}
			case 8:	
			{
				((TConfig<int>*) hs->get("globalints"))->operator+(1);
				if(hs->getInt("globalints")>tycfg->sub("globalints")->size()-1)hs->setC("globalints",0);
				break;
			}
			case 9:	
			{
				((TConfig<int>*) hs->get("thfloat"))->operator+(1);
				if(hs->getInt("thfloat")>thcfg->sub("floats")->size()-1)hs->setC("thfloat",0);
				break;
			}
			case 10:	
			{
				((TConfig<int>*) hs->get("datafile"))->operator+(1);
				if(hs->getInt("datafile")>thcfg->sub("files")->size()-1)hs->setC("datafile",0);
				break;
			}
			case 11:	
			{
				((TConfig<int>*) hs->get("stringedit"))->operator+(1);
				if(hs->getInt("stringedit")>tycfg->sub("strings")->size()-1)hs->setC("stringedit",0);
				break;
			}
			case 12:	
			{
				((TConfig<int>*) hs->get("globalpaths"))->operator+(1);
				if(hs->getInt("globalpaths")>tycfg->sub("paths")->size()-1)hs->setC("globalpaths",0);
				break;
			}
			case 13:	
			{
				((TConfig<int>*) hs->get("online"))->operator+(1);
				if(hs->getInt("online")>oncfg->sub("themes")->size()-1)hs->setC("online",0);
				break;
			}
			case 14:	
			{
				((TConfig<int>*) hs->get("objects"))->operator+(1);
				if(hs->getInt("objects")>rtcfg->sub("object")->size()-1)hs->setC("objects",0);
				break;
			}
			default: break;
		}
	}
}

void pressescape()
{
	if(rtcfg->getBool("showhelpscreen")) {rtcfg->setBool("showhelpscreen",0);rtcfg->set("showhelp",helpid("helpscreenlist"));}
	else if (thcfg->sub("objects")->sub("video")->getInt("drawhere")==3)	stopvideo();
	else window.close();
}

char *keychar(int ekey)
{
	if(ekey==sf::Keyboard::Escape)	return (char*)"Escape";
	if(ekey==sf::Keyboard::Return)	return (char*)"Return";
	if(ekey==sf::Keyboard::Q)		return (char*)"Q";
	if(ekey==sf::Keyboard::LShift)	return (char*)"Shift_L";
	if(ekey==sf::Keyboard::RShift)	return (char*)"Shift_R";
	if(ekey==sf::Keyboard::LControl)return (char*)":LControl";
	if(ekey==sf::Keyboard::RControl)return (char*)"RControl";
	if(ekey==sf::Keyboard::LAlt)	return (char*)"LAlt";
	if(ekey==sf::Keyboard::RAlt)	return (char*)"RAlt";
	if(ekey==sf::Keyboard::LSystem)	return (char*)"LSystem";
	if(ekey==sf::Keyboard::RSystem)	return (char*)"RSystem";
	if(ekey==sf::Keyboard::Menu)	return (char*)"Menu";
	if(ekey==sf::Keyboard::SemiColon)return (char*)"Semicolon";
	if(ekey==sf::Keyboard::Slash)	return (char*)"Slash";
	if(ekey==sf::Keyboard::Equal)	return (char*)"Equal";
	if(ekey==sf::Keyboard::Dash)	return (char*)"Minus";
	if(ekey==sf::Keyboard::LBracket)return (char*)"LBracket";
	if(ekey==sf::Keyboard::RBracket)return (char*)"RBracket";
	if(ekey==sf::Keyboard::Comma)	return (char*)"Comma";
	if(ekey==sf::Keyboard::Period)	return (char*)"Period";
	if(ekey==sf::Keyboard::Quote)	return (char*)"Quote";
	if(ekey==sf::Keyboard::BackSlash)return (char*)"Backslash";
	if(ekey==sf::Keyboard::Tilde)	return (char*)"Tilde";
	if(ekey==sf::Keyboard::Space)	return (char*)"Space";
	if(ekey==sf::Keyboard::Return)	return (char*)"Return";
	if(ekey==sf::Keyboard::BackSpace)return (char*)"Backspace";
	if(ekey==sf::Keyboard::Tab)		return (char*)"Tab";
	if(ekey==sf::Keyboard::PageUp)	return (char*)"Prior";
	if(ekey==sf::Keyboard::PageDown)return (char*)"Next";
	if(ekey==sf::Keyboard::End)		return (char*)"End";
	if(ekey==sf::Keyboard::Home)	return (char*)"Home";
	if(ekey==sf::Keyboard::Insert)	return (char*)"Insert";
	if(ekey==sf::Keyboard::Delete)	return (char*)"Delete";
	if(ekey==sf::Keyboard::Add)		return (char*)"Plus";
	if(ekey==sf::Keyboard::Subtract)return (char*)"Minus";
	if(ekey==sf::Keyboard::Multiply)return (char*)"Multiply";
	if(ekey==sf::Keyboard::Divide)	return (char*)"Divide";
	if(ekey==sf::Keyboard::Pause)	return (char*)"Pause";
	if(ekey==sf::Keyboard::F1)		return (char*)"F1";
	if(ekey==sf::Keyboard::F2)		return (char*)"F2";
	if(ekey==sf::Keyboard::F3)		return (char*)"F3";
	if(ekey==sf::Keyboard::F4)		return (char*)"F4";
	if(ekey==sf::Keyboard::F5)		return (char*)"F5";
	if(ekey==sf::Keyboard::F6)		return (char*)"F6";
	if(ekey==sf::Keyboard::F7)		return (char*)"F7";
	if(ekey==sf::Keyboard::F8)		return (char*)"F8";
	if(ekey==sf::Keyboard::F9)		return (char*)"F9";
	if(ekey==sf::Keyboard::F10)		return (char*)"F10";
	if(ekey==sf::Keyboard::F11)		return (char*)"F11";
	if(ekey==sf::Keyboard::F12)		return (char*)"F12";
	if(ekey==sf::Keyboard::F13)		return (char*)"F13";
	if(ekey==sf::Keyboard::F14)		return (char*)"F14";
	if(ekey==sf::Keyboard::F15)		return (char*)"F15";
	if(ekey==sf::Keyboard::Left)	return (char*)"Left";
	if(ekey==sf::Keyboard::Right)	return (char*)"Right";
	if(ekey==sf::Keyboard::Up)		return (char*)"Up";
	if(ekey==sf::Keyboard::Down)	return (char*)"Down";
	if(ekey==sf::Keyboard::Numpad0)	return (char*)"0";
	if(ekey==sf::Keyboard::Numpad1)	return (char*)"1";
	if(ekey==sf::Keyboard::Numpad2)	return (char*)"2";
	if(ekey==sf::Keyboard::Numpad3)	return (char*)"3";
	if(ekey==sf::Keyboard::Numpad4)	return (char*)"4";
	if(ekey==sf::Keyboard::Numpad5)	return (char*)"5";
	if(ekey==sf::Keyboard::Numpad6)	return (char*)"6";
	if(ekey==sf::Keyboard::Numpad7)	return (char*)"7";
	if(ekey==sf::Keyboard::Numpad8)	return (char*)"8";
	if(ekey==sf::Keyboard::Numpad9)	return (char*)"9";
	if(ekey==sf::Keyboard::A)		return (char*)"A";
	if(ekey==sf::Keyboard::Z)		return (char*)"Z";
	if(ekey==sf::Keyboard::E)		return (char*)"E";
	if(ekey==sf::Keyboard::R)		return (char*)"R";
	if(ekey==sf::Keyboard::T)		return (char*)"T";
	if(ekey==sf::Keyboard::Y)		return (char*)"Y";
	if(ekey==sf::Keyboard::U)		return (char*)"U";
	if(ekey==sf::Keyboard::I)		return (char*)"I";
	if(ekey==sf::Keyboard::O)		return (char*)"O";
	if(ekey==sf::Keyboard::P)		return (char*)"P";
	if(ekey==sf::Keyboard::Q)		return (char*)"Q";
	if(ekey==sf::Keyboard::S)		return (char*)"S";
	if(ekey==sf::Keyboard::D)		return (char*)"D";
	if(ekey==sf::Keyboard::F)		return (char*)"F";
	if(ekey==sf::Keyboard::G)		return (char*)"G";
	if(ekey==sf::Keyboard::H)		return (char*)"H";
	if(ekey==sf::Keyboard::J)		return (char*)"J";
	if(ekey==sf::Keyboard::K)		return (char*)"K";
	if(ekey==sf::Keyboard::L)		return (char*)"L";
	if(ekey==sf::Keyboard::M)		return (char*)"M";
	if(ekey==sf::Keyboard::W)		return (char*)"W";
	if(ekey==sf::Keyboard::X)		return (char*)"X";
	if(ekey==sf::Keyboard::C)		return (char*)"C";
	if(ekey==sf::Keyboard::V)		return (char*)"V";
	if(ekey==sf::Keyboard::B)		return (char*)"B";
	if(ekey==sf::Keyboard::N)		return (char*)"N";
	if(ekey==sf::Keyboard::Num0)	return (char*)"0";
	if(ekey==sf::Keyboard::Num1)	return (char*)"1";
	if(ekey==sf::Keyboard::Num2)	return (char*)"2";
	if(ekey==sf::Keyboard::Num3)	return (char*)"3";
	if(ekey==sf::Keyboard::Num4)	return (char*)"4";
	if(ekey==sf::Keyboard::Num5)	return (char*)"5";
	if(ekey==sf::Keyboard::Num6)	return (char*)"6";
	if(ekey==sf::Keyboard::Num7)	return (char*)"7";
	if(ekey==sf::Keyboard::Num8)	return (char*)"8";
	if(ekey==sf::Keyboard::Num9)	return (char*)"9";
	else return (char*)"none";
}

bool checkkey(std::string Key,std::string comp)
{
	if (strcasecmp(incfg->sub("key")->sub(Key)->getString("key").c_str(),comp.c_str()) == 0)  return 1;
	else return 0;
}

sf::Keyboard::Key ksf(std::string Key)
{
	if (checkkey(Key,"Escape"))			return sf::Keyboard::Escape;
	if (checkkey(Key,"Return"))			return sf::Keyboard::Return;
	if (checkkey(Key,"Shift_L"))		return sf::Keyboard::LShift;
	if (checkkey(Key,"Shift_R"))		return sf::Keyboard::RShift;
	if (checkkey(Key,"Control_L"))		return sf::Keyboard::LControl;
	if (checkkey(Key,"Control_R"))		return sf::Keyboard::RControl;
	if (checkkey(Key,"Alt_L"))			return sf::Keyboard::LAlt;
	if (checkkey(Key,"Alt_R"))			return sf::Keyboard::RAlt;
	if (checkkey(Key,"Super_L"))		return sf::Keyboard::LSystem;
	if (checkkey(Key,"Super_R"))		return sf::Keyboard::RSystem;
	if (checkkey(Key,"Menu"))			return sf::Keyboard::Menu;
	if (checkkey(Key,"semicolon"))		return sf::Keyboard::SemiColon;
	if (checkkey(Key,"slash"))			return sf::Keyboard::Slash;
	if (checkkey(Key,"equal"))			return sf::Keyboard::Equal;
	if (checkkey(Key,"minus"))			return sf::Keyboard::Dash;
	if (checkkey(Key,"bracketleft"))	return sf::Keyboard::LBracket;
	if (checkkey(Key,"bracketright"))	return sf::Keyboard::RBracket;
	if (checkkey(Key,"comma"))			return sf::Keyboard::Comma;
	if (checkkey(Key,"period"))			return sf::Keyboard::Period;
	if (checkkey(Key,"dead_acute"))		return sf::Keyboard::Quote;
	if (checkkey(Key,"backslash"))		return sf::Keyboard::BackSlash;
	if (checkkey(Key,"dead_grave"))		return sf::Keyboard::Tilde;
	if (checkkey(Key,"space"))			return sf::Keyboard::Space;
	if (checkkey(Key,"KP_Enter"))		return sf::Keyboard::Return;
	if (checkkey(Key,"BackSpace"))		return sf::Keyboard::BackSpace;
	if (checkkey(Key,"Tab"))			return sf::Keyboard::Tab;
	if (checkkey(Key,"Prior"))			return sf::Keyboard::PageUp;
	if (checkkey(Key,"Next"))			return sf::Keyboard::PageDown;
	if (checkkey(Key,"End"))			return sf::Keyboard::End;
	if (checkkey(Key,"Home"))			return sf::Keyboard::Home;
	if (checkkey(Key,"Insert"))			return sf::Keyboard::Insert;
	if (checkkey(Key,"Delete"))			return sf::Keyboard::Delete;
	if (checkkey(Key,"KP_Add"))			return sf::Keyboard::Add;
	if (checkkey(Key,"KP_Subtract"))	return sf::Keyboard::Subtract;
	if (checkkey(Key,"KP_Multiply"))	return sf::Keyboard::Multiply;
	if (checkkey(Key,"KP_Divide"))		return sf::Keyboard::Divide;
	if (checkkey(Key,"Pause"))			return sf::Keyboard::Pause;
	if (checkkey(Key,"F1"))				return sf::Keyboard::F1;
	if (checkkey(Key,"F2"))				return sf::Keyboard::F2;
	if (checkkey(Key,"F3"))				return sf::Keyboard::F3;
	if (checkkey(Key,"F4"))				return sf::Keyboard::F4;
	if (checkkey(Key,"F5"))				return sf::Keyboard::F5;
	if (checkkey(Key,"F6"))				return sf::Keyboard::F6;
	if (checkkey(Key,"F7"))				return sf::Keyboard::F7;
	if (checkkey(Key,"F8"))				return sf::Keyboard::F8;
	if (checkkey(Key,"F9"))				return sf::Keyboard::F9;
	if (checkkey(Key,"F10"))			return sf::Keyboard::F10;
	if (checkkey(Key,"F11"))			return sf::Keyboard::F11;
	if (checkkey(Key,"F12"))			return sf::Keyboard::F12;
	if (checkkey(Key,"F13"))			return sf::Keyboard::F13;
	if (checkkey(Key,"F14"))			return sf::Keyboard::F14;
	if (checkkey(Key,"F15"))			return sf::Keyboard::F15;
	if (checkkey(Key,"Left"))			return sf::Keyboard::Left;
	if (checkkey(Key,"Right"))			return sf::Keyboard::Right;
	if (checkkey(Key,"Up"))				return sf::Keyboard::Up;
	if (checkkey(Key,"Down"))			return sf::Keyboard::Down;
	if (checkkey(Key,"KP_0"))			return sf::Keyboard::Numpad0;
	if (checkkey(Key,"KP_1"))			return sf::Keyboard::Numpad1;
	if (checkkey(Key,"KP_2"))			return sf::Keyboard::Numpad2;
	if (checkkey(Key,"KP_3"))			return sf::Keyboard::Numpad3;
	if (checkkey(Key,"KP_4"))			return sf::Keyboard::Numpad4;
	if (checkkey(Key,"KP_5"))			return sf::Keyboard::Numpad5;
	if (checkkey(Key,"KP_6"))			return sf::Keyboard::Numpad6;
	if (checkkey(Key,"KP_7"))			return sf::Keyboard::Numpad7;
	if (checkkey(Key,"KP_8"))			return sf::Keyboard::Numpad8;
	if (checkkey(Key,"KP_9"))			return sf::Keyboard::Numpad9;
	if (checkkey(Key,"A"))				return sf::Keyboard::A;
	if (checkkey(Key,"Z"))				return sf::Keyboard::Z;
	if (checkkey(Key,"E"))				return sf::Keyboard::E;
	if (checkkey(Key,"R"))				return sf::Keyboard::R;
	if (checkkey(Key,"T"))				return sf::Keyboard::T;
	if (checkkey(Key,"Y"))				return sf::Keyboard::Y;
	if (checkkey(Key,"U"))				return sf::Keyboard::U;
	if (checkkey(Key,"I"))				return sf::Keyboard::I;
	if (checkkey(Key,"O"))				return sf::Keyboard::O;
	if (checkkey(Key,"P"))				return sf::Keyboard::P;
	if (checkkey(Key,"Q"))				return sf::Keyboard::Q;
	if (checkkey(Key,"S"))				return sf::Keyboard::S;
	if (checkkey(Key,"D"))				return sf::Keyboard::D;
	if (checkkey(Key,"F"))				return sf::Keyboard::F;
	if (checkkey(Key,"G"))				return sf::Keyboard::G;
	if (checkkey(Key,"H"))				return sf::Keyboard::H;
	if (checkkey(Key,"J"))				return sf::Keyboard::J;
	if (checkkey(Key,"K"))				return sf::Keyboard::K;
	if (checkkey(Key,"L"))				return sf::Keyboard::L;
	if (checkkey(Key,"M"))				return sf::Keyboard::M;
	if (checkkey(Key,"W"))				return sf::Keyboard::W;
	if (checkkey(Key,"X"))				return sf::Keyboard::X;
	if (checkkey(Key,"C"))				return sf::Keyboard::C;
	if (checkkey(Key,"V"))				return sf::Keyboard::V;
	if (checkkey(Key,"B"))				return sf::Keyboard::B;
	if (checkkey(Key,"N"))				return sf::Keyboard::N;
	if (checkkey(Key,"0"))				return sf::Keyboard::Num0;
	if (checkkey(Key,"1"))				return sf::Keyboard::Num1;
	if (checkkey(Key,"2"))				return sf::Keyboard::Num2;
	if (checkkey(Key,"3"))				return sf::Keyboard::Num3;
	if (checkkey(Key,"4"))				return sf::Keyboard::Num4;
	if (checkkey(Key,"5"))				return sf::Keyboard::Num5;
	if (checkkey(Key,"6"))				return sf::Keyboard::Num6;
	if (checkkey(Key,"7"))				return sf::Keyboard::Num7;
	if (checkkey(Key,"8"))				return sf::Keyboard::Num8;
	if (checkkey(Key,"9"))				return sf::Keyboard::Num9;

	return sf::Keyboard::F1;
}

void toggleedit()
{
	if(rtcfg->getBool("showhelpscreen"))	sethelp(0);
	else
	{
		((TConfig<int>*) rtcfg->get("edmode"))->operator+(1);
		if(rtcfg->getInt("edmode")>3)rtcfg->set("edmode",0);

		switch(rtcfg->getInt("edmode"))
		{
			case 0:	{shownote(lacfg->getString("editmode")+" "+lacfg->getString("disabled"));break;}
			case 1:	{shownote(lacfg->getString("editmode")+" "+lacfg->getString("menuentries"));break;}
			case 2:	{shownote(lacfg->getString("editmode")+" "+lacfg->getString("cmdargs"));break;}
			case 3:	{shownote(lacfg->getString("editmode")+" "+lacfg->getString("title"));break;}
			default: break;
		}	
	}
}

void input()
{
	int ekey;
		while( window.pollEvent(event))
		{
			
			if((rtcfg->getBool("searchmode") || rtcfg->getBool("editmode")) && (event.type == event.TextEntered))
			{

				if (rtcfg->getBool("searchmode"))	{searchinput();break;}

				if (rtcfg->getBool("exporting"))	{editorinput(rtcfg->getString("exportname"),3);break;}

				if (rtcfg->getBool("editmode") && ((Ishowmenu()>=0) && (Iem()==0 || (Iem()!=0 && rtcfg->getInt("edmode")==3))) && !Bvidmenu() && !Bp3tmenu() && !Bthememenu())	
				{startselection();break;}
			} 
			
			//ekey=event.Text.Unicode;
			if ((!rtcfg->getBool("searchmode") && !rtcfg->getBool("editmode") && rtcfg->getInt("inputchanged")==0) && (event.type== event.KeyPressed))
			{
				ekey=event.key.code;
	
				if (rtcfg->getBool("editkeys"))
				{
					std::string edkeystring=incfg->sub("key")->getKey(rtcfg->sub("helpsel")->getInt("key"));
					incfg->sub("key")->subC(edkeystring)->set("key",keychar(ekey));
					rtcfg->setBool("editkeys",0);	
				}
		
				else if (!rtcfg->getBool("editkeys"))
				{
				
					if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
					{
					if (ekey==ksf("startselection"))	startselection();
					if (ekey==ksf("changeicon"))		{
															rtcfg->setBool("choosemicon",!rtcfg->getBool("choosemicon"));
															if(rtcfg->getBool("choosemicon"))	{rtcfg->setBool("choosewp",0);shownote(lacfg->getString("changemenuiconon"));}
															else			{shownote(lacfg->getString("changemenuiconoff"));}
														}
					if (ekey==ksf("changebg"))			{
															rtcfg->setBool("choosewp",!rtcfg->getBool("choosewp"));
															if(rtcfg->getBool("choosewp"))	{rtcfg->setBool("choosemicon",0);shownote(lacfg->getString("changewpon"));}
															else							{shownote(lacfg->getString("changewpoff"));}
														}

					if (ekey==ksf("export"))			{startexporttheme();}
					if (ekey==ksf("togglesetpicking"))	{setpicktog();}
					if (ekey==ksf("togglecolorpicking")){colorpicktog();}
					if (ekey==ksf("rotationpicking"))	{changerotpick();}
					if (ekey==ksf("toggleedit"))		{toggleedit();}
					if (ekey==ksf("searchmode"))		{rtcfg->set("inputchanged",3);}
//					if (ekey==ksf(Keyreloadxml))		{if(thcfg->sub("objects")->sub("slide1")->getBool("enabled")){readxmlprogram();}}
//					if (ekey==ksf(Keytoggleslideshow))	{toggleslideshow();}
//					if (ekey==ksf("rss"))				thcfg->sub("objects")->sub("rss")->getBool("enabled"));if(thcfg->sub("objects")->sub("rss")->getBool("enabled") &&!rtcfg->getBool("rssinitialized")){initrss();readrss();}};
					if (ekey==ksf("help"))				{rtcfg->setBool("showhelpscreen",1);sethelp(1);}
					if (ekey==ksf("switchhelpscreen"))	{rtcfg->setBool("showhelpscreen",!rtcfg->getBool("showhelpscreen"));}

					if (ekey==ksf("savecfgs"))			{saveglobalcfgs();}
					if (ekey==ksf("savetheme"))			{savetheme();}

					if (ekey==ksf("updatemenu"))		{parsedesktopfiles();savemenuxmlcfg();}
					if (ekey==ksf("favourites"))		{addtofavourites();savemenuxmlcfg();}

					if (ekey==ksf("minus"))				{dokeychangeval(0);}
					if (ekey==ksf("plus"))				{dokeychangeval(1);}

					if (ekey==ksf("choiceleft"))		{if(rtcfg->getBool("menueditmode"))moveentry(-1);else {if(Iem()!=0)gameid(0);};}
					if (ekey==ksf("choiceright"))		{if(rtcfg->getBool("menueditmode"))moveentry(1) ;else {if(Iem()!=0)gameid(1);};}
					
					if (ekey==ksf("decchangevalue"))	{playsfx();	edchangevalue(-1);}
					if (ekey==ksf("incchangevalue"))	{playsfx();	edchangevalue(1);}
					if (ekey==ksf("fastdown"))			{menupagedown();}
					if (ekey==ksf("fastup"))			{menupageup();}
					if (ekey==ksf("toggleemu"))			{toggleemumode();}
					if (ekey==ksf("togglevideomenu"))	{togglevideomenu();}
					if (ekey==ksf("updatepreview"))		forcepreviewupdate();

					if (ekey==ksf("changetyphontheme"))	togglethememenu();
					if (ekey==ksf("togglep3tmenu"))		togglep3tmenu();
					if (ekey==ksf("changeshader"))		{
															((TConfig<int>*) thcfg->sub("shader")->get("usebackshader"))->operator+(1);
															if(useBackShader()>=shcfg->sub("shaders")->size()){thcfg->sub("shader")->set("usebackshader",0);}
															loadshadertex();
															shownote(lacfg->getString("changeshader")+" "+getPrefix(shcfg->sub("shaders")->sub(useBackShader())->getString("name"))+" "+lacfg->getString("createdby")+" "+shcfg->sub("shaders")->sub(useBackShader())->getString("author"));
														}
					if (ekey==ksf("toggleshader"))		toggleshaders();

					if (ekey==ksf("up"))				dokeyup();
					if (ekey==ksf("down"))				dokeydown();
					if (ekey==ksf("left"))				dokeyleft();
					if (ekey==ksf("right"))				dokeyright();
					}

					if (ekey==ksf("exit"))				pressescape();
					if (ekey==ksf("movewindow"))		movewindow(tycfg->sub("globalswitches")->getBool("movewindowup"));
					if (ekey==ksf("togglevideo"))		switchplayvideohere();
					if (ekey==ksf("togglepicking"))		mousepicktog();
					if (ekey==ksf("videovolume"))		videovol(tycfg->sub("globalints")->getInt("changevaluespeed"));
					if (ekey==ksf("menueditmode"))		{
															rtcfg->setBool("menueditmode",!rtcfg->getBool("menueditmode"));
															if(rtcfg->getBool("menueditmode"))	{shownote(lacfg->getString("menueditmode")+" "+lacfg->getString("enabled"));}
															else			{shownote(lacfg->getString("menueditmode")+" "+lacfg->getString("disabled"));}
														}
				}
			}
			
			if(event.type==event.Closed)
				window.close();

			if ((!rtcfg->getBool("searchmode") && !rtcfg->getBool("editmode")) && (event.type==event.KeyReleased))
			{
				unsigned short stillpressed = 0;

				if (!stillpressed)
				{
					if 
					(
						ekey==ksf("up") 		||
						ekey==ksf("down")		||
						ekey==ksf("fastdown")	||
						ekey==ksf("fastup")
					)
					{
						rtcfg->set("tempselr",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("R"));
						rtcfg->set("tempselg",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("G"));
						rtcfg->set("tempselb",thcfg->sub("objects")->sub("menu")->sub("menuselfont")->getInt("B"));
						rtcfg->setBool("menuisscrolling",0);
					}
								
					if 
					(
						ekey==ksf("up")			||
						ekey==ksf("down")		||
						ekey==ksf("right")		||
						ekey==ksf("left")		||
						ekey==ksf("toggleemu")	||
						ekey==ksf("fastdown")	||
						ekey==ksf("fastup")		||
						ekey==ksf("minus")		||
						ekey==ksf("plus")			
					)
					{
						selchanged();
						if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled") && thcfg->sub("ints")->getInt("menuicondockstyle")==1){rtcfg->setBool("undock",1);}
					}
				}
			}

			if(event.type==event.JoystickButtonPressed && rtcfg->getBool("usejoypad")) 
			{
				if (rtcfg->getBool("waitforjoypad"))
				{
					int done=0;
					while (!done)
					{
						if (event.JoystickButtonPressed)
						{
							incfg->sub("joypad")->set(rtcfg->sub("helpsel")->getInt("joypad"),(int)event.joystickButton.button);
							done = 1;
						}
					}
					rtcfg->setBool("waitforjoypad",0);
				}
				else
				{
					if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
					{
					int jodev=tycfg->sub("globalints")->getInt("joypaddevice");
					
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("dechelpscreen")))		sethelp(0);
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("inchelpscreen")))		rtcfg->setBool("showhelpscreen",!rtcfg->getBool("showhelpscreen"));
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("nexttyphontheme")))		nexttyphontheme();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("switchplayvideohere")))	switchplayvideohere();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("menupageup")))			menupageup();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("menupagedown")))			menupagedown();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("togglevideomenu")))		togglevideomenu();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("toggleemumode")))		toggleemumode();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("escape")))				pressescape();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("startselection")))		startselection();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("up")))					dokeyup();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("right")))				dokeyright();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("down")))					dokeydown();
					if (sf::Joystick::isButtonPressed(jodev, incfg->sub("joypad")->getInt("left")))					dokeyleft();
					}
				}
			}

			if(event.type==event.JoystickMoved && rtcfg->getBool("usejoypad"))
			{
				if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
				{
					if (event.joystickMove.axis == sf::Joystick::X)
					{
						if (event.joystickMove.position > tycfg->sub("globalints")->getInt("joypaddeadzone"))
						dokeyright();
						else if (event.joystickMove.position < -tycfg->sub("globalints")->getInt("joypaddeadzone"))
						dokeyleft();
					}
					else if (event.joystickMove.axis == sf::Joystick::Y)
					{
						if (event.joystickMove.position > tycfg->sub("globalints")->getInt("joypaddeadzone"))
						dokeydown();
						else if (event.joystickMove.position < -tycfg->sub("globalints")->getInt("joypaddeadzone"))
						dokeyup();
					}
				}
			}
			
			if(event.type==event.MouseButtonPressed)
			{
				if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==3 && pick()==2)
				{
					switch(event.mouseButton.button) 
					{
						case sf::Mouse::Left:	{rtcfg->setBool("clickedleft",1);break;}
						case sf::Mouse::Right:	{rtcfg->setBool("clickedright",1);break;}
						default:break;			
					}
				}
				else
				{
				switch(event.mouseButton.button) 
				{
					case sf::Mouse::Left:
					{
						//0
						if(pick()==1 ||	pick()==2 || pick()==3)
						{rtcfg->setBool("clickedleft",1);break;}
						
						if(pick()==4){mousetogobject();break;}

						{startselection();break;}
					}

					case sf::Mouse::Middle:
					{
						if (pick()==1)	{
											rtcfg->setBool("rotatez",!rtcfg->getBool("rotatez"));
											if(rtcfg->getBool("rotatez"))	shownote(lacfg->getString("zrot"));
											else 							shownote(lacfg->getString("xyrot"));
											break;
										}
						if (pick()==2)	{mouseblockobjects();break;}
						if (pick()==3)	{mouseblockobjects();break;}
						if (pick()==4)	{mouseblockobjects();break;}

						dokeyleft();
						break;
					}

					case sf::Mouse::Right:
					{
						if	(pick()==2 || pick()==3 || pick()==4)	{rtcfg->setBool("clickedright",1);break;}
						
						if (pick()==1)
						{
							if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size())
							{
								thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("rotx",0);
								thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("roty",0);
								thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("rotz",0);
							}
							break;
						}
						dokeyright();
						break;
					}

					default:break;
				}
				}
			}
			if (event.type == event.MouseWheelMoved)
			{
				if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
				{
				if (event.mouseWheel.delta==1)
				{
						if (pick()==2)		{mousezobjects(-1);break;}
						if (pick()==4)		{mousechangedata(0);break;}
						if (pick()==3)		{
													if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size())
													{
														((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("A"))->operator+(tycfg->sub("globalints")->getInt("changevaluespeed"));
														if(thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getInt("A")>=255){thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("A",255);}
													}
													break;
											}
						if(pick()==1)		{mousedepthobjects(-(float)tycfg->sub("globalints")->getInt("changevaluespeed"));break;}
									
						dokeyup();

						break;
				}
				if (event.mouseWheel.delta==-1)	
				{
						if(pick()==2)		{mousezobjects(+1);break;}
						if(pick()==4)		{mousechangedata(1);break;}
						if(pick()==3)		{
													if(rtcfg->getInt("picked")<=thcfg->sub("objects")->size())
													{
														((TConfig<int>*) thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->get("A"))->operator-(tycfg->sub("globalints")->getInt("changevaluespeed"));
														if(thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->getInt("A")<=0){thcfg->sub("objects")->sub(rtcfg->getInt("picked"))->set("A",0);}
													}
													break;
											}
						if(pick()==1)		{mousedepthobjects((float)tycfg->sub("globalints")->getInt("changevaluespeed"));break;}
						
						dokeydown();
						break;
				}		
				}
			}

			if(event.type==event.MouseButtonReleased)
			{
				rtcfg->set("picked",thcfg->sub("objects")->get("wallpaper")->id);
				switch(event.mouseButton.button) 
				{
					case sf::Mouse::Left:
					{
						rtcfg->setBool("clickedleft",0);
						break;
					}	
					case sf::Mouse::Middle:
					{
						if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled") && thcfg->sub("ints")->getInt("menuicondockstyle")==1){rtcfg->setBool("undock",1);}
						break;
					}
					
					case sf::Mouse::Right:
					{
						rtcfg->setBool("clickedright",0);
						if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled") && thcfg->sub("ints")->getInt("menuicondockstyle")==1){rtcfg->setBool("undock",1);}
						break;
						
					}						
					default: break;
				}
				rtcfg->setBool("stillpicked",0);
			}
			
	//		if(event.type==event.Resized){resizescreen(event.size.width, event.size.height);}

			if(event.type==event.MouseMoved)
			{
				if(	pick()==1 || pick()==2 || pick()==3 || pick()==4 || 
					(shcfg->sub("shaders")->sub(useBackShader())->getBool("mouse") && useShader())
				)
				{
					sf::Vector2i position = sf::Mouse::getPosition(window);
					rootX = position.x;
					rootY = position.y;

					if(pick()==1 || pick()==2 || pick()==3 || pick()==4)
					{
						if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==3)  scalefullscreenvideo();
						else mousepos(rootX,rootY);
						oldrootX=rootX;
						oldrootY=rootY;
					}
				}
			}
		}	
}
