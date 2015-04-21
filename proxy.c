#include <stdio.h>

#include <string.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <stdlib.h>

#include <pthread.h>

#include <netdb.h>

#include <errno.h>

struct server_data{

	//char request[10000];

	int receive;

	int sock;

	struct sockaddr* browserAddr;

};



int server(uint16_t port, int cacheSize);

void* handleRequests(void* data);



#define MAX_MSG_LENGTH (520000)

#define MAX_HTTP_HEADER 8000

#define MAX_BACK_LOG (5)



int main(int argc, char ** argv)

{

	int port = atoi(argv[1]);

	int cacheSize = atoi(argv[2]);

	server(port, cacheSize);

	printf("exiting \n");

}



int server(uint16_t port, int cacheSize)

{

 int sock, receive;

	struct sockaddr_in server_addr;

	struct sockaddr* browser_addr;

	char request[MAX_MSG_LENGTH];



	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("Create socket error:");

		return 1;

	}

	 server_addr.sin_family = AF_INET;

	 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	 server_addr.sin_port = htons(port);



 	bind (sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

	listen(sock, 10);



	while(1){

		

		receive = accept(sock, NULL,NULL);

		pthread_t thread;

		struct server_data* data = (struct server_data*) malloc(sizeof(struct server_data));

		//strcpy(data -> request, request);

		data -> receive = receive;

		data -> sock = sock;

		data -> browserAddr = browser_addr;

		pthread_create(&thread, NULL, handleRequests, (void*)data);

		



//			break;

	}	

	printf("Socket created\n");

	return 0;



}

void* handleRequests(void* data){

	printf("spawning new thread! \n");

	struct server_data* currData = (struct server_data*) data;

	char request[MAX_MSG_LENGTH];	

	int receive = currData -> receive;

//	int sock = currData -> sock;

	struct sockaddr* browser_addr = currData -> browserAddr;

	int port = 80;

		

	printf("Server started %d\n", port);



	int recv_len = 0;

	

		printf("entering loop \n");



		recv_len = read(receive, request, MAX_MSG_LENGTH);

		printf("recieved request\n");

		if(recv_len == 0){

			printf("killing \n");			

			kill();

                }

		char protocol [1000], url [2048], type [1000];

		int r = sscanf(request,"%[^ ] %[^ ] %[^ ]" ,type, url, protocol);

		if(r != 0) {

			

			printf("doesn't match with sscanf call! \n");		

		};

		printf(" stats: %s %s %s %d \n", type, url, protocol, r);

		printf("%s %d \n", request, recv_len);

		if(strcmp(type, "GET")){

			printf("NOT GET REQUEST\n");			

			return NULL;	

		}

		struct addrinfo* ai = (struct addrinfo*) malloc(sizeof(struct addrinfo));

		char newUrl[2048];

		int p= sscanf(url, "http://%[^/]", newUrl) != 0;

		if(p != 0){

			sscanf(url, "https://%[^/]", newUrl) != 0;

		}

		int g = getaddrinfo(newUrl, "80", NULL, &ai);

		printf("g:%d newURL: %s", g, newUrl);

		struct addrinfo* ai2;

		int c = 1;

		int serveSock;

		printf("newUrl created: %s \n", newUrl);

		if ((serveSock = socket(AF_INET, SOCK_STREAM, 0 ))<0){

				printf("socket failed! \n");

			}

		struct sockaddr_in *tempAddr;

		char ipAddr[32];

		for(ai2 = ai; ai2 != NULL; ai2 = ai2 -> ai_next){

			// c = connect(serveSock, (struct sockaddr*)ai -> ai_addr, ai -> ai_addrlen);

        	tempAddr = (struct sockaddr_in *) ai2->ai_addr;

        	strcpy(ipAddr , inet_ntoa( tempAddr->sin_addr));

			//printf("request sent %d \n",s);

			//if(c == 0) break;

			//printf("%d %d %d %s\n",c, errno, g, newUrl);

		struct sockaddr_in finalAddr;

		printf("IP ADDR: %s \n", ipAddr);

		finalAddr.sin_addr.s_addr = inet_addr(ipAddr);

    	finalAddr.sin_family = AF_INET;

    	finalAddr.sin_port = htons(80);	

    	c = connect(serveSock,(struct sockaddr*) &finalAddr, sizeof(finalAddr));

		

		}



		if(c){

			printf("no connections made :(\n");

			close(serveSock);

			return NULL;		

		}

		int s = send(serveSock, request, recv_len,0);

			

		char response[MAX_MSG_LENGTH];

	

		while (1) {		

		

		int response_len = read(serveSock, response, MAX_MSG_LENGTH);

		

		if(s < 0){

			printf("THIS DOESNT WORK\n");

		}

		if(response_len <= 0){

			

			break;

      		

		}	

		int s = write(receive, response, response_len/*,0*/);

		printf("waiting for response: %s %d \n", newUrl, response_len);

	}

	printf("Closing socket \n");

	close(receive);

	close(serveSock);

	return NULL;

}

