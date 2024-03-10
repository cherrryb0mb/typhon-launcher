#include "fonthandling.h"
#include "editor.h"
#include "themehandling.h"
#include "windowhandling.h"
#include "glhandling.h"
#include "systeminfo.h"
#include "filehandling.h"
#include "inputhandling.h"
#include "values.h"
#include "xmlconfig.h"

FXFont 		font, musicdurationfontfx,
			titlefontfx,clockfontfx, datefontfx,
			settingsfontfx, sysinfofontfx, notifyfontfx,
			menuselfontfx, rssfontfx, counterfontfx, rominfofontfx;

vec4f veccol4f(int a,int b,int c, int d)
{
	return	vec4f(norm255(a),norm255(b),norm255(c),norm255(d));
}

std::string itos (int intos)
{
	std::stringstream out;
	out << intos;
	return out.str();
}

std::string ftos (float intos)
{
	std::stringstream out;
	out << intos;
	return out.str();
}

void textcol(std::string thistext, FXFont *thisfont,int alpha)
{
	if
	(
		rtcfg->getInt("picked")==thcfg->sub("objects")->get(thistext)->id &&
		pick()!=3 && pick()!=4
	)
	{
		if(rtcfg->sub("block")->getBool(thcfg->sub("objects")->get(thistext)->id))
		{thisfont->setColour(veccol4f(1,1,255,rtcfg->getInt("menualpha")));}
		else
		{thisfont->setColour(veccol4f(255,1,1,rtcfg->getInt("menualpha")));}
	}	
	else
	{
		if (pick()==4 && !thcfg->sub("objects")->sub(thistext)->getBool("enabled"))
		{thisfont->setColour(veccol4f(127,127,127,66));}
		else
		{
			if (alpha==0) alpha=thcfg->sub("objects")->sub(thistext)->getInt("A");

			if(thcfg->sub("switches")->getBool("usemonthcolors"))
			{
				TyphonConfig* mc=rtcfg->sub("monthcolors")->sub(rtcfg->getInt("monthcolor"));
				thisfont->setColour(vec4f(
				mc->getFloat("R"),
				mc->getFloat("G"),
				mc->getFloat("B"),
				norm255(alpha)
				));	
			}
			else

			{
				TyphonConfig* co=thcfg->sub("objects")->sub(thistext);
				thisfont->setColour(veccol4f
				(co->getInt("R"),co->getInt("G"),co->getInt("B"),alpha));
			}
		}
	}
}

float drawfontsize(int fontchange)
{
	return fontchange/(hdh/rtcfg->getInt("height"));
}

void textrot(std::string thistext, std::string drawtext,FXFont *thisfont, int heightcheck)
{
	TyphonConfig* ro=rtcfg->sub("objects")->sub(thistext);
	TyphonConfig* to=thcfg->sub("objects");
	TyphonConfig* tt=thcfg->sub("objects")->sub(thistext);

	if (heightcheck==1) // menulist
	ro->set("height",drawfontsize(to->sub("menu")->getInt("size")));
	else if (heightcheck==2) // menu selection
	ro->set("height",drawfontsize(to->sub("menu")->getInt("menuselfontsize")));
	else if (heightcheck==3) // sysinfo
	ro->set("height",drawfontsize(to->sub("cpu")->getInt("size")));
	else
	ro->set("height",drawfontsize(to->sub(thistext)->getInt("size")));
	ro->set("width",thisfont->getWidth(drawtext));

	glTranslatef(ro->getFloat("width")/2,ro->getFloat("height")/4,0);

	if(thcfg->sub("switches")->getBool("rotateselection") && strcmp(thistext.c_str(), "menu") == 0)
	{
		((TConfig<float>*) rtcfg->get("rotselx"))->operator+(10);
		if(rtcfg->getFloat("rotselx")>=360)rtcfg->set("rotselx",(float)0);
		glRotatef(rtcfg->getFloat("rotselx"), 1.0, 0.0, 0.0);
	}

	glRotatef(tt->getInt("rotx"), 1.0, 0.0, 0.0);
	glRotatef(tt->getInt("roty"), 0.0, 1.0, 0.0);
	glRotatef(tt->getInt("rotz"), 0.0, 0.0, 1.0);	

	glTranslatef(-ro->getFloat("width")/2,-ro->getFloat("height")/4,0);

	float rx = (tt->getInt("roty")); // 0° to 90°
	float ry = (tt->getInt("rotx")); // vector rx,ry has rotationindependant length

	if (rx==0 && ry==0) setdropshadow();
	else
	{
		float nlength =tt->getFloat("depth");
		float rlength = sqrt((rx*rx)+(ry*ry)); // length of vector rx,ry
		
		float nx=rx/rlength;
		float ny=ry/rlength; // vector nx,ny now has length 1

		thisfont->shadowOffset(-nx*nlength,-ny*nlength);
	}
}

void switchfont(bool up)
{
	if (rtcfg->sub("fonts")->size()>0)
	{
		if(up)
		{
			if(rtcfg->getInt("fontcount")==0)	{rtcfg->set("fontcount",rtcfg->sub("fonts")->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get("fontcount"))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt("fontcount")==rtcfg->sub("fonts")->size()-1)	{rtcfg->set("fontcount",0);}
			else 							{((TConfig<int>*) rtcfg->get("fontcount"))->operator+(1);}
		}
		thcfg->sub("files")->setC("fontfile",rtcfg->sub("fonts")->sub(rtcfg->getInt("fontcount"))->getString("file"));
		updatefont();
	}
}

void listfonts()
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir((rtcfg->sub("scanned")->getString("fontpath")).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		int k=0;
		for (int j=0; j<num_entries; j++)
		{
			if (strstr(sdit[j]->d_name, ".ttf") ||
				strstr(sdit[j]->d_name, ".otf"))
			{
				std::stringstream entry;
				entry << "fonts" << k;
				rtcfg->sub("fonts")->sub(entry.str(),k++)->set("file",sdit[j]->d_name);
			}
		}
	}
}

void drawmenuseltext(std::string thistext)
{
	TyphonConfig* tm=thcfg->sub("objects")->sub("menu");

	if(rtcfg->getBool("iscursel"))	{menuselfontfx.setColour(veccol4f(rtcfg->getInt("tempselr"),rtcfg->getInt("tempselg"),rtcfg->getInt("tempselb"),rtcfg->getInt("menualpha")));}
	else							{menuselfontfx.setColour(veccol4f(tm->getInt("R"),tm->getInt("G"),tm->getInt("B"),rtcfg->getInt("menualpha")));}

	char striptext[500];
	int entrylength= strlen(thistext.c_str());
	strcpy(striptext,thistext.c_str());

	if (entrylength >thcfg->sub("ints")->getInt("menumaxtextsellength"))
	{
	strncpy(striptext,thistext.c_str(),thcfg->sub("ints")->getInt("menumaxtextsellength"));
	striptext[thcfg->sub("ints")->getInt("menumaxtextsellength")]='\0';
	strcat (striptext, "...");
	}

	rtcfg->sub("objects")->sub("menu")->set("width",menuselfontfx.getWidth(striptext));
	
	if (thcfg->sub("ints")->getInt("menualign")==2)
	{glTranslatef(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub("menu")->getFloat("width"),0,0);} //right

	if (thcfg->sub("ints")->getInt("menualign")==3)
	{glTranslatef((rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub("menu")->getFloat("width"))/2,0,0);} //right

	textrot("menu",striptext,&menuselfontfx,2);
	menuselfontfx.draw(0.0,0.0, striptext,thcfg->sub("objects")->sub("menu")->getFloat("depth"));
}

void drawmenutext(std::string thistext,int showid)
{
	if(thcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(showid))->getBool("enabled") || pick()==4)
	{
	char striptext[500];
	int entrylength= strlen(thistext.c_str());
	strcpy(striptext,thistext.c_str());

	if (entrylength >thcfg->sub("ints")->getInt("menumaxtextlength"))
	{
		strncpy(striptext,thistext.c_str(),thcfg->sub("ints")->getInt("menumaxtextlength"));
		striptext[thcfg->sub("ints")->getInt("menumaxtextlength")]='\0';
		strcat (striptext, "...");
	}

	glPushMatrix();
	
	rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(showid))->set("width",font.getWidth(striptext));
	
	if (thcfg->sub("ints")->getInt("menualign")==2)
	{glTranslatef(rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(showid))->getFloat("width"),0,0);}

	if (thcfg->sub("ints")->getInt("menualign")==3)
	{glTranslatef((rtcfg->sub("objects")->sub("menu")->getInt("maxwidth")-rtcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(showid))->getFloat("width"))/2,0,0);}
	
	textrot(thcfg->sub("objects")->getKey(showid),striptext,&font,1);
	font.draw(0.0,0.0, striptext,thcfg->sub("objects")->sub(thcfg->sub("objects")->getKey(showid))->getFloat("depth"));

	glPopMatrix();
	}
}

void drawgenerictext(std::string thistext,float depth)
{
	rtcfg->set("dummyfontwidth",font.getWidth(thistext));
	font.draw(0.0,0.0, thistext,depth);
}

void drawtitletext(std::string thistext)
{
	glPushMatrix();
	textcol("title",&titlefontfx,rtcfg->getInt("menualpha"));
	textrot("title",thistext,&titlefontfx,0);
	float tdepth=thcfg->sub("objects")->sub("title")->getFloat("depth");
	
	if (thcfg->sub("ints")->getInt("titlealign")==1) //left
	{
		titlefontfx.draw(0.0,0.0, thistext,tdepth);
	}
	
	if (thcfg->sub("ints")->getInt("titlealign")==2) //right
	{
		titlefontfx.draw(thcfg->sub("objects")->sub("title")->getInt("maxtitlelength")-rtcfg->sub("objects")->sub("title")->getFloat("width"),0.0, thistext,tdepth);
	}
		
	if (thcfg->sub("ints")->getInt("titlealign")==3) //center
	{
		titlefontfx.draw((thcfg->sub("objects")->sub("title")->getInt("maxtitlelength")-rtcfg->sub("objects")->sub("title")->getFloat("width"))/2,0.0, thistext,tdepth);
	}
	
	glPopMatrix();
}

void drawsettingstext(std::string thistext, bool align)
{
	if (align)	settingsfontfx.draw(thcfg->sub("objects")->sub("setbox")->getInt("width")/2-settingsfontfx.getWidth(thistext),0.0, thistext,0.0);
	else 		settingsfontfx.draw(0.0,0.0, thistext,0.0);
}

void drawrsstext(std::string thistext)
{
	textcol("rss",&rssfontfx,0);
	textrot("rss",thistext,&rssfontfx,0);
	rssfontfx.draw(0.0,0.0, thistext,thcfg->sub("objects")->sub("rss")->getFloat("depth"));
}

void updatefont()
{
	rtcfg->set("ttffontfile",rtcfg->sub("scanned")->getString("fontpath")+"/"+ thcfg->sub("files")->getString("fontfile"));
	FILE *fontcheck = fopen(rtcfg->getString("ttffontfile").c_str(),"r");

	if(fontcheck)	fclose (fontcheck);

	else
	{
		std::string pkgdatadir=PKGDATADIR;
		rtcfg->set("ttffontfile",pkgdatadir+"/generic/fonts/Tuffy-Bold.ttf");
#ifdef DEBUG
		printf("WARNING - font not found - using defaults!\n");
#endif //DEBUG
		FILE *fontcheck = fopen(rtcfg->getString("ttffontfile").c_str(),"r");
		if(fontcheck)
		{
			fclose (fontcheck);
		}
		else
		{
			printf("ERROR   - configured font %s not found!\n",rtcfg->getString("ttffontfile").c_str());
			printf("	  exiting now - define both a valid fontpath and font\n");
			printf("	  in the configfile and try again\n");
			exit(1);
		}
	}
 	fontmanager.init();

	resizefonts();
	setdropshadow();
	setdropshadowstrength(thcfg->sub("floats")->getFloat("dropshadowstrength"));
}

void setdropshadowstrength(float ds)
{
	font.shadowStrength(ds);
	menuselfontfx.shadowStrength(ds);
	musicdurationfontfx.shadowStrength(ds);
	titlefontfx.shadowStrength(ds);
	clockfontfx.shadowStrength(ds);
	datefontfx.shadowStrength(ds);
	sysinfofontfx.shadowStrength(ds);
	notifyfontfx.shadowStrength(ds);
	rssfontfx.shadowStrength(ds);
	counterfontfx.shadowStrength(ds);
	settingsfontfx.shadowStrength(ds);
}

void setdropshadow()
{
	float dx= 1.0, dy= 1.0;
	switch (thcfg->sub("ints")->getInt("dropshadow"))
	{
		case 1:  dx= 1.0;dy= 1.0;break; // top right
		case 2:  dx=-1.0;dy= 1.0;break;	// top left
		case 3:  dx= 1.0;dy=-1.0;break;	// bottom right
		case 4:  dx=-1.0;dy=-1.0;break;	// bottom left
		default: break;
		break;
	}

	font.shadowOffset(dx,dy);
	menuselfontfx.shadowOffset(dx,dy);
	musicdurationfontfx.shadowOffset(dx,dy);
	titlefontfx.shadowOffset(dx,dy);
	clockfontfx.shadowOffset(dx,dy);
	datefontfx.shadowOffset(dx,dy);
	sysinfofontfx.shadowOffset(dx,dy);
	notifyfontfx.shadowOffset(dx,dy);
	rssfontfx.shadowOffset(dx,dy);
	counterfontfx.shadowOffset(dx,dy);
	settingsfontfx.shadowOffset(dx,dy);
}

void changemaxtextlength(int by, std::string thislength)
{
	((TConfig<int>*) thcfg->sub("ints")->get(thislength))->operator+(by);
	if(rtcfg->getBool("initdone"))	shownote(lacfg->getString("changed")+thislength+lacfg->getString("to")+itos(thcfg->sub("ints")->getInt(thislength)));
}

void resizemusfont()
{
	musicdurationfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("sound")->getInt("size")));
}

void resizetitfont()
{
	titlefontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("title")->getInt("size")));
}

void resizeclofont()
{
	clockfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("clock")->getInt("size")));
}
void resizedatfont()
{
	datefontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("date")->getInt("size")));
}

void resizesysfont()
{
	sysinfofontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("cpu")->getInt("size")));
}

void resizenotfont()
{
	notifyfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("notifier")->getInt("size")));
}

void resizerssfont()
{
	rssfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("rss")->getInt("size")));
}

void resizeromfont()
{
	counterfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("entrycount")->getInt("size")));
}

void resizerominfofont()
{
	rominfofontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("platform")->getInt("size")));
}

void resizesetfont()
{
	settingsfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("setbox")->getInt("size")));
}

void resizemenfont()
{
	font = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("menu")->getInt("size")));
	std::string selwidth;
	for ( int i = 0; i < thcfg->sub("ints")->getInt("menumaxtextlength"); i++) 	{selwidth+="W";}
	rtcfg->set("maxmenuselwidth",(int)font.getWidth(selwidth));
	if(rtcfg->getBool("initdone"))
	{
		if(rtcfg->getInt("maxmenuselwidth")>rtcfg->sub("objects")->sub("menu")->getInt("maxwidth"))
		{changemaxtextlength(-1,"menumaxtextlength");}
		if((int)font.getWidth(selwidth+"W")<rtcfg->sub("objects")->sub("menu")->getInt("maxwidth"))
		{changemaxtextlength(+1,"menumaxtextlength");}
	}
}

void resizeselfont()
{	
	menuselfontfx = fontmanager.grab(rtcfg->getString("ttffontfile"), drawfontsize(thcfg->sub("objects")->sub("menu")->getInt("menuselfontsize")));

	std::string selwidth;
	for ( int i = 0; i < thcfg->sub("ints")->getInt("menumaxtextsellength"); i++) 	{selwidth+="W";}
	rtcfg->set("maxmenuselwidth",(int)menuselfontfx.getWidth(selwidth));
	if(rtcfg->getInt("maxmenuselwidth")>rtcfg->sub("objects")->sub("menu")->getInt("maxwidth"))
	{changemaxtextlength(-1,"menumaxtextsellength");}
	if((int)menuselfontfx.getWidth(selwidth+"W")<rtcfg->sub("objects")->sub("menu")->getInt("maxwidth"))
	{changemaxtextlength(+1,"menumaxtextsellength");}
}

void resizefonts()
{
	resizemenfont();
	resizemusfont();
	resizetitfont();
	resizedatfont();
	resizeclofont();
	resizesysfont();
	resizenotfont();
	resizerssfont();
	resizeromfont();
	resizerominfofont();
	resizesetfont();
	resizeselfont();
}

