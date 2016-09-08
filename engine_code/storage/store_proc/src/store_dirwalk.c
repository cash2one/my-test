/**
 * @file store_dirwalk.c
 * @brief  便利目录函数的实现
 * @author liu_danbing <liudanbing@chanct.com>
 * @version 1.0
 * @date 2013-11-08
 */

#include "gms_store.h"
#include "dirwalk.h"

/* -------------------------------------------*/
/**
 * @brief  检查文件名必须为
 *           20130603-212312.ok
 *
 * @param name
 *
 * @returns   
 */
/* -------------------------------------------*/
static int check_file_name(char *name)
{
    int retn = 0;
        
#if 0
    if (strlen(name) != 18) {
        retn = -1;
        goto END;
    }
#endif
    char *end = NULL;
    for (end = name; *end != '\0'; ++end);
    
    if (strncmp((end-3), ".ok", 3) != 0) {
        retn = -1;
        goto END;
    }

#if 0
    if (strncmp((name+15), ".ok", 3) != 0) {
        retn = -1;
        goto END;
    }
#endif


END:
    return retn;
}


void split(char* src_str, char* delim, char (*result)[ABSOLUT_FILE_PATH_SIZE])
{
    int num = 0;
    char *get_str = NULL;
    char *next_str = src_str;

    printf("%s\n", next_str);
    while ((get_str = strtok_r(next_str, delim, &next_str)) != NULL) {
        strncpy(result[num++], get_str, ABSOLUT_FILE_PATH_SIZE);
    }
}

/* -------------------------------------------*/
/**
 * @brief  检查这个设备状态入库文件是否是自身设备的
 *
 * @param name  设备注册文件
 * @param devid 本端devid
 *
 * @returns   
 *          0   是自身的
 *          1   非自身的
 *          -1  错误
 */
/* -------------------------------------------*/
int check_file_is_localdev(char *name, char *devid)
{
    int retn = 0;
    char (*result)[ABSOLUT_FILE_PATH_SIZE] = NULL;
    char file_name[ABSOLUT_FILE_PATH_SIZE];

    strncpy(file_name, name, ABSOLUT_FILE_PATH_SIZE);

    result = (char (*)[ABSOLUT_FILE_PATH_SIZE])malloc(2*ABSOLUT_FILE_PATH_SIZE);
    if (result == NULL) {
        retn = -1;
        goto END;
    }

    split(file_name, "_", result);

#if 0
    printf("result[0]: %s\n", result[0]);
    printf("result[1]: %s\n", result[1]);
#endif

    retn = strncmp(devid, result[1], 14);

END:
    if (result != NULL) {
        free(result);
    }
    return retn;
}


static void do_walk(dirwalk_t* walk_info, char* dir, int deep, void (*func)(dirwalk_t*, char*, int))
{
    char name[ABSOLUT_FILE_PATH_SIZE];
    struct dirent *dp;
    DIR *dfd;
    char level_name[DIR_FILE_SIZE];

    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "do_walk: can not open %s\n", dir);
        return;
    }

    ++deep;
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;    
        }
        if (strlen(dir) + strlen(dp->d_name) + 2 > ABSOLUT_FILE_PATH_SIZE) {
            fprintf(stderr, "do_walk: name %s too long\n", dp->d_name);
            continue;
        }

        sprintf(name, "%s/%s", dir, dp->d_name);
#if 0
        printf("new_name: %s\n", name);
        printf("new deep:%d\n", deep);
#endif
        /* 判断该文件夹属于哪个级别的文件夹 */
        switch (deep) {
            case ROOT_LEVEL:    
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->root)) {
                    func(walk_info, name, deep);    
                }
                break;
            case TYPE_LEVEL:
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->type)) {
                    func(walk_info, name, deep);    
                }
                break;
            case KINDS_LEVEL:
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->kinds)) {
                    func(walk_info, name, deep);    
                }
                break;
            case YEAR_LEVEL:
                if (strcmp(name_by_level(name, level_name, 0), walk_info->year) >= 0) {
                    func(walk_info, name, deep);    
                }
                break;
            case MONTH_LEVEL:
                if ((strcmp(name_by_level(name, level_name, 1), walk_info->year) > 0)
                || (strcmp(name_by_level(name, level_name, 0), walk_info->month) >= 0)) {
                    func(walk_info, name, deep);    
                }
                break;
            case DAY_LEVEL:
                if ((strcmp(name_by_level(name, level_name, 2), walk_info->year) > 0)
                || (strcmp(name_by_level(name, level_name, 1), walk_info->month) > 0)
                || (strcmp(name_by_level(name, level_name, 0), walk_info->day) >= 0)) {
                    func(walk_info, name, deep);    
                }
                break;
            case FILE_LEVEL:
                func(walk_info, name, deep);    
                break;
            default:
                break;
        }
    }
    closedir(dfd);
}

static void get_file_name(dirwalk_t* walk_info, char *name, int deep)
{
    struct stat stbuf;    
    char level_name[DIR_FILE_SIZE];

#if 0
    printf("-->name %s\n", name);
    printf("-->deep %d\n", deep);
#endif

    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "get_file_name: cannot access %s\n", name);
        return ;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
        /* 该name 是一个文件夹 */

        /* 判断该文件夹属于哪个级别的文件夹 */
        switch (deep) {
            case ROOT_LEVEL:    
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->root)) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case TYPE_LEVEL:
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->type)) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case KINDS_LEVEL:
                if (!strcmp(name_by_level(name, level_name, 0), walk_info->kinds)) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case YEAR_LEVEL:
                if (strcmp(name_by_level(name, level_name, 0), walk_info->year) >= 0) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case MONTH_LEVEL:
                if ((strcmp(name_by_level(name, level_name, 1), walk_info->year) > 0)
                || (strcmp(name_by_level(name, level_name, 0), walk_info->month) >= 0)) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case DAY_LEVEL:
                if ((strcmp(name_by_level(name, level_name, 2), walk_info->year) > 0)
                || (strcmp(name_by_level(name, level_name, 1), walk_info->month) > 0)
                || (strcmp(name_by_level(name, level_name, 0), walk_info->day) >= 0)) {
                    do_walk(walk_info, name, deep, get_file_name);    
                }
                break;
            case FILE_LEVEL:
                break;
            default:
                break;
        }
    }
    else {
    /* 该name 是一个文件 */
        /* 将该文件名加入链表当中 */
        //printf("find file. deep = %d\n", deep);
        if (deep == FILE_LEVEL) {
            file_data_t* file = calloc(1, sizeof(file_data_t));
            strncpy(file->path, name, ABSOLUT_FILE_PATH_SIZE);
            list_enqueue(walk_info->list, file);
        }
    }
}


/* -------------------------------------------*/
/**
 * @brief    通过指定的文件路径 和 指定的时间临界
 *            得到对应路径下 临界时间之后的 文件名称
 *            并封装到链表中 供后续使用
 *
 * @param root                文件根目录 
 * @param type                文件类型 例如"events"为事件 
 * @param kinds                文件种类 例如“ae”为异常事件
 *                                         “ve”为病毒事件等
 * @param time_str            时间临界值 "2013-08-09 23:03:21"格式
 * @param file_list_head    得到的链表
 *
 * @returns   
 *                            0
 */
/* -------------------------------------------*/
int dirwalk_dumplist_by_time(char *root, char* type, char* kinds, char* time_str, list_t *file_list_head)
{
    dirwalk_t * walk_info = calloc(1, sizeof(dirwalk_t));
    int deep = 0;
    char root_name[DIR_FILE_SIZE];
        
    /* 初始化 目录信息 */
    strncpy(walk_info->root, root, DIR_FILE_SIZE);
    strncpy(walk_info->type, type, DIR_FILE_SIZE);
    strncpy(walk_info->kinds, kinds, DIR_FILE_SIZE);

    /* 时间信息 */
    split_time_str(time_str, walk_info->year,
                             walk_info->month,
                             walk_info->day,
                             NULL,
                             NULL,
                             NULL);
    
//    printf("year = %s mon = %s day = %s\n", walk_info->year, walk_info->month, walk_info->day);

    walk_info->list = file_list_head;


    strncpy(root_name, "/", 2);
    strncat(root_name, root, DIR_FILE_SIZE-2);

    get_file_name(walk_info, root_name, deep);    

    free(walk_info);

    return 0;
}

static void get_date(char *name, char *year, char *month, char *day)
{
    /* 得到年 */
    strncpy(year, name, 4);        
    year[4] = '\0';

    /* 得到月 06 去掉 0  */
    if (name[4] == '0') {
        strncpy(month, name+5, 1);
        month[1] = '\0';
    }
    else {
        strncpy(month, name+4, 2);
        month[2] = '\0';
    }

    /* 得到 天 03 去掉 0 */
    if (name[6] == '0') {
        strncpy(day, name+7, 1);
        day[1] = '\0';
    }
    else {
        strncpy(day, name+6, 2);
        day[2] = '\0';
    }
}

static void save_file_info(char *name, list_t* list)
{
    file_data_t* file = calloc(1, sizeof(file_data_t));

    /* file path */
    strncpy(file->path, name, ABSOLUT_FILE_PATH_SIZE);
    
    /* file name */
    name_by_level(name, file->file_name, 0);

    get_date(name_by_level(name, file->file_name, 0),
               file->year, file->month, file->day);

#if 0
    printf("file_name:%s\n", file->file_name);
    printf("year:%s\n", file->year);
    printf("month:%s\n", file->month);
    printf("day:%s\n", file->day);
#endif

    
    list_enqueue(list, file);
}

/* -------------------------------------------*/
/**
 * @brief  只遍历当前目录， 目录下所有.ok结尾的
 *            文件名加入到链表中 
 *
 * @param root
 * @param file_list_head
 *
 * @returns   
 *                SUCC  0
 *                FAIL  -1
 */
/* -------------------------------------------*/
int dirwalk_current_list(char *root, list_t *file_list_head)
{
    char name[ABSOLUT_FILE_PATH_SIZE];
    struct dirent *dp;
    DIR *dfd;
    struct stat stbuf;    
    int retn = 0;

    if ((dfd = opendir(root)) == NULL) {
        fprintf(stderr, "can not open %s\n", root);
        retn = -1;
        goto EXIT;
    }
        

    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;    
        }
        if (strlen(root) + strlen(dp->d_name) + 2 > ABSOLUT_FILE_PATH_SIZE) {
            fprintf(stderr, "name  %s too long\n", dp->d_name);
            continue;
        }

        snprintf(name, ABSOLUT_FILE_PATH_SIZE-1, "%s/%s", root, dp->d_name);

        if (stat(name, &stbuf) == -1) {
            fprintf(stderr, "%d: -----cannot access %s, %s\n", __LINE__, name, dp->d_name);
            retn = -1;
            goto EXIT;
        }

        if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
            /* 是个文件夹 */
            continue;
        }
        else {
            /* 是一个文件 */
            
            /* 判断文件名字是否符合条件 */
            retn = check_file_name(dp->d_name);
            if (retn < 0) {
                retn = 0;
                continue;
            }
            
            save_file_info(name, file_list_head);
        }
    }

EXIT:
    closedir(dfd);
    return retn;
}

