/* $Id$ */
/*
 * Copyright (c) 2009 Dimitri Sokolyuk <demon@dim13.org>
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	parse(char *macaddr, char *s);
int	emit(char *net, char *msg, size_t sz);
int	wol(char *net, char *mac);
void	usage(void) __attribute__((noreturn));

int
parse(char *macaddr, char *s)
{
	char	*p;
	int 	i;
	long	l;

	for (i = 0; i < 6; ++i) {
		l = strtol(s, &p, 0x10);
		macaddr[i] = (char)l;
		s = p + 1;
	}

	return 0;
}

int
emit(char *net, char *msg, size_t sz)
{
	struct	hostent *he;
	struct	sockaddr_in sin;
	int	fd;
	int	yes = 1;

	he = gethostbyname(net);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr = *((struct in_addr *)he->h_addr);
	sin.sin_port = htons(7);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(fd);
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
	sendto(fd, msg, sz, 0, (struct sockaddr *)&sin, sizeof(sin));
	close(fd);

	return 0;
}

int
wol(char *net, char *mac)
{
	char	msg[17][6];
	char	macaddr[6];
	int	i;

	mac = strdup(mac);
	assert(mac);
	parse(macaddr, mac);

	memset(msg[0], 0xff, sizeof(msg[0]));
	for (i = 0; i < 16; ++i)
		memcpy(msg[i+1], macaddr, sizeof(macaddr));
	
	emit(net, (char *)msg, sizeof(msg));
	free(mac);

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
main(int argc, char **argv)
{
	char	*bcast = "255.255.255.255";
	int	c;

	while ((c = getopt(argc, argv, "n:")) != -1)
		switch (c) {
		case 'n':
			bcast = strdup(optarg);
			assert(bcast);
			break;
		default:
			usage();
		}
	
	argc -= optind;
	argv += optind;

	if (!argc)
		usage();
	
	wol(bcast, *argv);
	
	return 0;
}
