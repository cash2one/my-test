#ifndef __SERVER_H__
#define __SERVER_H__

#define SERVICE_GROUP_MAX_NUM   (65536)

enum {
	mail = 1,			//电子邮件
	terminal_ctrl,	//终端控制
 	transfer,			//文件传输
 	net_manage,	//网络管理
 	traffic_agent,	//流量代理
 	net_security,	//网络安全
 	update_soft,		//软件更新
 	net_game,		//网络游戏
	p2p,			//p2p下载  
 	stream_media = 10,	//网络流媒体
 	im,				//即时通讯
 	voip,			//网络电话
 	stock_trading,	//金融交易
   	database,		//数据库
 	general_proto = 15,	//通用协议
 	other = 20			//其它     	
};
extern uint32_t service_group_print(uint32_t appid);
extern uint16_t service_group_find(uint16_t appid);
extern uint8_t service_mobile_find(uint16_t appid);
extern uint32_t service_group_table_create(void);  //wdb_calc222
extern uint32_t service_group_init(void);  //wdb_calc222

#endif
