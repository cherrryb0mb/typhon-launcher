#include "includes.h"
#include "systeminfo.h"
#include "values.h"
#include "timerfunc.h"
#include "windowhandling.h"
#include "glhandling.h"
#include "xmlconfig.h"

double ti, tf;
int i1,i2,i3,i4,i5,i6;

// get systeminfo functions

void getip()
{
	struct ifreq ifa;
	struct sockaddr_in *i;
	int fd;
	strcpy (ifa.ifr_name, (char*)tycfg->sub("strings")->getString("networkdevice").c_str());
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
	perror ("socket");
	exit (EXIT_FAILURE);
	}

	if(ioctl(fd, SIOCGIFADDR, &ifa))
	{
	perror ("ioctl");
	exit (EXIT_FAILURE);
	}

	i = (struct sockaddr_in*)&ifa.ifr_addr;
	
	char *ip = NULL;
	if ((ip = inet_ntoa (i->sin_addr)) != NULL)
	{
		rtcfg->set("ip",tycfg->sub("strings")->getString("networkdevice")+" - "+ip);
	}
}

long gettickcount()
{
	struct sysinfo si;
	if(sysinfo(&si) == 0) return si.uptime;
	else return -1L;
}

void getfreeram()
{
	struct sysinfo meminfo;
	sysinfo(&meminfo);
	rtcfg->sub("memusage")->set("free",lacfg->getString("freemem")+" "+itos(meminfo.mem_unit * meminfo.freeram/1024/1024)+" MB");
	rtcfg->sub("memusage")->set("total",lacfg->getString("totalmem")+" "+itos(meminfo.mem_unit * meminfo.totalram/1024/1024)+" MB");
}

void getuptime()
{
	int updays, uphours, upminutes;
	struct sysinfo info;
//	struct tm *current_time;
	time_t current_secs;

	time(&current_secs);
//	current_time = 
	localtime(&current_secs);

	sysinfo(&info);

	updays = (int) info.uptime / (60*60*24);
	upminutes = (int) info.uptime / 60;
	uphours = (upminutes / 60) % 24;
	upminutes %= 60;
	if (updays)			{rtcfg->set("uptime",lacfg->getString("uptime")+" "+itos(updays)+"d "+itos(uphours)+"h "+itos(upminutes)+"m");}
	else if(uphours)	{rtcfg->set("uptime",lacfg->getString("uptime")+" "+itos(uphours)+"h "+itos(upminutes)+"m");}
	else				{rtcfg->set("uptime",lacfg->getString("uptime")+" "+itos(upminutes)+"m");}
}

double time_so_far()
{
	struct timeval tp;
	if (gettimeofday(&tp, (struct timezone *) NULL) == -1)
	perror("gettimeofday");
	return ((double) (tp.tv_sec)) +
	(((double) tp.tv_usec) * 0.000001 );
}

 // cpucheck based on danibel.bonetti's post here http://www.linuxquestions.org/questions/programming-9/proc-stat-file-problem-for-cpu-usage-369302/#post3580984

int cpucheck1()
{
	char c[10];
	FILE *f1;
	ti = time_so_far();
	f1 = fopen("/proc/stat", "r");
	int cpu=fscanf(f1, "%s\t%d\t%d\t%d\n", c, &i1, &i2, &i3);
	fclose(f1);
	return cpu;
}

int cpucheck2()
{
	char c[10];
	FILE *f1;
	tf = time_so_far();
	f1 = fopen("/proc/stat", "r");
	int cpu=fscanf(f1, "%s\t%d\t%d\t%d\n", c, &i4, &i5, &i6);
	fclose(f1);
	int t = (i4+i5+i6)-(i1+i2+i3);
	char ccpu[100];
	sprintf(ccpu, "CPU-%.1f%%",((t / ((tf-ti) * 100)) * 100)/get_nprocs());
	rtcfg->sub("cpuusage")->set("cpu",ccpu);
	return cpu;
}

//diskusage() function heavily based on Version Henrik Harmsens GPL program FileRunner

std::string diskusage(std::string disk) 
{
	int i;
	struct statvfs stat;
	double b, x;
	char tmp[5];
	char dsize[5];
	std::string result;

	i = statvfs(disk.c_str(), &stat);
	if (i != 0)
	{return lacfg->getString("notreadable");}
	else
	{
		b = (double)stat.f_bavail * (double)stat.f_frsize;
		if (b < 1024) {
		x = b;
		strcpy(tmp, "");
		} else if (b >= 1024*1024*1024) {
		x = b / (1024*1024*1024.0);
		strcpy(tmp, "G");
		} else if (b >= 1024*1024) {
		x = b / (1024*1024.0);
		strcpy(tmp, "M");
		} else {
		x = b / (1024.0);
		strcpy(tmp, "k");
	}

	if (x < 10 && (tmp[0]))
	sprintf(dsize, "%.1f%s", x, tmp);
	else
	sprintf(dsize, "%d%s", (int)x, tmp);
	result=disk+" - "+lacfg->getString("free")+" "+dsize;
	}
	return result;
}

void readsysinfos()
{
	getfreeram();
	getuptime();
	getip();
	TyphonConfig* du=rtcfg->sub("diskusage");
	TyphonConfig* st=tycfg->sub("strings");

	du->set("disk1",diskusage(st->getString("disk1")));
	du->set("disk2",diskusage(st->getString("disk2")));
	du->set("disk3",diskusage(st->getString("disk3")));
}

// draw system infos:

void drawsys(std::string info,std::string thistext)
{
	glPushMatrix();
	textcol(info,&sysinfofontfx,0);
	setpos(info);
	textrot(info,thistext,&sysinfofontfx,3);
	sysinfofontfx.draw(0.0,0.0, thistext,thcfg->sub("objects")->sub(info)->getFloat("depth"));
	glPopMatrix();
}

void drawnotify()
{
	glPushMatrix();
	textcol("notifier",&notifyfontfx,0);
	setpos("notifier");
	rtcfg->set("notifywidth",notifyfontfx.getWidth(rtcfg->getString("note")));
	textrot("notifier",rtcfg->getString("note"),&notifyfontfx,0);
	notifyfontfx.draw(0.0,0.0, rtcfg->getString("note"),thcfg->sub("objects")->sub("notifier")->getFloat("depth"));
	glPopMatrix();
}

void infopick()
{
	if(pick()==2){shownote(lacfg->getString("mousepickon"));}
	else 			{shownote(lacfg->getString("mousepickoff"));}
}

void inforotpick()
{
	if(pick()==1) 	{shownote(lacfg->getString("rotationpickon"));}
	else 			{shownote(lacfg->getString("rotationpickoff"));}
}

void infocolpick()
{
	if(pick()==3)	{shownote(lacfg->getString("colorpickon"));}
	else 			{shownote(lacfg->getString("colorpickoff"));}
}

void infosetpick()
{
	if(pick()==4)	{shownote(lacfg->getString("settingspickon"));}
	else 			{shownote(lacfg->getString("settingspickoff"));}
}

void shownote(std::string note)
{
	if(thcfg->sub("objects")->sub("notifier")->getBool("enabled"))
	{rtcfg->set("note",note);rtcfg->setBool("shownotify",1);}	
}
