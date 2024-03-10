#include <SFML/Audio.hpp>
#include "sound.h"
#include "themehandling.h"
#include "values.h"
#include "windowhandling.h"
#include "glmenu.h"
#include "glhandling.h"
#include "videoplayerhandling.h"
#include "filehandling.h"
#include "timerfunc.h"
#include "inputhandling.h"
#include "xmlconfig.h"

sf::Music Music;
sf::SoundBuffer buffer;
sf::Sound soundclick;

void loadsfx()
{
	std::string absolutesoundpath=rtcfg->sub("scanned")->getString("soundpath")+"/"+thcfg->sub("files")->getString("soundfile");
	
	FILE *sfxpath = fopen(absolutesoundpath.c_str(),"r");
	if(sfxpath)
	{fclose(sfxpath);}
	else
	{
		std::string pkgdatadir=PKGDATADIR;
		absolutesoundpath=pkgdatadir+"/generic/sfx/cursor.wav";
	}

	if (!buffer.loadFromFile(absolutesoundpath)){thcfg->sub("switches")->set("playsound",0);}
	soundclick.setBuffer(buffer);
}

void loadmusic()
{
	if (thcfg->sub("switches")->getBool("playmusic"))
   	{
		std::string absolutemusicpath=rtcfg->sub("scanned")->getString("musicpath")+"/"+thcfg->sub("files")->getString("musicfile");

		FILE *muspath = fopen(absolutemusicpath.c_str(),"r");
		if(muspath)
		{
			if (!Music.openFromFile(absolutemusicpath)){thcfg->sub("switches")->set("playmusic",0);}
			fclose (muspath);
		}
		else
		{thcfg->sub("switches")->set("playmusic",0);}
	}
}

void playmusic() 	{loadmusic();Music.play();}

void drawmusictitle()
{
	if ((thcfg->sub("objects")->sub("musictitle")->getBool("enabled") &&
		 thcfg->sub("switches")->getBool("playmusic"))|| pick()==4)
	{
		glPushMatrix();
		textcol("musictitle",&musicdurationfontfx, 0);
		glTranslatef
		(
			rtcfg->sub("objects")->sub("musictitle")->getFloat("posx"),
			rtcfg->sub("objects")->sub("musictitle")->getFloat("posy"),
			thcfg->sub("objects")->sub("musictitle")->getInt("posz")
		);
		rtcfg->set("musicwidth",musicdurationfontfx.getWidth(thcfg->sub("files")->getString("musicfile")));

		textrot("musictitle",thcfg->sub("files")->getString("musicfile"),&musicdurationfontfx,0);
		musicdurationfontfx.draw(0.0,0.0, thcfg->sub("files")->getString("musicfile"),thcfg->sub("objects")->sub("musictitle")->getFloat("depth"));
		glPopMatrix();
	}
}

void drawmusicduration()
{
	if((thcfg->sub("objects")->sub("sound")->getBool("enabled") && 
		thcfg->sub("switches")->getBool("playmusic"))|| pick()==4
	)
	{
		glPushMatrix();
		textcol("sound",&musicdurationfontfx, 0);
		setpos("sound");
		rtcfg->set("musicwidth",musicdurationfontfx.getWidth(rtcfg->getString("musicduration")));
		textrot("sound",rtcfg->getString("musicduration"),&musicdurationfontfx,0);
		musicdurationfontfx.draw(0.0,0.0, rtcfg->getString("musicduration"),thcfg->sub("objects")->sub("musictitle")->getFloat("depth"));
		glPopMatrix();
	}
}

void initaudio()
{
	if(thcfg->sub("switches")->getBool("playmusic") && !tycfg->sub("globalswitches")->getBool("startmusiconlaunch")) 	loadmusic();
	if(tycfg->sub("globalswitches")->getBool("startmusiconlaunch")){thcfg->sub("switches")->setBool("playmusic",1);playmusic();}
	if(thcfg->sub("switches")->getBool("playsound"))											loadsfx();
}

void playsfx()
{
	if (thcfg->sub("switches")->getBool("playsound"))	{soundclick.play();}
}

void stopsound()
{
	if (thcfg->sub("switches")->getBool("playsound"))
	{
		soundclick.stop();
		thcfg->sub("switches")->set("playsound",0);
	}
}

void getmusicdur()
{
	char mudur[50];
	sprintf(mudur, "%4.2f / %4.2f", Music.getPlayingOffset().asSeconds(),Music.getDuration().asSeconds());
	rtcfg->set("musicduration",mudur);
}

void stopmusic()
{
	Music.stop();
	rtcfg->set("musicduration"," ");
}

void togglesound()
{
	thcfg->sub("switches")->setBool("playsound",!thcfg->sub("switches")->getBool("playsound"));
	if (thcfg->sub("switches")->getBool("playsound")){playsfx();}
	else {stopsound();}
}

void togglemusic()
{
	thcfg->sub("switches")->setBool("playmusic",!thcfg->sub("switches")->getBool("playmusic"));
	if (thcfg->sub("switches")->getBool("playmusic")){playmusic();}
	else {stopmusic();}
}

void listaudiofiles(std::string type,std::string path)
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir(rtcfg->sub("scanned")->getString(path).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		int k=0;
		for (int j=0; j<num_entries; j++)
		{
			if
			(
				compare(sdit[j]->d_name, ".ogg") || 
				compare(sdit[j]->d_name, ".wav")

			) 
			{
			std::stringstream entry;
			entry << type << k;
			rtcfg->sub(type)->sub(entry.str(),k++)->set("file",sdit[j]->d_name);
			}
		}
	}
}

void musicvol(int musicvolume){Music.setVolume(musicvolume);}

void switchmusic(bool up)
{
	if (rtcfg->sub("music")->size()>0)
	{
		if(up)
		{
			if(rtcfg->getInt("musiccount")==0)	{rtcfg->set("musiccount",rtcfg->sub("music")->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get("musiccount"))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt("musiccount")==rtcfg->sub("music")->size()-1)	{rtcfg->set("musiccount",0);}
			else 							{((TConfig<int>*) rtcfg->get("musiccount"))->operator+(1);}
		}
		thcfg->sub("files")->setC("musicfile",rtcfg->sub("music")->sub(rtcfg->getInt("musiccount"))->getString("file"));
		thcfg->sub("switches")->setBoolC("playmusic",1);
		playmusic();
	}
}

void switchsfx(bool up)
{
	if (rtcfg->sub("sfx")->size()>0)
	{
		if(up)
		{
			if(rtcfg->getInt("sfxcount")==0)	{rtcfg->set("sfxcount",rtcfg->sub("sfx")->size()-1);}
			else 							{((TConfig<int>*) rtcfg->get("sfxcount"))->operator-(1);}
		}
		else
		{
			if(rtcfg->getInt("sfxcount")==rtcfg->sub("sfx")->size()-1)	{rtcfg->set("sfxcount",0);}
			else 							{((TConfig<int>*) rtcfg->get("sfxcount"))->operator+(1);}
		}
		thcfg->sub("files")->setC("soundfile",rtcfg->sub("sfx")->sub(rtcfg->getInt("sfxcount"))->getString("file"));
		loadsfx();
		playsfx();
	}
}

void musicdone() 
{
	if(thcfg->sub("switches")->getBool("playnextmusictrack"))
	{
		switchmusic(1);
		thcfg->sub("switches")->setBool("playmusic",1);playmusic();
	}
	else
	stopmusic();
}
