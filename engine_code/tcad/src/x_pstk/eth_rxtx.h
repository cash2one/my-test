typedef void (*PKT_INPUT)(struct m_buf *mbuf);
extern void packet_input_register(PKT_INPUT pFunc);
