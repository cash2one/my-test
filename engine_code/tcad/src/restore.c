#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <time.h>

#include "misc.h"
#include "mbuf.h"
#include "tcad.h"
#include "statistics.h"
#include "rwlock.h"
#include "utaf_session.h"
#include "pcap_gen.h"
#include "restore.h"

static rwlock_t restore_lock;

extern volatile time_t g_utaf_time_jiffies;
extern int udp_send_log(char *log_info, int length);

static restore_t *restore_list = NULL;

void check_restores(struct m_buf *mbuf)
{
    session_item_t *si = (session_item_t *)mbuf->psession_item;
    restore_t *rl;

    if (NULL == si) {
        return;
    }

    if (APP_UNABLE_TO_IDENTIFY == si->service_type) {
        return;
    }

    read_lock(&restore_lock);
    for (rl = restore_list; rl; rl = rl->next)
    {
        if ( rl->rid == si->service_type )
        {
            if (NULL != rl->fp) {
                ca_pcap_write(rl->fp, (char *)mbuf->mac_header, mbuf->total_len);
            }
            break;
        }
    }
    read_unlock(&restore_lock);
    
}

uint32_t restore_check(uint16_t rid, char *buf)
{
    restore_t *rl;

    read_lock(&restore_lock);
    for (rl = restore_list; rl; rl = rl->next) {
        if (rl->rid == rid && 0 == strncmp(rl->buf, buf, BUF_SIZE-1)) {
            read_unlock(&restore_lock);
            return 1;
        }
    }
    read_unlock(&restore_lock);
    
    return 0;
}

uint32_t restore_add(uint16_t rid, char *name, char *buf)
{
    restore_t *rl;
    //int i;
    FILE *fp = NULL;
    char file_name[128]={0};
    struct tm *tblock;
    time_t t;
    char f[64] = {0};

    if (0 != restore_check(rid, buf)) {
        return TCAD_RETCODE_FLOW_EXIST;
    }

    rl = malloc(sizeof(restore_t));

    if (NULL == rl) {
        return TCAD_RETCODE_FAILED;
    }

    memset(rl, 0, sizeof(restore_t));

    rl->rid = rid;
    strncpy(rl->name, name, SAM_STR_SIZE);
    strncpy(rl->buf, buf, BUF_SIZE-1);

    tblock = localtime(&t);
    sprintf(f, "%04d%02d%02d%02d%02d%02d",
        (tblock->tm_year + 1900), (tblock->tm_mon + 1), tblock->tm_mday,
        tblock->tm_hour, tblock->tm_min, tblock->tm_sec);

    sprintf(rl->filename, "gms.%s.%s.pcap", rl->name, f);
	sprintf(file_name, "%s%s", PCAP_FILE_DIR, rl->filename);
    remove(file_name);
	fp = fopen(file_name, "a+");

    rl->fp = fp;
    if (NULL != fp) {
        (void)ca_pcap_write_header(fp);
    }
    
    write_lock(&restore_lock);
    rl->next = restore_list;
    restore_list = rl;
    write_unlock(&restore_lock);

    return TCAD_RETCODE_OK;
}

uint32_t restore_del(uint16_t rid, char *buf)
{
    restore_t *rl, *tmp = NULL;

    write_lock(&restore_lock);
    for (rl = restore_list; rl; rl = rl->next) {
        if (rl->rid == rid && 0 == strncmp(rl->buf, buf, BUF_SIZE-1)) {
            if (NULL == tmp) {
                restore_list = rl->next;
            } else {
                tmp->next = rl->next;
            }

            if (NULL != rl->fp) {
                fclose(rl->fp);
            }

            free(rl);
            write_unlock(&restore_lock);
            return TCAD_RETCODE_OK;
        }

        tmp = rl;
    }
    write_unlock(&restore_lock);
    
    return TCAD_RETCODE_FLOW_NOT_EXIST;
}

uint32_t restore_mod(uint16_t rid, char *buf)
{
    restore_t *rl;

    write_lock(&restore_lock);
    for (rl = restore_list; rl; rl = rl->next) {
        if (rl->rid == rid) {
            strncpy(rl->buf, buf, BUF_SIZE-1);
            write_unlock(&restore_lock);
            return TCAD_RETCODE_OK;
        }
    }
    write_unlock(&restore_lock);
    
    return TCAD_RETCODE_FLOW_NOT_EXIST;
}

void restore_init(void)
{
    rwlock_init(&restore_lock);
}

