/*
 * cap_record.h
 *
 * Author: Grip2
 * Date: 2003/04/16
 * Remark:
 * Last: 2003/04/20
 */ 

#ifndef _GRIP_RECORD_
#define _GRIP_RECORD_

enum { G_RECORD_RECORD = 0, G_RECORD_PLAY };

long record_open(char *capfile, int mode, int num);
int record_record(long handle, const unsigned char *raw_pkt, int raw_pkt_len);
int record_play(long handle, unsigned char *buf, int size);
int record_close(long handle);

#endif
