#include "includes.h"
#include "xmlconfig.h"

class typhonCLI
{
	public:
	typhonCLI(int argc, char* argv[]);
	TyphonConfig* getTConfig();
	std::string getString(std::string tag);
	int getInt(std::string tag);
	bool getBool(std::string tag);
	bool exists(std::string tag);

	private:
	TyphonConfig* _config;
};
