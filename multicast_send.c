#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	const int delay_secs = 1;
	
	if (argc != 3)
	{
		printf("Usage: %s mcast_group port\n", argv[0]);
		exit(1);
	}

	char* group = argv[1];
	int port = atoi(argv[2]);

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		perror("socket");
		return 1;
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(group);
	addr.sin_port = htons(port);

	int counter = 0;
	while (1)
	{
		char message[256];
		int size = sprintf(message, "heartbeat %d", counter);
		int nbytes = sendto(fd, message, size, 0, (struct sockaddr*) &addr, sizeof(addr));
		if (nbytes < 0)
		{
			perror("sendto");
			return 1;
		}
		printf("-->%s\n", message);
		sleep(delay_secs);
		counter++;
	}

	return 0;
}