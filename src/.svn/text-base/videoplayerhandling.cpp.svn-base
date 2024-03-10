#ifdef WITHSFM
#include <sfeMovie/Movie.hpp>
#endif // WITHSFM
#include "videoplayerhandling.h"
#include "picturehandling.h"
#include "windowhandling.h"
#include "glhandling.h"
#include "inputhandling.h"
#include "p3thandling.h"
#include "filehandling.h"
#include "xmlconfig.h"
#include "systeminfo.h"

#ifdef WITHSFM
sfe::Movie movie;

void playvideo(){movie.play();}
void pausevideo(){movie.pause();}
void stopvideo(){movie.stop();thcfg->sub("objects")->sub("video")->set("drawhere",0);}
void bindvideo(){sf::Texture::bind(&movie.getCurrentImage());}

void setvideovol(int vol) 
{
	movie.setVolume(vol);
	shownote(lacfg->getString("videovolume")+itos(thcfg->sub("ints")->getInt("videovolume")));
}

void videovol(int change)
{
	((TConfig<int>*) thcfg->sub("ints")->get("videovolume"))->operator+(change);

	if(thcfg->sub("ints")->getInt("videovolume")>rtcfg->sub("thintbarrier")->subC("videovolume")->getInt("max"))
	{thcfg->sub("ints")->setC("videovolume",rtcfg->sub("thintbarrier")->subC("videovolume")->getInt("max"));}

	if(thcfg->sub("ints")->getInt("videovolume")<rtcfg->sub("thintbarrier")->subC("videovolume")->getInt("min"))
	{thcfg->sub("ints")->setC("videovolume",rtcfg->sub("thintbarrier")->subC("videovolume")->getInt("min"));}
	setvideovol(thcfg->sub("ints")->getInt("videovolume"));
}

#else //WITHSFM
void bindvideo(){}
void stopvideo(){}
void setvideovol(int vol){}
#endif // WITHSFM
#ifdef WITHSFM

extern std::string switchmediasuffix(std::string thismedia,TyphonConfig *suflist); // from filehandling.cpp

int moviestate()	{return movie.getStatus();}

void scalefullscreenvideo()
{
	changey=(rootY-oldrootY)/icheight;
	changex=(rootX-oldrootX)/icwidth;
	
	if (pick()==2)
	{
		if (rtcfg->getBool("clickedleft"))	
		{
			((TConfig<int>*) vicfg->sub("videos")->sub(Ics())->get("posx"))->operator-(changex);
			((TConfig<int>*) vicfg->sub("videos")->sub(Ics())->get("posy"))->operator+(changey);
		}
		if (rtcfg->getBool("clickedright"))	
		{
			((TConfig<int>*) vicfg->sub("videos")->sub(Ics())->get("width"))->operator+(changex);
			((TConfig<int>*) vicfg->sub("videos")->sub(Ics())->get("height"))->operator-(changey);
		}
	}
}

// switch video view - fullscreen, background, preview
void switchplayvideohere()
{
	TyphonConfig* vi=thcfg->sub("objects")->sub("video");
	((TConfig<int>*) vi->get("drawhere"))->operator+(1);
	if (vi->getInt("drawhere")==2)	{rtcfg->setBool("previewwasused",thcfg->sub("objects")->sub("preview")->getBool("enabled"));}
	if (vi->getInt("drawhere")==3)	{
										rtcfg->setBool("mousewas",!tycfg->sub("globalswitches")->getBool("hidemouse"));
										window.setMouseCursorVisible(0);
										thcfg->sub("objects")->sub("preview")->setBool("enabled",rtcfg->getBool("previewwasused"));
									}
	if (vi->getInt("drawhere")>3)	{window.setMouseCursorVisible(rtcfg->getBool("mousewas"));thcfg->sub("objects")->sub("video")->set("drawhere",0);}
	if (vi->getInt("drawhere")==0)	{pausevideo();}
	if (vi->getInt("drawhere")==1)	
	{
		if (moviestate()==1)	playvideo(); 
		else 					startplayvideo();
/*
		{
//			tycfg->set("usedvideo",vicfg->sub("videos")->sub(Ics())->getString("file"));
			startplayvideo();
		}
*/
	}
}

// draw video in background
void drawvideo()
{
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	texcol("video");
	setpos("video");
	bindvideo();
	rotateme("video");
	if (roundf(thcfg->sub("objects")->sub("video")->getFloat("depth"))==0)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex3f( rtcfg->sub("objects")->sub("video")->getFloat("width")	,1.0f				, 0);
		glTexCoord2f(1, 1); glVertex3f( rtcfg->sub("objects")->sub("video")->getFloat("width")	,rtcfg->sub("objects")->sub("video")->getFloat("height")	, 0);
		glTexCoord2f(0, 1); glVertex3f( -1.0f													,rtcfg->sub("objects")->sub("video")->getFloat("height")	, 0);
		glTexCoord2f(0, 0); glVertex3f( -1.0f													,1.0f				, 0);
		glEnd();
	}
	else
	{
		drawcube
		(
			rtcfg->sub("objects")->sub("video")->getFloat("width"),
			rtcfg->sub("objects")->sub("video")->getFloat("height"),
			thcfg->sub("objects")->sub("video")->getFloat("depth"),
			"video",
			"videoback"
		);
	}
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

// draw video as preview
void drawvideopreview()
{
	thcfg->sub("objects")->sub("preview")->setBool("enabled",0);
	thcfg->sub("objects")->sub("video")->set("drawhere",2);

	glPushMatrix();
	texcol("preview");
	setpos("preview");

	bindvideo();
	rotateme("preview");
	
	if (roundf(thcfg->sub("objects")->sub("preview")->getFloat("depth"))!=0)
	{
		drawcube
		(
			rtcfg->sub("objects")->sub("preview")->getFloat("width"),
			rtcfg->sub("objects")->sub("preview")->getFloat("height"),
			thcfg->sub("objects")->sub("preview")->getFloat("depth"),
			"video",
			"videoback"
		);
	}
	else
	{
		glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex3f( rtcfg->sub("objects")->sub("preview")->getFloat("width")	,1.0f				, 0);
		glTexCoord2f(1, 1); glVertex3f( rtcfg->sub("objects")->sub("preview")->getFloat("width")	,rtcfg->sub("objects")->sub("preview")->getFloat("height")	, 0);
		glTexCoord2f(0, 1); glVertex3f( -1.0f														,rtcfg->sub("objects")->sub("preview")->getFloat("height")	, 0);
		glTexCoord2f(0, 0); glVertex3f( -1.0f														,1.0f				, 0);
		glEnd();
	}
	glPopMatrix();
}

// draw video fullscreen
void drawvideofull()
{
	glPushMatrix();
	if (Bvidmenu())
	glTranslatef(-vicfg->sub("videos")->sub(Ics())->getInt("posx"),-vicfg->sub("videos")->sub(Ics())->getInt("posy"),0);
	else
	glTranslatef(-rtcfg->getInt("width")/2-1,-rtcfg->getInt("height")/2-1,0);
	bindvideo();
	if (Bvidmenu())
	drawquad(vicfg->sub("videos")->sub(Ics())->getInt("width"),vicfg->sub("videos")->sub(Ics())->getInt("height"));
	else	
	drawquad(rtcfg->sub("objects")->sub("wallpaper")->getFloat("width")+1,-rtcfg->sub("objects")->sub("wallpaper")->getFloat("height")+2);
	glPopMatrix();
}

// create a list of all found videos
void listvideos()
{
	FILE *vxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("videoxml").c_str(),"r");
	if(vxmlcfgcheck)
	{
#ifdef DEBUG
		printf("found videolist %s\n",rtcfg->sub("configs")->getString("videoxml").c_str());
#endif // DEBUG
		vicfg->loadXML(rtcfg->sub("configs")->getString("videoxml"));
		fclose(vxmlcfgcheck);
	}
	else
	{	
		struct dirent **sdit;
		int num_entries;
		if ((num_entries = scandir((rtcfg->sub("scanned")->getString("videopath")).c_str(), &sdit, fileselectAll, alphasort))< 0){}
		else
		{
			int k=0;
			for (int j=0; j<num_entries; j++)
			{
				if
				(
					compare(sdit[j]->d_name, ".wmv") ||
					compare(sdit[j]->d_name, ".avi") ||
					compare(sdit[j]->d_name, ".mpg") ||
					compare(sdit[j]->d_name, ".mkv") ||
					compare(sdit[j]->d_name, ".mp4") ||
					compare(sdit[j]->d_name, ".ogv") ||
					compare(sdit[j]->d_name, ".flv")
				)
				{
					std::stringstream vidtry;
					vidtry << "video" << k;
					vicfg->sub("videos")->sub(vidtry.str(),k++)->set("file",sdit[j]->d_name);
					vicfg->sub("videos")->sub(vidtry.str())->set("width",rtcfg->getInt("width"));
					vicfg->sub("videos")->sub(vidtry.str())->set("height",rtcfg->getInt("height"));
					vicfg->sub("videos")->sub(vidtry.str())->set("posx",rtcfg->getInt("width")/2);
					vicfg->sub("videos")->sub(vidtry.str())->set("posy",rtcfg->getInt("height")/2);
				}
			}
			vicfg->saveXML(rtcfg->sub("configs")->getString("videoxml"));
		}
	}
}

void prepvideopreview()
{
	if (Bvidmenu())
	{
		rtcfg->set("media",thcfg->sub("paths")->getString("videopreviewpath")+"/"+getPrefix(vicfg->sub("videos")->sub(Ics())->getString("file"))+".mpg");
	}
	else
	{
		if (Iem()==0 || Ishowmenu()==0)
		{
			togentryname(1);
			rtcfg->set("media",thcfg->sub("paths")->getString("previewvideopath")+"/"+mentry(Ishowmenu()-1,Ics())+".png");
			togentryname(0);
		}
		if (Iem()!=0)
		rtcfg->set("media",emcfg->sub(IeID()-1)->getString("romvideospath")+"/"+getPrefix(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getString("file"))+".mpg");
	}
}

void updatevideopreviewpath()
{
	if (!Bvidmenu())
	{ prepvideopreview(); }

	FILE *prevcheck = fopen(rtcfg->getString("media").c_str(),"r");
	if(prevcheck)	
	{fclose(prevcheck);}
	else
	{rtcfg->set("media",setEnvhome(rtcfg->getString("media")));}
	rtcfg->set("media",switchmediasuffix(rtcfg->getString("media"),rtcfg->sub("suffix")->sub("video")));
	
// none of the suffixes found - fallback to defaults

	prevcheck = fopen(rtcfg->getString("media").c_str(),"r");
	if(prevcheck){fclose (prevcheck);}
	else
	{
		rtcfg->setBool("videofailed",1);
		updatepreview();
		rtcfg->set("media",rtcfg->getString("currentprev"));
		rtcfg->setBool("videofailed",0);
	}
}

void startplayvideo()
{
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==0)
	thcfg->sub("objects")->sub("video")->set("drawhere",1);

	{
		if(
		(Bvidmenu()) ||
		(thcfg->sub("switches")->getBool("playvideoonstart")) 
		)
		{rtcfg->set("media",rtcfg->sub("scanned")->getString("videopath")+"/"+tycfg->getString("usedvideo"));}
		else 	{updatevideopreviewpath();};

		if (movie.openFromFile(rtcfg->getString("media").c_str()))
		{
			playvideo();
			if (thcfg->sub("objects")->sub("video")->getBool("back"))
			updateimage(rtcfg->sub("scanned")->getString("wallpaperpath"), thcfg->sub("textures")->getString("videoback"), getTextureS("videoback"));
		}
		
		if (tycfg->sub("globalswitches")->getBool("autoscreenshot") && !rtcfg->getBool("selchanged")) startscrotting(tycfg->getString("usedvideo"),rtcfg->sub("scanned")->getString("videopreviewpath"));
	}
}

void togglevideomenu()
{
	if (Ishowmenu()>=1)
	{
		setcs(0);
		if(!Bvidmenu())
		{
			thcfg->sub("objects")->sub("video")->set("videomenu",Ishowmenu());
			setem(0);
			if(Bp3tmenu())		tycfg->sub("globalints")->set("p3tmenu",-1);
			if(Bthememenu())	tycfg->sub("globalints")->set("thememenu",-1);
		}
		else
		thcfg->sub("objects")->sub("video")->set("videomenu",-1);

		if(vicfg->sub("videos")->size()<1)
		thcfg->sub("objects")->sub("video")->set("videomenu",-1);

		updatepreview();
	}
}

void changevideo(bool up)
{
	if(Ishowmenu()!=0)
	{
		if (up)
		{
			if(Ics()==0)	{setcs(vicfg->sub("videos")->size()-1);}
			else 			{setcs(Ics()-1);}
			tycfg->set("usedvideo",vicfg->sub("videos")->sub(Ics())->getString("file"));
		}
		else
		{
			if(Ics()==vicfg->sub("videos")->size()-1)	{setcs(0);}
			else 			{setcs(Ics()+1);}
			tycfg->set("usedvideo",vicfg->sub("videos")->sub(Ics())->getString("file"));
		}
	}		
}
#else // WITHOUT SFM
void togglevideomenu(){}
void changevideo(bool up){}
void startplayvideo(){}
void switchplayvideohere(){}
void scalefullscreenvideo(){}
void videovol(int change){}
void listvideos(){}
#endif // WITHSFM
