/*
 * cap_record.c
 *
 * Author: Grip2
 * Date: 2003/04/16
 * Remark:
 * Last: 2003/04/20
 */ 

#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "cap_record.h"
//#include "misc.h"
#include "sniff_cap.h"
//#include "timer.h"
//#include "trace_api.h"

#define EMSG printf
#define DMSG printf
#define OUT_CAP_FILE "gsnif.cap"
#define DEFAULT_RECORD_NUM 65535 
//added by duanjigang@2009-04-16 --start
//#define _SPECIAL_DRV_
//#include "dfp_ether_decode.h"
//extern  int use_special_mac;
//added by duanjigang@2009-04-16 --end

#if 0

============================================ changed into sniff_cap.h ===========================

/* NAT SNIFFER PRO .CAP FORMAT
 * +------------+
 * | file head  |
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

__attribute__ ((__packed__))
struct sniff_file_head {
	char title[12];		
	int file_time;
	int item_num;
	char recv1[8];	
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

struct __attribute__ ((__packed__)) sniff_item_head
{
	int time_offset;
	char resv1[4];
	short pack_len;
	short pack_len1;
	char resv2[11];		/* SNIFF_ITEM_RESV2 */
	int flag:8;
	char resv3[16];		/* SNIFF_ITEM_RESV3 */

#define SNIFF_ITEM_RESV1 "\x00\x00\x00\x00"
#define SNIFF_ITEM_RESV2 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define SNIFF_ITEM_RESV3 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
};
#endif

typedef struct __record {
	struct sniff_file_head file_head;
	int fd;
	int save_guard; /* 0 is close, >0 is open */
} record_t;

 /* print error info to stderr */ 
int prnerr(const char *format, ...)
{
        va_list ap;
        int ret;

        va_start(ap, format);
        ret = vfprintf(stderr, format, ap);
        va_end(ap);

        return ret;
}

static inline void init_sniff_file_head(struct sniff_file_head *p_sfh)
{
	p_sfh->file_time = time(NULL);
	p_sfh->item_num = 0;
	p_sfh->file_len = sizeof(struct sniff_file_head);
	p_sfh->flag = p_sfh->file_len - 16;

	/* fixed section */
	memcpy(p_sfh->title, SNIFF_FILE_TITLE, sizeof(p_sfh->title));
	memcpy(p_sfh->recv1, SNIFF_FILE_RECV1, sizeof(p_sfh->recv1));
	memcpy(p_sfh->recv2, SNIFF_FILE_RECV2, sizeof(p_sfh->recv2));
}

static inline void init_sniff_item_head(struct sniff_item_head *p_sih)
{
//	memcpy(p_sih->resv1, SNIFF_ITEM_RESV1, sizeof(p_sih->resv1));
	memcpy(p_sih->resv2, SNIFF_ITEM_RESV2, sizeof(p_sih->resv2));
	memcpy(p_sih->resv3, SNIFF_ITEM_RESV3, sizeof(p_sih->resv3));
	p_sih->pack_len = 0;
	p_sih->pack_len1 = 0;
}

static inline int save_raw_pkt(record_t *rdp, const unsigned char *raw_pkt, int raw_pkt_len)
{
	struct sniff_item_head item_head;

	if (rdp->save_guard == 0) 
		goto exit;

	/* file head struct */
	rdp->file_head.item_num++;
	rdp->file_head.file_len += sizeof(item_head) + raw_pkt_len;
	rdp->file_head.flag += rdp->file_head.file_len - 16; /* ? */
	
	/* data item head */
	init_sniff_item_head(&item_head);
	item_head.pack_len = raw_pkt_len;
	item_head.pack_len1 = raw_pkt_len;
//	item_head.time_offset = time(NULL) - rdp->file_head.file_time;
//	changed according to rpe_action
	item_head.time_offset = (get_time() - (rdp->file_head).file_time) * 1000000;
	item_head.flag = 0; /* ? */

	/* save buff to file */
	lseek(rdp->fd, 0, SEEK_SET);
	write(rdp->fd, &rdp->file_head, sizeof(rdp->file_head));
	lseek(rdp->fd, 0, SEEK_END);
	write(rdp->fd, &item_head, sizeof(item_head));
	//added by duanjigang@2009-04-16 --start
	//if(use_special_mac)
	//{	
	//	write(rdp->fd, special_mac, 14);
	//	write(rdp->fd, raw_pkt+14, raw_pkt_len - 14);
//	}else
//	{
		write(rdp->fd, raw_pkt, raw_pkt_len);
//	}
	//added by duanjigang@2009-04-16 --end
exit:
	return rdp->save_guard--;
}

long record_open(char *capfile, int mode, int num)
{
	record_t *rdp = NULL;
	int len;

	rdp = (record_t *) malloc(sizeof(record_t));
	if (!rdp) {
		perror("malloc for record");
		goto err;
	}
	

	switch (mode) {
	case G_RECORD_RECORD:			
		if (capfile != NULL && capfile[0] != 0) {
			rdp->fd = open(capfile, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
			if (rdp->fd < 0) {
				perror(capfile);
				goto err;
			}
		} else {
			rdp->fd = open(OUT_CAP_FILE, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
			if (rdp->fd < 0) {
				perror(OUT_CAP_FILE);
				goto err;
			}
		}

		init_sniff_file_head(&(rdp->file_head));

		if (num > 0)
			rdp->save_guard = num;
		else
			rdp->save_guard = DEFAULT_RECORD_NUM;
		
		break;
	case G_RECORD_PLAY:
		if (capfile != NULL && capfile[0] != 0) {
			rdp->fd = open(capfile, O_RDONLY);
			if (rdp->fd < 0) {
				perror(capfile);
				goto err;
			}
		} else {
			rdp->fd = open(OUT_CAP_FILE, O_RDONLY);
			if (rdp->fd < 0) {
				perror(OUT_CAP_FILE);
				goto err;
			}
		}

		len = sizeof(struct sniff_file_head);
		if (read(rdp->fd, &rdp->file_head, len) != len) {
			prnerr("%s bad file\n", capfile);
			goto err;
		}

		DMSG("-------%d----\n", rdp->file_head.item_num);
		if (memcmp(&rdp->file_head, SNIFF_FILE_TITLE, 3) != 0) {
			prnerr("%s unknown format\n", capfile);
			goto err;
		}

		if (num > 0)
			rdp->save_guard = num;
		else
			rdp->save_guard = rdp->file_head.item_num;
		
		break;
	default:
		goto err;
	}

	return (long) rdp;
err:
	if (rdp) 
		free(rdp);
	return -1;
}

int record_record(long handle, const unsigned char *raw_pkt, int raw_pkt_len)
{
	return save_raw_pkt((record_t *) handle, raw_pkt, raw_pkt_len);
}

int record_close(long handle)
{
	record_t *rdp = (record_t *) handle;
	
	close(rdp->fd);
	free(rdp);
	return 0;
}

int record_play(long handle, unsigned char *buf, int size)
{
	record_t *rdp = (record_t *) handle;
	struct sniff_item_head item_head;
	int len = 0;
	int try = 0;

	DMSG("============rdp->save_guard:%d=========\n", rdp->save_guard);
	if (rdp->save_guard-- <= 0) {
		return 0;
	}

	while (1) {
		len = sizeof(struct sniff_item_head);
		if (read(rdp->fd, &item_head, len) != len) {
			if (rdp->save_guard != 0 && try++ < 3) {
				lseek(rdp->fd, sizeof(struct sniff_file_head), SEEK_SET);
				continue;
			}
			prnerr("data format error\n");
			goto err;
		}

//		printf("=======%s==========\n", __func__);
//		print_buf2((char *)(&item_head), len, 0);

		len = item_head.pack_len1;
		DMSG("len: %d\n", len);
		if (len > size) {
			prnerr("no enough buffer space\n");
			goto err;
		}

		if (read(rdp->fd, buf, len) != len) {
			prnerr("read data error\n");
			goto err;
		}

		if (item_head.pack_len == item_head.pack_len1) {
			break;
		} else {
			EMSG("item head: len(%d) != len1(%d)\n", item_head.pack_len, item_head.pack_len1);
			goto err_length;
		}
	}
	
	return len;
err:
	return -1;
err_length:
	// len != len1
	return -2;
}

