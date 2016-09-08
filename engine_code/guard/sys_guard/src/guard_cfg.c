#include"file_path.h"

pql_conf_t	pql_conf;
guard_conf_t     guard_conf_cfg;
version_conf_t  version_conf;
cfg_desc pql_cfg[] = {

         {"POSTGRESQL", "db_name", &(pql_conf.psql_dbname), IP_STR_SIZE, CFG_TYPE_STR, 1, 0, "gms_db"},
         {"POSTGRESQL", "user", &(pql_conf.psql_user), USER_NAME_SIZE, CFG_TYPE_STR, 1, 0, "gms_user"},
        {"POSTGRESQL", "pwd", &(pql_conf.psql_pwd), PASSWORD_SIZE, CFG_TYPE_STR, 0, 0, "123456"},
        {"POSTGRESQL", "ip", &(pql_conf.psql_ip), IP_STR_SIZE, CFG_TYPE_STR, 0, 0, "127.0.0.1"},
        {"POSTGRESQL", "port", &(pql_conf.psql_port), PORT_STR_SIZE, CFG_TYPE_STR, 0, 0, "5432"},
        {"POSTGRESQL", "unix", &(pql_conf.psql_unix), CFG_SIZE_YN_UINT8, CFG_TYPE_YN_UINT8, 1, 0, "no"},
        {"POSTGRESQL", "domain_sock", &(pql_conf.psql_domain), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/tmp/.s.PGSQL.5432"},
        {"", "", NULL, 0, 0, 0, 0, ""},
};

cfg_desc g_guard_cfg[] = {
        {"regular_guard", "file_path", &(guard_conf_cfg.regular_guard_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/cronjob.conf"},
        {"service_class", "file_path", &(guard_conf_cfg.service_class_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/servce_class.conf"},
        {"devid", "file_path", &(guard_conf_cfg.devid_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/devid.cfg"},
        {"libversion", "file_path", &(guard_conf_cfg.libversion_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/libversion.cfg"},
        {"sysversion", "file_path", &(guard_conf_cfg.sysversion_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/sysversion.cfg"},
        {"process", "file_path", &(guard_conf_cfg.process_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/guard/conf/process.conf"},
        {"pql_conf", "file_path", &(guard_conf_cfg.pql_conf_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/gms/storage/conf/storage.cfg"},
        {"sysstatus_file", "file_path", &(guard_conf_cfg.sysstatus_file_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/filedata/gms/comm/devstat/up/"},
        {"flow", "file_path", &(guard_conf_cfg.flow_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/permdata/flow/"},
        {"dns", "file_path", &(guard_conf_cfg.dns_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/permdata/dns/"},
        {"URL", "file_path", &(guard_conf_cfg.URL_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/permdata/url/"},
        {"abb_online", "file_path", &(guard_conf_cfg.abb_online_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/engine/abb/online/"},
        {"abb_offline", "file_path", &(guard_conf_cfg.abb_offline_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/engine/abb/offline/"},
        {"mtd", "file_path", &(guard_conf_cfg.mtd_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/engine/mtd/"},
        {"url", "file_path", &(guard_conf_cfg.url_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/engine/url/"},
        {"virus", "file_path", &(guard_conf_cfg.virus_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/tmpdata/engine/virus/"},
        {"wrong", "file_path", &(guard_conf_cfg.wrong_path), FILE_PATH_SIZE, CFG_TYPE_STR, 0, 0, "/data/permdata/wrong/"},
        {"", "", NULL, 0, 0, 0, 0, ""},
};

cfg_desc version_cfg[] = {

        {"versioin", "sysversion", &(version_conf.sysversion), VERSION_SIZE, CFG_TYPE_STR, 0, 0, "1.1.1.1"},
        {"", "", NULL, 0, 0, 0, 0, ""}, 
};
