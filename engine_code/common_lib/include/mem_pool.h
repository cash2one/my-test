#ifndef _MEM_POOL_H
#define _MEM_POOL_H

#include <sys/types.h>

typedef struct _mempool_t
{
	int total_num;	// total num
	int unit_size;	// include the size of header
	int data_size;	// the size of pure data

	// ˫��ѭ������
	void	*used_head;	// ʹ�ù������ݱ�ͷ
	int used_num;	// ʹ�ù������ݸ���

	// ˫��ѭ������
	void	*free_head;	// δʹ�õ����ݱ�ͷ
	int free_num;	// δʹ�õ����ݸ���

	unsigned char *bucket_pool;	// ʵ�ʴ�����ݵ�λ��
} mempool_t;

/*
 * ���ܣ���ʼ��
 * ʧ�ܣ�����NULL
 * �ɹ�������mempoolָ��
 */
mempool_t *mempool_init(int nmemb, int unit_size);

/*
 * ���ܣ����
 */
void mempool_clean(mempool_t *mempool);

/************************************************
 *         Name: mempool_reset
 *  Description: ���mempool�ĸ�λ����
 ************************************************/
void mempool_reset(mempool_t *mempool);

/*
 * ���ܣ��ӹ����ڴ��з���һ��ռ�
 * ʧ�ܣ�����NULL
 * �ɹ�����������Ŀռ�
 */
void *mempool_malloc(mempool_t *mempool);

/*
 * ���ܣ��ӹ����ڴ��з���һ��ռ䣬��տռ�
 * ʧ�ܣ�����NULL
 * �ɹ�����������Ŀռ�
 */
void *mempool_calloc(mempool_t *mempool);

/*
 * ���ܣ��ͷ�data��ָ��Ŀռ�
 */
void mempool_free(mempool_t *mempool, void *data);

/* 
 * ��ȡ�������������
 */
void *mempool_oldest_data(mempool_t *mempool);

/* 
 * ��ȡ�������������
 */
void *mempool_newest_data(mempool_t *mempool);

/*
 * ���ܣ�����ռ�����ڵ㣬��ÿ��Ԫ��ִ��node_action����
 * data��ÿ���ڵ�����ݣ���Ҫ��ʹ���߽���ָ��
 * arg��������Ҫ����������
 * ����ֵ��0��ֹͣ������1����������
 */
typedef int (*node_action)(void *data,void *arg);
#define OLD_DIR 0
#define NEW_DIR 1
/* 
 * direction: 0 �������ݷ����ƶ� 1 �������ݷ����ƶ�
 * begin_data: ��ʼλ����ָ�������
 */
int mempool_iterate(mempool_t *mempool, void *begin_data, int direction, 
				node_action action, void *arg);

/*
 * ���ܣ���ӡmempoolռ�����ڵ����Ϣ��
 * verbose=0��ֻ��ӡmempool�ṹ��Ϣ�� 
 * verbose=1�򲻽��ܹ���ӡmempool�ṹ��Ϣ��
 * 		�����ܹ���ӡÿ���ڵ����Ϣ
 */
void mempool_print(mempool_t *mempool, int verbose);

/*
 * ���ܣ�����id��ȡ�ڵ��data��
 * �����idδʹ�ã��򷵻�NULL
 */
void *mempool_dataget_byid(mempool_t *mempool, int id);

/*
 * ���ܣ��鿴id��ָ��Ľڵ��Ƿ��Ѿ�ʹ��
 */ 
int mempool_dataused(mempool_t *mempool, int id);

/*
 * ���ܣ�ȡ��ռ�����ĵ�һ���ڵ�
 */
void *mempool_dataget(mempool_t *mempool);

#endif
