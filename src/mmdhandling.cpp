#ifdef WITHMMD
#include "mmdhandling.h"
#include "picturehandling.h"
#include "windowhandling.h"
#include "values.h"
#include "filehandling.h"
#include "xmlconfig.h"
#include "systeminfo.h"
#include "glhandling.h"

#ifdef WITHBULLET
extern cBulletPhysics	g_clBulletPhysics;
#endif
static cPMDModel g_clPMDModel;
static cVMDMotion g_clVMDMotion;

int openmodel()
{
	std::string modelpath=rtcfg->sub("scanned")->getString("modeldir")+"/"+thcfg->sub("files")->getString("mmdmodelfile");
	char pwd[FILENAME_MAX];
	std::string opwd=getcwd(pwd,PATH_MAX);

	int mmdpath=chdir (rtcfg->sub("scanned")->getString("modeldir").c_str());
	if (!mmdpath)
	{
		FILE *checkmodel = fopen(modelpath.c_str(),"r");
		if (checkmodel)	{
							g_clPMDModel.load( modelpath.c_str() ); 
							openmotion();
							fclose (checkmodel);
						}
		else			{thcfg->sub("objects")->sub("mmd")->setBool("enabled",0);}
	}
	return chdir (opwd.c_str());
}

void openmotion()
{
	std::string motionpath=rtcfg->sub("scanned")->getString("motiondir")+"/"+thcfg->sub("files")->getString("mmdmotionfile");
	g_clVMDMotion.load( motionpath.c_str() );
	g_clPMDModel.setMotion( &g_clVMDMotion, false );
	g_clPMDModel.updateMotion( 0.0f );
	g_clPMDModel.resetRigidBodyPos();
	g_clPMDModel.setLoop(thcfg->sub("switches")->getBool("mmdmotionloop"));
}

void togglephysics()
{
	thcfg->sub("switches")->setBoolC("mmdphysics",!thcfg->sub("switches")->getBool("mmdphysics"));
	g_clPMDModel.enablePhysics(thcfg->sub("switches")->getBool("mmdphysics"));
}

void initmodelgl()
{
#ifdef WITHBULLET
	g_clBulletPhysics.initialize();
#endif
	if(thcfg->sub("objects")->sub("mmd")->getBool("enabled"))openmodel();
}

void displaymodel()
{
	glPushMatrix();
	texcol("mmd");
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_NORMALIZE );

	const float	fLightPos[] = { 0.45f, 0.55f , 1.0f, 0.0f };
	const float	fLightDif[] = { 0.9f, 0.9f, 0.9f, 1.0f },
				fLightAmb[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	glLightfv( GL_LIGHT2, GL_POSITION, fLightPos );
	glLightfv( GL_LIGHT2, GL_DIFFUSE, fLightDif );
	glLightfv( GL_LIGHT2, GL_AMBIENT, fLightAmb );
	glLightfv( GL_LIGHT2, GL_SPECULAR, fLightAmb );

	glEnable( GL_LIGHT2 );

	glRotatef(180, 0.0, 1.0, 0.0);

	glTranslatef
	(
		-rtcfg->sub("objects")->sub("mmd")->getFloat("posx"),
		rtcfg->sub("objects")->sub("mmd")->getFloat("posy"),
		-(thcfg->sub("objects")->sub("mmd")->getInt("posz"))
	);
	rtcfg->sub("objects")->sub("mmd")->set("width",-(rtcfg->sub("objects")->sub("mmd")->getFloat("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx")));
	rtcfg->sub("objects")->sub("mmd")->set("height",(rtcfg->sub("objects")->sub("mmd")->getFloat("scaley")*rtcfg->sub("objects")->sub("mmd")->getInt("calcy")));



	glTranslatef(0,rtcfg->sub("objects")->sub("mmd")->getFloat("height")/2,0);

	glRotatef(thcfg->sub("objects")->sub("mmd")->getInt("rotx"), 1.0, 0.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub("mmd")->getInt("roty"), 0.0, 1.0, 0.0);
	glRotatef(thcfg->sub("objects")->sub("mmd")->getInt("rotz"), 0.0, 0.0, 1.0);	

	glTranslatef(0,-rtcfg->sub("objects")->sub("mmd")->getFloat("height")/2,0);	

// pickbox
	if((rtcfg->getInt("picked")==thcfg->sub("objects")->get("mmd")->id) || pick()==4)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable( GL_DEPTH_TEST );
		glTranslatef((rtcfg->sub("objects")->sub("mmd")->getFloat("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx"))/2,0,0);
		drawquad
		(
			-(rtcfg->sub("objects")->sub("mmd")->getFloat("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx")),
			(rtcfg->sub("objects")->sub("mmd")->getFloat("scaley")*rtcfg->sub("objects")->sub("mmd")->getInt("calcy"))
		);
		glTranslatef(-(rtcfg->sub("objects")->sub("mmd")->getFloat("scalex")*rtcfg->sub("objects")->sub("mmd")->getInt("calcx"))/2,0,0);
		glEnable( GL_DEPTH_TEST );
	}
// pickbox end
	if(thcfg->sub("objects")->sub("mmd")->getBool("enabled"))
	{
		glScalef
		(
			rtcfg->sub("objects")->sub("mmd")->getFloat("scalex"),
			rtcfg->sub("objects")->sub("mmd")->getFloat("scaley"),
			(float)thcfg->sub("objects")->sub("mmd")->getFloat("depth")
		);

		if(thcfg->sub("objects")->sub("mmd")->getBool("autorotate"))
		glRotatef(rtcfg->getFloat("autorotate"), 0.0, 1.0, 0.0);

		g_clPMDModel.render();
		g_clPMDModel.renderForShadow();
	}
	glPopMatrix();

	glDisable (GL_LIGHT2);
	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);
}

void idlemodel()
{
	Vector3	vecCamPos;
	float fElapsedFrame = Clock.getElapsedTime().asSeconds()*(30);
	if( fElapsedFrame > 10.0f )	fElapsedFrame = 10.0f;

	g_clPMDModel.updateMotion( fElapsedFrame );
	g_clPMDModel.updateNeckBone( &vecCamPos );
#ifdef WITHBULLET
	g_clBulletPhysics.update( fElapsedFrame );
#endif
	g_clPMDModel.updateSkinning();
}

void quitmodel()
{
	g_clPMDModel.release();
#ifdef WITHBULLET
	g_clBulletPhysics.release();
#endif
}

void listmmd(std::string searchdir,std::string suffix,std::string cat1,std::string cat2)
{
	struct dirent **sdit;
	int num_entries;
	if ((num_entries = scandir((searchdir).c_str(), &sdit, fileselectAll, alphasort))< 0){}
	else
	{
		int k=0;
		for (int j=0; j<num_entries; j++)
		{
			if (compare(sdit[j]->d_name, suffix.c_str()))
			{
				std::stringstream entry;
				entry << cat1 << k;
				rtcfg->sub(cat2)->set(entry.str(),sdit[j]->d_name,k++);
			}
		}
	}
	rtcfg->set("currentmodel",0);
	rtcfg->set("currentmotion",0);
}

void updatemodel(bool up)
{
	if (up)
	{
		if(rtcfg->getInt("currentmodel")==rtcfg->sub("models")->size()-1)		rtcfg->set("currentmodel",0);
		else {((TConfig<int>*) rtcfg->get("currentmodel"))->operator+(1);}
	}
	else
	{
		if(rtcfg->getInt("currentmodel")==0)		rtcfg->set("currentmodel",rtcfg->sub("models")->size()-1);
		else {((TConfig<int>*) rtcfg->get("currentmodel"))->operator-(1);}
	}
	thcfg->sub("files")->setC("mmdmodelfile",rtcfg->sub("models")->getString(rtcfg->getInt("currentmodel")));
	if (thcfg->sub("objects")->sub("mmd")->getBool("enabled")){openmodel();}
	shownote(lacfg->getString("pmd")+getPrefix(thcfg->sub("files")->getString("mmdmodelfile")));
}

void updatemotion(bool up)
{
	if (up)
	{
		if(rtcfg->getInt("currentmotion")==rtcfg->sub("motion")->size()-1)		rtcfg->set("currentmotion",0);
		else {((TConfig<int>*) rtcfg->get("currentmotion"))->operator+(1);}
	}
	else
	{
		if(rtcfg->getInt("currentmotion")==0)		rtcfg->set("currentmotion",rtcfg->sub("motion")->size()-1);
		else {((TConfig<int>*) rtcfg->get("currentmotion"))->operator-(1);}
	}
	thcfg->sub("files")->setC("mmdmotionfile",rtcfg->sub("motion")->getString(rtcfg->getInt("currentmotion")));
	if (thcfg->sub("objects")->sub("mmd")->getBool("enabled")){openmotion();}
	shownote(lacfg->getString("vmd")+getPrefix(thcfg->sub("files")->getString("mmdmotionfile")));
}

void togglemotionloop()
{
	thcfg->sub("switches")->setBoolC("mmdmotionloop",!thcfg->sub("switches")->getBool("mmdmotionloop"));
	g_clPMDModel.setLoop(thcfg->sub("switches")->getBool("mmdmotionloop"));
}

#endif // WITHMMD
