#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/if.h>

#include "lock_file.h"
#include "misc.h"

static char transtbl_i2a[17] = "0123456789ABCDEF";

/*
 * str_bin2asc
 * example: 0x12345678 --> "12 34 56 78\0"
 * so if want change all binary string of X Bytes, the length of 
 * ascii string must be 3*XBytes
 */
int str_bin2asc(char *bin_str, int bs_len, char *asc_str, int as_maxlen)
{
	int as_len = 0;
	int real_bs_len = 0;
	int i = 0;
	unsigned char low = 0, high = 0;

	if (!bin_str || !asc_str)
		goto err;

	real_bs_len = MIN((as_maxlen / 3), bs_len);

	for (i = 0; i < real_bs_len; i++) {
		high = bin_str[i] >> 4;
		low = bin_str[i] & 0xf;

		asc_str[as_len++] = transtbl_i2a[high];
		asc_str[as_len++] = transtbl_i2a[low];
		asc_str[as_len++] = ' ';
	}

	if (as_len > 0) {
		as_len--;	// ȥ???????Ŀո?
		asc_str[as_len] = '\0';
	}

	return as_len;

err:
	return -1;
}

/*
 * This function get var between two deli
 *	0: failed
 *  	>1:successed
 *	begin: 	databuffer's begin
 *	end :  	databuffer's end
 *	fdeli: 	begin's Deli
 *	flen:  	begin's Deli length
 *	ldeli:  last's Deli
 *	llen:   last'Deli length
 *	var : 	the return var
 *	varlen:	var's length;
 */
int getvarbydeli(char *begin, char *end, char *fdeli, int flen, char *ldeli, int llen, char *var, int maxvarlen)
{
	int varlen;
	char *index;
	char *fvar = 0;
	char *lvar = 0;

	/*from begin to end */
	for (index = begin; index < end; index++) {
		if (flen == 1) {
			if (*index == *fdeli) {
				fvar = index + 1;
				break;
			} else {
				continue;
			}
		}
	}
	if (!fvar)
		return 0;

	/*from end to begin */
	index = end - 1;
	while (index > begin) {
		if (llen == 1) {
			if (*index == *ldeli) {
				lvar = index;
				break;
			} else {
				index--;
				continue;
			}

		}
	}
	if (!lvar)
		return 0;
	varlen = lvar - fvar;
	if (varlen >= maxvarlen) {
		return 0;
	}
	strncpy(var, fvar, (size_t)varlen);
	var[varlen] = '\0';
#ifdef	DEBUG_DUMP
	printf("%s", var);
#endif
	return 1;

}

#define	SF_SIFUNC(str, addr, maxlen, type, func) \
						\
	if (!addr || maxlen < (int)sizeof(type)) {\
		goto err; 			\
	} 					\
						\
	if (!str) {				\
		*(type *) addr = 0; 		\
	} else { 				\
		*(type *) addr = (type)func(str, NULL, 0); 	\
	} 					\
						\
	return (sizeof(type)); 			\
						\
err: 						\
	return -1;

/* 
 * split funcs
 * return: n >= 0, result len in "addr", n < 0, error
 * sf_s2i: convert string to int
 */
int sf_s2i(char *str, void *addr, int maxlen)
{
	SF_SIFUNC(str, addr, maxlen, int, strtol);
}

int sf_s2ul(char *str, void *addr, int maxlen)
{
	SF_SIFUNC(str, addr, maxlen, unsigned long, strtoul);
}

int sf_s2us(char *str, void *addr, int maxlen)
{
	SF_SIFUNC(str, addr, maxlen, unsigned short, strtoul);
}

int sf_scp(char *str, void *addr, int maxlen)
{
	int n = 0;

	if (!addr || maxlen < 1)
		goto err;

	if (!str) {
		*(char *) addr = '\0';
		goto ret;
	}

	if (strlen(str) + 1 > (unsigned int) maxlen) {
		*(char *) addr = '\0';
		goto ret;
	}

	strcpy(addr, str);

ret:
	return n;
err:
	return -1;
}

/*
 * split a string to n part, every part will be a param to 'func', and fill the result
 * to the address stored in struct split_var 's addr.
 * if 'func' == NULL, call string copy(sf_scp) as default func
 */
int str_split(char *orig_str, int delim, split_fp func, struct split_var split_var_table[], int split_var_max)
{
	char *tbuf = NULL;
	char *head = NULL;
	int nvar = 0;
	char *p = NULL;

	if (orig_str == NULL || orig_str[0] == '\0') {
		return -1;
	}

	tbuf = malloc(strlen(orig_str) + 1);
	if (!tbuf) {
		goto err;
	}
	strcpy(tbuf, orig_str);

	split_fp t_func = sf_scp;
	if (func != NULL) {
		t_func = func;
	}

	head = tbuf;
	while (head != NULL) {
		p = strchr(head, delim);
		if (p) {
			*p = '\0';
		}

		if (t_func(head, split_var_table[nvar].addr, split_var_table[nvar].maxlen) < 0) {
			fprintf(stderr, "Error: splited data convert error(%s)\n", head);
			continue;
		}

		if ((++nvar) == split_var_max) {
			break;
		}

		head = (p != NULL) ? (p + 1) : NULL;
	}

	free(tbuf);
	return nvar;

err:
	if (tbuf) {
		free(tbuf);
		tbuf = NULL;
	}

	return -1;
}

void print_buf(char *buf, int nbuf, int column)
{
	int i;
	int col;

	if (buf == NULL) {
		return;
	}

	if (column == 0)
		col = 30;
	else
		col = column;

	for (i = 0; i < nbuf; i++) {
		printf("%x%x ", buf[i] >> 4, buf[i] & 0x0f);
		if ((i + 1) % col == 0)
			printf("\n");
	}
	printf("\n");

	return;
}

#if 0
#define REVERSE_CHAR(ch) \
        (!isascii(ch) || isprint (ch) ? (ch) : '.')
#else
#define REVERSE_CHAR(ch) \
	(isprint (ch) ? (ch) : '.')
#endif
void print_buf2(char *buf, int nbuf, int column)
{
	int i = 0, j = 0;
	int col = 0;
	char ch = 0;
	int first_line = 1;
	char *begin = NULL;

	if (buf == NULL || nbuf <= 0) {
		return;
	}

	if (column == 0) {
		col = 16;
	} else {
		col = column;
	}

	for (i = 0; i < nbuf; i++) {
		if (first_line) {
			printf("%08xh: ", i);
			first_line = 0;
		}

		printf("%x%x ", (buf[i] >> 4) & 0xf, buf[i] & 0xf);
		if ((i + 1) % col == 0) {
			printf("; ");
			begin = buf + i - (col - 1);
			for (j = 0; j < col; j++) {
				ch = *(begin + j);
				printf("%c", REVERSE_CHAR(ch));
			}
			printf("\n");
			first_line = 1;
		}
	}

	if (first_line == 1) {
		return;
	}

	for (j = 0; j < col - i % col; j++)
		printf("   ");

	printf("; ");

	for (j = 0; j < i % col; j++) {
		ch = *(buf + i + j - i % col);
		printf("%c", REVERSE_CHAR(ch));
	}

	printf("\n");

	return;
}

/* 
 * search any char in 'delim' which is first ocurr in 'string' 
 */
char *new_strchr(char *string, char *delim)
{
	int str_i;

	if (string == NULL || delim == NULL)
		return NULL;

	for (str_i = 0; (unsigned int) str_i < strlen(string); str_i++) {
		char *p;

		p = strchr(delim, string[str_i]);	/* find char in delimiters */
		if (p != NULL) {
			break;
		}
	}

	if ((unsigned int) str_i == strlen(string)) {
		return NULL;
	} else
		return string + str_i;

}

char *mem_find(char * mem_mother, char * mem_son, int mother_len, int son_len)
{
	int i;

	if (mem_mother == NULL || mem_son == NULL || mem_son == 0 || son_len == 0) {
		return NULL;
	}
	for (i = 0; i < mother_len - son_len + 1; i++) {
		if (memcmp(mem_mother + i, mem_son, son_len) == 0) {
			return mem_mother + i;
		}
	}
	return NULL;
}

unsigned int IP_strtoNum(char * ip)
{
	struct in_addr sin_addr;

	if (ip == NULL){
		return -1;
	}

	if (!inet_aton(ip, &sin_addr))
		return -1;

	return htonl(sin_addr.s_addr);
}

/*
 * search_nosense_char
 * 寻找无意义的字符，这些字符是指' ', '\t', '\n', '\r'和0字符
 * NULL: not found
 * step: >=0 = forward search
 * 	<0 = backward search
 */
char *search_nosense_char(char *begin, int nsearch, int step)
{
	char *p = NULL, *end = NULL;

	p = begin;
	if (step >= 0) {
		end = begin + nsearch;
		do {
			if (*p == ' ' || *p == '\t' 
				|| *p == '\r' || *p == '\n' 
				|| *p == '\0') {
				break;
			}
		} while (++p < end);

		if (*p == '\0' || p == end) {
			return NULL;
		} else {
			return p;
		}
	} else {
		end = begin - nsearch;
		do {
			if (*p == '\r' || *p == '\n' 
				|| *p == '\t' || *p == ' ') {
				break;
			}
		} while (--p < end);

		if (p == end) {
			return NULL;
		} else {
			return p;
		}
	}
}

/*
 * search_sense_char
 * 寻找有意义的字符，这些字符是指除了' ', '\t', '\n', '\r'和0之外的字符
 * NULL: not found
 * step: >=0 = forward search
 * 	<0 = backward search
 */
char *search_sense_char(char *begin, int nsearch, int step)
{
	char *p = NULL, *end = NULL;

	p = begin;
	if (step >= 0) {
		end = begin + nsearch;
		while ((p < end) 
			&& (*p == ' ' || *p == '\t' 
				|| *p == '\r' || *p == '\n')) {
			p++;
		}

		if (*p == '\0' || p == end) {
			return NULL;
		} else {
			return p;
		}
	} else {
		end = begin - nsearch;
		while ((p > end) 
			&& (*p == '\r' || *p == '\n' 
				|| *p == '\t' || *p == ' ')) {
			p--;
		}

		if (p == end) {
			return NULL;
		} else {
			return p;
		}
	}
}

/* ɾ??strline??β?ķǿɴ?ӡ?ַ? */
char *strim(char *strline)
{
	if (unlikely(strline == NULL || strline[0] == '\0')) {
		goto err;
	}

	char *pline = search_sense_char(strline, strlen(strline), 0);
	if (pline == NULL) {
		*pline = '\0';
		goto err;
	}

	char *p = search_sense_char(pline + strlen(pline) - 1, strlen(pline), -1);
	if (p != NULL) {
		*(p + 1) = '\0';
	} else {
		*pline = '\0';
		goto err;
	}

	return pline;

err:
	return NULL;
}

int execute(char *command, char *buf, int bufmax)
{
#define CMD_LEN 1024

	FILE *fp = NULL;
	char cmd[CMD_LEN] = { 0 };
	int len = 0;

	snprintf(cmd, CMD_LEN, "%s 2>&1", command);
	if ((fp = popen(cmd, "r")) == NULL) {
		return -1;
	}

	if (buf == NULL || bufmax == 0) {
		goto ret;
	}

	while ((buf[len] = fgetc(fp)) != EOF && (++len) < bufmax);

	while (len > 0 && (buf[len - 1] == 0x0a || buf[len - 1] == 0x0d)) {
		len--;
	}

	buf[len] = '\0';

#ifdef MISC_DEBUG
	printf("len: %d\n", len);
	print_buf2(buf, len, 0);
#endif

ret:
	pclose(fp);
	return len;
}

// get path from cmd    
static int get_current_dir(char *cmd, char *path, int max_len)
{
	char *pidx = NULL;
	int copy_len = 0;

	if ((pidx = strrchr(cmd, '/')) != NULL) {
		copy_len = pidx - cmd;
		if ((copy_len == 1) && (cmd[0] == '.')) {
			return 0;
		}

		if (copy_len >= max_len) {
			return -1;
		}

		memcpy(path, cmd, copy_len);
		path[copy_len] = '\0';
	}

	return copy_len;
}

void change_workdir(char *cmd_path)
{
	if (cmd_path == NULL || cmd_path[0] == '\0') {
		return;
	}

	char path[1024] = {0};
	if (get_current_dir(cmd_path, path, 1024) > 0) {
#ifdef MISC_DEBUG
		printf("path: %s\n", path);
#endif
		chdir(path);
	}
}

void godaemon(void)
{
	pid_t fs;

	fs = fork();
	if (fs > 0) {
		exit(0);	/* parent */
	}

	fs = fork();
	if (fs > 0) {
		exit(0);	/* parent */
	}

	setsid();

	/* redirect stdin/stdout/stderr to /dev/null */
	close(0);
	close(1);
	close(2);

	open("/dev/null", O_RDWR);
	// After open,the file descriptor is 0;
	dup(0);
	// then bring the 1 
	dup(0);
	// At last, bring the 2

	return;
}

/************************************************
 *         Name: check_process_exist
 *  Description: 检查进程是否存在, process_name为进程名
 *               
 *       Return: 0: no exist
 *               1: exist
 ************************************************/
int check_process_exist(char *process_name)
{
	char tmp_file[16] = {0};
	snprintf(tmp_file, 16, "/tmp/%s.lock", process_name);
	FILE *lock_fp = fopen_lock(tmp_file, "w");

	char pid_file[32] = {0};
	snprintf(pid_file, 32, "/var/run/%s.pid", process_name);
	pid_t pid = -1;
	FILE *fp = NULL;
	/* Read pid of running shutdown from a file */
	if ((fp = fopen_lock(pid_file, "r")) != NULL) {
		fscanf(fp, "%d", &pid);
		fclose_unlock(fp);
	} else {
		goto p_no_exist;
	}       

	/* See if we are already running. */
	if (pid > 0 && kill(pid, 0) == 0) {
		char proc_stat_file[32] = {0};
		snprintf(proc_stat_file, 32, "/proc/%d/stat", pid);
		if (access(proc_stat_file, F_OK)) {
			goto p_exist;
		} else {
			goto p_no_exist;
		}
	}

p_no_exist:
	fp = fopen_lock(pid_file, "w");
	if (fp) {
		fprintf(fp, "%d\n", getpid());
		fclose_unlock(fp);
	}

	if (lock_fp) {
		fclose_unlock(lock_fp);
	}
	return 0;
p_exist:
	printf("process %s is still running, pid is %d\n", process_name, pid);
	if (lock_fp) {
		fclose_unlock(lock_fp);
	}
	return -1;
}

int fread_oneline(char *filename, char *buff, int len)
{
	FILE *fp = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
		return -1;	// old dt
	}

	char *pbuf = buff;
	int buf_len = 0;

	while (memset(buff, 0, len) != NULL && fgets(buff, len - 1, fp) != NULL) {
		pbuf = strim(buff);
		if (pbuf == NULL || pbuf[0] == '#') {
			// ????
			continue;
		}

		buf_len = strlen(pbuf);
		if (pbuf != buff) {
			memmove(buff, pbuf, buf_len);
			buff[buf_len + 1] = '\0';
		}

		break;
	}

	fclose(fp);

	return buf_len;
}

// added by zhangyunlu
int get_ifr_mac(char *eth, char *mac, int sz)
{
        int sock_mac = -1;
        struct ifreq ifr_mac;

        sock_mac = socket(AF_INET, SOCK_STREAM, 0);

        memset(&ifr_mac,0,sizeof(ifr_mac));
        strncpy(ifr_mac.ifr_name, eth, strlen(eth));
        if((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0) {
                goto err;
        }

        snprintf(mac, sz, "venus mac:%02X:%02X:%02X:%02X:%02X:%02X",
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
                (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

        close(sock_mac);

        return 0;
err:
        return -1;
}
//added end 

