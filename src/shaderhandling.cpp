#include "shaderhandling.h"
#include "picturehandling.h"
#include "glmenu.h"
#include "shadermanager.h"
#include "videoplayerhandling.h"
#include "windowhandling.h"
#include "systeminfo.h"
#include "timerfunc.h"
#include "p3thandling.h"
#include "xmlconfig.h"
#include "filehandling.h"
#include "glhandling.h"
#include <SFML/Graphics/Shader.hpp>
#include <string>

sf::Texture shadtexture;

void toggleshaders()
{
	if(useShader()==0)	thcfg->sub("shader")->setBool("useshader",1);
	else 				thcfg->sub("shader")->setBool("useshader",0);
}

int switchshadertex(std::string thistexture, sf::Texture *texi)
{
	if ((strcmp(rtcfg->getString("oldshadertex").c_str(), thistexture.c_str()) != 0))
	{
	rtcfg->set("shcount",(float)0);
	sf::Image shadtex;
	shadtex.loadFromFile(thistexture);
	shadtex.flipVertically();
	texi->loadFromImage(shadtex, sf::IntRect ());
	sf::Texture::bind(texi);
	rtcfg->set("oldshadertex",thistexture);
	return true;
	}
	return false;
}

void updateshaders(ShaderManager &shaderman)
{	
	TyphonConfig* sh=shcfg->sub("shaders")->sub(useBackShader());
	if(sh->getBool("texture"))
	shaderman.getShader(useBackShader()).setParameter("tex0", shadtexture);

	if(sh->getBool("mouse"))
	{shaderman.getShader(useBackShader()).setParameter("mouse",(rootX),(rootY),(rootX),(rootY));}

	if(sh->getBool("size"))
	shaderman.getShader(useBackShader()).setParameter("resolution",(rtcfg->getInt("width")),(rtcfg->getInt("height")));

	if(sh->getBool("time"))
	{
//		if (rtcfg->getFloat("shcount")<30)	
		((TConfig<float>*) rtcfg->get("shcount"))->operator+(thcfg->sub("floats")->getFloat("shaderspeed"));
//		else rtcfg->set("shcount",(float)0);
		shaderman.getShader(useBackShader()).setParameter("time",rtcfg->getFloat("shcount"));
	}

	if(sh->getBool("color"))
	{
		TyphonConfig* mc=rtcfg->sub("monthcolors")->sub(rtcfg->getInt("monthcolor"));
		rtcfg->setBool("wantshadercolor",1);	clocktimer();	rtcfg->setBool("wantshadercolor",0);
		shaderman.getShader(useBackShader()).setParameter("color",
		mc->getFloat("R"),
		mc->getFloat("G"),
		mc->getFloat("B"));
	}
}

void addusershaders()
{
	std::string shadsuf=".sfx";
	struct dirent **sdit;
	int num_entries;
	bool addnew=1;

	std::string usershaderpath=setEnvhome(shcfg->getString("shaderpath"));
	if ((num_entries = scandir((usershaderpath).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		for (int j=0; j<num_entries; j++)
		{
			if (strstr(sdit[j]->d_name, ".sfx"))
			{
				addnew=1;
				for(int s = 0; s <  shcfg->sub("shaders")->size()-1; s++)
				{if(compare(sdit[j]->d_name,shcfg->sub("shaders")->sub(s)->getString("name")))	addnew=0;}
				
				if (addnew)
				{
					std::string shad=usershaderpath+"/"+sdit[j]->d_name;
					TyphonConfig* sh=shcfg->sub("shaders")->sub(getPrefix(sdit[j]->d_name));
					shcfg->sub("shaders")->subC(getPrefix(sdit[j]->d_name))->set("name",sdit[j]->d_name);
					sh->set("author","unknown");
					sh->setBool("time",grep(shad,"float time"));
					sh->setBool("mouse",grep(shad,"vec2 mouse"));
					sh->setBool("size",grep(shad,"vec2 resolution"));
					sh->setBool("color",grep(shad,"vec3 color"));
					sh->setBool("texture",grep(shad,"sampler2D tex"));
					addnew=0;
				}
			}
		}
	}
	shcfg->saveXML(rtcfg->sub("configs")->getString("shaderxml"));
}

void loadShader(int shadid, ShaderManager &shaderman)
{
	std::string shaderfile=shcfg->sub("shaders")->sub(shadid)->getString("name");
	std::string sp= rtcfg->getString("systemshaderpath")+shaderfile;
	FILE *shcheck = fopen(sp.c_str(),"r");
	if(shcheck)
	{fclose(shcheck);}
	else{sp=checkpath(shcfg->getString("shaderpath"),rtcfg->getString("systemshaderpath"))+shaderfile;}
	if (!shaderman.loadShaderFile(shadid, sp, sf::Shader::Type::Fragment)){
		exit(1); //loadshaderfile handles giving you a nice message itself	
	}	
}

void loadShaderConfig(){
	FILE *shcfgcheck = fopen(rtcfg->sub("configs")->getString("shaderxml").c_str(),"r");
	if(shcfgcheck)
	{
		shcfg->loadXML(rtcfg->sub("configs")->getString("shaderxml"));
		fclose(shcfgcheck);
	}
	else 
	{
		setshaderdefaults();
		shcfg->saveXML(rtcfg->sub("configs")->getString("shaderxml"));
	}
}
void initshaders(ShaderManager &shaderman)
{
	addusershaders();
	for(int s = 0; s <  shcfg->sub("shaders")->size(); s++)
	{
		loadShader(s,shaderman);
	}

}

void shaderwp(std::string thiswallpaper)
{
	std::string thisshadertex=rtcfg->sub("scanned")->getString("wallpaperpath")+"/"+thiswallpaper;

	FILE *shadcheck = fopen(thisshadertex.c_str(),"r");
	if(!shadcheck)
	{
		thisshadertex=rtcfg->sub("scanned")->getString("wallpaperpath")+"/"+thcfg->sub("textures")->getString("preview");
	}
	
	switchshadertex(thisshadertex,&shadtexture);
}

void loadshadertex()
{
	if(shcfg->sub("shaders")->sub(useBackShader())->getBool("texture") && thcfg->sub("shader")->getBool("useshader"))
	{

		if(thcfg->sub("ints")->getInt("shadertexture")==1)			// preview texture
		{switchshadertex(rtcfg->getString("currentprev"), &shadtexture);}
		
		else if(thcfg->sub("ints")->getInt("shadertexture")==2)		// wallpaper texture
		{
			if (thcfg->sub("switches")->getBool("usemenuwallpaper"))
			{
				int thiswpid=thcfg->sub("textures")->get("wallpaper0")->id+Ishowmenu();
				std::string wpcat=thcfg->sub("textures")->getKey(thiswpid);
				shaderwp(thcfg->sub("textures")->getString(wpcat));
			}
			else 
			switchshadertex(rtcfg->getString("currentback"), &shadtexture);
		}
		else if(thcfg->sub("ints")->getInt("shadertexture")==3)		// generic texture
		{switchshadertex(gTex("ownshadertexture"), &shadtexture);}
	}
}

void drawshadebackground(ShaderManager &shaderman)
{
	if(useShader())
	{
		glPushMatrix();
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glTranslatef(-rtcfg->getInt("width")/2,-rtcfg->getInt("height")/2,0);
		updateshaders(shaderman);
		sf::Shader::bind(&shaderman.getShader(useBackShader()));
		sf::Texture::bind(&shadtexture);
		drawquad(rtcfg->getInt("width"),rtcfg->getInt("height"));
		glUseProgram(0);

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
}
