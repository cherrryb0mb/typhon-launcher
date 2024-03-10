#include "includes.h"
#include "networking.h"
#include "xmlconfig.h"
#include "fonthandling.h"

std::string getHost(std::string str)
{
	size_t pos=str.find_first_of("/");
	return str.substr(0,pos);
}

std::string getUri(std::string str)
{
	size_t pos=str.find_first_of("/");
	return str.substr(pos);
}

std::string noVersion(std::string str)
{
	if (compare(str.c_str(),"_v"))
	{
		size_t pos=str.find_first_of("_v");
		return str.substr(0,pos);
	}
	else return str;
}

std::string noRomInfo(std::string str)
{
	if (compare(str.c_str(),"("))
	{
		size_t pos=str.find_first_of("(");
		return str.substr(0,pos);
	}
	else return str;
}

std::string splitCamel(std::string str)
{
	std::string result;
	int i=0;
	char U,L;
	while (str[i])
	{
		U=str[i];
		L=str[i-1];
		if (isupper(U) && islower(L))
		result+=" ";

		result+=str[i];
		i++;
	}
	return result;
}

std::string splitDigit(std::string str)
{
	std::string result;
	int i=0;
	char U,L;
	while (str[i])
	{
		U=str[i];
		L=str[i-1];
		if (isdigit(U) && islower(L))
		result+=" ";

		result+=str[i];
		i++;
	}
	return result;
}

void downloadurito(std::string src, std::string dest)
{
	sf::Http Http;
	Http.setHost(cutstring(getHost(src),"http://"));
	sf::Http::Request Request;
	Request.setMethod(sf::Http::Request::Get);
	Request.setUri(getUri(src));
	sf::Http::Response Page = Http.sendRequest(Request);
	int filesize=0;
	filesize = Page.getBody().size();

	std::string sFileContainer = Page.getBody();

	const char * cs = sFileContainer.c_str ();

	std::ofstream toFile(dest.c_str(), std::ios::out | std::ios::binary);
	for (int i = 0; i<filesize; i++)
	{toFile.put(cs[i]);}
	toFile.close();
}

void getrominfo(int e,int r,int m)
{
	//if (Iem()!=0)
	{
		bool alreadyhere=0;
		rtcfg->set("currententry",rmcfg->sub("Roms")->sub(e)->sub(r)->getString("file"));
		std::string rominfo=rtcfg->getString("rominfodir")+"/"+itos(e);
		mkdir(rominfo.c_str(), 0755);
		rominfo+="/"+getPrefix(rtcfg->getString("currententry"))+".xml";

		FILE *check = fopen(rominfo.c_str(),"r");
		if(check)	{alreadyhere=1;fclose (check);}
		else
		{
			if (tycfg->sub("globalswitches")->getBool("useinternet"))
			{
				std::string	search=noVersion(rtcfg->getString("currententry")).c_str();	// cut version
							search=noRomInfo(search).c_str();	// cut rominfo
							search=getPrefix(search).c_str();	// cut suffix
							search=splitCamel(search).c_str();	// split camelCase
							search=splitDigit(search).c_str();	// split digit

				for (int i=0; i<rmcfg->sub("striptitle")->size(); i++)
				{search=cutstring(search.c_str(),rmcfg->sub("striptitle")->getString(i));}

				downloadurito(tycfg->sub("strings")->getString("scraperhost")+"/"+tycfg->sub("strings")->getString("scraperuri")+"?what="+search,rominfo);
				prcfg->wipe();
				prcfg->loadXML(rominfo);
				if(!prcfg->sub("games")->exists(0))		remove(rominfo.c_str());
			}
		}

		prcfg->wipe();
		prcfg->loadXML(rominfo);
		if(m==1)	downloadrompreview(e,r);
		if(!alreadyhere && prcfg->sub("games")->exists(0))	{if(prcfg->sub("games")->sub(0)->exists("title"))prcfg->saveXML(rominfo);}
	}
}

void getproginfo()
{
	if (tycfg->sub("globalswitches")->getBool("useinternet"))
	{
		mkdir(tycfg->sub("tempdirs")->getString("import").c_str(), 0755);
		std::string	search=mentry(Ishowmenu()-1,Ics()); // TODO mentry
		std::string temp=tycfg->sub("tempdirs")->getString("import")+"/"+search+".xml";
		downloadurito(tycfg->sub("strings")->getString("scraperhost")+"/"+tycfg->sub("strings")->getString("scraperuri")+"?what="+search,temp);

		prcfg->wipe();
		prcfg->loadXML(temp);
		if (prcfg->sub("games")->size()>0)
		{
			std::string pic=cutstring(prcfg->sub("games")->sub(0)->sub("images")->sub("boxart")->getString(0),"http://");
			if(compare(pic,"back")) pic=cutstring(prcfg->sub("games")->sub(0)->sub("images")->sub("boxart")->getString(1),"http://");
			downloadurito(pic,rtcfg->getString("currentprevpath")+search+".jpg");
		}
		removerec(tycfg->sub("tempdirs")->getString("import"));
	}
}

void scraperoms(int e,int m)
{
	if(rmcfg->sub("Roms")->exists(e))
	{
		for (int i=0; i<rmcfg->sub("Roms")->sub(e)->size(); i++)
		getrominfo(e,i,m);
	}
}

std::string boxart(int e,int r,int num)
{
	return prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(e)->sub(r)->getInt("gameid"))->sub("images")->sub("boxart")->getString(num);
}

void downortextprev(int e,int r)
{
	std::string romprevs=rtcfg->getString("rominfodir")+"/boxart/";
	std::string front=romprevs+"front/";
	std::string back=romprevs+"back/";
	mkdir(romprevs.c_str(), 0755);
	mkdir(front.c_str(), 0755);
	mkdir(back.c_str(), 0755);

	std::string rominfo=rtcfg->getString("rominfodir")+"/"+itos(e)+"/"+getPrefix(rtcfg->getString("currententry"))+".xml";

	for ( int i = 0; i < prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(e)->sub(r)->getInt("gameid"))->sub("images")->sub("boxart")->size(); i++)
	{

		if(compare(boxart(e,r,i),"http://"))
		{
						printf("http check %s\n",boxart(e,r,i).c_str());

			if (tycfg->sub("globalswitches")->getBool("useinternet"))
			{
				std::string file;
				if(compare(boxart(e,r,i),"front"))	
				{
					file=front+getFile(boxart(e,r,i));
					FILE *check = fopen(file.c_str(),"r");
					if(check)	fclose(check);
					else 		downloadurito(cutstring(boxart(e,r,i),"http://"),file);
					rtcfg->set("currentprev",file);
				}
				if(compare(boxart(e,r,i),"back"))
				{
					file=back+getFile(boxart(e,r,i));
					FILE *check = fopen(file.c_str(),"r");
					if(check)	fclose(check);
					else 		downloadurito(cutstring(boxart(e,r,i),"http://"),file);
				}

				rtcfg->set("currentprevback",file);
				TyphonConfig* ba=prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(e)->sub(r)->getInt("gameid"))->sub("images")->sub("boxart");
				ba->setC(ba->getKey(i),file);
				if(prcfg->sub("games")->exists(0))	{if(prcfg->sub("games")->sub(0)->exists("title"))prcfg->saveXML(rominfo);}
			}
			else
			{rtcfg->set("currentprev",fallbackmedia(rtcfg->getString("currentprev"),2));}
		}
		else
		{
			if(compare(boxart(e,r,i),"front"))	
			{rtcfg->set("currentprev",boxart(e,r,i));}

			else if(compare(boxart(e,r,i),"back") && thcfg->sub("objects")->sub("preview")->getBool("back"))
			{rtcfg->set("currentprevback",boxart(e,r,i));}
			else
			{rtcfg->set("currentprevback",rtcfg->getString("currentprev"));}
		}
	}
}

void downloadrompreview(int e,int r)
{
#ifdef DEBUG
	printf("found %i game entries\n",prcfg->sub("games")->size());
#endif // DEBUG

	if (prcfg->sub("games")->size()>0)
	{
		if(rmcfg->sub("Roms")->sub(e)->sub(r)->getInt("gameid")>prcfg->sub("games")->size()-1)
		rmcfg->sub("Roms")->sub(e)->sub(r)->set("gameid",0);
		
		if (prcfg->sub("games")->sub(rmcfg->sub("Roms")->sub(e)->sub(r)->getInt("gameid"))->sub("images")->sub("boxart")->size()>0)
		{downortextprev(e,r);}
	}
	else
	{
		rtcfg->set("currentprev",fallbackmedia(rtcfg->getString("currentprev"),2));
	}	
}
