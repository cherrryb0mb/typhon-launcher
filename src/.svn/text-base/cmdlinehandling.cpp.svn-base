#include "cmdlinehandling.h"

bool isNumber(char* text)
{
	for (unsigned int i=0; i< strlen(text); i++)
	{
		if (text[i]<48 || text[i]>57) return false;
	}
	return true;
}

typhonCLI::typhonCLI(int argc, char* argv[])
{
	_config = new TyphonConfig();
	for (int i=0; i<argc; i++)
	{
		int firstSign=argv[i][0];
		if (firstSign==45) {
			char* tagName=(argv[i]+1);
			if (i>argc-2||argv[i+1][0]==45)
			{
				_config->setBool(tagName,1);
			}
			else
			{
				char* tagValue=argv[++i];
				if (isNumber(tagValue)) _config->set(tagName,atoi(tagValue));
				else _config->set(tagName,tagValue);
			}
		}
	}
}

TyphonConfig* typhonCLI::getTConfig()
{
	return _config;
}

bool typhonCLI::exists(std::string tag)
{
	return _config->exists(tag);
}

std::string typhonCLI::getString(std::string tag)
{
	return _config->getString(tag);
}

int typhonCLI::getInt(std::string tag)
{
	return _config->getInt(tag);
}

bool typhonCLI::getBool(std::string tag)
{
	return _config->getBool(tag);
}
