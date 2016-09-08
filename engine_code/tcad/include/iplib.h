#ifndef __IPLIB_H__
#define __IPLIB_H__

#define IPLIB_DIR     "/tftpboot/chanct/dp_release"       // iplib�ļ�����Ŀ¼

enum
{
	IPLIB_OK,
	IPLIB_FAIL,
	IPLIB_FILE_FAIL,            /* ��IP��ַ���ļ�ʧ�� */
};

typedef struct iplib_info_tag_s
{
	unsigned short country;      /* ���� */
	unsigned short isp_id;      /* ��Ӫ��ID */
	unsigned int province_id;   /* ʡ��ֱϽ�С�������ID */
	unsigned int city_id;       /* ����ID */
}iplib_info_t;

typedef struct ipinfo_tag_s
{
	uint32_t ip_start;
	uint32_t ip_end;
	
	uint32_t province;
	uint32_t city;

    uint32_t index;
	uint16_t service;
	uint16_t country;
}ipinfo_t;

/*
 * ����IP��ַ������Ŀ¼
 * ������unsigned char *dirĿ¼���999�ֽ�
 * ����ֵ��IPLIB_OK�ɹ���IPLIB_FAILʧ��
 */
uint32_t iplib_set_dir(char *dir);

/*
 * IP��ַ���ʼ��
 * ����ֵ��IPLIB_OK�ɹ���IPLIB_FAILʧ��
 */
unsigned int iplib_init(void);

/*
 * IP��ַ������
 */
void iplib_destroy(void);

/*
 * IP��ַ��ѯ
 * ������unsigned int ipΪ����ֵ��IPv4��ַ�������ֽ���
 *       iplib_info_t *ipinfoΪ���ֵ��ip��ַ��Ӧ����Ϣ
 * ����ֵ��IPLIB_OK���ҳɹ���IPLIB_FAIL����ʧ��
 */
unsigned int iplib_find(unsigned int ip, iplib_info_t *ipinfo);

/*
 * IP��ַ����
 * ������ipinfo_t *iiΪ����ֵ��ip��ַ��Ӧ����Ϣ
 * ����ֵ��IPLIB_OK����ɹ���IPLIB_FAIL����ʧ��
 */
uint32_t iplib_insert(ipinfo_t *ii);

/*
 * IP��ַɾ��
 * ������ipinfo_t *iiΪ����ֵ��ip��ַ��Ӧ����Ϣ
 * ����ֵ��IPLIB_OKɾ���ɹ���IPLIB_FAILɾ��ʧ��
 */
uint32_t iplib_delete(ipinfo_t *ii);

#endif