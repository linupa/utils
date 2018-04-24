#include "fileio.h"

data_stream::data_stream(char *file_name)
{
	open(file_name);
}

data_stream::data_stream(void)
{
	fp = NULL;
	index = 0;
}


int data_stream::read_line(void)
{
	int num = 0;
	bool done = false;

	entry.clear();

	char buffer[1025], *tok;

	if ( fp == NULL )
		return num;

	while ( !done )
	{
//		fprintf(stderr, "INDEX: %d\n", index);
		fseek(fp, index, SEEK_SET);
		int length = fread(buffer, 1, 1024, fp);
//		fprintf(stderr, "LENGTH: %d\n", length);
		buffer[length] = '\0';
		char *nl = strchr(buffer, '\n');
		
		if ( length == 0 )
			break;

		if ( nl == NULL )
		{
			char *sp = strrchr(buffer, ' ');

			index += sp - buffer + 1;
			*sp = '\0';

			if ( length != 1024 )
				done = true;
		}
		else
		{
			done = true;
			index += nl - buffer + 1;
			*nl = '\0';
		}

	//	cerr << index << endl;
		tok = strtok(buffer, " \t");
		while ( tok )
		{
			double value = atof(tok);

			entry.push_back(value);
			tok = strtok(NULL, " \t");

//			printf("%f ", value);
		}
	}
//	printf("\n");

	return entry.size();
}

const vector<double> data_stream::get_entry(void)
{
	return entry;
}

data_stream::~data_stream()
{
	cerr << "Destroy data stream" << endl;
	entry.clear();
	close();
}

bool data_stream::open(const char *file_name)
{
	bool ret;
	fp = NULL;
	if ( (fp = fopen(file_name, "r")) == NULL )
	{
		cerr << "FILE OPEN ERROR!!!" << endl;
		ret = false;
	}
	else
	{
		cerr << "FILE " << file_name << " OPENED..." << endl;
		ret = true;
	}
	
	index = 0;

	return ret;
}

void data_stream::reset(void)
{
	fseek(fp, 0, SEEK_SET);
}

void data_stream::close()
{
	if ( fp )
		fclose(fp);

	fp = NULL;
}
