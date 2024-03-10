#include "includes.h"
#include "emuhandling.h"
#include "confighandling.h"
#include "windowhandling.h"
#include "videoplayerhandling.h"
#include "inputhandling.h"
#include "values.h"
#include "filehandling.h"
#include "systeminfo.h"
#include "xmlconfig.h"
#include "networking.h"

void setemu( int thismenu,int thisemu)
{
	tycfg->set("showmenu",thismenu);
	setem(thisemu);

	if (Bvidmenu())
	{((TConfig<int>*) thcfg->sub("objects")->sub("video")->get("videomenu"))->operator-(1);}
}

void listroms()
{
	FILE *romxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("romxml").c_str(),"r");
	if(romxmlcfgcheck)
	{
#ifdef DEBUG
		printf("found romlist %s\n",rtcfg->sub("configs")->getString("romxml").c_str());
#endif //DEBUG
		loadromxmlcfg();
		fclose(romxmlcfgcheck);
	}
	else
	{
		for (int i=0; i<emcfg->size(); i++)
		{
			std::string* emuPath=emcfg->sub(i)->getStringP("rompath");
			std::string* emuSuffix=emcfg->sub(i)->getStringP("romsuffix");
			
				struct dirent **sdit;
				int num_entries;
				if ((num_entries = scandir((*emuPath).c_str(), &sdit, fileselectAll, alphasort))< 0){}
				else
				{
					int k=0;
					for (int j=0; j<num_entries; j++)
					{
						if (strstr(sdit[j]->d_name, (*emuSuffix).c_str()))
						{
							std::stringstream romtry;
							romtry << "rom" << k;
							std::stringstream emtry;
							emtry << "emulator" << i;	
							rmcfg->sub("Roms")->sub(emtry.str(),i)->sub(romtry.str(),k++)->set("file",sdit[j]->d_name);
					//		rmcfg->sub("Roms")->sub(emtry.str(),i)->sub(romtry.str())->set("args","");
							rmcfg->sub("Roms")->sub(emtry.str(),i)->sub(romtry.str())->set("gameid",0);
						}
					}
				}
		}
		if(rmcfg->sub("Roms")->size()>0)
		{
			// add two examples for user defineable strip name entries
			rmcfg->sub("striptitle")->setCa("strip","32x -");
			rmcfg->sub("striptitle")->setCa("strip","GBA -");
			rmcfg->saveXML(rtcfg->sub("configs")->getString("romxml"));
		}
	}
}

// open emulator list
void openemulist()
{
	rtcfg->setBool("selchanged",1);

	switch(Ics())
	{
		case 1:  setemu(1,1);break;
		case 2:  setemu(2,1);break;
		case 3:  setemu(3,1);break;
		case 4:  setemu(4,1);break;
		case 5:  setemu(5,1);break;
		case 6:  setemu(6,1);break;
		case 7:  setemu(7,1);break;
		case 8:  setemu(1,2);break;
		case 9:  setemu(2,2);break;
		case 10: setemu(3,2);break;
		case 11: setemu(4,2);break;
		case 12: setemu(5,2);break;
		case 13: setemu(6,2);break;
		case 14: setemu(7,2);break;
		case 15: setemu(1,3);break;
		case 16: setemu(2,3);break;
		case 17: setemu(3,3);break;
		case 18: setemu(4,3);break;
		case 19: setemu(5,3);break;
		case 20: setemu(6,3);break;
		case 21: setemu(7,3);break;
		default: break;
		break;
	}
}

void reademucfg()
{

	FILE *emuxmlcfgcheck = fopen(rtcfg->sub("configs")->getString("emuxml").c_str(),"r");
	if(emuxmlcfgcheck)
	{
		loademuxmlcfg();
		fclose(emuxmlcfgcheck);
	}
	else
	{	
		setemudefaults();
		saveemuxmlcfg();
	}
}

void toggleemumode()
{
	int emucnt=rmcfg->sub("Roms")->size();
	if(Ishowmenu()*(Iem()+1)>emucnt){setem(0);}
	
	else if(Ishowmenu()>=1 && Ishowmenu()<=emucnt)
	{
		incem(1);
		if (Iem()>3)	{setem(0);}
		
		if (Iem()!=0)
		{
			if(Bp3tmenu())		tycfg->sub("globalints")->set("p3tmenu",-1);
			if(Bthememenu())	tycfg->sub("globalints")->set("thememenu",-1);
			if(Bvidmenu())		thcfg->sub("objects")->sub("video")->set("videomenu",-1);
		}
	}
	if (Ics()>Ime()-1)	rtcfg->sub("curSel")->set(Ishowmenu(),Ime()-1);
	if (Iem()!=0)	getrominfo(IeID()-1,Ics(),0);
	updatepreview();
	if(thcfg->sub("objects")->sub("menuicon")->getBool("enabled")) {loadmenuicons();};
}

void gameid(bool up)
{
//	if (Iem()!=0)
	{
		TyphonConfig* rom =rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics());

		if (up)
		{
			((TConfig<int>*) rom->get("gameid"))->operator+(1);
			if(rom->getInt("gameid")>prcfg->sub("games")->size()-1) rom->set("gameid",0);
		}
		else
		{
			((TConfig<int>*) rom->get("gameid"))->operator-(1);
			if(rom->getInt("gameid")<0) rom->set("gameid",prcfg->sub("games")->size()-1);	
		}
		printf("gameid %i\n",rom->getInt("gameid"));
		rmcfg->saveXML(rtcfg->sub("configs")->getString("romxml"));
		forcepreviewupdate();
	}
}
void scrapeallroms()
{
	int mode=0;
		 if (compare(rtcfg->getString("scrape"),"rominfo")) mode=1;
	else if (compare(rtcfg->getString("scrape"),"boxart")) mode=2;
	else if (compare(rtcfg->getString("scrape"),"fanart")) mode=3;
	else if (compare(rtcfg->getString("scrape"),"all")) mode=4;
	
	if(mode==0)
	{
		printf ("%s\n",lacfg->getString("noscrape").c_str());exit(0);
	}
	
	else if(mode==1)
	{
		printf ("%s - %s\n",lacfg->getString("rominfo").c_str(),lacfg->getString("patient").c_str());

		for (int i=0; i<emcfg->size(); i++)
		{
			scraperoms(i,0);
		}
		exit(0);
	}

	else if(mode==2)
	{
		printf ("%s - %s\n",lacfg->getString("boxart").c_str(),lacfg->getString("patient").c_str());
		for (int i=0; i<emcfg->size(); i++)
		{
			scraperoms(i,1);
		}
		exit(0);
	}
	else if(mode==3)
	{
		printf ("%s - %s\n",lacfg->getString("fanart").c_str(),lacfg->getString("patient").c_str());
	}
	else if(mode==4)
	{
		printf ("%s - %s\n",lacfg->getString("everything").c_str(),lacfg->getString("patient").c_str());
	}

}
