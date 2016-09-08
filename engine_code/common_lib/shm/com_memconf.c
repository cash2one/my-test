#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "com_memconf.h"
#include "shm.h"
//#include "misc.h"
/* #define strlen(str) strlen((const char*)str)
 * #define strncmp(src, dst, len) strncmp((const char *)src, (const char *)dst, len)
 * #define strcmp(src1, src2) strcmp((const char *)src1, (const char *)src2)
 * #define strcpy(dst, src) strcpy((char*) dst, (const char*)src)
 * #define strncpy(dst, src, len) strncpy((char*) dst,(const char*)src, len)
 * #define fgets(buf, len ,fd) fgets((char*)buf, len, fd)
 * #define fopen(path,mode)  fopen((const char *)path, mode);
 * #define strtol(nptr, endptr, base) (unsigned int)strtol((const char*)nptr, endptr, base);
 */

int disp_mode;

int GetPara(char * para_name, char * para1, char * para2, char * para3, char * rpara)
{
	int klen;

	klen = strlen(para_name);
	if (!strncmp(para1, para_name, klen)) {
		if (strlen(para3) != 0) {
			// printf("1 level = %s\n", para3);             
			strcpy(rpara, para3);
		} else {
			if (strlen(para2) != 0) {
				if (para2[0] != '=') {
					strcpy(rpara, para2);
				} else {
					printf("3 level = %s\n", &para1[1]);
					// con->level = strtol(&buf1[1], NULL, 10);
					strcpy(rpara, &para2[1]);
				}
			} else {
				if (para1[klen] == '=') {
					printf("4 level = %s\n", &para1[klen + 1]);
					// con->level = strtol(&buf0[klen+1], NULL, 10);        
					strcpy(rpara, &para1[klen + 1]);
				}
			}
		}
	}
	return 1;
}

int GetItemHead(FILE * filefd, char * itemid)
{
	char buf[512], buf0[200], *p = NULL;
	int returni;

	while (1) {
		bzero(buf, 512);
		p = (char *)fgets(buf, 512, filefd);
		if (p == NULL) {
			returni = -1;
			break;
		}
		if (buf[0] == '#')
			continue;
		bzero(buf0, 200);

		sscanf((char*)buf, "%s", buf0);
		if (strlen(buf0) <= 0)
			continue;
		if (buf0[0] == '#')
			continue;
		// printf("buf0 :%s \n", buf0);
		if (!strncmp(buf0, itemid, strlen(itemid))) {
			returni = 1;
			break;
		}
	}
	return returni;
}

int ComReadMemConfPname(FILE * filefd, char * pname, TPROGMEMCONF * meminfo)
{
	char buf[512], buf0[200], buf1[200], buf2[200], *p = NULL;
	char resdata[200];
	int returni;
	int plen;
	int spi;

	returni = GetItemHead(filefd, (char *)"program_start");
	if (returni == 1) {
		spi = 0;
		plen = strlen(pname);
		while (1) {
			bzero(buf, 512);
			p = (char*)fgets(buf, 512, filefd);
			if (p == NULL) {
				returni = -1;
				break;
			}
			if (buf[0] == '#')
				continue;
			bzero(buf1, 200);
			bzero(buf2, 200);
			bzero(buf0, 200);

			sscanf((char *)buf, "%s%s%s", buf0, buf1, buf2);

			// printf("buf0 :%s, buf1 %s  buf2 %s \n", buf0, buf1, buf2);
			if (!strcmp(buf0, pname)) {
				bzero(resdata, 200);
				GetPara(pname, buf0, buf1, buf2, resdata);
				strncpy(meminfo->pmem[spi].spname, resdata, sizeof(meminfo->pmem[spi].spname));
				spi++;
			}
			if (!strncmp(buf0, "program_end", strlen("program_end"))) {
				meminfo->tptotal = spi;
				if (spi > 0)
					returni = 1;
				else
					returni = -1;
				break;
			}
		}
	} else {
		if ((disp_mode & 0x04) == 0x04)
			printf("program_end bad\n");
	}
	return returni;
}

int ComReadMemConfPnameInfo(FILE * filefd, char * spname, char * stotal)
{
	char buf[512], buf0[200], buf1[200], buf2[200], *p = NULL;
	int returni;

	// printf("\nComReadMemConfPnameInfo start\n");
	returni = GetItemHead(filefd, (char *)"sprogram_start");
	if (returni == 1) {
		while (1) {
			bzero(buf, 512);
			p =(char *)fgets(buf, 512, filefd);
			if (p == NULL) {
				returni = -1;
				break;
			}
			if (buf[0] == '#')
				continue;
			bzero(buf1, 200);
			bzero(buf2, 200);
			bzero(buf0, 200);

			sscanf((char*)buf, "%s%s%s", buf0, buf1, buf2);

			// printf("buf0 :%s, buf1 %s  buf2 %s \n", buf0, buf1, buf2);
			if (!strncmp(buf0, "program_name", strlen("program_name"))) {
				GetPara((char *)"program_name", buf0, buf1, buf2, spname);
				continue;
			}
			if (!strncmp(buf0, "program_mem_total", strlen("program_mem_total"))) {
				GetPara((char *)"program_mem_total", buf0, buf1, buf2, stotal);

				continue;
			}

			if (!strncmp(buf0, "sprogram_end", strlen("sprogram_end"))) {
				returni = 1;
				break;
			}
#if 0
			else {
				returni = -1;
				printf("ComReadMemConfPnameInfo Read  Error: Read\n");
				break;
			}
#endif
		}
	} else {
		returni = -1;
	}
	return returni;
}

int ComReadMemConfPnameMemInfo(FILE * filefd, char * mid, char * mname,
				   char * mtype, char * mlen, char * mwid, char * mflag)
{
	char buf[512], buf0[200], buf1[200], buf2[200], *p = NULL;
	int returni;

	// printf("ComReadMemConfPnameMemInfo Start\n");
	returni = GetItemHead(filefd, (char *)"mem_start");
	if (returni == 1) {
		while (1) {
			bzero(buf, 512);
			p = (char *)fgets(buf, 512, filefd);
			if (p == NULL) {
				returni = -1;
				if ((disp_mode & 0x04) == 0x04) {
					printf("mem_end error\n");
				}
				break;
			}
			if (buf[0] == '#')
				continue;
			bzero(buf1, 200);
			bzero(buf2, 200);
			bzero(buf0, 200);

			sscanf((char*)buf, "%s%s%s", buf0, buf1, buf2);

			// printf("buf0 :%s, buf1 %s  buf2 %s \n", buf0, buf1, buf2);
			if (!strncmp(buf0, "Mem_id", strlen("Mem_id"))) {
				GetPara((char*)"Mem_id", buf0, buf1, buf2, mid);
				continue;
			}
			if (!strncmp(buf0, "Mem_name", strlen("Mem_name"))) {
				GetPara((char*)"Mem_name", buf0, buf1, buf2, mname);
				continue;
			}
			if (!strncmp(buf0, "Mem_type", strlen("Mem_type"))) {
				GetPara((char*)"Mem_type", buf0, buf1, buf2, mtype);
				continue;
			}
			if (!strncmp(buf0, "Mem_lenth", strlen("Mem_lenth"))) {
				GetPara((char*)"Mem_lenth", buf0, buf1, buf2, mlen);
				continue;
			}
			if (!strncmp(buf0, "Mem_width", strlen("Mem_width"))) {

				GetPara((char*)"Mem_width", buf0, buf1, buf2, mwid);
				// printf("mwid %s\n", mwid);   
				continue;
			}
			if (!strncmp(buf0, "Mem_flag", strlen("Mem_flag"))) {
				GetPara((char*)"Mem_flag", buf0, buf1, buf2, mflag);
				continue;
			}

			if (!strncmp(buf0, "mem_end", strlen("mem_end"))) {
				returni = 1;
				break;
			} else {
				returni = -1;
				break;
			}
		}
	} else {
		if ((disp_mode & 0x04) == 0x04)
			printf("mem_start bad\n");
	}
	return returni;
}

int ComJulyProg(char * prog, TPROGMEMCONF * meminfo, int * pi)
{
	int i;

	for (i = 0; i < 10; i++) {
		if (!(strcmp(prog, meminfo->pmem[i].spname))) {
			*pi = i;
			return 1;
		}
	}
	return -1;
}

int ComReadMemConf(char * mfile, char * pname, TPROGMEMCONF * meminfo)
{
	FILE *commfd = NULL;
	char spname[10], stotal[10];
	char mid[10], mname[10], mtype[10], mlen[10], mwid[12], mflag[12];
	int rflag, ipflag, ijflag, imflag;
	unsigned int memi, pi;

	if ((mfile == NULL) || (strlen(mfile) == 0)) {
		fprintf(stderr, "Read Comm Conf File Not Exist Error.\n");
		return -1;
	}
	commfd = fopen(mfile, "r");

	if (commfd == NULL) {
		commfd = fopen(mfile, "r");
		if (commfd == NULL) {
			fprintf(stderr, "Read CommConf:Open File Error.\n");
			return -2;
		}
	}
	rflag = ComReadMemConfPname(commfd, pname, meminfo);
	if (rflag == 1) {
		while (1) {
			bzero(spname, 10);
			bzero(stotal, 10);
			ipflag = ComReadMemConfPnameInfo(commfd, spname, stotal);

			if (ipflag == -1)
				break;
			// printf("spname %s pmtotal = %d\n", spname, pi);      
			ijflag = ComJulyProg(spname, meminfo, (int*)&pi);
			if (ijflag == -1)
				continue;
			// printf("spname %s pmtotal = %d\n", spname, pi);
			meminfo->pmem[pi].pmtotal = strtol(stotal, NULL, 10);
			// printf("spname %s pi = %d pmtotal = %d\n", spname, pi, meminfo->pmem[pi].pmtotal);
			for (memi = 0; memi < meminfo->pmem[pi].pmtotal; memi++) {
				bzero(mid, 10);
				bzero(mname, 10);
				bzero(mtype, 10);
				bzero(mlen, 10);
				bzero(mwid, 12);
				bzero(mflag, 12);

				imflag = ComReadMemConfPnameMemInfo(commfd, mid, mname, mtype, mlen, mwid, mflag);
				if (imflag == -1) {
					return -1;
					if ((disp_mode & 0x04) == 0x04)
						printf("abc bad\n");
					break;
				}
				meminfo->pmem[pi].mem[memi].mem_id = strtol(mid, NULL, 16);
				strncpy(meminfo->pmem[pi].mem[memi].mname, mname, sizeof(meminfo->pmem[pi].mem[memi].mname));
				meminfo->pmem[pi].mem[memi].mtype = strtol(mtype, NULL, 16);
				meminfo->pmem[pi].mem[memi].mlenth = strtol(mlen, NULL, 10);
				// printf("mwid2222222222 %s\n", mwid);
				meminfo->pmem[pi].mem[memi].mwidth = strtol(mwid, NULL, 10);

				if (strcmp(pname, spname)) {
					if (mflag[0] == 'w')
						mflag[0] = 'r';
					else if (mflag[0] == 'r')
						mflag[0] = 'w';
				}
				meminfo->pmem[pi].mem[memi].mflag = mflag[0];
			}
		}
	} else {
		if ((disp_mode & 0x04) == 0x04)
			printf("aaaaaaaaaaaaaa bad\n");
		fclose(commfd);
		return -1;
	}
	fclose(commfd);
	return 1;

}

int GetSelfName(char * selfname)
{
	int pid, len;
	char item1[50];
	char temp[200], buf[100], item2[50];
	FILE *fp;

	bzero(temp, 200);
	pid = getpid();
	sprintf((char*)temp, "/proc/%d/stat", pid);
	fp = fopen(temp, "r");
	if (fp != NULL) {
		fgets(buf, 100, fp);
		fclose(fp);
		sscanf((char*)buf, "%s %s", item1, item2);
		len = strlen(item2);
		bzero(selfname, len);
		strncpy(selfname, &item2[1], len - 2);
		return 1;
	} else
		return -1;

}

#if 1
int ComInitMem(TPROGMEMCONF * memassign, char * memfile)
{
	unsigned int i, m;
	char selfname[100];
	int maddrflag;
	int snameflag;
	int confflag;

	maddrflag = 1;
	bzero(&memassign->tpname[0], sizeof(TPROGMEMCONF));
	snameflag = GetSelfName(selfname);
	if (snameflag == 1) {
		confflag = ComReadMemConf(memfile, selfname, memassign);
		if (confflag < 0) {
			if ((disp_mode & 0x04) == 0x04)
				printf("Com Read memconf file error\n");
			return -1;
		}
	} else {
		if ((disp_mode & 0x04) == 0x04)
			printf("Get Self Program Name Error\n");
		return -1;
	}
	//printf("memassign.tpname = %d\n", memassign->tptotal);
	for (i = 0; i < memassign->tptotal; i++) {
		//printf("\nme:massign.tpname = %s\n", memassign->pmem[i].spname);
		//printf("memassign.tpname = %d\n", memassign->pmem[i].pmtotal);
		for (m = 0; m < memassign->pmem[i].pmtotal; m++) {

			//printf("\nmem_id = %x\n", memassign->pmem[i].mem[m].mem_id);
			//printf("mname  = %s\n", memassign->pmem[i].mem[m].mname);
			//printf("mtype  = %x\n", memassign->pmem[i].mem[m].mtype);
			//printf("mlenth = %d\n", memassign->pmem[i].mem[m].mlenth);
			//printf("mwidth = %d\n", memassign->pmem[i].mem[m].mwidth);
			/* 
			 *  //printf("mflag  = %c\n", memassign->pmem[i].mem[m].mflag);
			 * memassign->pmem[i].mem[m].msp = (int8_t *)init_mem(
			 *      memassign->pmem[i].mem[m].mflag,\
			 *      memassign->pmem[i].mem[m].mem_id,\
			 *      memassign->pmem[i].mem[m].mlenth,\
			 *      memassign->pmem[i].mem[m].mwidth);
			 */
			memassign->pmem[i].mem[m].msp = (char *) init_shm(memassign->pmem[i].mem[m].mem_id,
									    memassign->pmem[i].mem[m].mlenth,
									    memassign->pmem[i].mem[m].mwidth);
			if (memassign->pmem[i].mem[m].msp == NULL) {
				maddrflag = 0;
			}
		}
	}
	if (maddrflag == 0)
		return -1;
	else
		return 1;
}

char *ComGetMemStartAddr(TPROGMEMCONF * memassign, int key)
{
	unsigned int i, m, addrflag;

	addrflag = 2;
	for (i = 0; i < memassign->tptotal; i++) {
		for (m = 0; m < memassign->pmem[i].pmtotal; m++) {
			if (key == memassign->pmem[i].mem[m].mem_id) {
				return ((char *) memassign->pmem[i].mem[m].msp);
			}
		}
	}
	return NULL;
}
#endif
/*
int32_t init_mem(int8_t *memfile)
{
	return ComInitMem(&mem_conf, memfile);
}

int8_t *get_mem_addr(int32_t key)
{
	return ComGetMemStartAddr(&mem_conf, key);
}
*/
