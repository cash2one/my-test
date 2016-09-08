#ifndef _OUTPUT_ACTION_H
#define _OUTPUT_ACTION_H

int trace_thread_lock();
int trace_thread_unlock();

int output_init();
int output_put(FILE *fp,char *msg);
int output_clean();

#endif
