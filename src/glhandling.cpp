#include "glhandling.h"
#include "videoplayerhandling.h"
#include "windowhandling.h"
#include "systeminfo.h"
#include "mmdhandling.h"
#include "inputhandling.h"
#include "timerfunc.h"
#include "shaderhandling.h"
#include "xmlconfig.h"
#include "rsshandling.h"
#include "picturehandling.h"
#include "filehandling.h"

void initgl()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
    glEnable(GL_TEXTURE_2D);	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity ();
	gluPerspective(90, (float)rtcfg->getInt("width") / (float)rtcfg->getInt("height"), 1, (float)rtcfg->getInt("width")*10);
	gluLookAt(0.0, 0.0, (float)rtcfg->getInt("height")/2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	
	glMatrixMode (GL_MODELVIEW);
	glViewport(0, 0,rtcfg->getInt("width"),rtcfg->getInt("height"));
}

void drawquad(float quadx, float quady)
{
	glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex3f( quadx	,quady	, 0);
		glTexCoord2f(1, 1); glVertex3f( quadx	,1.0f	, 0);
		glTexCoord2f(0, 1); glVertex3f( -1.0f	,1.0f	, 0);
		glTexCoord2f(0, 0); glVertex3f( -1.0f	,quady	, 0);
	glEnd();
}

void drawcube(float cubex, float cubey,float cubez,std::string obj,std::string bobj)
{
		 if (compare("previewreflection",obj)) obj="preview";
	else if (compare("setbox",obj) || compare("rombox",obj)) obj="settings";

//	glEnable(GL_DEPTH_TEST); 
	if (compare("video",obj))
	{
	//right
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	1.0f, 	-cubez); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	cubey,  -cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(cubex, 	cubey,  	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(cubex, 	1.0f, 	0);
		glEnd();
	//left
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, 	1.0f, 	0); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, 	cubey,  	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	cubey,  	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	1.0f, 	-cubez);
		glEnd();		
	//top
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	1.0f,  	-cubez); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	1.0f,  	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	1.0f,  	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	1.0f,  	-cubez);
		glEnd();
	//bottom
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	cubey, 	0); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	cubey, 	-cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	cubey, 	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	cubey, 	0);
		glEnd();
	// back
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f( -1.0f, 	1.0f,	-cubez);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f( -1.0f, 	cubey, 	-cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f( cubex, 	cubey, 	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f( cubex, 	1.0f,	-cubez);
		glEnd();	
	//front
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f( cubex, 	1.0f,	0);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f( cubex, 	cubey, 	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f( -1.0f, 	cubey, 	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f( -1.0f, 	1.0f,	0);
		glEnd();
	}
	else
	{
	//right
		sf::Texture::bind(getTextureS(obj)); 
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	cubey, 	-cubez); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	1.0f,  -cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(cubex, 	1.0f,  	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(cubex, 	cubey, 	0);
		glEnd();
	//left
		sf::Texture::bind(getTextureS(obj)); 
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, 	cubey, 	0); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, 	1.0f,  	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	1.0f,  	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	cubey, 	-cubez);
		glEnd();
	//top
		sf::Texture::bind(getTextureS(obj)); 
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	1.0f,  	0); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	1.0f,  	-cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	1.0f,  	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	1.0f,  	0);
		glEnd();
	//bottom
		sf::Texture::bind(getTextureS(obj)); 
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(cubex, 	cubey, 	-cubez); 
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(cubex, 	cubey, 	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 	cubey, 	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 	cubey, 	-cubez);
		glEnd();
	//front
		sf::Texture::bind(getTextureS(obj)); 
		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f( cubex, 	cubey,	0);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f( cubex, 	1.0f, 	0);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f( -1.0f, 	1.0f, 	0);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f( -1.0f, 	cubey,	0);
		glEnd();

	// back
		if(thcfg->sub("objects")->sub(obj)->getBool("back"))
		sf::Texture::bind(getTextureS(bobj)); 
		else
		sf::Texture::bind(getTextureS(obj)); 

		glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f( -1.0f, 	cubey,	-cubez);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f( -1.0f, 	1.0f, 	-cubez);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f( cubex, 	1.0f, 	-cubez);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f( cubex, 	cubey,	-cubez);
		glEnd();		
	}

}

void cuboquad(std::string obj,std::string bobj)
{
	if (roundf(thcfg->sub("objects")->sub(obj)->getFloat("depth"))==0)
	{
		drawquad
		(
			rtcfg->sub("objects")->sub(obj)->getFloat("width"),
			rtcfg->sub("objects")->sub(obj)->getFloat("height")
		);
		
		if(thcfg->sub("objects")->sub(obj)->getBool("back"))
		{
			glTranslatef(rtcfg->sub("objects")->sub(obj)->getFloat("width"),0,-1);
			sf::Texture::bind(getTextureS(bobj)); 
			drawquad
			(
				-rtcfg->sub("objects")->sub(obj)->getFloat("width"),
				rtcfg->sub("objects")->sub(obj)->getFloat("height")
			);
		}
	}
	else
	{
		drawcube
		(
			rtcfg->sub("objects")->sub(obj)->getFloat("width"),
			rtcfg->sub("objects")->sub(obj)->getFloat("height"),
			thcfg->sub("objects")->sub(obj)->getFloat("depth"),
			obj,
			bobj
		);
	}	
}

void setpos(std::string obj)
{
	glTranslatef
	(
		rtcfg->sub("objects")->sub(obj)->getFloat("posx"),
		rtcfg->sub("objects")->sub(obj)->getFloat("posy"),
		thcfg->sub("objects")->sub(obj)->getInt("posz")
	);
}

void rotateme(std::string obj)
{
	glTranslatef
	(
		rtcfg->sub("objects")->sub(obj)->getFloat("width")/2,
		rtcfg->sub("objects")->sub(obj)->getFloat("height")/2,
		-thcfg->sub("objects")->sub(obj)->getFloat("depth")/2
	);
		
	glRotatef(thcfg->sub("objects")->sub(obj)->getInt("rotx"), 1.0, 0.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub(obj)->getInt("roty"), 0.0, 1.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub(obj)->getInt("rotz"), 0.0, 0.0, 1.0);	

	if(
		thcfg->sub("objects")->sub(obj)->exists("autorotate") &&
		thcfg->sub("objects")->sub(obj)->getBool("autorotate"))
	glRotatef(rtcfg->getFloat("autorotate"), 0.0, 1.0, 0.0);

	glTranslatef
	(
		-rtcfg->sub("objects")->sub(obj)->getFloat("width")/2,
		-rtcfg->sub("objects")->sub(obj)->getFloat("height")/2,
		thcfg->sub("objects")->sub(obj)->getFloat("depth")/2
	);
}

/*
void resizescreen(int x, int y)// LATER TODO
{
	rtcfg->set("width"		,x);
	rtcfg->set("height"		,y);
	proportions();
	window.create( sf::VideoMode(rtcfg->getInt("width"),rtcfg->getInt("height"),32), "typhon", sf::Style::Resize);

//	sf::Vector2i size = sf::Vector2i(rtcfg->getInt("width"),rtcfg->getInt("height"));
	//window.setSize(static_cast<sf::Vector2u>(size));
//	window.setActive();
//	window.setView(sf::View(sf::FloatRect(0, 0, (float)rtcfg->getInt("width"), (float)rtcfg->getInt("height"))));
	glViewport(0, 0, rtcfg->getInt("width"),rtcfg->getInt("height"));
//	window.setActive(false);
}
*/

void drawrectbox();

void display(ShaderManager &shaderman)
{
	window.setFramerateLimit(30);
	window.setActive();
//	window.UseVerticalSync(false);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   	if(rtcfg->sub("shaders")->getBool("allowed") && useShader()
   	#ifdef WITHSFM
	&& (thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
	#endif // WITHSFM
	)
   	drawshadebackground(shaderman);
        	
	glPushMatrix();

	glTranslatef
	(
		rtcfg->sub("objects")->sub("wallpaper")->getFloat("width")/1024,
		rtcfg->sub("objects")->sub("wallpaper")->getFloat("height")/1024,
		-rtcfg->sub("objects")->sub("wallpaper")->getFloat("depth")/1024
	);
		
	glRotatef(thcfg->sub("objects")->sub("wallpaper")->getInt("rotx"), 1.0, 0.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub("wallpaper")->getInt("roty"), 0.0, 1.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub("wallpaper")->getInt("rotz"), 0.0, 0.0, 1.0);	

	glTranslatef
	(
		-rtcfg->sub("objects")->sub("wallpaper")->getFloat("width")/1024,
		-rtcfg->sub("objects")->sub("wallpaper")->getFloat("height")/1024,
		rtcfg->sub("objects")->sub("wallpaper")->getFloat("depth")/1024
	);
			
	#ifdef WITHSFM
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==1) drawvideo();
	#endif // WITHSFM

	#ifdef WITHSFM
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
	#endif // WITHSFM
	drawwallpapers();

	#ifdef WITHSFM
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==2) drawvideopreview();
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")!=3)
	#endif // WITHSFM
	{
		drawpictures();
		if (thcfg->sub("objects")->sub("rectbox")->getBool("enabled") || pick()==4)
		drawrectbox();	    

		#ifdef WITHMMD
		if(thcfg->sub("objects")->sub("mmd")->getBool("enabled") || pick()==4)
		displaymodel();
		#endif //WITHMMD
	}

	#ifdef WITHSFM
	if(thcfg->sub("objects")->sub("video")->getInt("drawhere")==3)			drawvideofull();
	else
	#endif // WITHSFM
	{
		if (!rtcfg->getBool("hidetext"))						{displayfonts();};
		if (thcfg->sub("objects")->sub("rss")->getBool("enabled") && rtcfg->getBool("rssinitialized"))	{drawrssfeed(rtcfg->getString("rss"));}
//		if (thcfg->sub("objects")->sub("rss")->getBool("enabled"))			{scrollrss(thcfg->sub("objects")->sub("rss")->getInt("speed"));}
	}

	glPopMatrix();

	window.display();
}
