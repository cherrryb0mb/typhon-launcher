#include "gim.h"
#include <string.h>
#include <png.h>

using namespace std;

GimVoodoo::GimVoodoo() {
	debug=false;
	header =(char*) malloc (sizeof(char)*128);
}

bool GimVoodoo::oeffnen(char* datei) {
	name=datei;
	gimFile = fopen(name, "r");
	if (!gimFile) return false;
	if (debug) printf("opened \"%s\" \n",name);
	fseek (gimFile , 0 , SEEK_END);
	fileSize = ftell (gimFile);
	rewind (gimFile);
	rgba =(char*) malloc (sizeof(char)*fileSize-128);
	if (debug) printf("reading header...\n");
	size_t read_size = 0;
	read_size = fread(header, sizeof(char), 128, gimFile);
	if (debug) printf("reading data...\n");
	read_size = fread(rgba, sizeof(char), fileSize-128, gimFile);
	return true;
}

bool GimVoodoo::checkHeader() {
	if (header[1]+header[2]+header[3]==221) { //0x47+0x49+0x4d
		if (debug) printf("found valid header: %s\n",header);
		width=((unsigned char)header[73]+0x100*(unsigned char)header[72]);
		height=((unsigned char)header[75]+0x100*(unsigned char)header[74]);
		
		
		
		if (debug) {
		 printf("extracting dimensions:\n");	
		 printf("width, height: %04X, %04X (hex)\n",width,height);
		 printf("width, height: %i, %i (dec)\n",width,height);
		}
		
		
		 int bheight=(int)(fileSize-128)/(width*4);
		 if (debug) printf("calculating height:%i\n",(int)bheight);
		 height=bheight;
		 
		 return true; 
		
	}
	return false;
}

bool GimVoodoo::makePNG(char* pngfilename) {
	if (debug) printf("opening png destinationfile: %s\n",pngfilename);
	FILE *fp = fopen(pngfilename, "wb");
    if (!fp) return false; 
    if (debug) printf("creating png structure 1,");
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr){fclose(fp);return false;}
	if (debug) printf("2,");
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
       fclose(fp);
       return false;
    }
    if (debug) printf("3!\n");
	if (setjmp(png_jmpbuf(png_ptr))) {
       png_destroy_write_struct(&png_ptr, &info_ptr);
       fclose(fp);
       return false;
    }

	png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    if (debug) printf("writing png header!\n");
	png_write_info(png_ptr, info_ptr);	

	//test rGBA pixel, for glitch-detection!
	char* test= (char*) malloc(4);
	test[0]=0;test[1]=0;test[2]=0;test[3]=0;
	
	png_byte *row_pointers[height];
	int h;
	if (debug) printf("filling png bytearray ...");


	/*
	* in some gims there are two zeropixels on the left, which distort the whole picture
	* therefore we have to look if the first 8 byte in the 2nd row are 00. if that's the case
	* we calculate in a correction of 2 for the position. 
	* as there are also "normal" gims, which might have a black transparent pixel at that position
	* we also check if the 3rd pixel is not 00 00 00.
	* wrong corrections might be possible of course, but that's quite unlikely.
	*/	

	if (*getXY(1,0) == *test && *getXY(1,1) == *test && *getXY(1,2) != *test) {	
		if (debug) printf("! found glitch - fixing...\n");
		korrektur=2;
	}

	for (h=0; h<height; h++) {
		row_pointers[h]=(png_byte*)rgba + h*(width+korrektur)*4;
	}
    if (debug) printf("done!\n");
    
    /* different method. could be helpful with changing colors or removing alpha
    png_bytep *row_pointers;
    row_pointers = (png_bytep *)malloc( sizeof(png_bytep) * height);
    int i,w,h;
    w=0; h=0;
    for(i = 0; i < height; i++)
    {
        int j;
        row_pointers[i] = (png_byte*) malloc(sizeof(png_byte) * width * 4);
        for(j = 0; j < width; j++)
        {
            row_pointers[i][0+4*j] = getXY(i,j)[0];   
            row_pointers[i][1+4*j] = getXY(i,j)[1];   
            row_pointers[i][2+4*j] = getXY(i,j)[2];
            row_pointers[i][3+4*j] = getXY(i,j)[3]; 
        }
    }
    */
    if (debug) printf("writing png to disc\n");
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
	if (debug) printf(" cleaning up ram\n");
	png_destroy_write_struct(&png_ptr, &info_ptr);
	if (debug) printf("done with gim2png conversion!\n");
	free(test);
	fclose(fp);
	return true;
}

inline char* GimVoodoo::getXY(int y, int x) {
	char* temp = (char*) malloc(4);
	int w=width+korrektur;
	temp[0]=rgba[(w*y+x)*4];
	temp[1]=rgba[(w*y+x)*4+1];
	temp[2]=rgba[(w*y+x)*4+2];
	temp[3]=rgba[(w*y+x)*4+3];
	return temp;
}

int gim2png(const char* gimfilename,char* pngfilename)
{
  GimVoodoo* myGim;
  myGim = new GimVoodoo();
  myGim->debug=false;
  myGim->oeffnen((char*)gimfilename);
  myGim->korrektur=0;

  if (myGim->checkHeader()) myGim->makePNG(pngfilename);
  else printf("no valid gim file\n");
  fclose(myGim->gimFile);
  return 0;
}
