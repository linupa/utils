#include "tinyxml.h"
#include <string>
#include <map>
#include <vector>

using namespace std;


#define RegisterValue(x) Default::_setMap(#x, x, sizeof(x))
class Default
{
public:
	static char *filename;
	Default(void);
	Default(const char *appname);

	enum
	{
		TYPE_STR,
		TYPE_INT,
		TYPE_REAL,
	};
	static void parseXml(const TiXmlNode *node);
	static void load(const char *appname);
	static void save(void);
	static void _setMap(const char *label, int &value, int length);
	static void _setMap(const char *label, double &value, int length);
	static void _setMap(const char *label, char *name, int length);
	static void _setMap(const char *label, int *value, int length);
	static void _setMap(const char *label, double *value, int length);
	static map<string, int>			map_type;
	static map<string, int>			map_length;
	static map<string, int *>		map_int;
	static map<string, double *>	map_real;
	static map<string, char *> 		map_str;

};
