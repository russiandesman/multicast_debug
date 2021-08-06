#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLEN 1024

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s mcast_group port\n", argv[0]);
		exit(1);
	}

	int recv_s;
	if ( (recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("recv socket");
		exit(1);
	}

	unsigned int yes = 1;
	if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		perror("setsockopt reuseaddr");
		exit(1);
	}

	struct sockaddr_in mcast_group = {0};
	mcast_group.sin_family = AF_INET;
	mcast_group.sin_port = htons((unsigned short int)strtol(argv[2], NULL, 0));
	mcast_group.sin_addr.s_addr = inet_addr(argv[1]);
	if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0)
	{
		perror("bind");
		exit(1);
	}

	struct ip_mreq mreq = {0};
	mreq.imr_multiaddr = mcast_group.sin_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("add_membership");
		exit(1);
	}

	for (;;)
	{
		int n;
		struct sockaddr_in from;
		char message [MAXLEN];
		int len=sizeof(from);
		if ( (n=recvfrom(recv_s, message, MAXLEN, 0, (struct sockaddr*)&from, &len)) < 0)
		{
			perror("recv");
			exit(1);
		}
		message[n] = '\0';
		printf("message from %s, size=%d\n", inet_ntoa(from.sin_addr), n);
		printf("\t%s \n", message);
	}
}