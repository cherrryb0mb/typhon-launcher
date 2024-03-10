#include <SFML/Graphics.hpp>
#include "picturehandling.h"
#include "glmenu.h"
#include "videoplayerhandling.h"
#include "values.h"
#include "p3thandling.h"
#include "glhandling.h"
#include "windowhandling.h"
#include "inputhandling.h"
#include "editor.h"
#include "timerfunc.h"
#include "filehandling.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "networking.h"

#ifdef WITHSCROT
#include <X11/Xutil.h>
#include <jpeglib.h>
#endif // WITHSCROT

extern std::string switchmediasuffix(std::string thismedia,TyphonConfig *suflist); // from filehandling.cpp

using namespace std;

sf::Texture* getTexture(int s)
{return (sf::Texture*)rtcfg->sub("textureids")->sub(rtcfg->sub("textureids")->sub(s)->id)->getInt("tid");}

sf::Texture* getIconTexture(int s)
{return (sf::Texture*)rtcfg->sub("textureids")->sub(rtcfg->sub("textureids")->sub(thcfg->sub("textures")->get("menuicon0")->id+s)->id)->getInt("tid");}

sf::Texture* getWallTexture(int s)
{return (sf::Texture*)rtcfg->sub("textureids")->sub(rtcfg->sub("textureids")->sub(thcfg->sub("textures")->get("wallpaper0")->id+s)->id)->getInt("tid");}

sf::Texture* getTextureS(std::string s)
{return (sf::Texture*)rtcfg->sub("textureids")->sub(rtcfg->sub("textureids")->sub(s)->id)->getInt("tid");}

Config* getTextureConfig()
{
	sf::Texture *dummytexture ;
	dummytexture = new sf::Texture();
	Config* testTexture = new TConfig<sf::Texture*>(dummytexture);
	testTexture->typ="int";
	return testTexture;
}
/*
void createtextures()
{
	for(int s = 0; s <  rtcfg->sub("textures")->size(); s++)
	{
		Config* tempcfg= getTextureConfig();
		rtcfg->sub("textures")->subC(shcfg->sub("textures")->sub(s)->getString("name"))->set("textureid",tempcfg);
	}
}
  */
    
void texcol(std::string thistex)
{
	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get(thistex)->id && pick()!=3 && pick()!=4)
	{
		if(rtcfg->sub("block")->getBool(thcfg->sub("objects")->get(thistex)->id))
		{glColor4ub(1,1,255,255);}
		else
		{glColor4ub(255,1,1,255);}
	}	
	else
	{
		if (pick()==4 && !thcfg->sub("objects")->sub(thistex)->getBool("enabled"))
		{glColor4ub(127,127,127,127);}
		else
		{
			if(thcfg->sub("switches")->getBool("usemonthcolors"))
			{
				TyphonConfig* col=rtcfg->sub("monthcolors")->sub(rtcfg->getInt("monthcolor"));

				glColor4f(
				col->getFloat("R"),
				col->getFloat("G"),
				col->getFloat("B"),
				norm255(thcfg->sub("objects")->sub(thistex)->getInt("A"))
				);	
			}
			else
			{	
				TyphonConfig* col=thcfg->sub("objects")->sub(thistex);
				glColor4ub(
						col->getInt("R"),
						col->getInt("G"),
						col->getInt("B"),			
						col->getInt("A")			
				);
			}
		}
	}	
}

int updatetexture(std::string thisimgpath,sf::Texture *thisimgID)
{
	int texstat=false;
	
	sf::Image Image;
	if (Image.loadFromFile(thisimgpath))
	{
		Image.flipVertically();

		texstat=true;
		thisimgID->loadFromImage(Image, sf::IntRect ());
		thisimgID->setSmooth(1);
	}
#ifdef WITHIM
	else
	{
		FILE *fcheck = fopen(thisimgpath.c_str(),"r+");
		if(fcheck)
		{	
			fclose(fcheck);
			systemstart("mogrify", thisimgpath,1);
			sf::Image Image;
			if (Image.loadFromFile(thisimgpath))
			updatetexture(thisimgpath,thisimgID);
			else
			{rtcfg->set("currentprev",fallbackmedia(rtcfg->getString("currentprev"),2));}
		}
	}
#endif
	return texstat;
}

void listpics()
{
	listpictures("wallpapers",rtcfg->sub("scanned")->getString("wallpaperpath"));
	listpictures("stencil",rtcfg->sub("scanned")->getString("stencilpicturepath"));
	listpictures("custom",rtcfg->sub("scanned")->getString("customimagepath"));
	listpictures("icons",rtcfg->sub("scanned")->getString("menuiconpath"));
}

void loadmenuwallpapers(int thiswp)
{
	if(!rtcfg->getBool("nop3tdata"))
	{
		int thiswpid=thcfg->sub("textures")->get("wallpaper0")->id+thiswp;
		std::string wpcat=thcfg->sub("textures")->getKey(thiswpid);
		updatewallpaper(thcfg->sub("textures")->getStringP(wpcat),getTextureS(wpcat));
	}
}

void initpictureids()
{
	for(int t = 0; t <  thcfg->sub("textures")->size(); t++)
	{
		Config* tempcfg= getTextureConfig();
		rtcfg->sub("textureids")->subC(thcfg->sub("textures")->getKey(t))->set("tid",tempcfg);
	}
}

void updateimage(std::string path,std::string img,std::string imgback)
{
	if (thcfg->sub("objects")->sub(img)->getBool("enabled"))
	{
		updateimage(rtcfg->sub("scanned")->getString(path), thcfg->sub("textures")->getString(img),getTextureS(img));
		if (thcfg->sub("objects")->sub(img)->getBool("back"))
		updateimage(rtcfg->sub("scanned")->getString(path), thcfg->sub("textures")->getString(imgback), getTextureS(imgback));
	}
}

void updateimages()
{
	listpics();

	if(!thcfg->sub("switches")->getBool("usemenuwallpaper"))		updatewallpaper(thcfg->sub("textures")->getStringP("wallpaper"),	getTextureS("wallpaper"));
	updateimage(rtcfg->sub("scanned")->getString("settingspicturepath"), thcfg->sub("textures")->getString("settings"), getTextureS("settings"));
	updatepreview();
	if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled"))			{loadmenuicons();}
	if (thcfg->sub("switches")->getBool("usemenuwallpaper"))	{for(int i = 0; i < 9; i++)	{loadmenuwallpapers(i);}}

	if (thcfg->sub("objects")->sub("wallpaper")->getBool("enabled") && thcfg->sub("objects")->sub("wallpaper")->getBool("back"))
	updateimage(rtcfg->sub("scanned")->getString("wallpaperpath"), thcfg->sub("textures")->getString("wallpaperback"), getTextureS("wallpaperback"));

	if (thcfg->sub("objects")->sub("icon")->getBool("enabled"))				{updateicon();}

	updateimage("stencilpicturepath","stencil","stencilback");
	updateimage("customimagepath","custom1","custom1back");
	updateimage("customimagepath","custom2","custom2back");
	updateimage("customimagepath","custom3","custom3back");
	
}

// thanks a lot to ehamberg (http://hamberg.no/erlend) for lending me his screenshot code from https://gist.github.com/767824
#ifdef WITHSCROT
int write_jpeg(XImage *img, const char* filename)
{
    FILE* outfile;
    unsigned long pixel;
    int x, y;
    char* buffer;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    JSAMPROW row_pointer;

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Failed to open output file %s\n",filename);
        return 1;
    }

    /* collect separate RGB values to a buffer */
    buffer = (char*)malloc(sizeof(char)*3*img->width*img->height);
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            pixel = XGetPixel(img,x,y);
            buffer[y*img->width*3+x*3+0] = (char)(pixel>>16);
            buffer[y*img->width*3+x*3+1] = (char)((pixel&0x00ff00)>>8);
            buffer[y*img->width*3+x*3+2] = (char)(pixel&0x0000ff);
        }
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline*(img->depth>>3)*img->width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    free(buffer);
    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    return 0;
}

void takescreenshot()
{
	Display* display = NULL;
	Screen* screen = NULL;
	XImage* xImageSample = NULL;

	display = XOpenDisplay(NULL);
	screen = DefaultScreenOfDisplay(display);
	xImageSample = XGetImage(display, DefaultRootWindow(display), 0, 0, screen->width, screen->height, AllPlanes, ZPixmap);

	if (xImageSample != NULL && display != NULL && screen != NULL) 
	{
		std::string  outfile = rtcfg->getString("scrotpath");
#ifdef DEBUG
		printf("taking scrot from %s\n",outfile.c_str());
#endif //DEBUG
		write_jpeg(xImageSample, outfile.c_str());
		XCloseDisplay(display);
		updatepreview();
	}
}
#else //WITHSCROT
void takescreenshot(){}
#endif // WITHSCROT

void startscrotting(std::string programfile,std::string thisscrotpath)
{
	thisscrotpath=setEnvhome(thisscrotpath);
	rtcfg->set("scrotpath",thisscrotpath+"/"+programfile+".jpg");
	if(tycfg->sub("globalswitches")->getBool("screenshotoverwrite"))
	{rtcfg->setBool("launchscrot",1);}
	else
	{
		FILE *fcheck = fopen(rtcfg->getString("scrotpath").c_str(),"r");
		if(fcheck)
		{
			fclose(fcheck);
#ifdef DEBUG
			printf("scrot already exists - doing nothing\n");
#endif //DEBUG
			rtcfg->setBool("launchscrot",0);
		}
		else
		{rtcfg->setBool("launchscrot",1);}
	}
}

void updateimage(std::string thisscannedpath,std::string thisimage, sf::Texture *thisimageID)
{
	std::string thisimagepath=thisscannedpath+"/"+thisimage;
	FILE *thisimagecheck = fopen(thisimagepath.c_str(),"r");
	if(!thisimagecheck)
	{
		printf("WARNING - picture %s not found!\n",thisimagepath.c_str());
		printf("	  disabling picture...\n");
	}

	updatetexture(thisimagepath,thisimageID);
	if(thisimagecheck){fclose(thisimagecheck);};
}

void updatewallpaper(std::string *thiswallpaper,sf::Texture *thisbackID)
{
	if (thcfg->sub("switches")->getBool("randomwallpaper"))
	{
		rtcfg->set("currentwp",rand() %rtcfg->sub("wallpapers")->size());
		sTex("wallpaper",rtcfg->sub("wallpapers")->sub(rtcfg->getInt("currentwp"))->getString("file"));
		*thiswallpaper=gTex("wallpaper");
	}
	
	rtcfg->set("currentback",rtcfg->sub("scanned")->getString("wallpaperpath")+"/"+*thiswallpaper);

	FILE *backcheck = fopen(rtcfg->getString("currentback").c_str(),"r");
	if(backcheck)
	{
		fclose(backcheck);
		rtcfg->setBool("emptyp3t",1);
	}
	else
	{
		if (isp3t() && !rtcfg->getBool("emptyp3t"))
		{rtcfg->setBool("nop3tdata",1);thcfg->sub("shader")->set("usebackshader",shcfg->sub("shaders")->sub("xmb")->id);}
		else
		{rtcfg->set("currentback",rtcfg->sub("scanned")->getString("wallpaperpath")+"/"+gTex("preview"));}
	}

	if(!rtcfg->getBool("nop3tdata"))
	{
		backcheck = fopen(rtcfg->getString("currentback").c_str(),"r");
		if(backcheck)	{fclose(backcheck);}
		else			{rtcfg->set("currentback",rtcfg->getString("datadir")+"/"+rtcfg->getString("previewdefault"));}
		updatetexture(rtcfg->getString("currentback"),thisbackID);
	}
}

void togglemenuwallpaper()
{
	int thiswpid=thcfg->sub("textures")->get("wallpaper0")->id+Ishowmenu();
	std::string wpcat=thcfg->sub("textures")->getKey(thiswpid);
	thcfg->sub("switches")->setBool("usemenuwallpaper",!thcfg->sub("switches")->getBool("usemenuwallpaper"));
	if(thcfg->sub("switches")->getBool("usemenuwallpaper")){updatewallpaper(thcfg->sub("textures")->getStringP(wpcat),getTextureS(wpcat));}
	else 																	{updatewallpaper(thcfg->sub("textures")->getStringP("wallpaper"),getTextureS("wallpaper"));}
}

void updateicon()
{
	if (thcfg->sub("objects")->sub("icon")->getBool("enabled"))
	{
		if(Iem()==0 || Ishowmenu()==0)
		{rtcfg->set("currenticon",mecfg->sub(Ishowmenu()-1)->sub("entry")->sub(Ics())->getString("icon"));}
		
		if (Ishowmenu()>=1 && Iem() !=0)
		{rtcfg->set("currenticon",emcfg->sub(IeID()-1)->getString("rompath")+"/"+getPrefix(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getString("file"))+".png");}
		
		FILE *iconcheck = fopen(rtcfg->getString("currenticon").c_str(),"r");
		if(iconcheck)	{fclose(iconcheck);}
		else	 		{rtcfg->set("currenticon",setEnvhome(rtcfg->getString("currenticon")));}

		rtcfg->set("currenticon",switchmediasuffix(rtcfg->getString("currenticon"),rtcfg->sub("suffix")->sub("pics")));

		iconcheck = fopen(rtcfg->getString("currenticon").c_str(),"r");
		if(iconcheck)	{fclose(iconcheck);}
		else			{rtcfg->set("currenticon",rtcfg->sub("scanned")->getString("menuiconpath")+"/"+gTex("icon"));}
		updatetexture(rtcfg->getString("currenticon"),getTextureS("icon"));
	}
}

void loadmenuicon(std::string iconmenu,sf::Texture *thisID)
{
	rtcfg->set("currentmenuicon",rtcfg->sub("scanned")->getString("menuiconpath")+"/"+iconmenu);
	FILE *micheck = fopen(rtcfg->getString("currentmenuicon").c_str(),"r");
	if(micheck)	{fclose(micheck);}
	else 		{rtcfg->set("currentmenuicon",fallbackmedia(rtcfg->getString("currentmenuicon"),1));}
	updatetexture(rtcfg->getString("currentmenuicon"),thisID);
}


void loaddefmenuicons(int thisicon)
{
	int thismiconid=thcfg->sub("textures")->get("menuicon0")->id+thisicon;
	std::string iconcat=thcfg->sub("textures")->getKey(thismiconid);
	loadmenuicon(thcfg->sub("textures")->getString(iconcat),getTextureS(iconcat));
}

void loademumenuicon(std::string thisemu,int thisemuicon,sf::Texture *thisID)
{
	rtcfg->set("currentmenuicon",rtcfg->sub("scanned")->getString("previewpath")+"/"+thisemu+".png");

	FILE *micheck = fopen(rtcfg->getString("currentmenuicon").c_str(),"r");
	if(micheck)	{fclose(micheck);}
	else
	{
		rtcfg->set("currentmenuicon",switchmediasuffix(rtcfg->getString("currentmenuicon"),rtcfg->sub("suffix")->sub("pics")));
		FILE *micheck = fopen(rtcfg->getString("currentmenuicon").c_str(),"r");
		if(micheck)	{fclose(micheck);}
		else		{loaddefmenuicons(thisemuicon);}
	}
	updatetexture(rtcfg->getString("currentmenuicon"),thisID);
}

void loadmenuicons()
{
	loadmenuicon(thcfg->sub("textures")->getString("menuicon0"),getTextureS("menuicon0"));

	for(int m = 1; m <  mecfg->size()+1; m++)
	{
		int thismiconid=thcfg->sub("textures")->get("menuicon0")->id+m;
		std::string iconcat=thcfg->sub("textures")->getKey(thismiconid);
		if(mecfg->sub(m-1)->getInt("emumenu")==0)	{loadmenuicon(thcfg->sub("textures")->getString(iconcat),getTextureS(iconcat));}
		if(mecfg->sub(m-1)->getInt("emumenu")==1)	{loademumenuicon(emulator(m),m,getTextureS(iconcat));}
		if(mecfg->sub(m-1)->getInt("emumenu")==2)	{loademumenuicon(emulator(m+mecfg->size()),m,getTextureS(iconcat));}
		if(mecfg->sub(m-1)->getInt("emumenu")==3)	{loademumenuicon(emulator(m+(mecfg->size()*2)),m,getTextureS(iconcat));}
	}
	loadmenuicon(thcfg->sub("textures")->getString("menuicon8"),getTextureS("menuicon8"));
}

void preppreview(std::string preppreviewpath)
{
	if (Bvidmenu())
	{
		rtcfg->set("currententry",getPrefix(vicfg->sub("videos")->sub(Ics())->getString("file").c_str()));
		rtcfg->set("currentprevpath",setEnvhome(thcfg->sub("paths")->getString("videopreviewpath"))+"/");
		rtcfg->set("currentprev",rtcfg->getString("currentprevpath")+rtcfg->getString("currententry")+".png");
	}

	else
	{
		if (Iem()==0 || Ishowmenu()==0)
		{
			togentryname(1);
			rtcfg->set("currententry",mentry(Ishowmenu()-1,Ics()));			
			togentryname(0);
			rtcfg->set("currentprevpath",preppreviewpath+"/");
			rtcfg->set("currentprev",rtcfg->getString("currentprevpath")+rtcfg->getString("currententry")+".png");
		}

		if (Iem()!=0)
		{
			rtcfg->set("currententry",getPrefix(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getString("file")));
				rtcfg->set("currentprevpath",emcfg->sub(IeID()-1)->getString("rompicturespath")+"/");

			rtcfg->set("currentprev",rtcfg->getString("currentprevpath")+rtcfg->getString("currententry")+".png");
		}
	}
}

void updatepreview()
{
	if ((thcfg->sub("objects")->sub("preview")->getBool("enabled")) || (thcfg->sub("ints")->getInt("shadertexture")==1 && useBackShader()!=0 && useShader()) || rtcfg->getBool("videofailed"))
	{
	//	if ((Ishowmenu()>=1 && !Bvidmenu()) || Ishowmenu()==0)
		preppreview(rtcfg->sub("scanned")->getString("previewpath"));

		FILE *prevcheck = fopen(rtcfg->getString("currentprev").c_str(),"r");
		if(prevcheck)	{rtcfg->setBool("foundprev",1);fclose(prevcheck);}
		else			{rtcfg->set("currentprev",setEnvhome(rtcfg->getString("currentprev")));}
		rtcfg->set("currentprev",switchmediasuffix(rtcfg->getString("currentprev"),rtcfg->sub("suffix")->sub("pics")));
		
		if (Bp3tmenu())
		{
			extractp3tpreview();
			rtcfg->set("currentprev",tycfg->sub("tempdirs")->getString("p3text")+"/info_preview.png");
			loadshadertex();
		}
		if (Bthememenu())
		{
			if (strcmp(getPrefix(rtcfg->sub("themes")->getString(Ics())).c_str(), "typhon") == 0)
			rtcfg->set("currentprev",rtcfg->getString("datadir")+"/themepreview.jpg");
			else if (strcmp(getPrefix(rtcfg->sub("themes")->getString(Ics())).c_str(), "p3t") == 0)
			rtcfg->set("currentprev",rtcfg->getString("datadir")+"/p3tpreview.jpg");
			else
			rtcfg->set("currentprev",rtcfg->sub("scanned")->getString("themedata")+"/"+getPrefix(rtcfg->sub("themes")->getString(Ics()))+"/themepreview.jpg");

			loadshadertex();
		}	
		prevcheck = fopen(rtcfg->getString("currentprev").c_str(),"r");
		if(prevcheck){fclose (prevcheck);}
		else
		{
			if(rtcfg->getBool("nop3tdata")){rtcfg->set("currentprev",rtcfg->getString("currentmenuicon"));}
			else
			{
					if (Iem()!=0)	downloadrompreview(IeID()-1,Ics());
					else
					rtcfg->set("currentprev",fallbackmedia(rtcfg->getString("currentprev"),2));
			}
		}
		rtcfg->setBool("foundprev",0);

		prevcheck = fopen(rtcfg->getString("currentprev").c_str(),"r");
		if(prevcheck){fclose (prevcheck);}
		else
		{rtcfg->set("currentprev",fallbackmedia(rtcfg->getString("currentprev"),2));}

		updatetexture(rtcfg->getString("currentprev"),getTextureS("preview"));

		if(thcfg->sub("objects")->sub("preview")->getBool("back"))
		{printf("upbacktex %s\n",rtcfg->getString("currentprevback").c_str());
			updatetexture(rtcfg->getString("currentprevback"),getTextureS("previewback"));
		}
	}
}
void forcepreviewupdate()
{
	if (Iem()==0)	{getproginfo();updatepreview();}
	else			{getrominfo(IeID()-1,Ics(),1);}
	updatetexture(rtcfg->getString("currentprev"),getTextureS("preview"));
}

float rfloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

void switchpictureeffects(int thiseffect,float thisblend,sf::Texture *thispicID,float thiswidth,float thisheight)
{
	switch (thiseffect)
	{
		case 1:		//blend and zoom
			glColor4f (1,1,1,thisblend);
			glTranslatef(0,0, 100*(1-thisblend));
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		case 2:		//just blend
			glColor4f (1,1,1,thisblend);
			glTranslatef(0,0, 0.1f);
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		case 3:		//fly to the right and blend
			glColor4f (1,1,1,thisblend);
			glTranslatef(thiswidth/2*(1-thisblend),0, 50.1f);
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		case 4:		//fly to the left and blend
			glColor4f (1,1,1,thisblend);
			glTranslatef(-rtcfg->sub("objects")->sub("slide2")->getFloat("height")/2*(1-thisblend),0, 50.1f);
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		case 5:		//fly up and blend
			glColor4f (1,1,1,thisblend);
			glTranslatef(0,thisheight/2*(1-thisblend), 50.1f);
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		case 6:		//fly down and blend
			glColor4f (1,1,1,thisblend);
			glTranslatef
			(
				0,
				-rtcfg->sub("objects")->sub("slide2")->getFloat("height")/2*(1-thisblend),
				50.1f
			);
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
		default:
			sf::Texture::bind(thispicID); 
			drawquad(thiswidth,thisheight );
		break;
	}
}

// draw the background
void drawback(sf::Texture *oldID,sf::Texture *newID)
{
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);

	if(rtcfg->getInt("picked")==thcfg->sub("objects")->get("wallpaper")->id && pick()!=3 && rtcfg->sub("block")->getBool(thcfg->sub("objects")->get("wallpaper")->id))
	{glColor4ub(1,1,255,255);}
	else
	{
		TyphonConfig* col=thcfg->sub("objects")->sub("wallpaper");
		glColor4ub
		(
			col->getInt("R"),
			col->getInt("G"),
			col->getInt("B"),			
			col->getInt("A")			
		);
	}
	
	setpos("wallpaper");
	sf::Texture::bind(newID); 
	cuboquad("wallpaper","wallpaperback");
	if(!rtcfg->getBool("scrollisdone") && !rtcfg->getBool("changemenu"))
	{
		((TConfig<float>*) rtcfg->get("blendwall"))->operator-(rtcfg->getFloat("elapsed"));
		switchpictureeffects(thcfg->sub("ints")->getInt("wallpaperblendeffect"),rtcfg->getFloat("blendwall"),oldID,rtcfg->sub("objects")->sub("wallpaper")->getFloat("width"),rtcfg->sub("objects")->sub("wallpaper")->getFloat("height"));
	}
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}

void scaleicons()
{
	if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled") && thcfg->sub("ints")->getInt("menuicondockstyle")!=0)
	{
		((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon0"))->operator+(0.3);
		if(rtcfg->sub("scaleicon")->getFloat("icon0")>=2.1)	rtcfg->sub("scaleicon")->setC("icon0",(float)2.1);
		((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon1"))->operator+(0.2);
		if(rtcfg->sub("scaleicon")->getFloat("icon1")>=1.80)	rtcfg->sub("scaleicon")->setC("icon1",(float)1.80);
		((TConfig<float>*) rtcfg->sub("scaleicon")->get("icon2"))->operator+(0.1);
		if(rtcfg->sub("scaleicon")->getFloat("icon2")>=1.5)	rtcfg->sub("scaleicon")->setC("icon2",(float)1.5);
	}
}

void iconcube(float fmicon,int thisicon)
{
	drawcube
	(
		rtcfg->sub("objects")->sub("menuicon")->getFloat("width")*fmicon,
		rtcfg->sub("objects")->sub("menuicon")->getFloat("height")*fmicon,
		thcfg->sub("objects")->sub("menuicon")->getFloat("depth")*fmicon,
		"menuicon"+itos(thisicon-1),"menuicon"+itos(thisicon-1)
	);	
}

// draw one menuicon
void drawsinglemenuicon(int thisicon)
{
	glDisable( GL_DEPTH_TEST );
	float fmicon=1.0;

	if (Ishowmenu()==thisicon)
	{
		fmicon=thcfg->sub("floats")->getFloat("menuiconscale");
		glTranslatef(-((fmicon*58.0-48.0)*icwidth), ((fmicon*58.0-48.0)*icheight),0.0);
	};
	if (Ishowmenu()>thisicon)
	{
		glTranslatef(-20*icwidth, 0, 0);
	}

	texcol("menuicon");

	if (thcfg->sub("ints")->getInt("menuiconxmbmode")==0)
	{glTranslatef(0, 0, thcfg->sub("objects")->sub("menuicon")->getInt("posz"));}
	if(thcfg->sub("ints")->getInt("menuicondockstyle")!=0)
	{
		if(thisicon==Ishowmenu())
		{glScalef(rtcfg->sub("scaleicon")->getFloat("icon0"), rtcfg->sub("scaleicon")->getFloat("icon0"), rtcfg->sub("scaleicon")->getFloat("icon0"));}
		if(thisicon==Ishowmenu()-1 || thisicon==Ishowmenu()+1)
		{glScalef(rtcfg->sub("scaleicon")->getFloat("icon1"),rtcfg->sub("scaleicon")->getFloat("icon1"),rtcfg->sub("scaleicon")->getFloat("icon1"));}	
		if(thisicon==Ishowmenu()-2 || thisicon==Ishowmenu()+2)
		{glScalef(rtcfg->sub("scaleicon")->getFloat("icon2"),rtcfg->sub("scaleicon")->getFloat("icon2"),rtcfg->sub("scaleicon")->getFloat("icon2"));}				
	}
	rotateme("menuicon");
	
	if (thcfg->sub("ints")->getInt("menuicontype")==2)	{iconcube(fmicon,thisicon);}
	
	else if (thcfg->sub("ints")->getInt("menuicontype")==3)
	{
		glRotatef(rfloat(0,2), 1.0, 0.0, 0.0);
		glRotatef(rfloat(0,2), 0.0, 1.0, 0.0);
		glRotatef(rfloat(0,2), 0.0, 0.0, 1.0);
		iconcube(fmicon,thisicon);
	}
	
	else if (thcfg->sub("ints")->getInt("menuicontype")==4)
	{
		glTranslatef
		(
		rtcfg->sub("objects")->sub("menuicon")->getFloat("width")/2,
		rtcfg->sub("objects")->sub("menuicon")->getFloat("height")/2,
		-thcfg->sub("objects")->sub("menuicon")->getFloat("depth")/2
		);
		((TConfig<float>*) rtcfg->get("rotmenucube"))->operator+(rfloat(0,0.5));
		glRotatef(rtcfg->getFloat("rotmenucube"), 1.0, 0.0, 0.0);
		glRotatef(rtcfg->getFloat("rotmenucube"), 0.0, 1.0, 0.0);
		glRotatef(rtcfg->getFloat("rotmenucube"), 0.0, 0.0, 1.0);
		glTranslatef
		(
			-rtcfg->sub("objects")->sub("menuicon")->getFloat("width")/2,
			-rtcfg->sub("objects")->sub("menuicon")->getFloat("height")/2,
			thcfg->sub("objects")->sub("menuicon")->getFloat("depth")/2
		);
		iconcube(fmicon,thisicon);
		if(rtcfg->getFloat("rotmenucube")>=360)rtcfg->set("rotmenucube",(float)0);
	}
	
	else
	{
		drawquad
		(
			rtcfg->sub("objects")->sub("menuicon")->getFloat("width")*fmicon,
			rtcfg->sub("objects")->sub("menuicon")->getFloat("height")*fmicon
		);
	}
	glEnable(GL_DEPTH_TEST);
}

void tBind(int b)
{
	int thismiconid=thcfg->sub("textures")->get("menuicon0")->id+b;
	sf::Texture::bind(getTexture(rtcfg->sub("textureids")->sub(thismiconid)->id));
}

// draw all menuicons
void drawmenuicons()
{
	if(rtcfg->getBool("changemenu") && rtcfg->getInt("scrollmenu")==1)	
	{((TConfig<float>*) rtcfg->get("scrollicon"))->operator+(thcfg->sub("floats")->getFloat("menuiconscrollspeed"));}
	if(rtcfg->getBool("changemenu") && rtcfg->getInt("scrollmenu")==2)
	{((TConfig<float>*) rtcfg->get("scrollicon"))->operator-(thcfg->sub("floats")->getFloat("menuiconscrollspeed"));}

	if (thcfg->sub("ints")->getInt("menuiconxmbmode")==0)
	{
		glPushMatrix();
		setpos("menuicon");
		tBind(Ishowmenu());
		drawsinglemenuicon(-1);
		glPopMatrix();
	}

	else
	{
		for(int m = 0; m <  mecfg->size()+2; m++)
		{
			rtcfg->sub("ipos")->set(m,(float)(rtcfg->getFloat("showmiconspace")*(m-Ishowmenu()+1)+rtcfg->getFloat("scrollicon")));
			
			if(m>0 || (m==0 && tycfg->getBool("showemulist")))
			{	
			glPushMatrix();


			if (thcfg->sub("ints")->getInt("menuiconxmbmode")==1)
			glTranslatef
			(
				rtcfg->sub("ipos")->getFloat(m)+rtcfg->sub("objects")->sub("menuicon")->getFloat("posx")-rtcfg->getFloat("showmiconspace"),
				rtcfg->sub("objects")->sub("menuicon")->getFloat("posy"),
				thcfg->sub("objects")->sub("menuicon")->getInt("posz")
			);
			else
			glTranslatef
			(
				rtcfg->sub("objects")->sub("menuicon")->getFloat("posx"),
				-rtcfg->getFloat("showmiconspace")+rtcfg->sub("ipos")->getFloat(m)+rtcfg->sub("objects")->sub("menuicon")->getFloat("posy"),
				thcfg->sub("objects")->sub("menuicon")->getInt("posz")
			);
			
			tBind(m);
			drawsinglemenuicon(m);
			glPopMatrix();
			}
		}
	}
}

void drawpic(std::string pic,std::string bpic)
{
	glPushMatrix();
	texcol(pic);
	setpos(pic);
	rotateme(pic);
	sf::Texture::bind(getTextureS(pic));
	cuboquad(pic,bpic);
	glPopMatrix();
}

void drawref(std::string pic)
{
	glPushMatrix();
	texcol(pic);
	setpos(pic);
	rotateme(pic);
	sf::Texture::bind(getTextureS("preview"));
	cuboquad(pic,"previewback");
	glPopMatrix();
}

/*
void drawpreveffect(std::string pic)
{
	glPushMatrix();
	texcol(pic);
	setpos(pic);
	rotateme(pic);
	updateshaders();
	getShader(useBackShader())->bind();
	shadtexture.bind();
	cuboquad(pic);
	getShader(useBackShader())->unbind();

	glPopMatrix();
}
*/

void drawwallpapers()
{
	if (thcfg->sub("objects")->sub("wallpaper")->getBool("enabled") && !thcfg->sub("objects")->sub("slide1")->getBool("enabled") && !rtcfg->getBool("nop3tdata"))
	{
		if (thcfg->sub("switches")->getBool("usemenuwallpaper"))
		{
			int thiswpid=thcfg->sub("textures")->get("wallpaper0")->id+Ishowmenu();
			std::string wpcat=thcfg->sub("textures")->getKey(thiswpid);
			std::string prevwp;
			std::string nextwp;

			if (Ishowmenu()==0)
			{
				prevwp=thcfg->sub("textures")->getKey(thcfg->sub("textures")->get("wallpaper0")->id+mecfg->size()+1);		
				nextwp=thcfg->sub("textures")->getKey(thiswpid+1);
			}
			else if (Ishowmenu()==mecfg->size()+1)	
			{
				prevwp=thcfg->sub("textures")->getKey(thiswpid-1);
				nextwp==thcfg->sub("textures")->getKey(thcfg->sub("textures")->get("wallpaper0")->id);
			}
			else 
			{
				prevwp=thcfg->sub("textures")->getKey(thiswpid-1);
				nextwp=thcfg->sub("textures")->getKey(thiswpid+1);
			}
			/*
				if(rtcfg->getBool("wallscroll"))	{
										if (tycfg->getBool("showemulist"))	{drawback(getTexture(rtcfg->sub("textureids")->sub("wallpaper0")->id),getTexture(rtcfg->sub("textureids")->sub("wallpaper1")->id));}	
										else 								{drawback(getTexture(rtcfg->sub("textureids")->sub("wallpaper8")->id),getTexture(rtcfg->sub("textureids")->sub("wallpaper1")->id));}	
									}
			*/
			if(thcfg->sub("switches")->getBool("fadewallpaper"))
			{
				if(rtcfg->getBool("wallscroll"))	{drawback(getTexture(rtcfg->sub("textureids")->sub(prevwp)->id),getTexture(rtcfg->sub("textureids")->sub(wpcat)->id));}
				else								{drawback(getTexture(rtcfg->sub("textureids")->sub(nextwp)->id),getTexture(rtcfg->sub("textureids")->sub(wpcat)->id));}	
			}
			else drawback(getTexture(rtcfg->sub("textureids")->sub(wpcat)->id),getTexture(rtcfg->sub("textureids")->sub(wpcat)->id));
		}
		else
		{
			drawback(getTexture(rtcfg->sub("textureids")->sub("wallpaper")->id),getTexture(rtcfg->sub("textureids")->sub("wallpaper")->id));
		}
	}
}

void drawpictures()
{
//	if (thcfg->sub("objects")->sub("slide1")->getBool("enabled"))						{showslideshow();}
	if (thcfg->sub("objects")->sub("preview")->getBool("enabled")	|| pick()==4)		{drawpic("preview","previewback");}
	if (thcfg->sub("objects")->sub("previewreflection")->getBool("enabled")|| pick()==4){drawref("previewreflection");}
	if (thcfg->sub("objects")->sub("icon")->getBool("enabled")		|| pick()==4)		{drawpic("icon","icon");}
	if (thcfg->sub("objects")->sub("stencil")->getBool("enabled")	|| pick()==4)		{drawpic("stencil","stencilback");}
	if (thcfg->sub("objects")->sub("menuicon")->getBool("enabled") 	|| pick()==4)		{drawmenuicons();}
	if (thcfg->sub("objects")->sub("custom1")->getBool("enabled")	|| pick()==4)		{drawpic("custom1","custom1back");}
	if (thcfg->sub("objects")->sub("custom2")->getBool("enabled")	|| pick()==4)		{drawpic("custom2","custom2back");}
	if (thcfg->sub("objects")->sub("custom3")->getBool("enabled")	|| pick()==4)		{drawpic("custom3","custom3back");}
}

void changemenuicon(bool up)
{
	if (rtcfg->sub("icons")->size()>0)
	{
		if(up)
		{
			if(rtcfg->getInt("currentmicon")==0)	{rtcfg->set("currentmicon",rtcfg->sub("icons")->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get("currentmicon"))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt("currentmicon")==rtcfg->sub("icons")->size()-1)	{rtcfg->set("currentmicon",0);}
			else 							{((TConfig<int>*) rtcfg->get("currentmicon"))->operator+(1);}
		}

		int thismiconid=thcfg->sub("textures")->get("menuicon0")->id+Ishowmenu();
		std::string iconcat=thcfg->sub("textures")->getKey(thismiconid);
		thcfg->sub("textures")->set(iconcat,rtcfg->sub("icons")->sub(rtcfg->getInt("currentmicon"))->getString("file"),thismiconid);
		loaddefmenuicons(Ishowmenu());
	}
}

void changewallpaper(bool up)
{
	if (rtcfg->sub("wallpapers")->size()>0)
	{
		if(up)
		{
			if(rtcfg->getInt("currentwp")==0)	{rtcfg->set("currentwp",rtcfg->sub("wallpapers")->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get("currentwp"))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt("currentwp")==rtcfg->sub("wallpapers")->size()-1)	{rtcfg->set("currentwp",0);}
			else 							{((TConfig<int>*) rtcfg->get("currentwp"))->operator+(1);}
		}

		if(thcfg->sub("switches")->getBool("usemenuwallpaper"))
		{
			int thiswpid=thcfg->sub("textures")->get("wallpaper0")->id+Ishowmenu();
			std::string wpcat=thcfg->sub("textures")->getKey(thiswpid);	
			thcfg->sub("textures")->set(wpcat,rtcfg->sub("wallpapers")->sub(rtcfg->getInt("currentwp"))->getString("file"),thiswpid);
			loadmenuwallpapers(Ishowmenu());
		}
		else
		{
			sTex("wallpaper",rtcfg->sub("wallpapers")->sub(rtcfg->getInt("currentwp"))->getString("file"));
			updatewallpaper(thcfg->sub("textures")->getStringP("wallpaper"),getTextureS("wallpaper"));
		}
	}
}

void changepicture(std::string listcat,std::string iconcat, std::string counter, bool up)
{
	if (rtcfg->sub(listcat)->size()>0)
	{

		if(up)
		{
			if(rtcfg->getInt(counter)==0)	{rtcfg->set(counter,rtcfg->sub(listcat)->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get(counter))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt(counter)==rtcfg->sub(listcat)->size()-1)	{rtcfg->set(counter,0);}
			else 							{((TConfig<int>*) rtcfg->get(counter))->operator+(1);}
		}

		thcfg->sub("textures")->setC(iconcat,rtcfg->sub(listcat)->sub(rtcfg->getInt(counter))->getString("file"));

		std::string picpath;
		if (strcmp(listcat.c_str(), "stencil") == 0) 	picpath="stencilpicturepath";
		else 											picpath="customimagepath";
		updateimage(rtcfg->sub("scanned")->getString(picpath), thcfg->sub("textures")->getString(iconcat), getTextureS(iconcat));
	}		
}

// search pictures:

void listpictures(std::string type,std::string searchpath)
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir((searchpath).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		int k=0;
		for (int j=0; j<num_entries; j++)
		{
			if
			(
				compare(sdit[j]->d_name, ".png") || 
				compare(sdit[j]->d_name, ".jpg") ||
				compare(sdit[j]->d_name, ".jpeg")||
				compare(sdit[j]->d_name, ".bmp")
			)
			{
			std::stringstream pictry;
			pictry << type << k;
			rtcfg->sub(type)->sub(pictry.str(),k++)->set("file",sdit[j]->d_name);
			}
		}
	}
}

void changemenuiconspace()
{
	((TConfig<float>*) thcfg->sub("floats")->get("menuiconspace"))->operator+(changex);
	rtcfg->set("showmiconspace",thcfg->sub("floats")->getFloat("menuiconspace")*icwidth);
}
