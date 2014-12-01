#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

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

		unsigned buflen = 1000000;
    char buffer[buflen];
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

		printf("***************************************************************************\n");
    bzero(buffer,buflen);
    printf("Press Enter to send reference XML:  simple-c");
    fgets(buffer,buflen-1,stdin);



	std::string refXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Circuit0><Adder1>0</Adder1><Adder8>0</Adder8><Adder16>0</Adder16><Adder32>0</Adder32><Mux2>0</Mux2><Mux4>0</Mux4><Outputs><Name>Out_2</Name><InputCount>3</InputCount><Name>Z_Out_2</Name><InputCount>3</InputCount></Outputs></Circuit0>";
    n = write(sockfd,refXML.c_str(), refXML.length()+1);
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,buflen);
    n = read(sockfd,buffer,buflen-1);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("RESULT FROM SERVER:\t%s\n",buffer);



		printf("***************************************************************************\n");
    bzero(buffer,buflen);
    printf("Press Enter to send reference XML: 4(1-bit adder)");
    fgets(buffer,buflen-1,stdin);

	refXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Circuit0><Adder1>4</Adder1><Adder8>0</Adder8><Adder16>0</Adder16><Adder32>0</Adder32><Mux2>0</Mux2><Mux4>0</Mux4><Outputs><Name>Out_2</Name><InputCount>2</InputCount><Name>Z_Out_2</Name><InputCount>2</InputCount><Name>Z_Out_2</Name><InputCount>2</InputCount><Name>Z_Out_2</Name><InputCount>2</InputCount></Outputs></Circuit0>";
    n = write(sockfd,refXML.c_str(), refXML.length()+1);
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,buflen);
    n = read(sockfd,buffer,buflen-1);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("RESULT FROM SERVER:\t%s\n",buffer);


		printf("***************************************************************************\n");
    bzero(buffer,buflen);
    printf("Press Enter to send reference XML: (8-bit adder)");
    fgets(buffer,buflen-1,stdin);

	refXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Circuit0><Adder1>0</Adder1><Adder8>1</Adder8><Adder16>0</Adder16><Adder32>0</Adder32><Mux2>0</Mux2><Mux4>0</Mux4><Outputs><Name>Out_2</Name><InputCount>2</InputCount><Name>Z_Out_2</Name><InputCount>2</InputCount></Outputs></Circuit0>";
    n = write(sockfd,refXML.c_str(), refXML.length()+1);
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,buflen);
    n = read(sockfd,buffer,buflen-1);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("RESULT FROM SERVER:\t%s\n",buffer);
    



		printf("***************************************************************************\n");
		bzero(buffer,buflen);
    printf("Press Enter to send reference XML: (8-bit adder tree)");
    fgets(buffer,buflen-1,stdin);

	refXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Circuit0><Adder1>0</Adder1><Adder8>7</Adder8><Adder16>0</Adder16><Adder32>0</Adder32><Mux2>0</Mux2><Mux4>0</Mux4><Outputs><Name>Out_2</Name><InputCount>8</InputCount></Outputs></Circuit0>";
    n = write(sockfd,refXML.c_str(), refXML.length()+1);
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,buflen);
    n = read(sockfd,buffer,buflen-1);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("RESULT FROM SERVER:\t%s\n",buffer);

		
		

    close(sockfd);
    return 0;
}
