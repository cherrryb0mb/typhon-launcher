#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

class GimVoodoo {
	public:
		GimVoodoo();  				// constructor
		bool oeffnen(char* datei);
		char* name;				 // filename for later overwriting
		int width;				 // image width
		int height;				 // image height
		bool makePNG(char* pngfilename); // write png file
		bool checkHeader();		 // check gim header
		char* getXY(int,int);	 // (4 Byte for RGBA)
		bool debug;
		int korrektur;			// fix distortion default=0
		FILE* gimFile;			 // Filehandle
	private:
//		FILE* gimFile;			 // Filehandle
		char* rgba;				 // 4*width*height bytearray for imagedata
		char* header;			 // 128 byte headerdata
		long fileSize;				 // full datasize (incl. header)
};

extern int gim2png(const char* gimfilename,char* pngfilename);
