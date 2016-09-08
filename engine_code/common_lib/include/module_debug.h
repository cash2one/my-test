#ifndef _MODULE_DEBUG_H
#define _MODULE_DEBUG_H

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include <ctype.h>	// for isdigit
#include <sys/types.h>	// for u_int32_t

#include "misc.h"

#define LOG_FILE "./debug.log"

//#define DEBUG_LOG_FILE
#define DEBUG_ERR_FILE
//#define DEBUG_WARN_FILE

//#define FULL_FORMAT
#define DEBUG_ENV_VARIABLE "debug_module"

// 模块ID，每加一个模块都必须增加一个名称
enum {
	NONE=-1,
	VXID_ID,
	MEMPOOL_ID,
	HASH_ID,
	STR_HASH_ID,
	TCP_HANDSHAKE_ID,
	TCP_STATE_ID,
	TCP_SESSION_ID,
	TCP_DECODE_ID,
	TCP_REASM_ID,
};

#ifndef debug_none
#define debug_none  		0x0;
#define debug_none_str		"none"
#endif

#ifndef debug_vxid
#define debug_vxid		((unsigned int)1 << VXID_ID)
#define debug_vxid_str		"vxid"
#endif

#ifndef debug_mempool
#define debug_mempool		((unsigned int)1 << MEMPOOL_ID)
#define debug_mempool_str	"mempool"
#endif

#ifndef debug_hash
#define debug_hash		((unsigned int)1 << HASH_ID)
#define debug_hash_str		"hash"
#endif

#ifndef debug_str_hash
#define debug_str_hash	((unsigned int)1 << STR_HASH_ID)
#define debug_str_hash_str	"str_hash"
#endif

#ifndef debug_tcp_handshake
#define debug_tcp_handshake	((unsigned int)1 << TCP_HANDSHAKE_ID)
#define debug_tcp_handshake_str	"tcp_handshake"
#endif

#ifndef debug_tcp_state
#define debug_tcp_state		((unsigned int)1 << TCP_STATE_ID)
#define debug_tcp_state_str	"tcp_state"
#endif

#ifndef debug_tcp_session
#define debug_tcp_session	((unsigned int)1 << TCP_SESSION_ID)
#define debug_tcp_session_str	"tcp_session"
#endif

#ifndef debug_tcp_decode
#define debug_tcp_decode	((unsigned int)1 << TCP_DECODE_ID)
#define debug_tcp_decode_str	"tcp_decode"
#endif

#ifndef debug_tcp_reasm
#define debug_tcp_reasm		((unsigned int)1 << TCP_REASM_ID)
#define debug_tcp_reasm_str	"tcp_reasm"
#endif


extern FILE *file;
extern int debug_init;
extern int debug_module;

#define str_cmp(src, op, dst) \
	(strncasecmp(src, dst, strlen(dst)) op 0)

#define search_module(src, module) \
	if (str_cmp(src, ==, module##_str)) { \
		return module; \
	}

static inline int module_name2id(const char *module_name)
{
	if (module_name == NULL) {
		goto id_err;
	}

	if (isdigit(module_name[0])) {
		return (strtol(module_name, NULL, 0));
	}

	switch (module_name[0]) {
		case 'm':
			search_module(module_name, debug_mempool);
			break;
		case 'h':
			search_module(module_name, debug_hash);
			break;
		case 's':
			search_module(module_name, debug_str_hash);
			break;
		case 't':
			search_module(module_name, debug_tcp_state);
			search_module(module_name, debug_tcp_session);
			search_module(module_name, debug_tcp_decode);
			search_module(module_name, debug_tcp_reasm);
			search_module(module_name, debug_tcp_handshake);
			break;
		case 'v':
			search_module(module_name, debug_vxid);
			break;
		default:
			break;
	}

id_err:
	return debug_none;
}

static inline void print_longformat(FILE * out_file, const char *file_path, const char *funcname, int line)
{
#ifdef FULL_FORMAT
	fprintf(out_file, "%s(func %s line %d)>> ", file_path, funcname, line);
	return;
#else
	return;
#endif
}

static inline FILE *open_log_file()
{
	if (unlikely(file == NULL)) {
		file = fopen(LOG_FILE, "a+");
		if (file == NULL) {
			fprintf(stderr, "open %s error\n", LOG_FILE);
			goto open_err;
		}
	}

	return file;

open_err:
	return NULL;
}

static inline FILE *get_log_file_handle()
{
#if defined(DEBUG_LOG)
	return stdout;
#elif defined(DEBUG_LOG_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline FILE *get_err_file_handle()
{
#if defined(DEBUG_ERR)
	return stderr;
#elif defined(DEBUG_ERR_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline FILE *get_warn_file_handle()
{
#if defined(DEBUG_WARN)
	return stdout;
#elif defined(DEBUG_WARN_FILE)
	return open_log_file();
#else
	return NULL;
#endif
}

static inline int get_debug_module()
{
	if (debug_init) {
		return debug_module;
	}

	const char *key = getenv(DEBUG_ENV_VARIABLE);
	debug_module = module_name2id(key);
//	printf("key: %s, debug_module: %d\n", key, debug_module);

	debug_init = 1;
	return debug_module;
}

#define mprint(DEBUG_LEVEL, DEBUG_MODULE, FILE_FUNC, ...) \
	if (unlikely(get_debug_module() & DEBUG_MODULE)) {\
		FILE *out_dir = FILE_FUNC(); \
		if (out_dir != NULL) { \
			fprintf(out_dir, "[%d]%s", DEBUG_MODULE, DEBUG_LEVEL); \
			print_longformat(out_dir, __FILE__, __FUNCTION__, __LINE__); \
			fprintf(out_dir, __VA_ARGS__);\
			fflush(out_dir);\
		} \
	}

// print normal message
#if defined(DEBUG_LOG) || defined(DEBUG_LOG_FILE)
#define mdump(DEBUG_MODULE, ...) \
	mprint("[--log--] ", DEBUG_MODULE, get_log_file_handle, __VA_ARGS__);
#else
#define mdump(DEBUG_MODULE, ...)
#endif

// print warning message
#if defined(DEBUG_WARN) || defined(DEBUG_WARN_FILE)
#define mwarn(DEBUG_MODULE, ...)\
	mprint("[**WARN**] ", DEBUG_MODULE, get_warn_file_handle, __VA_ARGS__);
#else
#define mwarn(DEBUG_MODULE, ...)
#endif

// print error message
#if defined(DEBUG_ERR) || defined(DEBUG_ERR_FILE)
#define merror(DEBUG_MODULE, ...)\
	mprint("[**ERROR**] ", DEBUG_MODULE, get_err_file_handle, __VA_ARGS__);
#else
#define merror(DEBUG_MODULE, ...)
#endif

#ifndef massert
#define massert(DEBUG_MODULE, s) \
	if (unlikely(s)) { \
		merror(DEBUG_MODULE, "%s\n", #s); \
		goto err; \
	}
#endif

#endif				/* _MODULE_DEBUG_H */
