#include "includes.h"
#include "fonthandling.h"
#include "rsshandling.h"
#include "glhandling.h"
#include "windowhandling.h"
#include "filehandling.h"
#include "xmlconfig.h"
#include "networking.h"
#include "values.h"


void scrollrss(int rssspeed)
{
	((TConfig<float>*) rtcfg->sub("objects")->sub("rss")->get("posx"))->operator-(rssspeed);
	if (rtcfg->sub("objects")->sub("rss")->getFloat("posx")<=(-rssfontfx.getWidth(rtcfg->getString("rss"))-rtcfg->getFloat("rssposxorg"))){rtcfg->sub("objects")->sub("rss")->set("posx",rtcfg->getFloat("rssposxorg"));}
}

void drawrssfeed(std::string thistext)
{
	scrollrss(thcfg->sub("objects")->sub("rss")->getInt("speed"));
	
	glPushMatrix();

	glTranslatef
	(
		rtcfg->sub("objects")->sub("rss")->getFloat("posx"),
		rtcfg->sub("objects")->sub("rss")->getFloat("posy"),
		thcfg->sub("objects")->sub("rss")->getInt("posz")
	);

	drawrsstext(thistext);	
	glPopMatrix();
}

// read rss xml

std::string readrssfeed(std::string filename)
{	
	TiXmlDocument doc(filename.c_str());
	if(!doc.LoadFile())
	{
		thcfg->sub("objects")->sub("rss")->set("enabled",0);
		return "empty";
	}
	char* rssout;
	TiXmlElement* pElem;
	TiXmlElement* pChild;
	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);
	int countrss=0;
	pElem = hDoc.FirstChildElement().Element();
	if(!pElem) { printf("no valid rss\n"); return "empty"; }
	hRoot = TiXmlHandle(pElem);
	rssout = (char*) malloc(strlen(" "));
    strcpy(rssout,"\0");
	for(pElem=hRoot.FirstChild("channel").FirstChild("item").Element(); pElem;
  		pElem = pElem->NextSiblingElement())
 	    {
 	    	pChild = hRoot.Child("channel",0).ChildElement("item",countrss).Element();

  			TiXmlElement* pcur = pChild->FirstChildElement("title");
  			if(pcur){
						if(pcur->GetText())
						{
							char *alt = (char*) malloc(strlen(rssout)+1);
							strcpy(alt,rssout);
							char *rssuffix = (char*) pcur->GetText();
							strcat (rssuffix,"--");
							rssout= (char*) realloc(alt,strlen(alt)+strlen(rssuffix)+1);
							strcat(rssout,pcur->GetText());	
						};
  			
					}
					countrss+=1;
		}
		rtcfg->setBool("rssinitialized",1);
//		rsslinelength=(int) strlen(rssout)*thcfg->sub("objects")->sub("rss")->getInt("size");
		return rssout;
}

void loadrss()
{
	if (thcfg->sub("objects")->sub("rss")->getBool("enabled"))	
	{
		rtcfg->set("rssposxorg",rtcfg->sub("objects")->sub("rss")->getFloat("posx"));
		mkdir(tycfg->sub("tempdirs")->getString("import").c_str(), 0755);
		rtcfg->set("rssfeed",tycfg->sub("tempdirs")->getString("import")+"/"+"rssfeed.xml");
		FILE *rssxmlcheck = fopen(rtcfg->getString("rssfeed").c_str(),"r");

		if(rssxmlcheck)	
		{fclose (rssxmlcheck);}
		else			
		{downloadurito(tycfg->sub("strings")->getString("rssfeed"), rtcfg->getString("rssfeed"));}
		rtcfg->set("rss",readrssfeed(rtcfg->getString("rssfeed").c_str()));
	}
}
