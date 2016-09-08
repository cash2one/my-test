/**
 * @file query_entry.c
 * @brief  查询主业务
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2014-05-12
 */


#include "query.h"

#define NIPQUAD_FMT "%03u.%03u.%03u.%03u"
#define NIPQUAD(addr) \
     ((unsigned char*)&addr)[0],\
     ((unsigned char*)&addr)[1],\
     ((unsigned char*)&addr)[2],\
     ((unsigned char*)&addr)[3]

#define FIN_FLAG        (0x01)
#define SYN_FLAG        (0x02)
#define RST_FLAG        (0x04)
#define PSH_FLAG        (0x08)
#define ACK_FLAG        (0x10)
#define URG_FLAG        (0x20)

/* -------------------------------------------*/
/**
 * @brief  将所有IP点分制形式转换
 * @by     hanhouchao
 *
 * @param src_str  原命令
 * @param dst_str  转换之后的命令
 * @param len      命令长度
 *
 * @returns   
 */
/* -------------------------------------------*/
int tran_ip(const char* src_str,char* dst_str,int len){
    //转换后的ip
    char dst_ip[100]={0};
    int dst_pos = 0;
    //将ip分段取出后存储在二维数组中
    char sarrs[100][80] = {0};
    int pos,sarra_pos,temp;
    for(pos=0;(pos<len) && (dst_pos<len);pos++){
        //将原文件内容按字符存储到一个数组里
        dst_str[dst_pos] = src_str[dst_pos];
        dst_pos++;
        //存储取出的ip段
        char store_ip[100]= {0};
        int store_pos = 0;
        //遇到字符ip进行反转ip处理
        if(((char)src_str[pos]=='i')&&((char)src_str[pos+1]=='p')){
            pos = pos+3;
            store_pos=0;
            temp = pos;
            for(store_pos = 0; store_pos < 16; store_pos++){
                if(src_str[pos]==' '||src_str[pos]=='\0')
                    break;
                store_ip[store_pos] = src_str[pos];
                pos++;
            }           
            //取到ip的四个数字
            char* str = store_ip;
            char* ptr = strtok(str,".");
            sarra_pos = 0;
            while(ptr != NULL){
                memcpy(sarrs[sarra_pos],str,ptr-str);
                sarrs[sarra_pos][ptr-str] = '\0';
                sarra_pos++;
                str = ptr;
                if (str > store_ip + store_pos)break;
                ptr = strtok(NULL ,".");
            }
            if(str < store_ip + store_pos){
                strcpy(sarrs[sarra_pos],str);
		if(sarrs[sarra_pos][strlen(sarrs[sarra_pos])-1]=='\n')
			sarrs[sarra_pos][strlen(sarrs[sarra_pos])-1]='\0';
                sarra_pos++;
            }

            //将反转后的ip地址写入dst_ip
            sprintf(dst_ip, "%s%c%s%c%s%c%s", sarrs[4], '.',sarrs[3], '.', sarrs[2], '.', sarrs[1]);
            dst_str[dst_pos] = 'p';
            dst_pos++;
            dst_str[dst_pos] = ' ';
            dst_pos++;
            for(store_pos = 0 ;store_pos < pos-temp ;store_pos++){
                dst_str[dst_pos] = dst_ip[store_pos];
                dst_pos++;
            }
            pos--;
        }
    }
    return 0;
}

void make_flag_str(char *str, uint8_t flag)
{
    if (flag & URG_FLAG) {
        strncat(str, "U", 1);
    }

    if (flag & ACK_FLAG) {
        strncat(str, "A", 1);
    }

    if (flag & PSH_FLAG) {
        strncat(str, "P", 1);
    }

    if (flag & RST_FLAG) {
        strncat(str, "R", 1);
    }

    if (flag & SYN_FLAG) {
        strncat(str, "S", 1);
    }

    if (flag & FIN_FLAG) {
        strncat(str, "F", 1);
    }
}


int query_callback(void *user, int qid, flowrec_t *rec, int count)
{
    query_conn_t *conn = (query_conn_t*)user;
     
    char time_format[] = "%Y-%m-%d %X";
    char first_time_str[TIME_FORMAT_SIZE];
    char end_time_str[TIME_FORMAT_SIZE];
    struct tm* time_p;
    time_t f_time;
    time_t l_time;
    char flag_str[7] = {0};
    char info[1024] = {0};
    int succ_write = 0;

    f_time = rec->first;
    l_time = rec->last;

    time_p = localtime(&f_time);
    strftime(first_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);

    time_p = localtime(&l_time);
    strftime(end_time_str, TIME_FORMAT_SIZE-1, time_format, time_p);


    make_flag_str(flag_str, rec->flags);

    sprintf(info, NIPQUAD_FMT"|"NIPQUAD_FMT"|"NIPQUAD_FMT"|"NIPQUAD_FMT"|"/*IP*/
            //"%2d|%2d|"/* INPUT, OUTPUT */
            "%8d|%8d|"/* Packets, bytes */
            "%s|%s|"/* starttime, endtime */
            "%5d|%5d|"/* sport, dport */
            //"%5d|"/*res*/
            "%6s|%6d|"/*flags, proto */
            //"%d|%d|%d"
            "\n",             
                                                    NIPQUAD(rec->rip),
                                                    NIPQUAD(rec->sip),
                                                    NIPQUAD(rec->dip),
                                                    NIPQUAD(rec->nip),
             //                                       rec->input,
             //                                      rec->output,
                                                    rec->packets,
                                                    rec->bytes,
                                                    first_time_str,
                                                    end_time_str,
                                                    rec->sport,
                                                    rec->dport,
            //                                      rec->res,
                                                    flag_str,
                                                    rec->proto
            //                                        rec->tos,
            //                                        rec->sas,
            //                                        rec->das
                                                    );    
    succ_write = write(conn->fd, (void*)info, strlen(info));
	if (succ_write == -1) {
		fprintf(stderr, "write error\n");
	}

    return 0;
}


int init_query_env(query_conn_t *conn, char *cmd)
{
	int retn = 0;

    
	conn->flowdb = flowdb2_open(g_query_info_cfg.flowdb_ini_path, O_RDONLY);
	if (conn->flowdb == NULL) {
		fprintf(stderr, "OPen flowdb[%s] Error!\n", g_query_info_cfg.flowdb_ini_path);
		CA_LOG(LOG_MODULE, LOG_PROC, "OPen flowdb[%s] Error!\n", g_query_info_cfg.flowdb_ini_path);
		retn = -1;
		goto EXIT;
	}

	conn->qopt.cube_handler = NULL;
	conn->qopt.rec_handler = query_callback;

	conn->qattr.nqueue = 1;
	conn->qattr.rate = 1000;
	conn->qattr.sample = 1;
	conn->qattr.fix = 0;

	memset(conn->flow_condition, 0, 64);
    strncpy(conn->flow_condition, cmd, 64);

EXIT:
	return retn;
}

int open_query_result_file(query_conn_t *conn)
{
    int retn = 0;
    char title[1024] = {0};

	conn->fd = open(conn->result_tmp_file, O_RDWR| O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if (conn->fd == -1) {
		fprintf(stderr, "open  error\n");
		CA_LOG(LOG_MODULE, LOG_PROC, "open  error\n");
        retn = -1;
        goto EXIT;
	}


    CA_LOG(LOG_MODULE, LOG_PROC, "Open file [%s] .. and begin write.\n", conn->result_tmp_file);
#if 1
    sprintf(title, "%15s|%15s|%15s|%15s|"//ip
                    //"%2s|%2s|"//input, output
                    "%8s|%8s|"//packet, bytes
                    "%19s|%19s|"//time
                    "%5s|%5s|"//sport, dport
                    //"%5s|"//res
                    "%6s|%6s|"//flags, proto
                    //"%s|%s|%s"//
                    "\n",
            "route ip",
            "sip",
            "dip",
            "next ip",
            //"input",
            //"output",
            "packets",
            "bytes",
            "first time",
            "last time",
            "sport",
            "dport",
        //    "res",
            "flags",
            "proto"
         //   "tos",
         //   "..",
         //   ".."
                );
    write(conn->fd, (void*)title, strlen(title));
#endif

EXIT:
    return retn;
}

void get_dst_file_path(query_conn_t *conn, char *file_path)
{
    /* 原文件格式为 /data/tmpdata/comm/query/flow/down/201405012222_uuid.ok */     

    char file_name[ABSOLUT_FILE_PATH_SIZE];
    char *file_end_p = NULL;
    char *limite_chr = NULL;
    int len = strlen(file_path);
    int i = 0;
    /* 得到文件名称 */
    for (file_end_p = file_path, i = 0; i < len; ++file_end_p, ++i);
    for (limite_chr = file_end_p-1; *limite_chr != '/'; --limite_chr);

    memset(file_name, 0, ABSOLUT_FILE_PATH_SIZE);
    strncpy(file_name, limite_chr + 1, file_end_p-(limite_chr+1));

    /* 封装新的绝对路径 */
    memset(conn->result_file, 0, ABSOLUT_FILE_PATH_SIZE);
    memset(conn->result_tmp_file, 0, ABSOLUT_FILE_PATH_SIZE);
    strcat(conn->result_file, g_query_info_cfg.dst_file_path);
    strcat(conn->result_file, file_name);
    strcat(conn->result_tmp_file, g_query_info_cfg.dst_file_path);
    strcat(conn->result_tmp_file, file_name);
    strcat(conn->result_tmp_file, ".tmp");

}

/* -------------------------------------------*/
/**
 * @brief  查询主业务函数
 *
 * @param query_cmd_file 查询命令文件
 *
 * @returns   
 */
/* -------------------------------------------*/
int query_entry(char *query_cmd_file)
{
    int retn = 0;

    query_conn_t query_conn;
    char cmd[1024];
    char trans_cmd[1024];

    memset(cmd, 0, 1024);
    retn = read_file_to_data(query_cmd_file, cmd);
    if (retn < 0) {
        CA_LOG(LOG_MODULE, LOG_PROC, "[-] Read file [%s] Error!\n");
        goto EXIT;
    }
	printf("cmdsrcip=%s",cmd);
    /* 逆转所有ip的点分支形式 */
    tran_ip(cmd, trans_cmd, strlen(cmd));

    printf("-------------------- %s -------------------\n", trans_cmd);

    retn = init_query_env(&query_conn, trans_cmd);
    if (retn < 0) {
        goto EXIT;
    }

    /* 得到目的文件路径 */
    get_dst_file_path(&query_conn, query_cmd_file);

    retn = open_query_result_file(&query_conn);


    query_conn.flow_query = flowquery_commit(query_conn.flowdb, query_conn.flow_condition, 0, query_conn.qattr, &(query_conn.qopt), &query_conn, NULL);
    flowquery_wait(query_conn.flow_query, NULL);


    /* 删除原文件 */
    rename(query_conn.result_tmp_file, query_conn.result_file);
    CA_LOG(LOG_MODULE, LOG_PROC, "Write Done move file [%s] -> file[%s] .\n", query_conn.result_tmp_file, query_conn.result_file);
    remove(query_cmd_file);
    CA_LOG(LOG_MODULE, LOG_PROC, "REMOVE file [%s], AND QUERY FLOW SUCC!!!\n",query_cmd_file);
EXIT:
    close(query_conn.fd);
	flowdb2_close(query_conn.flowdb);
    return retn;
}

