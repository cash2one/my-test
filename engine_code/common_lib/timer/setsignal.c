/*
 * setsignal.h
 *
 * Author: Grip2
 * Date: 2003/06/20
 * Remark:
 * Last: 2003/06/20
 */ 

#include <string.h>
#include <stdio.h>
#include <signal.h>

void (*setsignal(int signum, void (*sighandler)(int, siginfo_t *, void *)))(int)
{
	struct sigaction action;
	struct sigaction old;

	//new.sa_handler = sighandler;

	memset(&action, 0, sizeof(action));
	action.sa_sigaction = sighandler;
	/* ·ÀÖ¹Á¬½ÓÖÐ¶Ï */
	action.sa_flags = SA_SIGINFO | SA_RESTART;
//	action.sa_flags = SA_SIGINFO;
	if (sigaction(signum, &action, &old) < 0) {
		printf("setsignal %d err!\n", signum);
		return (SIG_ERR);
	}

	return old.sa_handler;
}
