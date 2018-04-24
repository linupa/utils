#ifndef _COMM_H_
#define _COMM_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define CMD_CALIBRATE				0x01000000
#define CMD_SET_TORQUE				0x02000000
#define CMD_SET_MODE				0x03000000
#define CMD_SET_MODE_RAW_TORQUE		0x03000001
#define CMD_SET_MODE_COMP_TORQUE	0x03000002
#define CMD_SET_STATE				0x04000000
#define CMD_SET_MASK				0xff000000
#define CMD_SET_CONFIGURATION		0x05000000

//#define CMD_DATA_LEN 10
#define CMD_DATA_LEN 20

#define NOTIFY_ADDR             "224.0.0.100"
#define NOTIFY_PORT             51123
#define CMD_PORT                51124
#define STT_PORT                51125
#define ORI_PORT				51126

using namespace std;

typedef struct
{
  pthread_mutex_t	*mutex;
  void				*win;
} udp_arg;

typedef struct
{
	int			index;
	int			count;
	long long	timeStamp;
	double		data[1];
} message;

#if 0
typedef struct
{
	int			command;
	int			data_len;
	long		buf[CMD_DATA_LEN];
    char        exp[CMD_DATA_LEN];

	// internal use
	char		received_check;
} command;
#else
typedef struct
{
	int			command;
	int			data_len;
	double		buf[CMD_DATA_LEN];

	// internal use
	char		received_check;
} command;
#endif

extern void *notify_address(void *arg);
extern void *receive_udp(void *arg);
extern void send_udp(message *pBuf);
extern message *allocMessage(int count);

class Message
{
public:
	int			index;
	int			size;
	long long	timeStamp;
	void		*data;
};

#define NUM_BUFFER 	(20)
class Comm
{
private:
	int socket_id;
	int portNum;
	static int index;
	struct sockaddr_in si_other;
	char *pSdBuffer;
	char **ppRdBuffer;
	int rdbufsize;
	int sdbufsize;
	pthread_t			thread_rd;
    pthread_mutex_t     mutex;
	int rd_idx;
	int wr_idx;

public:
	Comm(const char *addr, int _portNum, int size=2000);
	Comm(int _portNum, int size=2000);
	int send(Message *pPkt);
	int send(message *pPkt);
	void listen(void);
	void operator<<(Message &msg);
	~Comm();
	Message read(void);
	void reading(void);
	bool empty(void);
	int verbose;
};

#endif
