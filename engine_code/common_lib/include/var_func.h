#ifndef _VAR_FUNC_H
#define _VAR_FUNC_H

typedef struct _func_proto_var {
	unsigned int len;
	void *addr;
} func_proto_var;

typedef struct _func_var_para {
	unsigned int para_len;
	void *para;
} func_var_para;

func_var_para *var_func_init(char *para);
void var_func_clean(func_var_para * func_vp);
int byte_get(func_proto_var * var, unsigned int offset, unsigned int depth, void *para);
int byte_jump(func_proto_var * var, unsigned int offset, unsigned int depth, void *para);
int isdataat(func_proto_var * var, unsigned int offset, unsigned int depth, void *para);

#endif
