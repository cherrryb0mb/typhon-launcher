#include "editor.h"
#include "videoplayerhandling.h"
#include "fonthandling.h"
#include "values.h"
#include "glhandling.h"
#include "windowhandling.h"
#include "inputhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"

std::string thisstring;

void drawsearcheditbox()
{
	glDisable( GL_DEPTH_TEST );
	glPushMatrix();
	texcol("searcheditbox");
	TyphonConfig* se=rtcfg->sub("objects")->sub("searcheditbox");
	TyphonConfig* te=thcfg->sub("objects")->sub("searcheditbox");

	glTranslatef
	(
		se->getFloat("posx"),
		se->getFloat("posy"),
		te->getInt("posz")+1
	);
	rotateme("searcheditbox");
	std::string mode;
	if (rtcfg->getBool("editmode"))		
	{
		if(rtcfg->getBool("exporting"))	mode=lacfg->getString("exportas");
		else							mode=lacfg->getString("editmode");
	}
	if (rtcfg->getBool("searchmode"))	{mode=lacfg->getString("searchmode");}
		
	//draw background
	sf::Texture::bind(getTexture(rtcfg->sub("textureids")->sub("settings")->id)); 

	drawquad(se->getFloat("width") ,se->getFloat("height"));

	se->set("width",settingsfontfx.getWidth(thisstring)+50);
	if(se->getFloat("width")<settingsfontfx.getWidth(mode)+100)se->set("width",settingsfontfx.getWidth(mode)+100);
		
	glPushMatrix();
	settingsfontfx.setColour(veccol4f(255,1,1,255));
	glTranslatef(50,-50, te->getInt("posz")+2);
	drawsettingstext(mode,0);
	glPopMatrix();
		
	glPushMatrix();
	glTranslatef(20,-100, te->getInt("posz")+2);

	TyphonConfig* he=rtcfg->sub("helpsel");

	if (rtcfg->getInt("showhelp")==helpid("themepaths") || 
		rtcfg->getInt("showhelp")==helpid("globalpaths") || 
		(rtcfg->getInt("showhelp")==helpid("emulatorsettings") && (
		he->getInt("emu")==2 ||
		he->getInt("emu")==5 ||
		he->getInt("emu")==6 ||
		he->getInt("emu")==7)))
	{
		FILE *pathcheck = fopen(setEnvhome(thisstring).c_str(),"r");
		if(pathcheck)
		{
			settingsfontfx.setColour(veccol4f(1,255,1,255));
			fclose(pathcheck);
		}
	}
	else
	{
		if(!expathcheck(thisstring.c_str()))
		settingsfontfx.setColour(veccol4f(1,255,1,255));
	}
	
	drawsettingstext(thisstring,0);
	glPopMatrix();

	glPopMatrix();
	glEnable( GL_DEPTH_TEST );
}

void searchlist(std::string searchstring)
{
	int foundsearch=0;
	std::string comp;

	for(int i=Ics()+1; i < Ime(); i++)
	{
		if (!foundsearch)
		{
			if(Iem()!=0)	comp=rmcfg->sub("Roms")->sub(IeID()-1)->sub(i)->getString("file");
			else 			comp=mentry(Ishowmenu()-1,i);
			
			if (compare(comp.c_str(),searchstring.c_str()))
			{
				foundsearch=1;
				setcs((i)%Ime());
			}
			else
			{setcs(0);}
		}
	}
}

void quitsearch()
{
	rtcfg->set("inputchanged",1);
	rtcfg->setBool("edsearchchanged",0);
	if(Ishowmenu()!=0 && Ishowmenu()!=8)
	searchlist(thisstring);
}

void searchinput()
{
	switch(event.key.code)
	{
		case 8:
		// Remove last character if pressed 8(backspace)
		if(thisstring.size())
		{
			thisstring=thisstring.substr(0, thisstring.size()-1);
			rtcfg->setBool("edsearchchanged",1);
		}
		break;
		case 13:
		// When press enter, do stuff:
		quitsearch();
		break;
		default:
		// Not backspace or enter? Append character to bufferstring!
		std::string tmp;
		tmp= static_cast<char>(event.text.unicode);
		thisstring+=tmp.c_str();
		rtcfg->setBool("edsearchchanged",1);
		break;
	}
	rtcfg->setBool("selchanged",1);
}

void quitedit(std::string *toeditstring,int type)
{
	rtcfg->set("inputchanged",1);
	rtcfg->setBool("edsearchchanged",0);
	switch(type)
	{
		case 1: // menu entry
		mecfg->sub(Ishowmenu()-1)->sub("entry")->sub(Ics())->set("binary",thisstring);
		break;

		case 2: // program args
		mecfg->sub(Ishowmenu()-1)->sub("entry")->sub(Ics())->set("args",thisstring);
		break;
		
		case 3: // export theme name
		{
			rtcfg->set("exportname",thisstring);
			rtcfg->set("exportnow",1);
			break;
		}

		case 4: // themepaths
		thcfg->sub("paths")->setC(thcfg->sub("paths")->getKey(rtcfg->sub("helpsel")->getInt("paths")),thisstring);
		checkpaths();
		break;

		case 5: // global paths
		tycfg->sub("paths")->setC(tycfg->sub("paths")->getKey(rtcfg->sub("helpsel")->getInt("globalpaths")),thisstring);
		//if(rtcfg->sub("helpsel")->getInt("globalpaths")==tycfg->sub("paths")->get("previewpath")->id)
		checkpaths();
		break;	

		case 6: // emu settings
		emcfg->sub(rtcfg->getInt("edemu"))->setC(emcfg->sub(rtcfg->getInt("edemu"))->getKey(rtcfg->sub("helpsel")->getInt("emu")),thisstring);
		break;

		case 7: // menu title
		mecfg->sub(Ishowmenu()-1)->set("title",thisstring);
		break;

		case 8: // emu title
		emcfg->sub(IeID()-1)->set("emutitle",thisstring);
		break;
		
		case 9: // global strings
		tycfg->sub("strings")->setC(tycfg->sub("strings")->getKey(rtcfg->sub("helpsel")->getInt("stringedit")),thisstring);
		break;
		default: break;
	}

	thisstring="\0";
}

//based on snipplet found in sfml-dev.org thread 
// "How do I display a string while typing it"

void editorinput(std::string toeditstring,int type)
{
	if (!rtcfg->getBool("edsearchchanged"))	{thisstring=toeditstring;}
	if (rtcfg->getBool("editmode"))
	{
		switch(event.key.code)
		{
			case 8:
			case sf::Keyboard::Delete:
			case sf::Keyboard::BackSpace:
			// Remove last character if pressed 8(backspace)
			if(toeditstring.size())
			{
				thisstring=thisstring.substr(0, thisstring.size()-1);
				rtcfg->setBool("edsearchchanged",1);
			}
			break;
			case 13:
			// When press enter, do stuff:
			quitedit(&toeditstring,type);
			
			break;
			default:
			// Not backspace or enter? Append character to bufferstring!
			std::string tmp;
			tmp= static_cast<char>(event.text.unicode);
			thisstring+=tmp.c_str();
			rtcfg->setBool("edsearchchanged",1);
			break;
		}
	}
}
