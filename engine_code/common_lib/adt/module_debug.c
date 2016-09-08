#include "module_debug.h"

FILE *file = NULL;		// ÎÄ¼þ¾ä±ú, Ö»ÓÐµ±Êä³öµ½ÎÄ¼þÊ±¿ÉÓÃ 
int debug_init = 0;		// ÊÇ·ñÒÑ¾­³õÊ¼»¯ 
int debug_module = debug_none;	// ¿ØÖÆ´òÓ¡Ä£¿é 
int module_max_id = 0;
