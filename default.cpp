#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "default.h"

using namespace std;

char*					Default::filename;
map<string, int>		Default::map_type;
map<string, int>		Default::map_length;
map<string, int *>		Default::map_int;
map<string, double *>	Default::map_real;
map<string, char *> 	Default::map_str;

Default::Default(void)
{
	filename = NULL;
}

Default::Default(const char *appname)
{
	load(appname);
}

void Default::parseXml(const TiXmlNode *node)
{
	do 
	{
		string 	label;
		char 	*value;
		value = (char *)malloc(1000*sizeof(char));
//		cerr << "Type:" << node->Type() << " " << node->Value() << endl;
		switch ( node->Type() )
		{
		case TiXmlNode::TINYXML_ELEMENT:
			{
				int		type;
				int 	length;
				int 	value_length;
				label = node->Value();
				if ( (value_length = strlen(node->FirstChild()->Value())) > 1000 )
				{
					free(value);
					value = (char *)malloc(value_length*sizeof(char));
				}
				strcpy(value, node->FirstChild()->Value());
//				cerr << label << " " << value << endl;
				
				map<string, int>::iterator it = map_type.find(label);
				map<string, int>::iterator it2 = map_length.find(label);
//				cerr << label << " " << endl;

				if ( it == map_type.end() )
				{
//					cerr << "Unused label" << endl;
					break;
				}
				type = it->second;
				length = it2->second;
//				cerr << label << " " << type << " " << length << endl;

				char *t;

				t = strtok(value, ",");
				int index = 0;
				union ds
				{
					double	*ptr_r;
					int		*ptr_i;
					char	*ptr_c;
				};
				ds _ds;
				switch ( type )
				{
				case TYPE_INT:
					_ds.ptr_i = (map_int.find(label))->second;
					break;
				case TYPE_REAL:
					_ds.ptr_r = (map_real.find(label))->second;
					break;
				case TYPE_STR:
					_ds.ptr_c = (map_str.find(label))->second;
					break;
				}
				while ( t != NULL && index < length)
				{
					double	dValue;
					int		iValue; 
					switch ( type )
					{
					case TYPE_INT:
						iValue = atoi(t);
//						cerr << iValue << endl;
						_ds.ptr_i[index] = iValue;
						break;
					case TYPE_REAL:
						dValue = atof(t);
//						cerr << dValue  << endl;
						_ds.ptr_r[index] = atof(t);
						break;
					case TYPE_STR:
						strncpy(_ds.ptr_c, t, length);
						break;
					}
					t = strtok(NULL, ",");
					index++;
				}
			}
			break;

		case TiXmlNode::TINYXML_DOCUMENT:
		case TiXmlNode::TINYXML_DECLARATION:
			if ( node->FirstChild() )
				parseXml(node->FirstChild());
			break;

		}
		free(value);
	}
	while ( node = node->NextSibling() );
//	cerr << "return" << endl;
}

void Default::load(const char *appname)
{
	char *name;
//	cerr << appname << endl;

	name = (char *)malloc((strlen(appname)+1)*sizeof(char));

	strcpy(name, appname);
	
	char *t, *pre, *post;
	
	t = strtok(name, "/");
	pre = t;
	do  
	{
		pre = t;
		t = strtok(NULL, "/");
	} while ( t );

	post = strtok(pre, ".");


//	cerr << "full name: " << appname << endl;

	filename = (char *)malloc((strlen(post)+2)*sizeof(char));
	filename[0] = '.';
	strcpy(filename+1, post);
	free(name);

//	cerr << "Config filename: " << filename << endl;

	TiXmlDocument doc;

	try
	{
		doc = TiXmlDocument( filename );
		doc.LoadFile();
	}
	catch( int e) 
	{
//		cerr << "No config file" << endl;
	}

	parseXml(&doc);
//	cerr << "config file done" << endl;



}

void Default::save(void)
{
	FILE *fp;

	fp = fopen(filename, "w");
	map<string, int *>::iterator iti = map_int.begin();
	for ( ; iti != map_int.end() ; iti++ )
	{
		string label;
		int length;

		label = iti->first;
		map<string, int>::iterator itl = map_length.find(label);
		length	= itl->second;

		int *value = iti->second;
		fprintf(fp, "<%s> ", label.c_str());
		for ( int j = 0 ; j < length ; j++ )
		{
			if ( j > 0 )
				fprintf(fp, ", ");
			fprintf(fp, "%d", value[j]);
		}
		fprintf(fp, " </%s>\n", label.c_str());
	}
	map<string, double *>::iterator itr = map_real.begin();
	for ( ; itr != map_real.end() ; itr++ )
	{
		string label;
		int length;

		label = itr->first;
		map<string, int>::iterator itl = map_length.find(label);
		length	= itl->second;

		double *value = itr->second;
		fprintf(fp, "<%s> ", label.c_str());
		for ( int j = 0 ; j < length ; j++ )
		{
			if ( j > 0 )
				fprintf(fp, ", ");
			fprintf(fp, "%f", value[j]);
		}
		fprintf(fp, " </%s>\n", label.c_str());
	}
	map<string, char *>::iterator its = map_str.begin();
	for ( ; its != map_str.end() ; its++ )
	{
		string label;
		int length;

		label = its->first;
		map<string, int>::iterator itl = map_length.find(label);
		length	= itl->second;

		char *value = its->second;
		fprintf(fp, "<%s> ", label.c_str());
		fprintf(fp, "%s", value);
		fprintf(fp, " </%s>\n", label.c_str());
	}

	fclose(fp);
}

void Default::_setMap(const char *label, int &value, int length)
{
	map_type.insert(make_pair(label, TYPE_INT));
	map_length.insert(make_pair(label, length / sizeof(int)));
	map_int.insert(make_pair(label, &value));
}
void Default::_setMap(const char *label, double &value, int length)
{
	map_type.insert(make_pair(label, TYPE_REAL));
	map_length.insert(make_pair(label, length / sizeof(double)));
	map_real.insert(make_pair(label, &value));
}
void Default::_setMap(const char *label, char *value, int length)
{
//	cerr << label << endl;
//	cerr << length << endl;
	map_type.insert(make_pair(label, TYPE_STR));
	map_length.insert(make_pair(label, length / sizeof(char)));
	map_str.insert(make_pair(label, value));
}
void Default::_setMap(const char *label, double *value, int length)
{
	cerr << label << endl;
	cerr << length << endl;
	map_type.insert(make_pair(label, TYPE_REAL));
	map_length.insert(make_pair(label, length / sizeof(double)));
	map_real.insert(make_pair(label, value));
}
void Default::_setMap(const char *label, int *value, int length)
{
//	cerr << label << endl;
//	cerr << length << endl;
	map_type.insert(make_pair(label, TYPE_INT));
	map_length.insert(make_pair(label, length / sizeof(int)));
	map_int.insert(make_pair(label, value));
}
