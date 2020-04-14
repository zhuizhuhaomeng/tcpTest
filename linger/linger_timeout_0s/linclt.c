/* simple client to test SO_LINGER option
 * See: http://deepix.github.io/2016/10/21/tcprst.html
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(1);
	}

	int portno;
	portno = atoi(argv[2]);

	/* create socket */
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	/* get server */
	struct hostent *server;
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	/* set linger option */
	{
		struct linger sl;
		bzero(&sl, sizeof(sl));
		sl.l_onoff = 1;
		sl.l_linger = 0;
		if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl)) < 0)
			perror("setsockopt");
	}

	/* connect to server */
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		perror("connect");
		exit(1);
	}

	/* send some data */
	char buffer[256];
	bzero(buffer, 256);
	int i, len = sprintf(buffer, "GET /\r\n\r\n");
	for (i=0; i<10; i++) {
		/* FIXME: n can be < len */
		int n = send(sockfd, buffer, len, 0);
		if (n < 0) {
			perror("send");
			exit(1);
		}
	}

	/* set non-blocking? */
#if 0
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)
		perror("fcntl");
#endif

	sleep(1);
	printf("calling close now\n");
	system("date");
	if (close(sockfd) < 0)
		perror("close");
	printf("exiting now\n");
	system("date");

	return 0;
}
