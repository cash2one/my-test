/* 
 * $Id: tcpdump_filelist_captor.c,v 1.5 2009/09/18 02:34:59 dengwei Exp $
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "misc.h"
#include "captor.h"
#include "tcpdump_file_captor.h"
//
//#include "trace_api.h"

#define RAW_SHM_SIZE 4096 * 4096
#define RAW_SHM_KEY 0x19760916

//static int init_shm(void);
//static void clean_shm(void);
static void *tcpdump_filelist_captor_getbase();
static int tcpdump_filelist_captor_munmap(void *address);
void *tcpdump_filelist_captor_mmap();
static long tcpdump_filelist_captor_open(void *private_info, int argc, char **argv);
static int tcpdump_filelist_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p);
static void tcpdump_filelist_captor_close(void *private_info, long hdlr);

/*
 * FORMAT
 *
 * Input files hava a 24 byte header starting with A1 B2 C3 D4 hex.
 * This is followed by a series of records of 76-1530 bytes as follows.
 * The last record may be incomplete.

 * Bytes   Contents (numbers are most significant byte (MSB) first)
 * 0-3     Time in seconds since 0000 Jan 1 1970 UCT
 * 4-7     Time in microseconds, 0-999999
 * 8-11    Record length - 16 (60-1514)
 * 12-15   Second copy of length, must be identical
 */

struct tcpdump_record_head {
	unsigned int time_seconds;
	unsigned int time_microsecondes;
	unsigned int len;
	unsigned int len_copy;
};
	
captor_t tcpdump_filelist_captor = {
	.name = "tcpdump_filelist",
	.open = tcpdump_filelist_captor_open,
	.close = tcpdump_filelist_captor_close,
	.capture = tcpdump_filelist_captor_capture,
	.mmap = tcpdump_filelist_captor_mmap,
	.munmap = tcpdump_filelist_captor_munmap,
	.getbase = tcpdump_filelist_captor_getbase,
};

static u_int8_t	raw_packet_space[RAW_PACKET_LEN];


int   g_bUsingTcpdumpListDriver = 1; //
FILE* g_fpFileLists = NULL;  //file handle of filelist file. 
FILE* g_fpCapFile = NULL;  //file handle for each cap files.
char  g_szCapFileName[1024 + 1] = {0};  
int   g_nCapRecordIndex = -1; 
FILE* g_fpQueryResult = NULL; //file handle for query result. 

long tcpdump_filelist_captor_open(void *private_info, int argc, char **argv)
{
	//sequence diagram.
	//0. check valid.
	//1. reset all properties.
	//2. open filelist file.
	//3. retrive the first capfile name.
	//4. open the capfile.
	//5. read capfiles'head, and check valid. 

	//0. check valid. 

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//i found bug of mt. 
	//if i  start ./mt,   argc=1. 
	//then i start ./mt x,  argc = 1 else.

//	printf("----------argc:%d\n", argc); 
//	printf("argv0:%s\n", argv[0]); 
//	printf("argv1:%s\n", argv[1]); 
	if (argc != 2) 
	{
		fprintf(stderr, "Syntax Error.\n");
		fprintf(stderr, "Usage: ./mt <list file> <result file>\n"); 
		return -1; 
	}
	
	//1. 
	if (g_fpFileLists != NULL)
	{
		fclose(g_fpFileLists);
		g_fpFileLists = NULL; 
	}

	if (g_fpCapFile != NULL)
	{
		fclose(g_fpCapFile);
		g_fpCapFile = NULL; 
	}

	//2.
	g_fpFileLists = fopen(argv[0], "r"); 

	if (g_fpFileLists == NULL)
	{
		DMSG("cannot open file: %s\n", argv[0]);
		printf("Error to Open FileList file:%s, %s, Cannot load the Driver!\n", argv[0], strerror(errno));  
		return -1; 
	}

	//
	g_fpQueryResult = fopen(argv[1], "w"); 

	if (g_fpQueryResult == NULL)
	{
		DMSG("cannot open file: %s\n", argv[0]);
		printf("Error to Open QueryResult for Write:%s, %s, Cannot load the Driver!\n", argv[1], strerror(errno));  
		return -1; 
	}


	//open drive ok, set using tcpdump file list driver; 
	g_bUsingTcpdumpListDriver  = 1; 

	//
	return 1; //return 1 (a gt 0 value) to tell mt open success.

}

int tcpdump_filelist_captor_capture(void *private_info, long hdlr, u_int8_t **pkt_buf_p)
{
takeoff:
	//1. 
	if (g_fpFileLists == NULL)
	{
		return -1; //file not opened. 
	}

	//2.
	if (g_fpCapFile == NULL)
	{
		//cap file not opened, please open it. 
		char szFileName[1024 + 1]; 

trylock:
		memset(szFileName, 0, sizeof(szFileName)); 
				
		//get filename.
		if (fgets(szFileName, 1024, g_fpFileLists) == NULL)
		{
			//no any record in filelist, please close all files, and say bye-bye to mt.
			
			//close files.
			fclose(g_fpFileLists); 
			g_fpFileLists = NULL; 

			//say good-day.
			return 0; //without any record. bye-bye, attention:return 0 NOT -1.
		}
		
		//trim the right \r\n 
		char* p = &szFileName[strlen(szFileName) -1];
		if (*p == '\n' || *p == '\r')
		{
			*p = '\0'; 
		}

		
		//get cap file name, print it out to tell user whoami. (ONLY IN DEBUG MODE)
		printf("tcpdump file name:%s\n", szFileName); 
		
		//please open the cap files, using rb mode. 
		g_fpCapFile = fopen(szFileName, "rb");

		//check valid.e
		if (g_fpCapFile == NULL)
		{
			//prompt user.
			printf("Error to Open Cap File:%s, %s\n", szFileName, strerror(errno)); 
			
			//sorry, i cannot open this cap file. may this files not exist, or i have suffice permission.
			//however, i don't care the reson, so let's try the next file,  
			
			goto trylock;  //tower, go around.
		}
		else
		{
			//open success. please check capfile's head if valid?
 			char file_head[24];    
			if (fread(file_head, sizeof(file_head), 1, g_fpCapFile) != 1) 
			{
				DMSG("Error to read file-head.\n");
				
				//I open cap file ok, but i cannot get it's head,  may the capfile has been corrupted,
				//so, i HAVE TO igore this file, and look for the next one. 
				
				//don't fogot to close it, and set handle to null.
				fclose(g_fpCapFile); 
				g_fpCapFile = NULL; 

				//to report go around. 
				goto trylock; //tower, i go around again, sorry.
			}

			//check head if valid?
			//check head's check sum and etc. 
			if (memcmp(&file_head,"\xd4\xc3\xb2\xa1", 4) != 0) 
			{ 
				DMSG("Error: Unknown tcpdump file version\n");

				//i am not ensure the version of this file.
				//to ensure safety, request go around.
				goto trylock; //tower, go around for safety reason.
			}

			//open cap file ok, to set record counter.
			
			//set the filename.
			strcpy(g_szCapFileName, szFileName); 

			//set the record count. 
			g_nCapRecordIndex = 0; 
		}
	}

	//set record counter
	g_nCapRecordIndex++; 

	//print info.
	printf("record idx:%d\n", g_nCapRecordIndex);  
	
	
	//now, we are clear to  read cap record from this file now.
	struct tcpdump_record_head record_head;

	//get record's head. 
	if (fread(&record_head, sizeof(record_head), 1, g_fpCapFile) != 1)
	{
		//reach EOF in the cap file, please open the next cap files. 

		//close the cap file.
		fclose(g_fpCapFile); 
		g_fpCapFile = NULL; 

		//open the next cap file.
		goto takeoff; //please rewind to takeoff point. 
	}

	//check snapshot, we don't  support snapshot, In tcpdump should using -s0.
	if (record_head.len != record_head.len_copy) 
	{
		fprintf(stderr, "Error:  record length error, we don\'t support snapshot length, \
			if you capture packets using tcpdump, please use -s0 parameter.\n");

		//in this case, we cannot recorgnize this kind of file, 
		//close the cap file.
		fclose(g_fpCapFile); 
		g_fpCapFile = NULL; 

		//open the next cap file.
		goto takeoff; //please rewind to takeoff point. 
	}
	
	//check cap's length, 
	if(record_head.len >= RAW_PACKET_LEN) 
	{
		fprintf(stderr, "Error: record_head.len TOO LARGE in Packet:%d\n", g_nCapRecordIndex );

		//in this case, we also cannot recorgnize this kind of file yet.
		//close the cap file.
		fclose(g_fpCapFile); 
		g_fpCapFile = NULL; 

		//open the next cap file.
		goto takeoff; //please rewind to takeoff point. 
	}

	//read the real cap's content.
	if (fread(raw_packet_space, record_head.len, 1, g_fpCapFile) != 1) 
	{
		//this cap record is not complete, may this file is corrupted. 
				
		fclose(g_fpCapFile); 
		g_fpCapFile = NULL; 

		//open the next cap file.
		goto takeoff; //please rewind to takeoff point. 
	}

	//we got cap record in raw_packet_space; 
	*pkt_buf_p = raw_packet_space;

	return record_head.len; 
}

void tcpdump_filelist_captor_close(void *private_info, long hdlr)
{
	if (g_fpFileLists != NULL)
	{
		fclose(g_fpFileLists);
		g_fpFileLists = NULL; 
	}

	if (g_fpCapFile != NULL)
	{
		fclose(g_fpCapFile);
		g_fpCapFile = NULL; 
	}

	if (g_fpQueryResult != NULL)
	{
		fclose(g_fpQueryResult); 
		g_fpQueryResult = NULL; 
	}

	return;
}

void *tcpdump_filelist_captor_getbase()
{
	return NULL; 
}

void *tcpdump_filelist_captor_mmap()
{
	return NULL;
}

int tcpdump_filelist_captor_munmap(void *address)
{
	return 0;
}

