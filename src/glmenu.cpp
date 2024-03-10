#include "glmenu.h"
#include "emuhandling.h"
#include "inputhandling.h"
#include "systeminfo.h"
#include "videoplayerhandling.h"
#include "values.h"
#include "editor.h"
#include "windowhandling.h"
#include "mmdhandling.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "filehandling.h"

void parsedesktopfiles()
{
	std::string desktopfiledir="/usr/share/applications/";
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir(desktopfiledir.c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			if (strstr(sdit[j]->d_name, ".desktop"))
			{
				std::string absfile=desktopfiledir+sdit[j]->d_name;
				int menu=6; // default menu is favourites
				std::string exec="";
				std::string name="";
				std::string desc="";
				std::string icon="";
				
#ifdef DEBUG
				printf("parsing desktopfile %s\n",absfile.c_str());
#endif
				std::string line;
				std::ifstream desktopfile (absfile.c_str());
				if (desktopfile.is_open())
				{
					while ( desktopfile.good() )
					{
						std::getline (desktopfile,line);
						if (line.find("Categories=", 0) != std::string::npos)
						{
							// games
							if
							(
								(line.find("Emulator;", 0) != std::string::npos && line.find("TerminalEmulator;", 0) == std::string::npos) ||
								line.find("Game;", 0) != std::string::npos
							)
							{menu=0;}
							// multimedia
							else if
							(
								line.find("AudioVideo", 0) != std::string::npos 	||
								line.find("Video", 0) != std::string::npos			||
								line.find("Audio", 0) != std::string::npos			||
								line.find("Player", 0) != std::string::npos			||
								line.find("DiscBurning", 0) != std::string::npos	||
								line.find("Recorder", 0) != std::string::npos		||
								line.find("TV", 0) != std::string::npos
							)
							{menu=1;}							
							// internet
							else if
							(
								line.find("Network", 0) != std::string::npos 		||
								line.find("WebBrowser", 0) != std::string::npos 	||
								line.find("Calendar", 0) != std::string::npos 		||
								line.find("Email", 0) != std::string::npos 			||
								line.find("News", 0) != std::string::npos
							)
							{menu=2;}
							// System
							else if
							(
								line.find("System", 0) != std::string::npos 			||
								line.find("Settings", 0) != std::string::npos 			||
								line.find("Printing", 0) != std::string::npos 			||
								line.find("TerminalEmulator", 0) != std::string::npos 	||
								line.find("FileManager", 0) != std::string::npos 		||
								line.find("Development", 0) != std::string::npos 		||
								line.find("Utility", 0) != std::string::npos 			||
								line.find("FileTools", 0) != std::string::npos
							)
							{menu=3;}
							// System
							else if
							(
								line.find("Office", 0) != std::string::npos 			||
								line.find("TextTools", 0) != std::string::npos
							)
							{menu=4;}							
							// Graphics
							else if
							(
								line.find("Art;", 0) != std::string::npos				||
								line.find("Graphics", 0) != std::string::npos
							)
							{menu=5;}
						}

						if (line.find("Name=", 0) != std::string::npos && 
							line.find("GenericName=", 0) == std::string::npos && 
							line.find("WMName=", 0) == std::string::npos &&
							line.find("SettingsName=", 0) == std::string::npos &&
							line.find("FullName=", 0) == std::string::npos
							
							)
						{name=cutstring(line.c_str(),"Name=");}
						
						if (line.find("Comment=", 0) != std::string::npos)
						{desc=cutstring(line.c_str(),"Comment=");}
						
						if (line.find("Icon=", 0) != std::string::npos)
						{
							std::string ticon=cutstring(line.c_str(),"Icon=");
							if(fExists(ticon)) 	{icon=ticon;}
							else 
							{
								for (int p=0; p<tycfg->sub("iconpaths")->size()-1; p++)
								{
									std::string pngpath=tycfg->sub("iconpaths")->getString(p)+"/"+ticon+".png";
									if(fExists(pngpath))
									{
										#ifdef WITHIM
										sf::Image Image;
										if (Image.loadFromFile(pngpath))
										icon=pngpath;
										else
										{
											icon=rtcfg->sub("scanned")->getString("iconconvpath")+"/"+getPrefix(getFile(pngpath))+".png";
											systemstart("convert", pngpath+" "+icon,1);
										}
										#else
										icon=pngpath;
										#endif // WITHIM
									}
									std::string xpmpath=tycfg->sub("iconpaths")->getString(p)+"/"+ticon+".xpm";
									if(fExists(xpmpath))
									{
										#ifdef WITHIM
										icon=rtcfg->sub("scanned")->getString("iconconvpath")+"/"+getPrefix(getFile(xpmpath))+".png";
										systemstart("convert", xpmpath+" "+icon,1);
										#endif
									}
								}
							}
						}

						if (line.find("Exec=", 0) != std::string::npos && line.find("TryExec=", 0) == std::string::npos)
						{exec=cutstring(line.c_str(),"Exec=");}
					}
						bool exists=0;
						for (int e=0; e<mecfg->sub(menu)->sub("entry")->size(); e++)
						{
							std::string old=mecfg->sub(menu)->sub("entry")->sub(e)->getString("name");
							if(strcmp(old.c_str(),name.c_str())==0 || strcmp(old.c_str(),name.c_str())==0)	exists=1;
						}
						if(!exists)
						{
							#ifdef DEBUG
							printf("added new entry %s\n",name.c_str());
							#endif
							TyphonConfig* tshort=mecfg->sub(menu)->sub("entry")->subCa("prog");
							tshort->set("binary",getExec(getFile(exec)));
							tshort->set("args","");
							tshort->set("name",name);
							tshort->set("icon",icon);
						}
						desktopfile.close();
				}
			}
		}
	}	
}

void calcfademenu()
{
	float fadespeed=thcfg->sub("floats")->getFloat("menufadespeed");
	if(thcfg->sub("floats")->getFloat("menufadeduration")!=fadespeed)
	{
		rtcfg->set("alphaspeed",(int)(255/(thcfg->sub("floats")->getFloat("menufadeduration")/fadespeed))+(int)fadespeed);
		if (rtcfg->getBool("changemenu") && rtcfg->getInt("scrollmenu")==1)
		{
			((TConfig<float>*) rtcfg->get("fademenu"))->operator+(fadespeed);
			((TConfig<int>*) rtcfg->get("menualpha"))->operator-(rtcfg->getInt("alphaspeed"));
		}
		if (rtcfg->getBool("changemenu") && rtcfg->getInt("scrollmenu")==2)
		{
			((TConfig<float>*) rtcfg->get("fademenu"))->operator-(fadespeed);
			((TConfig<int>*) rtcfg->get("menualpha"))->operator-(rtcfg->getInt("alphaspeed"));
		}
		if (thcfg->sub("switches")->getBool("invertfade"))
		{
			if(rtcfg->getFloat("oldfademenu")!=0 && rtcfg->getInt("scrollmenu")==1)
			{
				((TConfig<float>*) rtcfg->get("oldfademenu"))->operator-(fadespeed);
				if(rtcfg->getFloat("oldfademenu")<0)rtcfg->set("oldfademenu",(float)0);
			}
			if(rtcfg->getFloat("oldfademenu")!=0 && rtcfg->getInt("scrollmenu")==2)
			{
				((TConfig<float>*) rtcfg->get("oldfademenu"))->operator+(fadespeed);
				if(rtcfg->getFloat("oldfademenu")>0)rtcfg->set("oldfademenu",(float)0);
			}
		}
		else
		{
			if(rtcfg->getFloat("oldfademenu")!=0 && rtcfg->getInt("scrollmenu")==1)
			{
				((TConfig<float>*) rtcfg->get("oldfademenu"))->operator+(fadespeed);
				if(rtcfg->getFloat("oldfademenu")>0)rtcfg->set("oldfademenu",(float)0);
			}
			if(rtcfg->getFloat("oldfademenu")!=0 && rtcfg->getInt("scrollmenu")==2)
			{
				((TConfig<float>*) rtcfg->get("oldfademenu"))->operator-(fadespeed);
				if(rtcfg->getFloat("oldfademenu")<0)rtcfg->set("oldfademenu",(float)0);
			}
		}
	}
	if
	(
		(rtcfg->getFloat("fademenu") >= thcfg->sub("floats")->getFloat("menufadeduration")  ||	rtcfg->getFloat("fademenu") <=-thcfg->sub("floats")->getFloat("menufadeduration")) ||
		(thcfg->sub("floats")->getFloat("menufadeduration")==fadespeed)
	)
	rtcfg->setBool("fadeisdone",1);
}

void drawentrycount(int curSel, int count)
{
// show current selected rom
	if 	(thcfg->sub("objects")->sub("entrycur")->getBool("enabled"))
	{
		glPushMatrix();
		textcol("entrycur",&counterfontfx,0);
		setpos("entrycur");
		textrot("entrycur",itos(curSel),&counterfontfx,0);
		counterfontfx.draw(0.0,0.0, itos(curSel),thcfg->sub("objects")->sub("entrycur")->getFloat("depth"));
		glPopMatrix();
	}
// show maxcount
	if 	(thcfg->sub("objects")->sub("entrycount")->getBool("enabled"))
	{
		glPushMatrix();
		textcol("entrycount",&counterfontfx,0);
		setpos("entrycount");
		textrot("entrycount",itos(count-1),&counterfontfx,0);
		counterfontfx.draw(0.0,0.0,itos(count-1),thcfg->sub("objects")->sub("entrycount")->getFloat("depth"));
		glPopMatrix();
	}
}

void drawmenuentries()
{
	if(Iem()!=0 && Ime()==0)	{printf("LISTROMS shouldn't happen here\n");listroms();}

	glPushMatrix();

	calcfademenu();
	int z;
	int showminid,showmaxid=0;	

	drawentrycount(Ics()+1, Ime()+1);

	setpos("menu");

// above mainbar

	if(!thcfg->sub("switches")->getBool("hideaboveselection"))
	{
		for(z = 0; z > -(thcfg->sub("ints")->getInt("menulistmin")+1); z--)
		{
			int z2=z;
			if(z2+Ics()<0){z2=0;};
			showminid=thcfg->sub("objects")->get("menumin1")->id-z2-1;

			glPushMatrix();
			if(rtcfg->getFloat("oldfademenu")!=0)	glTranslatef(rtcfg->getFloat("oldfademenu"),0,0);
			else 				glTranslatef(rtcfg->getFloat("fademenu"),0,0);
			if(z2+Ics()!=Ics())
			{
				if(z2<=-1 && z2>=-(thcfg->sub("ints")->getInt("menulistmin")))
				{
					glTranslatef
					(
						icwidth *thcfg->sub("objects")->sub(showminid)->getInt("posx"),
						icheight*thcfg->sub("objects")->sub(showminid)->getInt("posy"),
						thcfg->sub("objects")->sub(showminid)->getInt("posz")
					);
					if (rtcfg->getInt("menualpha")==255) 	textcol(thcfg->sub("objects")->getKey(showminid),&font,0);
					else									textcol(thcfg->sub("objects")->getKey(showminid),&font,rtcfg->getInt("menualpha"));
				}

				drawmenutext(mentry(Ishowmenu()-1,Ics()+z2),showminid);
			}
			glPopMatrix();
		}
	}

// current selection
		glPushMatrix();
		rtcfg->setBool("iscursel",1);
		if(rtcfg->getFloat("oldfademenu")!=0)	glTranslatef(rtcfg->getFloat("oldfademenu"),0,0);
		else 									glTranslatef(rtcfg->getFloat("fademenu"),0,0);

		drawmenuseltext(mentry(Ishowmenu()-1,Ics()));
		rtcfg->setBool("iscursel",0);
		glPopMatrix();

// after current selection

		for(z = 1; z <  thcfg->sub("ints")->getInt("menulistmax")+1; z++)
		{
			int z2=z;

			if (Iem()!=0)	{if(z2+Ics()>rmcfg->sub("Roms")->sub(IeID()-1)->size()-1){z2=0;};}
			else 			{if(z2+Ics()>Ime()-1){z2=0;};}
			
			glPushMatrix();
			if(rtcfg->getFloat("oldfademenu")!=0)	glTranslatef(rtcfg->getFloat("oldfademenu"),0,0);
			else 									glTranslatef(rtcfg->getFloat("fademenu"),0,0);

			if(z2+Ics()!=Ics())
			{
				if(z2>=1 && z2<=thcfg->sub("ints")->getInt("menulistmax"))
				{
					showmaxid=thcfg->sub("objects")->get("menumax1")->id+z2-1;

					glTranslatef
					(
						-icwidth *thcfg->sub("objects")->sub(showmaxid)->getInt("posx"),
						-icheight*thcfg->sub("objects")->sub(showmaxid)->getInt("posy"),
						thcfg->sub("objects")->sub(showmaxid)->getInt("posz")
					);

					if (rtcfg->getInt("menualpha")==255) 	textcol(thcfg->sub("objects")->getKey(showmaxid),&font,0);
					else									textcol(thcfg->sub("objects")->getKey(showmaxid),&font,rtcfg->getInt("menualpha"));
				}
				drawmenutext(mentry(Ishowmenu()-1,Ics()+z2),showmaxid);
			}
			glPopMatrix();
		}
	glPopMatrix();
}

void drawrectbox()
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	setpos("rectbox");
	rotateme("rectbox");
	texcol("rectbox");
	glLineWidth (thcfg->sub("ints")->getInt("rectboxlinewidth"));

	glPolygonMode (GL_FRONT, GL_LINE);
	glPolygonMode (GL_BACK, GL_LINE);
	
	glRecti(rtcfg->sub("objects")->sub("rectbox")->getFloat("width"), rtcfg->sub("objects")->sub("rectbox")->getFloat("height"), 0, 0); 
	
	glPolygonMode (GL_FRONT, GL_FILL);
	glPolygonMode (GL_BACK, GL_FILL);

	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void clocktimer()
{
	char monthbuf[2];

	time_t timer;
	struct tm* date;

	timer = time(0);
	date = localtime(&timer);

// for milliseconds
	struct timeval tv;
	gettimeofday(&tv, NULL);
	timer=tv.tv_sec;
	
	if(thcfg->sub("objects")->sub("clock")->getBool("enabled"))
	{
		char clockbuf[30];
		switch(thcfg->sub("ints")->getInt("clockformat"))
		{
			case 0:	{strftime(clockbuf,30,"%T",date);break;}
			case 1:	{
						 char msbuf[30];
						 sprintf(msbuf, "%ld", tv.tv_usec);
						 strftime(clockbuf,30,"%T",date);
						 strcat(clockbuf, ".");
						 strcat(clockbuf, msbuf);break;
					}
			case 2:	{strftime(clockbuf,30,"%I:%M %p",date);break;}
			case 3:	{strftime(clockbuf,30,"%I:%M:%S %p",date);break;}	
		default: break;
		break;	
		}
		rtcfg->set("clock",clockbuf);
	}
	
	if(thcfg->sub("objects")->sub("date")->getBool("enabled") && !rtcfg->getBool("havethedate"))
	{
		char datebuf[30];
		switch(thcfg->sub("ints")->getInt("dateformat"))
		{
			case 0:	{strftime(datebuf,40,"%d.%m.%Y",date);break;}
			case 1:	{strftime(datebuf,40,"%d.%m",date);break;}
			case 2:	{strftime(datebuf,40,"%d/%m/%Y",date);break;}
			case 3:	{strftime(datebuf,40,"%d/%m",date);break;}
			case 4:	{strftime(datebuf,40,"%A, %d.%m.%Y",date);break;}
			case 5:	{strftime(datebuf,40,"%A, %d. %B",date);break;}	
			case 6:	{strftime(datebuf,40,"%A, %d. %B %Y",date);break;}	
			case 7:	{strftime(datebuf,40,"%A, %d.%m, Week %W",date);break;}		
			case 8:	{strftime(datebuf,40,"%A, %d. %B, Week %W",date);break;}
			case 9:	{strftime(datebuf,40,"%A, %d %B %Y, Week %W",date);break;}
			case 10:{strftime(datebuf,40,"%m/%d/%Y",date);break;}
			case 11:{strftime(datebuf,40,"%m/%d",date);break;}
		default: break;
		break;	
		}
		char themeversion[20];
		strftime(themeversion,20,"%y.%m",date);			
		rtcfg->set("themeversion",themeversion);
		rtcfg->setBool("havethedate",1);
		rtcfg->set("date",datebuf);
	}

	if((thcfg->sub("switches")->getBool("usemonthcolors") && rtcfg->getInt("monthcolor")==0) || rtcfg->getBool("wantshadercolor"))
	{
		strftime(monthbuf,30,"%m",date);
		rtcfg->set("monthcolor",atoi(monthbuf)-1); 
	}
}

void drawclock()
{
	glPushMatrix();
	textcol("clock",&clockfontfx,0);
	setpos("clock");
	textrot("clock",rtcfg->getString("clock"),&clockfontfx,0);
	clockfontfx.draw(0.0,0.0, rtcfg->getString("clock"),thcfg->sub("objects")->sub("clock")->getFloat("depth"));
	glPopMatrix();
}

void drawdate()
{
	glPushMatrix();
	textcol("date",&datefontfx,0);
	setpos("date");
	textrot("date",rtcfg->getString("date"),&datefontfx,0);
	datefontfx.draw(0.0,0.0, rtcfg->getString("date"),thcfg->sub("objects")->sub("date")->getFloat("depth"));
	glPopMatrix();
}

void drawtitles()
{
	glPushMatrix();

	setpos("title");
	
// normal menu listing

	if (Ishowmenu()>=0 && Iem()==0){drawtitletext(title(Ishowmenu()-1));}

// emulator listing

	if (Ishowmenu()>=1 && Iem()!=0)
	{
		if (thcfg->sub("switches")->getBool("useemutitles"))	{drawtitletext(emutitle(IeID()-1));}
		else  													{drawtitletext(emulator(IeID()-1));}
	}
	glPopMatrix();
}


void drawromdesc()
{
	if (Iem()!=0 &&(thcfg->sub("objects")->sub("description")->getBool("enabled") || pick()==4))
	{
		if (thcfg->sub("objects")->sub("rombox")->getBool("enabled"))
		{
			glPushMatrix();
			setpos("rombox");
			texcol("rombox");
			sf::Texture::bind(getTexture(rtcfg->sub("textureids")->sub("settings")->id)); 
			cuboquad("rombox","rombox");
			glPopMatrix();
		}
		int gameid=rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getInt("gameid");
		std::string thistext=" ";

		if (gameid<=prcfg->sub("games")->size())
		{
			if(prcfg->sub("games")->sub(gameid)->exists("description"))
			thistext=prcfg->sub("games")->sub(gameid)->getString("description");
		}
		glPushMatrix();
		textcol("description",&rominfofontfx,0);
		setpos("description");
	//	textrot("description",thistext,&rominfofontfx,3);
		rtcfg->sub("objects")->sub("description")->set("width",rominfofontfx.getWidth(thistext));

		if(rtcfg->sub("objects")->sub("description")->getFloat("width")>rtcfg->sub("objects")->sub("rombox")->getFloat("width"))
		{
			int N = thistext.length();
			int M = lrint((rtcfg->sub("objects")->sub("description")->getFloat("width")+rominfofontfx.getWidth("WWWW"))/rtcfg->sub("objects")->sub("rombox")->getFloat("width"))+1;

			rtcfg->set("romboxlinespace",-(showH(thcfg->sub("objects")->sub("rombox")->getInt("height")/M)));

			int at, pre=0, i;
			for (pre = i = 0; i < M; ++i)
			{
				at = (N+N*i)/M;
				size_t pos=thistext.substr(pre, at-pre).find_last_of(" ")+1;
				if(i!=M-1)at=pre+pos;
				textrot("description",thistext.substr(pre, at-pre),&rominfofontfx,3);

				rominfofontfx.draw(0.0,0.0, thistext.substr(pre, at-pre),thcfg->sub("objects")->sub("description")->getFloat("depth"));
				glTranslatef(0, -rtcfg->getFloat("romboxlinespace"), 0);
				pre = at;
			}

		}
		else
		rominfofontfx.draw(0.0,0.0, thistext,thcfg->sub("objects")->sub("description")->getFloat("depth"));
		glPopMatrix();
	}
}

void drawrominfo(std::string info)
{
	if (Iem()!=0 &&(thcfg->sub("objects")->sub(info)->getBool("enabled") || pick()==4))
	{
		std::string thistext;
		if(compare(info,"romtitle"))thistext=prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getInt("gameid"))->getString("title");
		else 						thistext=info+":"+prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(IeID()-1)->sub(Ics())->getInt("gameid"))->getString(info);
		glPushMatrix();
		textcol(info,&rominfofontfx,0);
		setpos(info);
		textrot(info,thistext,&rominfofontfx,3);
		rominfofontfx.draw(0.0,0.0, thistext,thcfg->sub("objects")->sub(info)->getFloat("depth"));
		glPopMatrix();
	}
}

void displayfonts()
{
	drawmenuentries();
	drawmusicduration();
	drawmusictitle();
	if (thcfg->sub("objects")->sub("title")->getBool("enabled") || pick()==4)						{drawtitles();}
	if (thcfg->sub("objects")->sub("clock")->getBool("enabled") || pick()==4)						{drawclock();}
	if (thcfg->sub("objects")->sub("date")->getBool("enabled") || pick()==4)						{drawdate();}
	if (thcfg->sub("objects")->sub("myip")->getBool("enabled") || pick()==4)						{drawsys("myip",rtcfg->getString("ip"));}
	if (thcfg->sub("objects")->sub("uptime")->getBool("enabled") || pick()==4)						{drawsys("uptime",rtcfg->getString("uptime"));}
	if (thcfg->sub("objects")->sub("totalmem")->getBool("enabled") || pick()==4)					{drawsys("totalmem",rtcfg->sub("memusage")->getString("total"));}
	if (thcfg->sub("objects")->sub("freemem")->getBool("enabled") || pick()==4)						{drawsys("freemem",rtcfg->sub("memusage")->getString("free"));}
	if (thcfg->sub("objects")->sub("disk1")->getBool("enabled") || pick()==4)						{drawsys("disk1",rtcfg->sub("diskusage")->getString("disk1"));}
	if (thcfg->sub("objects")->sub("disk2")->getBool("enabled") || pick()==4)						{drawsys("disk2",rtcfg->sub("diskusage")->getString("disk2"));}
	if (thcfg->sub("objects")->sub("disk3")->getBool("enabled") || pick()==4)						{drawsys("disk3",rtcfg->sub("diskusage")->getString("disk3"));}
	if (thcfg->sub("objects")->sub("cpu")->getBool("enabled") || pick()==4)							{drawsys("cpu",rtcfg->sub("cpuusage")->getString("cpu"));}
	if ((thcfg->sub("objects")->sub("notifier")->getBool("enabled") && rtcfg->getBool("shownotify")) || pick()==4)	{drawnotify();}

	if(prcfg->sub("games")->size()>0)
	{
		drawrominfo("coop");
		drawromdesc();
		drawrominfo("developer");
		drawrominfo("esrb");
		drawrominfo("platform");
		drawrominfo("publisher");
		drawrominfo("reldate");
		drawrominfo("romtitle");

	}
	if(rtcfg->getBool("showhelpscreen"))															{drawhelpscreen();}
	if (rtcfg->getBool("editmode") || rtcfg->getBool("searchmode"))									{drawsearcheditbox();}	
}

void domenucurve()
{
	for(int z = 1; z <  thcfg->sub("ints")->getInt("menulistmax"); z++)
	{thcfg->sub("objects")->sub(thcfg->sub("objects")->get("menumax1")->id+z)->set("posx",thcfg->sub("ints")->getInt("menucurve")*z);}

	for(int z = 1; z <  thcfg->sub("ints")->getInt("menulistmin"); z++)
	{thcfg->sub("objects")->sub(thcfg->sub("objects")->get("menumin1")->id+z)->set("posx",-thcfg->sub("ints")->getInt("menucurve")*z);}
}

void addtofavourites()
{	
	if (Iem()==0 && !Bvidmenu() && !Bp3tmenu() && !Bthememenu())
	{
		TyphonConfig* tshort=mecfg->sub(6)->sub("entry")->subCa("prog");
		TyphonConfig* dat=mecfg->sub(Ishowmenu()-1)->sub("entry")->sub(Ics());

		tshort->set("binary",dat->getString("binary"));
		tshort->set("args",dat->getString("args"));
		tshort->set("name",dat->getString("name"));
		tshort->set("icon",dat->getString("icon"));
	}
}

void addentry(int menu, std::string entry,int here,std::string args)
{	
	printf("vor addentry here %i size %i\n",here,mecfg->sub(menu)->sub("entry")->size());

	mecfg->sub(menu)->sub("entry")->insertSubCa("prog",here);

	printf("TODO - size %i here it %i\n",mecfg->sub(menu)->sub("entry")->size(),here);
	mecfg->saveXML("/tmp/crashytyphon-menu.xml");

	mecfg->sub(menu)->sub("entry")->sub(here)->set("binary",entry);
	mecfg->sub(menu)->sub("entry")->sub(here)->set("args",args);
}

void delentry(int menu, int here)
{
//	printf("vor delentry here %i size %i\n",here,mecfg->sub(menu)->sub("entry")->size());
	if(mecfg->sub(menu)->sub("entry")->size()<2) {}
	else if(here>=mecfg->sub(menu)->sub("entry")->size()-1)
	{
		dokeyup();
		//printf("erase %i von %i\n",here,mecfg->sub(menu)->sub("entry")->size());
		mecfg->sub(menu)->sub("entry")->erase(here);
//		printf("nach erase here %i von %i\n",here,mecfg->sub(menu)->sub("entry")->size());
	}
	else 
	{mecfg->sub(menu)->sub("entry")->erase(here);}
}

void moveentry(int to)
{
	std::string binary=mentry(Ishowmenu()-1,Ics());
	std::string args=margs(Ishowmenu()-1,Ics());
printf("menu ist %i\n",Ishowmenu()-1);
printf("binary ist %s\n",binary.c_str());

	int newmenu=Ishowmenu()-1+to;
	if (newmenu<0) newmenu=mecfg->size()-1;
	else if (newmenu>mecfg->size()) newmenu=0;
printf("newmenu ist %i\n",newmenu);

	printf("newmenusize %i\n",mecfg->sub(newmenu)->sub("entry")->size());
	addentry(newmenu,binary,mecfg->sub(newmenu)->sub("entry")->size()+1,args);
	delentry(Ishowmenu()-1,Ics());
}

void addentries(int menu,std::string path)
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir(path.c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			std::string file=(sdit[j]->d_name);
			addentry(menu,file,j,"");
		}
	}
}

void drawhelpscreen()
{
	std::string helpinfo;
	int max=0;
	int ls=0;
	std::string start=lacfg->getString("press")+" "+incfg->sub("key")->sub("startselection")->getString("key")+" ";
	std::string plusorminus=lacfg->getString("press")+" "+incfg->sub("key")->sub("plus")->getString("key")+" "+lacfg->getString("or")+" "+incfg->sub("key")->sub("minus")->getString("key")+" ";
	std::string leftorright=lacfg->getString("press")+" "+incfg->sub("key")->sub("left")->getString("key")+" "+lacfg->getString("or")+" "+incfg->sub("key")->sub("right")->getString("key");
	
	switch(rtcfg->getInt("showhelp"))
	{
		case 0:	
		{
			helpinfo=start+lacfg->getString("changemenu");
			max=lacfg->sub("helpscreens")->size();
			ls=max;
			break;
		}
		case 1:	
		{
			helpinfo=start+lacfg->getString("hotkey")+" "+incfg->sub("key")->sub(rtcfg->sub("helpsel")->getInt("key"))->getString("desc");
			max=incfg->sub("key")->size();
			ls=(max/3)+1;
			rtcfg->sub("helplist")->set("left",max/3);
			rtcfg->sub("helplist")->set("center",max*2/3-max/3);
			break;
		}
		case 2:	
		{
			helpinfo=start+lacfg->getString("mapbutton")+" "+incfg->sub("joypad")->getKey(rtcfg->sub("helpsel")->getInt("joypad"));
			max=incfg->sub("joypad")->size();
			ls=max;
			break;
		}
		case 3:	
		{
			helpinfo=start+lacfg->getString("edpath");
			max=thcfg->sub("paths")->size();
			ls=max;
			break;
		}
		case 4:	
		{
			helpinfo=start+lacfg->getString("togswitch")+" "+thcfg->sub("switches")->getKey(rtcfg->sub("helpsel")->getInt("switches"));
			max=thcfg->sub("switches")->size();
			ls=max;
			break;
		}
		case 5:	
		{
			helpinfo=start+lacfg->getString("togswitch")+" "+tycfg->sub("globalswitches")->getKey(rtcfg->sub("helpsel")->getInt("globalswitches"));
			max=tycfg->sub("globalswitches")->size();
			ls=max;
			break;
		}
		case 6:
		{
			helpinfo=start+lacfg->getString("edemu")+" - "+leftorright+" "+lacfg->getString("changeemu");
			max=emcfg->sub(rtcfg->getInt("edemu"))->size();
			ls=max;
			break;
		}	
		case 7:	
		{
			helpinfo=plusorminus+lacfg->getString("value")+" "+thcfg->sub("ints")->getKey(rtcfg->sub("helpsel")->getInt("thint"));

			max=thcfg->sub("ints")->size();
			ls=(max/3)+1;
			rtcfg->sub("thintlist")->set("left",max/3);
			rtcfg->sub("thintlist")->set("center",max*2/3-max/3);
			break;
		}
		case 8:
		{
			helpinfo=plusorminus+lacfg->getString("value")+" "+tycfg->sub("globalints")->getKey(rtcfg->sub("helpsel")->getInt("globalints"));

			max=tycfg->sub("globalints")->size();
			ls=max;
			break;
		}	
		case 9:
		{
			helpinfo=plusorminus+lacfg->getString("value")+" "+thcfg->sub("floats")->getKey(rtcfg->sub("helpsel")->getInt("thfloat"));
			max=thcfg->sub("floats")->size();
			ls=max;
			break;
		}		
		case 10:
		{
			helpinfo=leftorright+" "+lacfg->getString("changedata");
			max=thcfg->sub("files")->size();
			ls=max;
			break;
		}
		case 11:	
		{
			helpinfo=start+lacfg->getString("stringedit");
			max=tycfg->sub("strings")->size();
			ls=max;
			break;
		}
		case 12:	
		{
			helpinfo=start+lacfg->getString("edpath");
			max=tycfg->sub("paths")->size();
			ls=max;
			break;
		}
		case 13:	
		{
			helpinfo=start+lacfg->getString("online");
			max=oncfg->sub("themes")->size();
			ls=max;
			if(max<1){rtcfg->setBool("showhelpscreen",0);sethelp(0);}
			break;
		}
		case 14:
		{
			helpinfo=thcfg->sub("objects")->getKey(rtcfg->getInt("edobject"))+" - "+leftorright+" "+lacfg->getString("changeobject")+" - "+plusorminus+lacfg->getString("value")+" "+rtcfg->sub("object")->getKey(rtcfg->sub("helpsel")->getInt("objects"));
			max=rtcfg->sub("object")->size();
			ls=max;
			break;
		}
		default: 
		{
			helpinfo="empty";
			break;
		}
	}
	
	glDisable(GL_DEPTH_TEST);
	float AAA=settingsfontfx.getWidth("AAA");

	glPushMatrix();
	setpos("setbox");
	texcol("setbox");
	sf::Texture::bind(getTexture(rtcfg->sub("textureids")->sub("settings")->id)); 

	cuboquad("setbox","settings");
	glPopMatrix();

	rtcfg->set("helpboxlinespace",(int)-(showH(thcfg->sub("objects")->sub("setbox")->getInt("height")/(ls+3))));

	TyphonConfig* sb=rtcfg->sub("objects")->sub("setbox");
	TyphonConfig* sz=thcfg->sub("objects")->sub("setbox");
	TyphonConfig* co=thcfg->sub("objects")->sub("menu")->sub("menuselfont");

	glPushMatrix();
	glTranslatef
	(
		sb->getFloat("posx")+AAA,
		sb->getFloat("posy")-rtcfg->getInt("helpboxlinespace"),
		sz->getInt("posz")
	);

	font.setColour(veccol4f(co->getInt("R"),co->getInt("G"),co->getInt("B"),255));

	drawgenerictext(helpinfo,0.0);
	glPopMatrix();

	glPushMatrix();

	glTranslatef
	(
		sb->getFloat("posx")+AAA,
		sb->getFloat("posy")-rtcfg->getInt("helpboxlinespace"),
		sz->getInt("posz")
	);

	for (int k=0; k<max; k++)
	{
		std::string helptext;
		bool doublemap=0;
		switch(rtcfg->getInt("showhelp"))
		{
			case 0:	{helptext=lacfg->sub("helpscreens")->getString(k);	break;}
			case 1:	
			{
				if(k!=rtcfg->sub("helpsel")->getInt(rtcfg->getInt("showhelp")))
				{
					if (strcmp(incfg->sub("key")->sub(k)->getString("key").c_str(), incfg->sub("key")->sub(rtcfg->sub("helpsel")->getInt(rtcfg->getInt("showhelp")))->getString("key").c_str()) == 0)
					doublemap=1;
				}
				
				if(k==rtcfg->sub("helplist")->getInt("left")) 
				glTranslatef(rtcfg->sub("objects")->sub("setbox")->getFloat("width")/3,rtcfg->sub("helplist")->getInt("left")*rtcfg->getInt("helpboxlinespace") ,0);

				else if(k==rtcfg->sub("helplist")->getInt("left")+rtcfg->sub("helplist")->getInt("center")) 
				glTranslatef(rtcfg->sub("objects")->sub("setbox")->getFloat("width")/3, rtcfg->sub("helplist")->getInt("center")*rtcfg->getInt("helpboxlinespace"),0);
				
				helptext=incfg->sub("key")->sub(k)->getString("desc")+" : "+incfg->sub("key")->sub(k)->getString("key");
				break;
			}
			case 2:	{helptext=incfg->sub("joypad")->getKey(k)+" : "+itos(incfg->sub("joypad")->getInt(k));	break;}
			case 3:	{helptext=thcfg->sub("paths")->getKey(k)+" : "+thcfg->sub("paths")->getString(k);	break;}
			case 4:	
			{
				std::string booly;
				if(thcfg->sub("switches")->getBool(k)) 	booly="true";
				else 									booly="false";
				helptext=thcfg->sub("switches")->getKey(k)+" : "+booly;
				break;
			}
			case 5:	
			{
				std::string booly;
				if(tycfg->sub("globalswitches")->getBool(k))booly="true";
				else 										booly="false";
				helptext=tycfg->sub("globalswitches")->getKey(k)+" : "+booly;
				break;
			}
			case 6:	{helptext=emcfg->sub(rtcfg->getInt("edemu"))->getKey(k)+" : "+emcfg->sub(rtcfg->getInt("edemu"))->getString(k);	break;}
			case 7:	
			{
				if(k==rtcfg->sub("thintlist")->getInt("left")) 
				glTranslatef(rtcfg->sub("objects")->sub("setbox")->getFloat("width")/3,rtcfg->sub("thintlist")->getInt("left")*rtcfg->getInt("helpboxlinespace") ,0);					

				else if(k==rtcfg->sub("thintlist")->getInt("left")+rtcfg->sub("thintlist")->getInt("center"))
				glTranslatef(rtcfg->sub("objects")->sub("setbox")->getFloat("width")/3, rtcfg->sub("thintlist")->getInt("center")*rtcfg->getInt("helpboxlinespace"),0);

				helptext=thcfg->sub("ints")->getKey(k)+" : "+itos(thcfg->sub("ints")->getInt(k));
				break;
			}
			case 8:	{helptext=tycfg->sub("globalints")->getKey(k)+" : "+itos(tycfg->sub("globalints")->getInt(k));	break;}
			case 9:	{helptext=thcfg->sub("floats")->getKey(k)+" : "+ftos(thcfg->sub("floats")->getFloat(k));	break;}
			case 10:{helptext=thcfg->sub("files")->getKey(k)+" : "+thcfg->sub("files")->getString(k);	break;}
			case 11:{helptext=tycfg->sub("strings")->getKey(k)+" : "+tycfg->sub("strings")->getString(k);	break;}
			case 12:{helptext=tycfg->sub("paths")->getKey(k)+" : "+tycfg->sub("paths")->getString(k);break;}
			case 13:{helptext=getFile(oncfg->sub("themes")->getString(k));	break;}
			case 14:{
						std::string objhelp=rtcfg->sub("object")->getString(k)+" : ";
						TyphonConfig* val =thcfg->sub("objects")->sub(rtcfg->getInt("edobject"));
						std::string keyname=rtcfg->sub("object")->getKey(k);
						if(val->exists(keyname))
						{
							if(k==0 || k==15|| k==16)
							{
								std::string booly;
								if(val->getBool(keyname))	booly="true";
								else 						booly="false";
								helptext=objhelp+booly;
							}
							else if(k==4)
							{helptext=objhelp+ftos(val->getFloat(keyname));}
							else 
							{helptext=objhelp+itos(val->getInt(keyname));}
							break;
						}
						else
						{helptext=objhelp+lacfg->getString("notavailable");	break;}
					}

			default:{helptext="empty";break;}		
		}

		glTranslatef(0, -rtcfg->getInt("helpboxlinespace"), 0);
		glPushMatrix();
		if(k==rtcfg->sub("helpsel")->getInt(rtcfg->getInt("showhelp")))
		font.setColour(veccol4f(rtcfg->getInt("tempselr"),rtcfg->getInt("tempselg"),rtcfg->getInt("tempselb"),255));
		else if (doublemap)
		font.setColour(veccol4f(255,1,1,255));
		else		textcol("title",&font,0);

		drawgenerictext(helptext,0.0);
		glPopMatrix();
	}
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}
