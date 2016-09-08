#ifndef _BASE_LINE_H_
#define _BASE_LINE_H_

#define WEEK_SIZE 	7
#define DAY_SIZE 	7
#define HOUR_SIZE 	24

typedef struct base_hour{
	 uint64_t tcp_new_conn_s[flow_dir];
	 uint64_t tcp_live_conn_s[flow_dir];
	 ddos_rate_t rate[e_ddos_stat_type_max][flow_dir];
	 time_t time;
	 uint8_t enable_flag;
	 spinlock_t base_lock;
}base_line_hour;

typedef struct base_day{
	base_line_hour h[HOUR_SIZE];
}base_line_day;

typedef struct base_week{
	base_line_day d[DAY_SIZE];
}base_line_week;

typedef struct base_sev_week{
	struct hlist_node list;
	struct hlist_node base_list;
	base_line_week  w[WEEK_SIZE];
	uint32_t ip;
}base_sev_week;

extern void base_hash_init(void);
extern ddos_stat_norm_t *ddos_base_line_get_result(uint32_t ip);
extern void add_to_timerinit(void);
extern int ddos_get_baseline_from_file(void);


#endif
