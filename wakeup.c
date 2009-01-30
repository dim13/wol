#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <err.h>

#define DEFAULTMAC "00:0a:5e:09:6a:25"
#define DEFAULTNET "10.0.0.255"

int	parsemac(unsigned char *, char *);

int
main(int argc, char **argv)
{
	struct hostent *he;
	struct sockaddr_in sin;
	struct in_addr inaddr;
	char   *msg;
	char   *mac = DEFAULTMAC;
	char   *net = DEFAULTNET;
	char    macaddr[6];
	int     sockfd;
	int     optval = 1;
	int     i, j;
	int     msglen = sizeof(macaddr) * 16 + 6;
	
	msg = malloc(sizeof(char) * msglen);
	memset(msg, 0xff, 6);
	parsemac(macaddr, mac);
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 6; j++) {
			msg[i * 6 + 6 + j] = macaddr[j];
		}
	}
//	he = gethostbyname(net);
	inet_aton(net, &inaddr);
	memset(&sin, NULL, sizeof(sin));
	sin.sin_family = AF_INET;
//	sin.sin_addr = *((struct in_addr *) he->h_addr);
	sin.sin_addr = inaddr;
	sin.sin_port = htons(7);
	sockfd = socket(AF_INET, SOCK_DGRAM, NULL);
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
	sendto(sockfd, msg, msglen, NULL, (struct sockaddr *)&sin, sizeof(sin));
	free(msg);
	close(sockfd);
	return 0;
}

int
parsemac(unsigned char * macaddr, char * mac)
{
	char   *pp;
	long    l;
	int     i;
	
	for (i = 0; i < 6; i++) {
		l = strtol(mac, &pp, 16);
		macaddr[i] = (unsigned char)l;
		mac = pp + 1;
	}
	return 0;
}
