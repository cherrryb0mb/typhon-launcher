#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include "TConfig.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <tinyxml.h>

TyphonConfig::~TyphonConfig() {
	delete conf;
}

TyphonConfig::TyphonConfig() {
	conf=new std::map<std::string, Config*>();
	id=-1;
}

void TyphonConfig::erase(std::string key) {
	conf->erase(key);
}

void TyphonConfig::erase(int id) {
	if (exists(id)) {
		std::string key=getKey(id);
		for (int i=(id+1); i<size();i++) {
			std::string k = getKey(i);
			Config* v = get(k);
			v->id=i-1;
		}
		erase(key);
	}
}

void TyphonConfig::switchid(int id1, int id2) {
	if (exists(id1) && exists(id2)) {
		Config* tmp = get(getKey(id1));
		get(getKey(id2))->id=id1;
		tmp->id=id2;
	}
	else if (id2>=size()) switchid(id1,size()-1);
	else if (id1>=size()) switchid(size()-1,id2);
}

void TyphonConfig::move(int id1, int id2) {
	if (exists(id1)) {
			std::string key = getKey(id1);
			Config* tmp = get(key);
			erase(id1);
			insert(key,tmp,id2);
	}
}

TyphonConfig* TyphonConfig::insertSubCa(std::string key,int id2) {
	subCa(key);
	int id1 = size()-1;
	move(id1,id2);
	return sub(id2);
}

void TyphonConfig::insert(std::string key, Config* value, int id) {
	if (id < size()) {
		if (key=="") {
			std::stringstream keyco;
			keyco << getKey(id) << "_neu";
			key=keyco.str();
		}
		for (int i=size()-1; i>=id; i--) {
			get(getKey(i))->id=i+1;
		}
		set(key,value);
		get(key)->id=id;
	}
	else {
		set(key, value);
		get(key)->id=size();
	}
}

void TyphonConfig::insert(std::string key, std::string value, int id) {
	insert(key, new TConfig<std::string>(value),id);
	get(key)->typ="string";
}

void TyphonConfig::insert(std::string key, int value, int id) {
	insert(key, new TConfig<int>(value),id);
	get(key)->typ="int";
}

void TyphonConfig::set(std::string key, Config* value) {
	conf->operator[](key)=value;
}

void TyphonConfig::set(std::string key, int value) {
	set(key, new TConfig<int>(value));
	get(key)->typ="int";
}

void TyphonConfig::set(std::string key, int value, int id) {
	set(key, value);
	get(key)->id=id;
}

void TyphonConfig::set(std::string key, std::string value) {
	set(key, new TConfig<std::string>(value));
	get(key)->typ="string";
}

void TyphonConfig::set(std::string key, std::string value, int id) {
	set(key, value);
	get(key)->id=id;
}

void TyphonConfig::setCa(std::string key, std::string value) {
	int newID=size();
	std::stringstream keyco;
	keyco << key << "_" << newID;
	if (exists(keyco.str())) {
			int oldid=get(keyco.str())->id;
			set(keyco.str(),value,oldid);
		}
	else
	{
		set(keyco.str(),value,newID);
	}
}

void TyphonConfig::setCa(std::string key, int value) {
	int newID=size();
	std::stringstream keyco;
	keyco << key << "_" << newID;
	if (exists(keyco.str())) {
			int oldid=get(keyco.str())->id;
			set(keyco.str(),value,oldid);
		}
	else
	{
		set(keyco.str(),value,newID);
	}
}

void TyphonConfig::setC(std::string key, std::string value) {
	if (exists(key)) {
			int oldid=get(key)->id;
			set(key,value,oldid);
		}
	else
	{
		int i = size();
		set(key,value,i);
	}
}

void TyphonConfig::setC(std::string key, int value) {
	if (exists(key)) {
			int oldid=get(key)->id;
			set(key,value,oldid);
		}
	else
	{
		int i = size();
		set(key,value,i);
	}
}

void TyphonConfig::setC(std::string key, float value) {
	if (exists(key)) {
			int oldid=get(key)->id;
			set(key,value,oldid);
		}
	else
	{
		int i = size();
		set(key,value,i);
	}
}

void TyphonConfig::set(std::string key, float value) {
	set(key, new TConfig<float>(value));
	get(key)->typ="float";
}

void TyphonConfig::set(std::string key, float value, int id) {
	set(key, value);
	get(key)->id=id;
}

void TyphonConfig::set(int key, float value)
{
	std::string skey = getKey(key);
	set(skey,value,key);
}

void TyphonConfig::set(int key, int value)
{
	std::string skey = getKey(key);
	set(skey,value,key);
}

void TyphonConfig::setBool(int key, bool value)
{
	std::string skey = getKey(key);
	setBool(skey,value,key);
}

void TyphonConfig::setBool(std::string key, bool value) {
	set(key, new TConfig<bool>(value));
	get(key)->typ="bool";
}

void TyphonConfig::setBool(std::string key, bool value, int id) {
	setBool(key,value);
	get(key)->id=id;
}

void TyphonConfig::setBoolC(std::string key, bool value) {
	if (exists(key)) {
			int oldid=get(key)->id;
			setBool(key,value,oldid);
		}
	else
	{
		int i = size();
		setBool(key,value,i);
	}
}

Config* TyphonConfig::get(std::string key) {
	if (!exists(key)) set(key,"0");
	return conf->operator[](key);
}

Config* TyphonConfig::get(int key)
{
	std::string skey =getKey(key);
	return get(skey);
}

int* TyphonConfig::getIntP(std::string key) {
	TConfig<int>* i;
	if (strcasecmp(get(key)->typ.c_str(),"string")==0) {
		TConfig<std::string>* ts = (TConfig<std::string>*) get(key);
		std::string* s = ts->get();
		return new int(atoi((*s).c_str()));
	}
	else i= (TConfig<int>*) get(key);
	return i->get();
}

int TyphonConfig::getInt(std::string key) {
	return *getIntP(key);
}

sf::Texture* TyphonConfig::getTexturePtr(std::string key){
	TConfig<sf::Texture>* texturecfg = (TConfig<sf::Texture>*) get(key);
	return texturecfg->get();	
}
sf::Shader* TyphonConfig::getShaderPtr(std::string key){
	TConfig<sf::Shader>* texturecfg = (TConfig<sf::Shader>*) get(key);
	return texturecfg->get();	
}

std::string* TyphonConfig::getStringP(std::string key) {
	TConfig<std::string>* s = (TConfig<std::string>*) get(key);
	return s->get();
}

std::string TyphonConfig::getString(std::string key) {
	return *getStringP(key);
}

float TyphonConfig::getFloat(std::string key){
	return *getFloatP(key);
}

float* TyphonConfig::getFloatP(std::string key){
	TConfig<float>* f = (TConfig<float>*) get(key);
	return f->get();
}

bool TyphonConfig::getBool(std::string key){
	
	if(!exists(key))return false;
	else if (!(*getBoolP(key))) return false;
	else return *getBoolP(key);
}

bool* TyphonConfig::getBoolP(std::string key){
	TConfig<bool>* b = (TConfig<bool>*) get(key);
	return b->get();
}

void TyphonConfig::addConfig(std::string key) {
	TyphonConfig* tc = new TyphonConfig();
	set(key, tc);
	get(key)->typ="map";
}

void TyphonConfig::addConfig(std::string key, int id) {
	addConfig(key);
	sub(key)->id=id;
}

TyphonConfig* TyphonConfig::sub(std::string key) {
	if (!exists(key)) addConfig(key);
	return (TyphonConfig*) get(key);
}

TyphonConfig* TyphonConfig::subC(std::string key) {
	if (exists(key)) return (TyphonConfig*) get(key);
	else {
		int i = size();
		addConfig(key,i);
		return sub(i);
	}
}

TyphonConfig* TyphonConfig::lastsub()
{
	int ID=size()-1;
	return sub(ID);
}


TyphonConfig* TyphonConfig::subCa(std::string key) {
	int newID=size();
	std::stringstream keyco;
	keyco << key << "_" << newID;
	if (exists(keyco.str()))
	{
		return (TyphonConfig*) get(keyco.str());
	}
	else
	{
		addConfig(keyco.str(),newID);
		return sub(newID);
	}
}

TyphonConfig* TyphonConfig::sub(std::string key, int id) {
	if (!exists(key) || !exists(id)) addConfig(key, id);
	return (TyphonConfig*) sub(id);
}

TyphonConfig* TyphonConfig::sub(int id) {
	if (id>=size()) id=size()-1;
	if (id<0) return NULL;
	std::map<std::string, Config*>::iterator p;
	for (p = conf->begin(); p!=conf->end(); p++) {
		if (p->second->id==id) return (TyphonConfig*) p->second;
	}
	return NULL;
}

char* TyphonConfig::getInt2String(std::string key) {
	char* str = new char[33];
	sprintf(str,"%d",getInt(key));
	return str;
}

char* TyphonConfig::getFloat2String(std::string key) {
	char* str = new char[33];
	sprintf(str,"%f",getFloat(key));
	return str;
}

bool TyphonConfig::exists(std::string key) {
	std::map<std::string, Config*>::const_iterator it = conf->find(key);
	if (it==conf->end()) return 0;
	else return 1;
}

bool TyphonConfig::exists(int id) {
	std::map<std::string, Config*>::iterator p;
	for (p = conf->begin(); p!=conf->end(); p++) {
		if (p->second->id==id) return true;
	}
	return false;
}

std::string TyphonConfig::getKey(int id) {
	if (!exists(id)) {
		printf("----------\n\tnot existing ID: %i , mapsize: %i\n",id,size());
//		printf("\tsee /tmp/typhonerrors.xml\n----------\n");
//		saveXML("/tmp/typhonerrors.xml");
	}
	std::map<std::string, Config*>::iterator p;
	for (p = conf->begin(); p!=conf->end(); p++) {
		if (p->second->id==id) return p->first;
	}
	return "xxx";
}
sf::Texture* TyphonConfig::getTexturePtr(int i) {
	return getTexturePtr(getKey(i));
}
sf::Shader* TyphonConfig::getShaderPtr(int i) {
	return getShaderPtr(getKey(i));
}
std::string TyphonConfig::getString(int i) {
	return getString(getKey(i));
}

std::string* TyphonConfig::getStringP(int i) {
	return getStringP(getKey(i));
}

int TyphonConfig::getInt(int i) {
	return getInt(getKey(i));
}

int* TyphonConfig::getIntP(int i) {
	return getIntP(getKey(i));
}

float TyphonConfig::getFloat(int i) {
	return getFloat(getKey(i));
}

float* TyphonConfig::getFloatP(int i) {
	return getFloatP(getKey(i));
}


bool TyphonConfig::getBool(int i) {
	return getBool(getKey(i));
}

bool* TyphonConfig::getBoolP(int i) {
	return getBoolP(getKey(i));
}


int TyphonConfig::size() {
	int i=0;
	std::map<std::string, Config*>::iterator p;
	//if(conf->)
	for (p = conf->begin(); p!=conf->end(); p++) {
		if (p->second->id>=0) i++;
	}
	return i;
}

inline const char* const BoolToString(bool b){
  return b ? "true" : "false";
}

inline const std::string FloatToString(float f) {
	std::ostringstream out;
	out << f;
	std::string str(out.str());
	return str;
}

TiXmlElement* TyphonConfig::toXML(std::string r) {
	TiXmlElement* root = new TiXmlElement(r.c_str());  
	std::map<std::string, Config*>::iterator p;
	for (p = conf->begin(); p!=conf->end(); p++) {
		if (strcasecmp(p->second->typ.c_str(),"map")!=0)	{
			std::string tagName = p->first;
			std::string typ=p->second->typ;
			int id=p->second->id;
			std::string tagValue;
			
			if (strcasecmp(typ.c_str(), "string")==0) tagValue = getString(p->first);
			if (strcasecmp(typ.c_str(), "int")==0) tagValue = getInt2String(p->first);
			if (strcasecmp(typ.c_str(), "bool")==0) tagValue = BoolToString(getBool(p->first));
			if (strcasecmp(typ.c_str(), "float")==0) tagValue = FloatToString(getFloat(p->first));
					
			TiXmlElement* key = new TiXmlElement(tagName.c_str());  
			TiXmlText * value = new TiXmlText(tagValue.c_str());
			key->SetAttribute("typ",typ.c_str());
			if (id>=0) key->SetAttribute("id",id);
			key->LinkEndChild(value);
			root->LinkEndChild(key);
		}
		else {
			std::string tagName = p->first;
			int id=p->second->id;
			TyphonConfig* tc = (TyphonConfig*) p->second;
			TiXmlElement* sroot = tc->toXML(tagName);
			sroot->SetAttribute("typ",p->second->typ);
			if (id>=0) sroot->SetAttribute("id",id);
			root->LinkEndChild(sroot);
		}
	}
	return root;
}

void TyphonConfig::parseXML(TyphonConfig* tc, TiXmlElement* pElem) {
	for (pElem=pElem; pElem; pElem=pElem->NextSiblingElement()) {
		TiXmlAttribute* pAttrib=pElem->FirstAttribute();
		std::string typ;
		int id=-1;
		while (pAttrib) {
			std::string aName = pAttrib->Name();
			std::string aValue = pAttrib->Value();
			if (strcasecmp(aName.c_str(),"typ")==0) typ=aValue;
			if (strcasecmp(aName.c_str(),"id")==0) id=atoi(aValue.c_str());
			pAttrib=pAttrib->Next();
		}
		if (strcasecmp(typ.c_str(), "map")==0) {
			TyphonConfig* stc=tc->sub(pElem->Value());
			tc->sub(pElem->Value())->id=id;
			TiXmlHandle hRoot=TiXmlHandle(pElem);
			TiXmlElement* subElem=hRoot.FirstChild().Element();
			parseXML(stc, subElem);
		}
		else
		if (strcasecmp(typ.c_str(), "string")==0) {
			std::string stringy="";
			if(pElem->GetText()!=NULL) stringy=pElem->GetText();
			tc->set(pElem->Value(), stringy,id);
		}
		else
		if (strcasecmp(typ.c_str(), "int")==0) {
			tc->set(pElem->Value(), atoi(pElem->GetText()),id);
		}
		else
		if (strcasecmp(typ.c_str(), "float")==0) {
			tc->set(pElem->Value(), (float) atof(pElem->GetText()),id);
		}
		else
		if (strcasecmp(typ.c_str(), "bool")==0) {
		const char* boolie=pElem->GetText();
		bool b=0;
		if(!strcasecmp(boolie,"true") || !strcasecmp(boolie,"yes") || !strcasecmp(boolie,"1") || !strcasecmp(boolie,"on") )		b=1;
		tc->setBool(pElem->Value(), b,id);
		}
	}
}

void TyphonConfig::saveXML(std::string filename){
	TiXmlDocument doc;  
	doc.LinkEndChild(toXML("TyphonConfig"));  
	doc.SaveFile(filename);
}

void TyphonConfig::replaceXML(std::string filename) {
	wipe();
	loadXML(filename.c_str());
}

void TyphonConfig::loadXML(std::string filename) {
	TiXmlDocument doc(filename.c_str());
	doc.LoadFile();
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if (!pElem) return;
	hRoot=TiXmlHandle(pElem);
	pElem=hRoot.FirstChild().Element();
	parseXML(this, pElem);
}

void TyphonConfig::wipe() {
	conf->clear();
}

template<class T> T TyphonConfig::addto(std::string key, T value) {
	((TConfig<T>*)get(key))->operator+(value);
	return *((TConfig<T>*)get(key))->get();
}
