#include "themehandling.h"
#include "picturehandling.h"
#include "p3thandling.h"
#include "sound.h"
#include "glmenu.h"
#include "systeminfo.h"
#include "glhandling.h"
#include "filehandling.h"
#include "xmlconfig.h"
#include "inputhandling.h"
#include <p3t.h>

void listp3ts()
{
	rtcfg->sub("scanned")->set("p3tpath",checkpath(tycfg->sub("paths")->getString("p3tpath"),rtcfg->getString("datadir")));

	FILE *xmlcfgcheck = fopen(rtcfg->sub("configs")->getString("p3txml").c_str(),"r");
	if(xmlcfgcheck)
	{
#ifdef DEBUG
		printf("found p3tlist %s\n",rtcfg->sub("configs")->getString("p3txml").c_str());
#endif //DEBUG
		p3cfg->loadXML(rtcfg->sub("configs")->getString("p3txml"));
		fclose(xmlcfgcheck);
	}
	else
	{	
		struct dirent **sdit;
		int num_entries;
		if ((num_entries = scandir((rtcfg->sub("scanned")->getString("p3tpath")).c_str(), &sdit, fileselectAll, alphasort))< 0){}
		else
		{
			int k=0;
			for (int j=0; j<num_entries; j++)
			{
				if (strstr(sdit[j]->d_name, ".p3t"))
				{
				std::stringstream p3try;
				p3try << "theme" << k;
				p3cfg->sub("ps3themes")->set(p3try.str(),sdit[j]->d_name,k++);
				}
			}
			p3cfg->saveXML(rtcfg->sub("configs")->getString("p3txml"));
		}
	}
}

// print the above list
void printp3ts()
{
	listp3ts();
	for (int p=0; p<p3cfg->sub("ps3themes")->size(); p++)
	printf("%s\n",getPrefix(p3cfg->sub("ps3themes")->getString(p)).c_str());
	exit(0);
}

// extract the selected p3t
void extractp3t()
{
	if(strcmp((char*)(getPrefix(tycfg->getString("p3tfile"))+".p3t").c_str(),"noneselected.p3t")!=0)
	{
		removerec(tycfg->sub("tempdirs")->getString("p3text"));
		rtcfg->set("themetype",PS3);
		std::string fullp3t;

		if (rtcfg->getBool("importingtheme"))	{fullp3t=rtcfg->getString("importtheme");}
		else									{fullp3t=rtcfg->sub("scanned")->getString("p3tpath")+"/"+getPrefix(tycfg->getString("p3tfile"))+".p3t";}
		FILE *p3there = fopen(fullp3t.c_str(),"r");
		if(p3there)
		{
			fclose (p3there);
			p3textract(fullp3t.c_str(),(char*)tycfg->sub("tempdirs")->getString("p3text").c_str());
			rtcfg->set("monthcolor",p3tcolor);
		}
		else
		{
			printf("p3tfile %s doesn't exist - exiting\n",fullp3t.c_str());
			exit(0);
		}
	}
}

// extract p3t preview
void extractp3tpreview()
{
	if(strcmp((char*)(getPrefix(tycfg->getString("p3tfile"))+".p3t").c_str(),"noneselected.p3t")!=0)
	{
		std::string fullp3t=rtcfg->sub("scanned")->getString("p3tpath")+"/"+getPrefix(tycfg->getString("p3tfile"))+".p3t";
		p3tprevextract(fullp3t.c_str(),(char*)tycfg->sub("tempdirs")->getString("p3text").c_str());
	}
}

// switch to next p3t
void changep3t()
{
	if (p3cfg->sub("ps3themes")->size()>0)
	{
		tycfg->set("themename","p3t");
		extractp3t();
		changetheme();
	}
}

// create themedatadir for a "typhoned" theme

void createthisthemebgdir()
{
	std::string ibgdir=rtcfg->getString("ithemedatadir")+"/backgrounds/";

	FILE *fcheck = fopen(ibgdir.c_str(),"r");
	if(fcheck)	{fclose (fcheck);}
	else		{mkdir(ibgdir.c_str(), 0755);}	
}

// checks if p3t theme file is already in themedir and copies the default if it wasn't found
void checkp3ttheme()
{
	std::string p3tinhome=rtcfg->getString("tythemedir") + "/p3t.xml";

	FILE *themecheck = fopen(p3tinhome.c_str(),"r");
	if(themecheck)	{fclose (themecheck);}
	else
	{
		std::string defaultp3tconf=rtcfg->getString("systemconfigdir")+ "p3t.xml";
		thcfg->loadXML(defaultp3tconf);
		thcfg->saveXML(p3tinhome);
	}
}

void resetp3tpaths()
{
	thcfg->sub("paths")->setC("wallpaperpath","backgrounds");
	thcfg->sub("paths")->setC("stencilpicturepath","\0");
	thcfg->sub("paths")->setC("customimagepath","\0");
	thcfg->sub("paths")->setC("menuiconpath","\0");
	thcfg->sub("paths")->setC("soundpath","\0");
}

// typhonify a p3t theme
void copyp3t()
{
	createthemedatadir();
	createthisthemedir();
	createthisthemebgdir();
	
	struct dirent **sdit;
	int num_entries;
	
	if ((num_entries = scandir(tycfg->sub("tempdirs")->getString("p3text").c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			if
			(
				compare(sdit[j]->d_name, ".png") || 
				compare(sdit[j]->d_name, ".jpg") || 
				compare(sdit[j]->d_name, ".wav")

			)
			{copyfromto(tycfg->sub("tempdirs")->getString("p3text")+"/"+sdit[j]->d_name,rtcfg->getString("ithemedatadir"));}
		}
	}

	num_entries=0;
	if ((num_entries = scandir(tycfg->sub("tempdirs")->getString("p3tebg").c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			if(compare(sdit[j]->d_name, ".jpg"))
			{copyfromto(tycfg->sub("tempdirs")->getString("p3tebg")+"/"+sdit[j]->d_name,rtcfg->getString("ithemedatadir")+"/backgrounds/");}
		}
	}

	resetp3tpaths();
	themepath(getPrefix(tycfg->getString("p3tfile")));
	savetheme();
	shownote(lacfg->getString("saved")+getPrefix(tycfg->getString("p3tfile")));
}

void togglep3tmenu()
{
	if (Ishowmenu()>=1)
	{
		setcs(0);
		if(!Bp3tmenu())
		{
			tycfg->sub("globalints")->setC("p3tmenu",Ishowmenu());
			setem(0);
			if(Bthememenu())	tycfg->sub("globalints")->set("thememenu",-1);
			if(Bvidmenu())		thcfg->sub("objects")->sub("video")->set("videomenu",-1);
		}
		else
		tycfg->sub("globalints")->setC("p3tmenu",-1);
		tycfg->set("p3tfile",p3cfg->sub("ps3themes")->getString(Ics()));
		updatepreview();
	}
}

void switchp3t(bool up)
{
	if(Ishowmenu()!=0)
	{
		if(up)
		{
			if(Ics()==0)	{setcs(p3cfg->sub("ps3themes")->size()-1);}
			else 			{setcs(Ics()-1);}
		}
		else
		{
			if(Ics()==p3cfg->sub("ps3themes")->size()-1)	{setcs(0);}
			else 			{setcs(Ics()+1);}
		}
		tycfg->set("p3tfile",p3cfg->sub("ps3themes")->getString(Ics()));
	}		
}

bool isp3t()
{	
	if (strcmp((char*)tycfg->getString("themename").c_str(),"p3t")==0) return 1;
	else return 0;
}
