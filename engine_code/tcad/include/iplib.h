#ifndef __IPLIB_H__
#define __IPLIB_H__

#define IPLIB_DIR     "/tftpboot/chanct/dp_release"       // iplib文件所在目录

enum
{
	IPLIB_OK,
	IPLIB_FAIL,
	IPLIB_FILE_FAIL,            /* 打开IP地址库文件失败 */
};

typedef struct iplib_info_tag_s
{
	unsigned short country;      /* 国家 */
	unsigned short isp_id;      /* 运营商ID */
	unsigned int province_id;   /* 省、直辖市、自治区ID */
	unsigned int city_id;       /* 地市ID */
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
 * 设置IP地址库所在目录
 * 参数：unsigned char *dir目录，最长999字节
 * 返回值：IPLIB_OK成功；IPLIB_FAIL失败
 */
uint32_t iplib_set_dir(char *dir);

/*
 * IP地址库初始化
 * 返回值：IPLIB_OK成功；IPLIB_FAIL失败
 */
unsigned int iplib_init(void);

/*
 * IP地址库销毁
 */
void iplib_destroy(void);

/*
 * IP地址查询
 * 参数：unsigned int ip为输入值，IPv4地址，主机字节序；
 *       iplib_info_t *ipinfo为输出值，ip地址对应的信息
 * 返回值：IPLIB_OK查找成功；IPLIB_FAIL查找失败
 */
unsigned int iplib_find(unsigned int ip, iplib_info_t *ipinfo);

/*
 * IP地址插入
 * 参数：ipinfo_t *ii为输入值，ip地址对应的信息
 * 返回值：IPLIB_OK插入成功；IPLIB_FAIL插入失败
 */
uint32_t iplib_insert(ipinfo_t *ii);

/*
 * IP地址删除
 * 参数：ipinfo_t *ii为输入值，ip地址对应的信息
 * 返回值：IPLIB_OK删除成功；IPLIB_FAIL删除失败
 */
uint32_t iplib_delete(ipinfo_t *ii);

#endif