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
};

int server(uint16_t port, int cacheSize);
void* handleRequests(void* data);
#define MAX_MSG_LENGTH (10000)
#define MAX_BACK_LOG (5)

int main(int argc, char ** argv)
{
	int port = atoi(argv[1]);
	int cacheSize = atoi(argv[2]);

		return server(port, cacheSize);
}

int server(uint16_t port, int cacheSize)
{
 int sock, receive;
	struct sockaddr_in server_addr;
	char request[MAX_MSG_LENGTH];

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
	 server_addr.sin_family = AF_INET;
	 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 server_addr.sin_port = htons(port);

 	bind (sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	while(listen(sock, 10) != 0){
                        printf("Listen error");

                }

	while(1){
		printf("spawning new thread!");
		receive = accept(sock, NULL,NULL);
		pthread_t thread;
		struct server_data* data = (struct server_data*) malloc(sizeof(struct server_data));
		//strcpy(data -> request, request);
		data -> receive = receive;
		data -> sock = sock;
		pthread_create(&thread, NULL, handleRequests, (void*)data);

//			break;
	}	
//	printf("Socket created\n");
	return 0;

}
void* handleRequests(void* data){
	struct server_data* currData = (struct server_data*) data;
	char request[MAX_MSG_LENGTH];	
	int receive = currData -> receive;
	int sock = currData -> sock;
	int port = 80;
		
	printf("Server started %d\n", port);

	int recv_len = 0;
	while (1) {
		printf("entering loop \n");

		recv_len = read(receive, request, MAX_MSG_LENGTH);
		if(recv_len == 0){
			kill();
                }
		char protocol [1000], url [1000], type [1000];
		int r = sscanf(request,"%[^ ] %[^ ] %[^ ]" ,type, url, protocol);
		printf("%s %s %s %d \n", type, url, protocol, r);
		//printf("%s  %s %d \n", reply, request, recv_len);
		if(strcmp(type, "GET")){
			printf("NOT GET REQUEST \n");			
			return NULL;	
		}
		struct addrinfo* ai = (struct addrinfo*) malloc(sizeof(struct addrinfo));
		char newUrl[1000];
		sscanf(url, "http://%[^/]", newUrl);	
		int g = getaddrinfo(newUrl, "80", NULL, &ai);
		struct addrinfo* ai2;
		int c = 1;
		int serveSock;
		for(ai2 = ai; ai2 != (struct addrinfo*)0; ai2 = ai2 -> ai_next){
			
			if ((serveSock = socket(AF_INET, SOCK_STREAM, 0 ))<0){
				printf("socket failed! \n");
			}
			 c = connect(serveSock, (struct sockaddr*)ai -> ai_addr, ai -> ai_addrlen);
			if(c == 0) break;
			printf("%d %d %d %s\n",c, errno, g, newUrl);
		}		
		if(c){
			printf("no connections made :(\n");
			return NULL;		
		}
		char response[10000];
		send(serveSock, request, MAX_MSG_LENGTH,0);
		int response_len = read(serveSock, response, MAX_MSG_LENGTH);

//		if (response_len < 0) {
//			printf("Recv error: \n");
//			return NULL;
//		}
//		if(response_len == 0){
//			
//			while(listen(serveSock, 10) < 0){
//				printf("Listen error");
//			}
  //      		receive = accept(sock, NULL,NULL);
	//	}	
		//printf( "%d \n", send(receive, request, MAX_MSG_LENGTH*3,0));//{
		printf("received response:  %s\n", response);
		//reply[recv_len] = 0;
		//memset(reply, 0, sizeof(reply));
	}
	printf("Closing socket");
	close(sock);
	return NULL;
}
