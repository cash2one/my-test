#ifndef __ICMP_H__
#define __ICMP_H__


extern void icmp_v4_input(struct m_buf *mbuf);
extern uint32_t icmp_init(void);



#endif

