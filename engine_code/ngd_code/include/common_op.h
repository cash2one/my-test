/*************************************************************************
	> File Name: common_op.h
	> Author: wangleizhang
	> Explain: 
	> Created Time: Wed 06 Nov 2013 01:10:27 PM EST
 ************************************************************************/
#ifndef COMMON_OP_H
#define COMMON_OP_H

#define SETBIT(x, y)	(x)|=(1<<(y))
#define CLRBIT(x, y)	(x)&=~(1<<(y))
#define GETBIT(x, y)	((x)>>(y))&1

#endif

