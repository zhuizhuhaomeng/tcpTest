/* simple server to test SO_LINGER option
 * See: http://deepix.github.io/2016/10/21/tcprst.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	/* create socket */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	int portno;
	portno = atoi(argv[1]);

	/* avoid "address already in use" errors */
	int enable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	/* bind to a port */
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(1);
	}

	listen(sockfd, 5);

	/* got something: accept */
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	int newsockfd;
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
			(struct sockaddr *) &cli_addr,
			&clilen);
	if (newsockfd < 0) {
		perror("accept");
		exit(1);
	}

	/* sleep, so that a client can fill up the socket buffer */
	sleep(30);

	/* now read it all */
	while (1) {
		char buffer[256];
		int n = read(newsockfd, buffer, sizeof(buffer));
		if (n < 0) {
			perror("read");
			exit(1);
		} else if (n == 0) {
			break;
		}
	}

	/* unreachable */
	return 0;
}
