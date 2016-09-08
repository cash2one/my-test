CFLAGS=-I../include/ 
all: sample_st sample_speed  sample_mt sample_send_mt sample_mt_chkpkt

CFLAGS += -O2
sample_st:sample_st.c

	gcc $(CFLAGS) -o sample_st sample_st.c -lpag -lpthread

sample_speed:sample_speed.c

	gcc $(CFLAGS) -o sample_speed sample_speed.c -lpag -lpthread

sample_mt:sample_mt.c

	gcc $(CFLAGS) -o sample_mt sample_mt.c -lpag -lpthread

sample_send_mt:sample_send_mt.c

	gcc $(CFLAGS) -o sample_send_mt sample_send_mt.c -lpag -lpthread

sample_mt_chkpkt:sample_mt_chkpkt.c
	gcc $(CFLAGS) -o sample_mt_chkpkt sample_mt_chkpkt.c -lpag -lpthread

clean :

	@rm -f sample_st sample_speed sample_mt sample_send_mt
