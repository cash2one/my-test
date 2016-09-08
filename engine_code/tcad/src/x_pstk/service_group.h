#ifndef __SERVER_H__
#define __SERVER_H__

#define SERVICE_GROUP_MAX_NUM   (65536)

enum {
	mail = 1,			//�����ʼ�
	terminal_ctrl,	//�ն˿���
 	transfer,			//�ļ�����
 	net_manage,	//�������
 	traffic_agent,	//��������
 	net_security,	//���簲ȫ
 	update_soft,		//�������
 	net_game,		//������Ϸ
	p2p,			//p2p����  
 	stream_media = 10,	//������ý��
 	im,				//��ʱͨѶ
 	voip,			//����绰
 	stock_trading,	//���ڽ���
   	database,		//���ݿ�
 	general_proto = 15,	//ͨ��Э��
 	other = 20			//����     	
};
extern uint32_t service_group_print(uint32_t appid);
extern uint16_t service_group_find(uint16_t appid);
extern uint8_t service_mobile_find(uint16_t appid);
extern uint32_t service_group_table_create(void);  //wdb_calc222
extern uint32_t service_group_init(void);  //wdb_calc222

#endif
