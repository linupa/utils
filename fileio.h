#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

class data_stream
{
public:
	data_stream(char *file_name);
	data_stream(void);
	~data_stream();
	int read_line(void);
	bool open(const char *file_name);
	void close();
	const vector<double> get_entry();
	void reset();

private:
	FILE			*fp;
	int				index;
	vector<double>	entry;
};

