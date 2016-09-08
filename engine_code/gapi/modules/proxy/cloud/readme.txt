1. 分离规则下发并且更新数据库  b86af4784dd5f0ba6141885ef8f51d88  online_down_rule.py
2. 更改最新的云程序 ebdfe523d9dd5ff170e895d0c3fee64e  cloud_comm_main.py
3. conf_pre_conf.json   增加了更新规则的日志  cloud_rule_log
                        增加了backup_dns,列表记录
                        增加了"MTX_KNOW_FILE":"rule/know", 
                        增加了"comm_mtx_kwonledge_down_path":"/data/tmpdata/comm/rule/knowledge/down/",
4. 增加DNS缓存机制  1a845551d181da3ba6e99caea619045e  comm_backup_DNS.py
5. gmsconfig.xml   添加 <comm_stat link = "0" />
