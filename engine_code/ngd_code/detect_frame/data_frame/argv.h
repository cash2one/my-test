/*
 * argv.h
 *
 * Copyright (c) 2001 Dug Song <dugsong@monkey.org>
 *
 * $Id: argv.h,v 1.1 2009/05/20 05:36:16 cvsroot Exp $
 */

#ifndef ARGV_H
#define ARGV_H

int	 argv_create(char *p, int argc, char *argv[]);
char	*argv_copy(char *argv[]);

#endif /* ARGV_H */
