#include "p3t.h"

int p3tprevextract(const char* p3t_filename,char* destdir)
{
	p3tTheme p3t_theme;

	if (p3tprevRip(p3t_filename, destdir,p3t_theme) != P3T_OK)
	{
		printf("could not process p3t file:%s\n\n", p3t_filename);
	}
	return 0;
}

#ifdef P3TLIB

int p3textract(const char* p3t_filename,char* destdir)
{
	p3tTheme p3t_theme;

	if (p3tRip(p3t_filename, destdir,p3t_theme) != P3T_OK)
	{
		printf("could not process p3t file:%s\n\n", p3t_filename);
	}
	return 0;
}

#else
int main(int argn, char** argv)
{
  if (argn != 3)
  {
    printf("usage: p3t <p3t_file> <destdir>\n\n");
    return -1;
  }

  p3tTheme p3t_theme;

  if (p3tRip(argv[1], argv[2],p3t_theme) != P3T_OK)
  {
    printf("could not process p3t file:%s\n\n", argv[1]);
  }

  return 0;
}

#endif
