#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>

using namespace std;

class Timestamp
{
private:
	int startSec;
	int startUsec;
	int curIdx;
	int mode;
	string name;
	vector<double>	elapsedTime;
	vector<int>		elapsedCount;

public:
	enum { 
		MODE_OVERLAP	= 0x0001,
		MODE_SHOW_USEC	= 0x0000,
		MODE_SHOW_MSEC	= 0x0010,
		MODE_SHOW_SEC	= 0x0020,
		MODE_SHOW_COUNT	= 0x0100
	};
	Timestamp(void);
	Timestamp(string str);
	void reset(void);
	void setBaseline(void);
	void setMode(int m);
	void checkElapsed(void);
	void checkElapsed(int idx);

	double getElapsedTime(int idx);
	int getElapsedCount(int idx);

	void dump( ostream &c );
	static double baseTime;
	static void resetTime(void);
	static double getCurrentTime(void);
	static double getElapsedTime(void);
};

ostream &operator<<(ostream &c, Timestamp &ts);

#endif
