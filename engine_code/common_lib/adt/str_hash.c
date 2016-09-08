
#include "str_hash.h"
#include "module_debug.h"
#include "hash.h"
#include "mem_pool.h"
#include "misc.h"

/* Recommand: DJB Hash Function */
static inline unsigned int DJB_hash(char *str, int len)
{
	unsigned int hash = 5381;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) + hash) + (unsigned char)(*str);
	}

	return hash;
}

/* RS Hash Function */
static inline unsigned int RS_hash(char *str, int len)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = hash * a + (unsigned char)(*str);
		a = a * b;
	}

	return hash;
}

/* JS Hash Function */
static inline unsigned int JS_hash(char *str, int len)
{
	unsigned int hash = 1315423911;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash ^= ((hash << 5) + (unsigned char)(*str) + (hash >> 2));
	}

	return hash;
}

/*  P. J. Weinberger Hash Function */
static inline unsigned int PJW_hash(char *str, int len)
{
	const unsigned int BitsInUnsignedInt = (unsigned int) (sizeof(unsigned int) * 8);
	const unsigned int ThreeQuarters = (unsigned int) ((BitsInUnsignedInt * 3) / 4);
	const unsigned int OneEighth = (unsigned int) (BitsInUnsignedInt / 8);
	const unsigned int HighBits = (unsigned int) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << OneEighth) + (unsigned char)(*str);

		if ((test = hash & HighBits) != 0) {
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return hash;
}

/* ELF Hash Function */
static inline unsigned int ELF_hash(char *str,  int len)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << 4) + (unsigned char)(*str);
		if ((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}

	return hash;
}

/* BKDR Hash Function */
static inline unsigned int BKDR_hash(char *str, int len)
{
	unsigned int seed = 131;	/* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash * seed) + (unsigned char)(*str);
	}

	return hash;
}

/* SDBM Hash Function */
static inline unsigned int SDBM_hash(char *str, int len)
{
	unsigned int hash = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (unsigned char)(*str) + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

/* DEK Hash Function */
static inline unsigned int DEK_hash(char *str, int len)
{
	unsigned int hash = len;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ (unsigned char)(*str);
	}
	return hash;
}

/* BP Hash Function */
static inline unsigned int BP_hash(char *str, int len)
{
	unsigned int hash = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = hash << 7 ^ (unsigned char)(*str);
	}

	return hash;
}

/* FNV Hash Function */
static inline unsigned int FNV_hash(char *str, int len)
{
	const unsigned int fnv_prime = 0x811C9DC5;
	unsigned int hash = 0;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash *= fnv_prime;
		hash ^= (unsigned char)(*str);
	}

	return hash;
}

/* AP Hash Function */
static inline unsigned int AP_hash(char *str, int len)
{
	unsigned int hash = 0xAAAAAAAA;
	int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) ^ (hash >> 3)) : (~((hash << 11) ^ (unsigned char)(*str) ^ (hash >> 5)));
	}

	return hash;
}

// 只做明文字符串的处理
typedef struct {
	char *string;
//	int str_len;

	void *data;
} data_node_t;

static inline int string_compare(unsigned int key, void *data_ht, void *arg)
{
	if (strcmp((char *)arg, ((data_node_t *)data_ht)->string) == 0) {
		return 1;
	} else {
		return 0;
	}
}

// ×Ö·û´®²»ÔÊÐíÖØ¸´
str_hash_t *str_hash_init(int max_items, int max_str_len, int alloc_str_mem)
{
	str_hash_t *str_hash = calloc(1, sizeof(str_hash_t));
	massert(debug_str_hash, str_hash == NULL);

	str_hash->data_hash = hash_init(max_items, string_compare);
	massert(debug_str_hash, str_hash->data_hash == NULL);

	str_hash->data_node_pool = mempool_init(max_items, sizeof(data_node_t));
	massert(debug_str_hash, str_hash->data_node_pool == NULL);

	if (alloc_str_mem != 0) {
		str_hash->str_pool = mempool_init(max_items, max_str_len);	// question ???
		massert(debug_str_hash, str_hash->str_pool == NULL);

		str_hash->max_str_len = max_str_len;
		str_hash->alloc_str_mem = alloc_str_mem;
	}

	return str_hash;

err:
	str_hash_clean(str_hash, 0);
	return NULL;
}

static void free_action_node(unsigned int key, void *data, void *arg)
{
	data_node_t *node = (data_node_t *)data;
	if (node && node->string) {
		free(node->string);
		node->string = NULL;
	}
}

void str_hash_clean(str_hash_t *str_hash, int free_string)
{
	if (str_hash) {
		if (str_hash->data_hash) {
			if (!(str_hash->alloc_str_mem) && free_string) {
				hash_used_iterate(str_hash->data_hash, free_action_node, NULL);
			}

			hash_free(str_hash->data_hash);
		}

		if (str_hash->data_node_pool) {
			mempool_clean(str_hash->data_node_pool);
		}

		if (str_hash->str_pool) {
			mempool_clean(str_hash->str_pool);
		}

		free(str_hash);
	}

	return;
}

int str_hash_put(str_hash_t *str_hash, char *str_key, void *data)
{
	data_node_t *data_node = mempool_calloc(str_hash->data_node_pool);
	massert(debug_str_hash, data_node == NULL);

	int len = strlen((char*)str_key);
	if (str_hash->str_pool) {
		massert(debug_str_hash, (int)strlen(str_key) >= str_hash->max_str_len);
		data_node->string = mempool_calloc(str_hash->str_pool);
		massert(debug_str_hash, data_node->string == NULL);

		ca_strcpy(data_node->string, str_hash->max_str_len, str_key);
	} else {
		data_node->string = str_key;
	}

//	data_node->str_len = len;
	data_node->data = data;

	unsigned int key = DJB_hash(str_key, len);
	mdump(debug_str_hash, "key: %u, data: %p\n", key, data);

	if (hash_put(str_hash->data_hash, key, data_node) == 0) {
		return 0;
	}
	
	// hash_put err, goto err

err:
	if (data_node != NULL) {
		if (data_node->string) {
			mempool_free(str_hash->str_pool, data_node->string);
		}

		mempool_free(str_hash->data_node_pool, data_node);
	}

	return -1;
}

void *str_hash_get(str_hash_t *str_hash, char *str_key)
{
	unsigned int key = DJB_hash(str_key, strlen(str_key));

	data_node_t *data_node = hash_get(str_hash->data_hash, key, str_key);
	if (data_node != NULL) {
		return (data_node->data);
	} else {
		return NULL;
	}
}

void *str_hash_remove(str_hash_t *str_hash, char *str_key)
{
	int len = strlen(str_key);
	unsigned int key = DJB_hash(str_key, len);

	data_node_t *data_node = hash_remove(str_hash->data_hash, key, str_key);
	massert(debug_str_hash, data_node == NULL);

	if (str_hash->str_pool) {
		mempool_free(str_hash->str_pool, data_node->string);
	}
	void *data = data_node->data;
	mempool_free(str_hash->data_node_pool, data_node);

	return data;

err:
	return NULL;
}

int str_hash_change(str_hash_t *str_hash, char *str_key, void *to_data)
{
	int len = strlen((char *)str_key);
	unsigned int key = DJB_hash(str_key, len);

	data_node_t *data_node = hash_get(str_hash->data_hash, key, str_key);
	massert(debug_str_hash, data_node == NULL);

	data_node->data = to_data;
	return 0;

err:
	return -1;
}

