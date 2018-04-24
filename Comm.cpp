#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "Comm.h"

#define BUFLEN 1024

#if 0
void *notify_address(void *arg)
{
	int interval = (int)arg;	
	int slen;

	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	struct sockaddr_in si_to;
	memset((char *) &si_to, 0, sizeof(si_to));
	si_to.sin_family = AF_INET;
	si_to.sin_port = htons(NOTIFY_PORT);
	si_to.sin_addr.s_addr = inet_addr(NOTIFY_ADDR);

//	fprintf(stderr, "ADDR: %08x(%d:%d)\n", si_to.sin_addr.s_addr, slen, sizeof(si_to));
	while (1)
	{
		char buf[10];
		sendto(s, buf, sizeof(buf), 0, (const sockaddr*)&si_to, sizeof(si_to));
		fprintf(stderr, "NOTIFY CONTROL PC Address\n");
		sleep(interval);
	}
}
#endif

struct sockaddr_in si_other;
int slen;

int Comm::index = 0;
Comm::Comm(int _portNum, int size)
{
	socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	portNum = _portNum;
	index = 0;

	si_other.sin_addr.s_addr = htonl(INADDR_ANY);
	pSdBuffer = (char *)malloc(size);
	sdbufsize = size;
	rdbufsize = size;

	verbose = 0;
}

Comm::Comm(const char *addr, int _portNum, int size)
{
	socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	portNum = _portNum;
	index = 0;

	si_other.sin_addr.s_addr = inet_addr(addr);
	pSdBuffer = (char *)malloc(size);
	sdbufsize = size;
	rdbufsize = size;

	verbose = 0;
}
int Comm::send(Message *pPkt)
{
	if ( socket_id == 0 )
	{
		cerr << "ERROR: Socket os not initialized" << endl ;
	}

	struct sockaddr_in si_to;
	memset((char *) &si_to, 0, sizeof(si_to));
	si_to.sin_family = AF_INET;
	si_to.sin_port = htons(portNum);
	si_to.sin_addr.s_addr = si_other.sin_addr.s_addr;

//	fprintf(stderr, "ADDR: %08x(%d:%d)\n", si_to.sin_addr.s_addr, slen, sizeof(si_to));
	pPkt->index = index;

	int bufsending = sizeof(Message) - sizeof(void *) + pPkt->size;
	
	if ( sdbufsize < bufsending )
	{
		free(pSdBuffer);
		sdbufsize = sdbufsize + bufsending;
		pSdBuffer = (char *)malloc(sdbufsize);
	}
	slen = sizeof(si_to);
	memcpy(pSdBuffer, pPkt, sizeof(Message) - sizeof(void *));
	memcpy(pSdBuffer + sizeof(Message) - sizeof(void *), pPkt->data, pPkt->size);
	sendto(socket_id, pSdBuffer, bufsending, 0, (const sockaddr*)&si_to, slen);
	++index;
}

int Comm::send(message *pPkt)
{
	if ( socket_id == 0 )
	{
		cerr << "ERROR: Socket os not initialized" << endl ;
	}

	struct sockaddr_in si_to;
	memset((char *) &si_to, 0, sizeof(si_to));
	si_to.sin_family = AF_INET;
	si_to.sin_port = htons(portNum);
	si_to.sin_addr.s_addr = si_other.sin_addr.s_addr;

//	fprintf(stderr, "ADDR: %08x(%d:%d)\n", si_to.sin_addr.s_addr, slen, sizeof(si_to));
	pPkt->index = index;

	int bufsending = sizeof(message) - sizeof(void *) + pPkt->count;
	
	slen = sizeof(si_to);
	sendto(socket_id, pPkt, bufsending, 0, (const sockaddr*)&si_to, slen);
	++index;
}

void Comm::operator<<(Message &msg)
{
	send(&msg);
}
	
Comm::~Comm(void)
{
	free(pSdBuffer);
}

void *thread_start(void *instance)
{
	Comm *comm = (Comm *)instance;	

	comm->reading();	
}
void Comm::listen(void)
{
	pthread_create( &thread_rd, NULL, thread_start, (void*)this);
}

void Comm::reading(void)
{
	int i;
	struct sockaddr_in si_me;
	char *rbuf;

	ppRdBuffer = (char **)malloc(sizeof(int *)*NUM_BUFFER );
	ppRdBuffer[0] = (char *)malloc(rdbufsize * NUM_BUFFER );
	for ( i = 1 ; i < NUM_BUFFER ; i++ )
	{
		ppRdBuffer[i] = ppRdBuffer[0] + rdbufsize * i;
	}
	rd_idx = wr_idx = 0;
	pthread_mutex_init( &mutex, NULL );

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(portNum);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(socket_id, (const sockaddr *)&si_me, (socklen_t)sizeof(si_me));

	rbuf = (char *)malloc(rdbufsize);
	fprintf(stderr, "COMM THREAD started\n");
	while (1)
	{
		int len;
		len = recvfrom(socket_id, rbuf, rdbufsize, 0, (sockaddr *)&si_other, (socklen_t*)&slen);
//		fprintf(stderr, "PACKET RECEIVE... %d(%d),%d[%d],%d[%d],%d[%d],%d[%d],%d[%d] %d\n", 
//				cmd->command, cmd->data_len, cmd->buf[0], cmd->exp[0], cmd->buf[1], cmd->exp[1], 
//				cmd->buf[2], cmd->exp[2], cmd->buf[3], cmd->exp[3], cmd->buf[4], cmd->exp[4], slen);	
//		fprintf(stderr, "ADDR: %08x(%d)\n", si_other.sin_addr.s_addr, slen);
		if ( verbose )
			std::cout << "PCK RCV " << len << endl;
//		head_command[4] = buf[4] * M_PI / 180.;

		if ( len <= 0 )
			continue;

		pthread_mutex_lock(&mutex);
		
		memcpy(ppRdBuffer[wr_idx], rbuf, len);
		wr_idx = (wr_idx+1) % NUM_BUFFER;

		pthread_mutex_unlock(&mutex);
	}

	return;
}

bool Comm::empty(void)
{
	if ( wr_idx == rd_idx )
		return true;
	else
		return false;
}

Message Comm::read(void)
{
	Message ret;

	if ( wr_idx == rd_idx )
	{
		ret.size = 0;
		return ret;
	}

	pthread_mutex_lock(&mutex);
		
	memcpy((void *)&ret, (void *)ppRdBuffer[rd_idx], sizeof(Message) - sizeof(void *));
	ret.data = (void *)malloc((ret.size+3)&(~0x03));
	memcpy(ret.data, (void *)(ppRdBuffer[rd_idx]+sizeof(Message) - sizeof(void *)), ret.size);
	rd_idx = (rd_idx + 1)%NUM_BUFFER;

	pthread_mutex_unlock(&mutex);

	return ret;
}
