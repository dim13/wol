/* $Id$ */
/*
 * Copyright (c) 2009 Dimitri Sokolyuk <sokolyuk@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char	*bcast = "255.255.255.255";

int
parsemac(char *macaddr, char *str)
{
	char	*p;
	int 	i;
	long	l;

	for (i = 0; i < 6; ++i) {
		l = strtol(str, &p, 0x10);
		macaddr[i] = (char)l;
		str = p + 1;
	}

	return 0;
}

void
usage()
{
	extern	char *__progname;

	fprintf(stderr, "usage: %s [-n network] mac\n", __progname);

	exit(1);
}

int
spit(char *net, char *msg, int len)
{
	struct	hostent *he;
	struct	sockaddr_in sin;
	int	fd;
	int	optval = 1;

	he = gethostbyname(net);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr = *((struct in_addr *)he->h_addr);
	sin.sin_port = htons(7);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
	sendto(fd, msg, len, 0, (struct sockaddr *)&sin, sizeof(sin));
	close(fd);

	return 0;
}

int
main(int argc, char **argv)
{
	char	msg[17][6];
	char	macaddr[6];
	char	*net, *mac;
	int	c, i;

	net = bcast;
	mac = NULL;

	while ((c = getopt(argc, argv, "n:")) != -1)
		switch (c) {
		case 'n':
			net = strdup(optarg);
			break;
		default:
			usage();
		}
	
	argc -= optind;
	argv += optind;

	if (!argc)
		usage();
	
	mac = strdup(*argv);
	parsemac(macaddr, mac);

	memset(msg[0], 0xff, sizeof(msg[0]));
	for (i = 0; i < 16; ++i)
		memcpy(msg[i+1], macaddr, sizeof(macaddr));
	
	spit(net, (char *)msg, sizeof(msg));

	return 0;
}
