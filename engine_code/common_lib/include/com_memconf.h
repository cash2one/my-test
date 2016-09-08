#ifndef __COMM_MEM_CONF_H__
#define __COMM_MEM_CONF_H__

#include <sys/types.h>
typedef struct
{
		int          mem_id;       // 共享内存的标识
		char        mname[48];    // 共享内存的名字
		unsigned int mtype;        // 共享内存的类型
		unsigned int mlenth;       // 共享内存的长度
		unsigned int mwidth;       // 共享内存的宽度
		char        mflag;        // 共享内存的读写标志
		char *       msp;          // 共享内存的地址
} MEMCONF;

typedef struct 
{
		char        spname[10];   // 应用程序的名称
		unsigned int pmtotal;      // 应用程序所用到的内存总数
		MEMCONF      mem[20];      // 应用程序所用到的各内存配置信息
} SPROGMEMCONF;

typedef struct
{
			char         tpname[100]; // 要初始共享内存的应用程序      
			unsigned int  tptotal;     // 程序所要用到的共享内存总数
			SPROGMEMCONF  pmem[10];    // 要包含的各应用程序内存配置信息
} TPROGMEMCONF;
// 缓冲区事件类型
#define MEM_TYPE0 0x0001  // 命令事件类型标志
#define MEM_TYPE1 0x0002  // 特征事件类型标志
#define MEM_TYPE2 0x0004  // 流量事件类型标志
#define MEM_TYPE3 0x0008  // 分布事件类型标志
#define MEM_TYPE4 0x0010  // reserved
#define MEM_TYPE5 0x0020  // reserved
#define MEM_TYPE6 0x0040  // reserved
#define MEM_TYPE7 0x0080  // reserved
#define MEM_TYPE8 0x0100  // reserved
#define MEM_TYPE9 0x0200  // reserved
#define MEM_TYPEA 0x0400  // reserved
#define MEM_TYPEB 0x0800  // reserved
#define MEM_TYPEC 0x1000  // reserved
#define MEM_TYPED 0x2000  // reserved
#define MEM_TYPEE 0x4000  // reserved
#define MEM_TYPEF 0x8000  // reserved

// 一级各共享内存区的关键字
#define MKEYRCMD1 0x118f    // 005工程：一级主处理模块要用到的读命令共享内存区
#define MKEYRCMD11 0x4f84   // 005工程：dt 2 com2, for dtreport
#define MKEYWCMD1 0x1f81    // 005工程：一级主处理模块要用到的读命令共享内存区
#define MKEYWEVENT12 0x1f82 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT13 0x1f83 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT14 0x1f84 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT15 0x1f85 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT16 0x1f86 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT17 0x1f87 // 005工程：一级主处理模块要用到的写事件数据共享内存区
#define MKEYWEVENT18 0x1f88 // 005工程：一级主处理模块要用到的写事件数据共享内存区

/* add for idp; 2006-08-01 */
#define MKEYIPSRFLOW 0x5f83 /* ips 2 comm2 flow */
#define MKEYIPSRREPORT 0x5f84 /* ips 2 comm2 report */
#define MKEYIPSRSIGN 0x528f /* csign 2 comm2 sign */

// 二级各共享内存区的关键字
#define MKEYWCMD2  0x218f   // 005工程：二级主处理模块要用到的读命令共享内存区 
#define MKEYRCMD2  0x2f81   // 005工程：二级主处理模块要用到的读命令共享内存区
#define MKEYRSIGN2 0x1f82   // 005工程：二级主处理模块要用到的读特征事件共享内存区
#define MKEYWSIGN2 0x2f82   // 005工程：二级主处理模块要用到的写特征事件共享内存区
#define MKEYRFLOW2 0x1f83   // 005工程：二级主处理模块要用到的读流量事件共享内存区
#define MKEYWFLOW2 0x2f83   // 005工程：二级主处理模块要用到的写流量事件共享内存区
#define MKEYRDIST2 0x1f84   // 005工程：二级主处理模块要用到的读分布事件共享内存区
#define MKEYWDIST2 0x2f84   // 005工程：二级主处理模块要用到的写分布事件共享内存区

// 三级各共享内存区的关键字
#define MKEYRCMD3   0x318f  // 005工程：三级主处理模块要用到的读命令共享内存区
#define MKEYWCMD3   0x3f81  // 005工程：三级主处理模块要用到的读命令共享内存区
#define MKEYREVENT3 0x328f  // 005工程：三级主处理模块要用到的读事件数据共享内存区
#define MKEYWEVENT3 0x3f82  // 005工程：三级主处理模块要用到的写事件数据共享内存区

// 和cli通信共享内存区的关键字
#define MKEYCLIRCMD   0xAB8f  // 005工程：从客户端读取命令数据的共享内存区
#define MKEYCLIWCMD   0xAf8B  // 005工程：写入客户端命令数据的共享内存区
#define MKEYDATAWCMD  0xAf8C  // 005工程：写入数据库管理器的数据共享内存区
#define MKEYRCSIGN    0x5f81
#define MKEYRDT	      0x4f81

// add for log ;IPS6050
#define MKEYAUDIT	0x6f81	// auditlog 
#define MKEYRUNNING	0x6f82	// runninglog
#define MKEYEVENT	0x6f83	// eventlog

#ifdef __cplusplus
extern "C"{
#endif
int ComInitMem(TPROGMEMCONF *memassign,char *memfile);
char *ComGetMemStartAddr(TPROGMEMCONF *memassign, int key);
int ComPutMem(char *aMem, char *aBuf, unsigned short mlen, unsigned short mtype, unsigned int user);
int ComGetMem(char *aMem, char *aBuf, unsigned short *mlen, unsigned short *mtype,unsigned int *user);

#ifdef __cplusplus
}
#endif
/*
int32_t init_mem(int8_t *memfile);
int8_t *get_mem_addr(int32_t key);
*/
#endif
