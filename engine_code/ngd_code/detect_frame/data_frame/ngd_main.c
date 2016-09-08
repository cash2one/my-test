/*************************************************************************
	> File Name: ngd_main.c
	> Author: wangleizhang
	> Explain:ngd主线程 
	> Created Time: Wed 23 Oct 2013 01:18:17 AM EDT
 ************************************************************************/
#include <unistd.h>
#include "ngd_init.h"
#include "thread.h"

int main(int argc, char *argv[])
{
    //读取命令
    //配置初始化
    if (ngd_init_entry(argc, argv) < 0) {
	goto RET;
    }

    //线程管理
    /*线程池初始化*/
    init_thread();
    /*线程调用*/
    thread_make();
    //

    while (1) {
	sleep(1);
    }

RET:

    /*清理插件链表*/
    free_plugin_list();
    return 0;
}
