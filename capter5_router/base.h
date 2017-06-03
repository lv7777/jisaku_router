typedef struct{
    int soc;
    u_char hwaddr[6];
    struct in_addr addr,subnet,netmask;
}DEVICE;

#define FLAG_FREE 0
#define FLAG_OK 1
#define FLAG_NG -1

typedef struct _data_buf_{
    _data_buf_ *before;
    _data_buf_ *after;
    time_t t;
    int size;
    unsigned char *data;
}DATA_BUF;

typedef struct{
    DATA_BUF *top
    DATA_BUF *bottom;
    unsigned long dno;
    unsigned long inBucketSize;//??
    pthread_mutex_t mutex;
}SEND_DATA;

typedef struct {
    int flag;
    in_addr_t addr;
    unsigned char hwaddr[6];
    time_t lasttime;
}IP2MAC;