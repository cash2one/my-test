#ifndef _SNIFF_CAP_H
#define _SNIFF_CAP_H

/*
 * sniffer 的文件格式都统一在该头文件中定义
 */

/* 
 * NAT SNIFFER PRO .CAP FORMAT
 * +------------+
 * | file_head  |
 * +------------+
 * | item1 head |
 * +------------+
 * | item1 data |
 * +------------+
 * | item2 head |
 * +------------+
 * | item2 data |
 * +------------+
 * |    ...     |
 * +------------+
 * |    ...     |
 * +------------+
 * | itemN head |
 * +------------+
 * | itemN data |
 * +------------+
 */

#define PACKED __attribute__ ((__packed__))

struct PACKED sniff_file_head {
	char title[12];		/* "XCP.002.002" */
	unsigned int file_time;
	int item_num;
	char recv1[8];		/* { 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00 } */
	int file_len;
	int flag;		/* flag = file_len - proto_code */
	char recv2[92];

#define SNIFF_FILE_TITLE "XCP\000002""\x2e""002\000"
#define SNIFF_FILE_RECV1 "\x00\x00\x80\x00\x80\x00\x00\x00"

#define SNIFF_FILE_RECV2 "\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\xE1\xF5\x05"\
			"\x00\x00\x00\x00\x00\x00\x00\x00\x14"\
			"\x00\x04\x00\x99\x9E\x36\x00\x00\x00"\
		   	"\x00\x00\x00\x00\x00\x00\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\x00\x00\x00"\
			"\x00\x00\x00\x00\x00\x00\x01\x00\xF8\xFF"
};

struct PACKED sniff_item_head{
	unsigned long long time_offset;  /* 单位为微秒 */
	short pack_len;
	short pack_len1;
	char resv2[11];		/* SNIFF_ITEM_RESV2 */
	int flag:8;
	char resv3[16];		/* SNIFF_ITEM_RESV3 */

#define SNIFF_ITEM_RESV2 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define SNIFF_ITEM_RESV3 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
};

#endif
