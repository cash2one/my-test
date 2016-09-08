#ifndef __COMM_MEM_CONF_H__
#define __COMM_MEM_CONF_H__

#include <sys/types.h>
typedef struct
{
		int          mem_id;       // �����ڴ�ı�ʶ
		char        mname[48];    // �����ڴ������
		unsigned int mtype;        // �����ڴ������
		unsigned int mlenth;       // �����ڴ�ĳ���
		unsigned int mwidth;       // �����ڴ�Ŀ��
		char        mflag;        // �����ڴ�Ķ�д��־
		char *       msp;          // �����ڴ�ĵ�ַ
} MEMCONF;

typedef struct 
{
		char        spname[10];   // Ӧ�ó��������
		unsigned int pmtotal;      // Ӧ�ó������õ����ڴ�����
		MEMCONF      mem[20];      // Ӧ�ó������õ��ĸ��ڴ�������Ϣ
} SPROGMEMCONF;

typedef struct
{
			char         tpname[100]; // Ҫ��ʼ�����ڴ��Ӧ�ó���      
			unsigned int  tptotal;     // ������Ҫ�õ��Ĺ����ڴ�����
			SPROGMEMCONF  pmem[10];    // Ҫ�����ĸ�Ӧ�ó����ڴ�������Ϣ
} TPROGMEMCONF;
// �������¼�����
#define MEM_TYPE0 0x0001  // �����¼����ͱ�־
#define MEM_TYPE1 0x0002  // �����¼����ͱ�־
#define MEM_TYPE2 0x0004  // �����¼����ͱ�־
#define MEM_TYPE3 0x0008  // �ֲ��¼����ͱ�־
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

// һ���������ڴ����Ĺؼ���
#define MKEYRCMD1 0x118f    // 005���̣�һ��������ģ��Ҫ�õ��Ķ�������ڴ���
#define MKEYRCMD11 0x4f84   // 005���̣�dt 2 com2, for dtreport
#define MKEYWCMD1 0x1f81    // 005���̣�һ��������ģ��Ҫ�õ��Ķ�������ڴ���
#define MKEYWEVENT12 0x1f82 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT13 0x1f83 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT14 0x1f84 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT15 0x1f85 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT16 0x1f86 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT17 0x1f87 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���
#define MKEYWEVENT18 0x1f88 // 005���̣�һ��������ģ��Ҫ�õ���д�¼����ݹ����ڴ���

/* add for idp; 2006-08-01 */
#define MKEYIPSRFLOW 0x5f83 /* ips 2 comm2 flow */
#define MKEYIPSRREPORT 0x5f84 /* ips 2 comm2 report */
#define MKEYIPSRSIGN 0x528f /* csign 2 comm2 sign */

// �����������ڴ����Ĺؼ���
#define MKEYWCMD2  0x218f   // 005���̣�����������ģ��Ҫ�õ��Ķ�������ڴ��� 
#define MKEYRCMD2  0x2f81   // 005���̣�����������ģ��Ҫ�õ��Ķ�������ڴ���
#define MKEYRSIGN2 0x1f82   // 005���̣�����������ģ��Ҫ�õ��Ķ������¼������ڴ���
#define MKEYWSIGN2 0x2f82   // 005���̣�����������ģ��Ҫ�õ���д�����¼������ڴ���
#define MKEYRFLOW2 0x1f83   // 005���̣�����������ģ��Ҫ�õ��Ķ������¼������ڴ���
#define MKEYWFLOW2 0x2f83   // 005���̣�����������ģ��Ҫ�õ���д�����¼������ڴ���
#define MKEYRDIST2 0x1f84   // 005���̣�����������ģ��Ҫ�õ��Ķ��ֲ��¼������ڴ���
#define MKEYWDIST2 0x2f84   // 005���̣�����������ģ��Ҫ�õ���д�ֲ��¼������ڴ���

// �����������ڴ����Ĺؼ���
#define MKEYRCMD3   0x318f  // 005���̣�����������ģ��Ҫ�õ��Ķ�������ڴ���
#define MKEYWCMD3   0x3f81  // 005���̣�����������ģ��Ҫ�õ��Ķ�������ڴ���
#define MKEYREVENT3 0x328f  // 005���̣�����������ģ��Ҫ�õ��Ķ��¼����ݹ����ڴ���
#define MKEYWEVENT3 0x3f82  // 005���̣�����������ģ��Ҫ�õ���д�¼����ݹ����ڴ���

// ��cliͨ�Ź����ڴ����Ĺؼ���
#define MKEYCLIRCMD   0xAB8f  // 005���̣��ӿͻ��˶�ȡ�������ݵĹ����ڴ���
#define MKEYCLIWCMD   0xAf8B  // 005���̣�д��ͻ����������ݵĹ����ڴ���
#define MKEYDATAWCMD  0xAf8C  // 005���̣�д�����ݿ�����������ݹ����ڴ���
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
