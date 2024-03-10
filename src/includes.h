#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <sys/stat.h> // required for mkdir
#include <dirent.h> // required for scandir
#include <sstream>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sys/sysinfo.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <ftw.h>
#include <unistd.h>

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>

