#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    printf("Please enter the message: ");
		std::string msg;
		std::getline(std::cin, msg);
		printf("\n");
		for(unsigned int i = 0; i < msg.length(); i++)
			printf("%d ", msg.at(i));

    n = write(sockfd, msg.c_str(), msg.length() + 1);

    if (n < 0) 
         error("ERROR writing to socket");

		msg = "";
    printf("Please enter the message: ");
		std::getline(std::cin, msg);
		printf("\n");
		for(unsigned int i = 0; i < msg.length(); i++)
			printf("%d ", msg.at(i));

    n = write(sockfd, msg.c_str(), msg.length() + 1);

    if (n < 0) 
         error("ERROR writing to socket");
    close(sockfd);
		printf("DONE\n");

	bzero(buffer, 256);
	//Set it so that receive is blocking
	int option = fcntl(sockfd, F_GETFL);
	option = option & ~O_NONBLOCK;
	fcntl(sockfd, F_SETFL, option);

	n = recv(sockfd, buffer, 5, 0); 
	buffer[6] = '\0';
	std::string data = buffer ;
	printf("DATA: %s\n", data.c_str());

    return 0;
}
