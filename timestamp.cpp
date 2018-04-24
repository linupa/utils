#include "timestamp.h"
#include <assert.h>

Timestamp::Timestamp(void)
{
	name = "---";

	return;
}

Timestamp::Timestamp(string str)
{
	name = str;

	return;
}

void Timestamp::reset(void)
{
	int i;
	
	if ( elapsedCount.size() == 0 )
		return;

	for ( i = 0 ; i < elapsedCount.size() ; i++ )
	{
		elapsedTime[i]	= 0.;
		elapsedCount[i]	= 0;
	}
}

void Timestamp::setMode(int m)
{
	mode = m;
}

void Timestamp::setBaseline(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	startSec	= tv.tv_sec;
	startUsec	= tv.tv_usec;
	curIdx = 0;
}

void Timestamp::checkElapsed(void)
{
	checkElapsed(curIdx);
	curIdx++;
}
void Timestamp::checkElapsed(int idx)
{
	struct timeval tv;
	int sec, usec;
	double dsec;

	gettimeofday(&tv, NULL);

	sec		= tv.tv_sec - startSec;
	usec	= tv.tv_usec - startUsec;

	dsec	= sec * 1000000. + usec;

	if  ( idx >= elapsedCount.size() )
	{
		for ( int i = elapsedCount.size() ; i <= idx ; i++ )
		{
			elapsedCount.push_back(0);
			elapsedTime.push_back(0.);
		}
	}
	assert(idx < elapsedCount.size() );

	if ( mode & MODE_OVERLAP )
	{
		elapsedCount[idx]	= 1;
		elapsedTime[idx]	= dsec;
	}
	else
	{
		elapsedCount[idx]	+= 1;
		elapsedTime[idx]	+= dsec;
	}
}

double Timestamp::getElapsedTime(int idx)
{
	if ( idx >= elapsedCount.size() || elapsedCount[idx] == 0 )
		return 0.;

	return elapsedTime[idx] / elapsedCount[idx];
}

int Timestamp::getElapsedCount(int idx)
{
	return elapsedCount[idx];
}

double Timestamp::baseTime = 0;
void Timestamp::resetTime(void)
{
	double ret;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	baseTime	= tv.tv_sec + tv.tv_usec*0.000001;
}
double Timestamp::getElapsedTime(void)
{
	double ret;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	ret	= tv.tv_sec + tv.tv_usec*0.000001;

	return ret - baseTime;
}

double Timestamp::getCurrentTime(void)
{
	double ret;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	ret	= tv.tv_sec + tv.tv_usec*0.000001;

	return ret;
}

void Timestamp::dump( ostream &c )
{
	int i;

	c << name; 
	double div;
	switch ( mode & 0x00F0 )
	{
		case MODE_SHOW_SEC:
			c << "(s):";
			div = 1000000.;
			break;
		case MODE_SHOW_MSEC:
			c << "(ms):";
			div = 1000.;
			break;
		default:
		case MODE_SHOW_USEC:
			c << "(us):";
			div = 1.;
			break;
	}
	for ( i = 0 ; i < elapsedTime.size() ; i++ )
	{
		if ( elapsedCount[i] > 0 )
		{
			c <<  elapsedTime[i] / elapsedCount[i] / div << " ";
			if ( mode & MODE_SHOW_COUNT )
				c << "(" << elapsedCount[i] << ") ";
		}
		else
			c << "--- " ;
	}
	c << endl;
}

ostream &operator<<(ostream &c, Timestamp &ts)
{
	ts.dump(c);
	return c;
}
