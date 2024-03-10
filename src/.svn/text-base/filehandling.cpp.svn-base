#include "includes.h"
#include "filehandling.h"
#include "picturehandling.h"
#include "themehandling.h"
#include "systeminfo.h"
#include "xmlconfig.h"
#include "confighandling.h"


using namespace std;

bool fExists(std::string file)
{
	FILE *check = fopen(file.c_str(),"r");
	if(check)	{fclose(check);return true;}
	else 		return false;
}

std::string cutstring(std::string str,std::string snip)
{
	if (compare(str.c_str(),snip))
	{
		size_t pos=str.find_first_of(snip);
		return str.substr(pos+snip.length());
	}
	else return str;
}

bool grep(std::string filename, std::string keyword)
{
	string searchline;
	fstream filecheck;

	filecheck.open(filename.c_str(), ios::in);

	while (getline(filecheck,searchline))
	{
		if (!filecheck) break;
		if (searchline.find(keyword, 0) != string::npos)
		return 1;
	}
	filecheck.close();
	return 0;
}

int fileselectAll(const struct dirent *entry) {
	if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
	return (false);
	else 
	return true;
}

// based on http://stackoverflow.com/questions/10195343/copy-a-file-in-an-sane-safe-and-efficient-way -> COPY-ALGORITHM-C++-WAY

void copyfromto(std::string abssrc,std::string desti)
{
	std::string destination=desti;
//	int status;
	struct stat st_buf;
   // status = 
    stat (desti.c_str(), &st_buf);

	if (S_ISDIR (st_buf.st_mode))
	{destination=desti+"/"+getFile(abssrc);}

	ifstream source(abssrc.c_str(), ios::binary);
	ofstream dest(destination.c_str(), ios::binary);

	istreambuf_iterator<char> begin_source(source);
	istreambuf_iterator<char> end_source;
	ostreambuf_iterator<char> begin_dest(dest); 
	copy(begin_source, end_source, begin_dest);

	source.close();
	dest.close();
}

void copyrecfromto(std::string orgdatadir,std::string fexportdir)
{
	struct dirent **sdit;
	int num_entries;

	if ((num_entries = scandir((orgdatadir).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			struct stat st_buf;
			std::string file=orgdatadir+"/"+sdit[j]->d_name;
			stat (file.c_str(), &st_buf);

			if (!S_ISDIR (st_buf.st_mode))
			{copyfromto(file,fexportdir);}
			else
			{
				std::string newdest=fexportdir+"/"+getFile(file)+"/";
				mkdir(newdest.c_str(), 0755);
				copyrecfromto(file,newdest);
			}
		}
	}
}

std::string getSuffix(std::string str)
{
	size_t pos=str.find_last_of(".");
	return str.substr(pos);
}

std::string getPrefix(std::string str)
{
	size_t pos=str.find_last_of(".");
	return str.substr(0,pos);
}

std::string getExec(std::string str)
{
	size_t pos=str.find_first_of(" ");
	return str.substr(0,pos);
}

std::string getImportname(std::string str)
{
	size_t pos=str.find_last_of("-v");
	return str.substr(0,pos-1);
}

std::string getFile(std::string abspath)
{
	size_t pos=abspath.find_last_of("/");
	return abspath.substr(pos+1);
}

std::string getAbsPath(std::string str)
{
	size_t pos=str.find_last_of("/");
	return str.substr(0,pos);
}

std::string setEnvhome(std::string str)
{
	int pos=str.find("~");
	if (pos!=-1)	return getenv("HOME")+str.substr(pos+1);
	else 			return str;
}

// delete files

int clear(const char *file, const struct stat *sb, int flag) {remove(file);return 0;}
void removerec(std::string thisdir) {ftw(thisdir.c_str(), clear,6);}

//	the getenv PATH check is based on 
//	http://h21007.www2.hp.com/portal/site/dspp/menuitem.863c3e4cbcdc3f3515b49c108973a801?ciid=88086d6e1de021106d6e1de02110275d6e10RCRD#one

bool expathcheck(const char *exe)
{
	char candidate[PATH_MAX+1];
	char *path, *dir;
	path = getenv("PATH");
	if (path != NULL)
	{
		path = strdup(path);
		for (dir = strtok(path, ":"); dir != NULL; dir = strtok(NULL, ":"))
		{
			strcpy(candidate, dir);
			strcat(candidate, "/");
			strcat(candidate, exe);
			if (access(candidate, X_OK) == 0)
			{
				free(path);
				return 0;
			}
		}
		free(path);
	}
	return 1;
}

void systemstart(std::string prog, std::string args,bool wait)
{
	std::string fullstart=prog+" "+args;
	if (!wait) fullstart+=" &";

	int start = 0;
	start=system(fullstart.c_str());	// maybe replace system with execvp one day
	if (start == -1) {printf("WARNING - can't start %s\n",fullstart.c_str()); }	
}

std::string switchmediasuffix(std::string thismedia,TyphonConfig *suflist)
{
	FILE *prevcheck = fopen(thismedia.c_str(),"r");
	if(prevcheck)	
	{rtcfg->setBool("foundprev",1);fclose (prevcheck);return thismedia;} 
	else
	{
		for (int i=0;i<suflist->size();i++)
		{

			std::string newsuf=suflist->getString(i);
			std::string newmedia=getPrefix(thismedia)+newsuf;

			FILE *newcheck = fopen(newmedia.c_str(),"r");
			if(newcheck)
			{
				thismedia=newmedia;
				fclose(newcheck);
				rtcfg->setBool("foundprev",1);
				return thismedia;
			}
		}
	}
	return thismedia;
}

// type is either 1 for icon or 2 for preview or 3 for video

std::string fallbackmedia(std::string thismedia,int type)
{
	FILE *prevcheck = fopen(thismedia.c_str(),"r");
	if(prevcheck)	{fclose(prevcheck);}
	else
	{
		if(type==1)
		{thismedia=rtcfg->sub("scanned")->getString("menuiconpath")+"/"+thcfg->sub("textures")->getString("icon");}
		
		if(type==2 || type==3)
		{thismedia=rtcfg->sub("scanned")->getString("previewpath")+"/"+gTex("preview");}

		FILE *prevcheck = fopen(thismedia.c_str(),"r");
		if(prevcheck)	{fclose(prevcheck);}
		else
		{	
			if(type==2 || type==3)
			{thismedia=rtcfg->sub("scanned")->getString("previewpath")+"/"+gTex("icon");}
			FILE *prevcheck = fopen(thismedia.c_str(),"r");
			if(prevcheck)	{fclose(prevcheck);}
			else
			{
				thismedia=rtcfg->getString("datadir")+"/";
				if(type==1)				thismedia+=rtcfg->getString("icondefault");
				if(type==2 || type==3)	thismedia+=rtcfg->getString("previewdefault");
			}
		}
	}
	return thismedia;
}

bool compare(std::string check1,std::string check2)
{
// strcasestr not in standard - therefore this function to not spread it over the whole code
	return (strcasestr(check1.c_str(),check2.c_str()));
}

void togentryname(bool off)
{
	if(off)
	{
		rtcfg->setBool("tempentrynames",tycfg->sub("globalswitches")->getBool("entrynames"));
		tycfg->sub("globalswitches")->setBool("entrynames",0);	
	}
	else
	{
		tycfg->sub("globalswitches")->setBool("entrynames",rtcfg->getBool("tempentrynames"));
	}
}
