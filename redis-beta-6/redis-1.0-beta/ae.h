#ifndef __AE_H__
#define __AE_H__

struct aeEventLoop;

/* Types and data structures */
// 
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);

/* File event structure */
typedef struct aeFileEvent {
    int fd;
    int mask; /* one of AE_(READABLE|WRITABLE|EXCEPTION) */
    aeFileProc *fileProc;				  // 处理客户端发过来的数据
    aeEventFinalizerProc *finalizerProc;  //最后确定的文件处理毁掉函数
    void *clientData;					  // 客户端法国来的数据
    struct aeFileEvent *next;			  // 指向下一个要处理的事件 轮询处理事件
} aeFileEvent;

/* Time event structure */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */		//时间事件的文件id
    long when_sec; /* seconds */				//秒
    long when_ms; /* milliseconds */				//毫秒
    aeTimeProc *timeProc;						//事件处理的程序
    aeEventFinalizerProc *finalizerProc;		//最终事件处理的程序
    void *clientData;							//客户端发过来的数据  注意这里的数据并没有长度
    struct aeTimeEvent *next;					//指向下一个时间事件结构体
} aeTimeEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    long long timeEventNextId;
    aeFileEvent *fileEventHead;
    aeTimeEvent *timeEventHead;
    int stop;
} aeEventLoop;

/* Defines */
#define AE_OK 0
#define AE_ERR -1

#define AE_READABLE 1
#define AE_WRITABLE 2
#define AE_EXCEPTION 4

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

/* Prototypes */
aeEventLoop *aeCreateEventLoop(void);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
void aeMain(aeEventLoop *eventLoop);

#endif
