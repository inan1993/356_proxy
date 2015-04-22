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

#include <fcntl.h>

#include <signal.h>

#include <stdbool.h>

#define d 0



//#include <winsock2.h>

//#ifndef SOL_TCP

//    #define SOL_TCP 6  // socket options TCP level

//#endif

//#ifndef TCP_USER_TIMEOUT

 //   #define TCP_USER_TIMEOUT 18  // how long for loss retry before timeout [ms]

//#endif

struct server_data{

	//char request[10000];

	int receive;

	int sock;

	struct sockaddr* browserAddr;

};

struct web_cache{

	struct web_cache *next;

	char* url;

	char* contents;

	int size;

};

struct web_cache* global_Cache = NULL;

int cacheSpace = 0;

int Max_Cache_Size;

int server(uint16_t port, int cacheSize);

void* handleRequests(void* data);

#define MAX_MSG_LENGTH (520000)

#define MAX_BACK_LOG (5)

#define CHUNK_SIZE 1024

int main(int argc, char ** argv)

{

	int port = atoi(argv[1]);

	Max_Cache_Size = atoi(argv[2]);

	cacheSpace = Max_Cache_Size;

	server(port, Max_Cache_Size);

	printf("exiting \n");

}



int server(uint16_t port, int cacheSize)

{

 int sock, receive;

	struct sockaddr_in server_addr;

	struct sockaddr* browser_addr;

	char request[MAX_MSG_LENGTH];

	signal(SIGPIPE, SIG_IGN);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("Create socket error:");

		return 1;

	}

	 server_addr.sin_family = AF_INET;

	 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	 server_addr.sin_port = htons(port);



 	bind (sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

	while(listen(sock, 10) != 0){

		if(d == 1)printf("Listen error");



                }



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

if(d == 1)	printf("Socket created\n");

	return 0;



}

void* handleRequests(void* data){

if(d == 1)	printf("spawning new thread! \n");

	struct server_data* currData = (struct server_data*) data;

	char request[MAX_MSG_LENGTH];	

	int receive = currData -> receive;

//	int sock = currData -> sock;

	struct sockaddr* browser_addr = currData -> browserAddr;

	int port = 80;

		

if(d == 1)	printf("Server started %d\n", port);



	int recv_len = 0;

	

if(d == 1)		printf("entering loop \n");



		recv_len = read(receive, request, MAX_MSG_LENGTH);

		printf("recieved request\n");

		if(recv_len == 0){

if(d == 1)	printf("killing \n");

			close(receive);			

			return NULL;

                }

		char protocol [1000], url [3000], type [1000];

		int r = sscanf(request,"%[^ ] %[^ ] %[^ ]" ,type, url, protocol);

		if(r != 0) {

if(d == 1)printf("doesn't match with sscanf call! \n");		

		};

if(d == 1)printf(" stats: %s %s %s %d \n", type, url, protocol, r);

if(d == 1)printf("%s %d \n", request, recv_len);

		if(strcmp(type, "GET")){

if(d == 1)printf("NOT GET REQUEST\n");

			close(receive);			

			return NULL;	

		}

		struct addrinfo* ai = (struct addrinfo*) malloc(sizeof(struct addrinfo));

		char newUrl[3000];

		sscanf(url, "http://%[^/]", newUrl);	

		int g = getaddrinfo(newUrl, "80", NULL, &ai);

		struct addrinfo* ai2;

		int c = 1;

		int serveSock;

if(d == 1)printf("newUrl created");

		if ((serveSock = socket(AF_INET, SOCK_STREAM, 0 ))<0){

if(d == 1)printf("socket failed! \n");

			}

		for(ai2 = ai; ai2 != (struct addrinfo*)0; ai2 = ai2 -> ai_next){

			

			

			 c = connect(serveSock, (struct sockaddr*)ai2 -> ai_addr, ai2 -> ai_addrlen);

			

			

			if(c == 0) break;

			printf("%d %d %d %s\n",c, errno, g, newUrl);

		}		

		if(c){

if(d == 1)printf("no connections made :(\n");

				close(serveSock);

				close(receive);

			return NULL;		

		}

		if(url == NULL){

			close(receive);

			close(serveSock);

			return;

		}

		if(checkInCache(url, receive, 1)){

			close(receive);

			close(serveSock);

			return;

		} 







		//fcntl(serveSock, F_SETFL, O_NONBLOCK);

		int s1 = write(serveSock, request, recv_len/*,0*/);

if(d == 1)printf("request sent %d \n",s1);

			

		char response[520000];

//		int timeout = 10000;  // user timeout in milliseconds [ms]

//		setsockopt (serveSock, SOL_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof (timeout));



		while (1) {		

		

		int response_len = recv_timeout(serveSock, 5, response);//recv(serveSock, response, MAX_MSG_LENGTH, 0);//recv_timeout(serveSock, 5, response);

		//recv(serveSock, response, MAX_MSG_LENGTH, 0);

		struct web_cache* newEntry = (struct web_cache*)malloc (sizeof(struct web_cache));

		newEntry -> url = url;

		newEntry -> contents = response;

		newEntry -> size = response_len;

		newEntry -> next = NULL;

		addToCache(newEntry);

if(d == 1)printf("waiting for response: from %s %d  \n", url, response_len);

		//printf("send that shouldnt be there: %s \n", response);

		//break;

		//printf("received response: %s \n", response);

		//receive = accept(sock, NULL,NULL);

		//int c2 = connect(sock, browser_addr, sizeof(browser_addr));

		//printf("second send: %d %d \n", c2, errno);

	//	send(receive, response, MAX_MSG_LENGTH,0);

//		if (response_len < 0) {

//			printf("Recv error: \n");

//			return NULL;

//		}





		if(response_len <= 0){

			close(receive);

			close(serveSock);

			//while(listen(serveSock, 10) < 0){

if(d == 1)	printf("Listen error \n");

				return NULL;

			//}

      		

		}

		int s = write(receive, response, response_len);	

		//printf( "%d \n", send(receive, request, MAX_MSG_LENGTH*3,0));//{

//		printf("received response:  %s\n", response);

		//reply[recv_len] = 0;

		//memset(reply, 0, sizeof(reply));

	}

if(d == 1)printf("Closing socket \n");

	close(receive);

	close(serveSock);

	return NULL;

}



int recv_timeout(int s , int timeout, char *result)

{

    int size_recv , total_size= 0;

    struct timeval begin , now;

    char chunk[CHUNK_SIZE];

    double timediff;

    

    //make socket non blocking

    fcntl(s, F_SETFL, O_NONBLOCK);

    

    //beginning time

    gettimeofday(&begin , NULL);

    

    while(1)

    {

        gettimeofday(&now , NULL);

        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

        if( total_size > 0 && timediff > timeout )

        {

            break;

        }

        else if( timediff > timeout*2)

        {

            break;

        }



        memset(chunk ,0 , CHUNK_SIZE); 

        if((size_recv =  recv(s , chunk , CHUNK_SIZE , 0) ) <= 0)

        {



            usleep(100000);

        }

        else

        {

            // printf("got a chunk of size %d\n", size_recv);

            memcpy(result+total_size, chunk, size_recv);

            

            total_size += size_recv;

            //reset beginning time

            gettimeofday(&begin , NULL);

        }

    }

    

    return total_size;

}



int checkInCache(char* url, int socket, int sendBool){

	struct web_cache *curr = global_Cache;

	while(curr!=NULL){

		if(strcmp (url, curr->url)==0){

			printf("Cache Hit! %d \n", sendBool);

			if(sendBool == 1) printf("YAYSOIUDHAFJDNFJAKBFLAKJSBDASKFBDAF\n");

		if(sendBool){

			// POINTER ?? &curr

			displayCacheHit(curr, socket);

			updateCacheLRU(url);

		}

			return 1;

		}

		curr = curr->next;

	}

	return 0;

}

void displayCacheHit(struct web_cache* cacheHit, int socket){

	int s;

	if((s = write(socket, cacheHit->contents, cacheHit->size))<0){

		perror("Writing Error!\n");

		return;

	}

// read() ???

	return;

}

void updateCacheLRU(char *url){

	struct web_cache* cacheHit;

    struct web_cache *curr = global_Cache;

    while(curr->next!=NULL){

    	printf("%p, %p, %s, \n, %s \n", curr, curr -> next, url, curr -> next -> url);

		if(strcmp (url, curr->next->url)==0){

			printf("Cache Updating!");

			cacheHit = curr->next;

    		curr->next = cacheHit->next;

    		struct web_cache* temp = global_Cache;

			cacheHit->next = temp;

			global_Cache = cacheHit;

			printf("Cache LRU updated!!\n");

			return;

		}

		curr = curr->next;

	}



		printf("CACHE HIT NOT FOUND FOR UPDATE or is ALREADY HEAD!! %s\n\n", url);

		return;

}

void printCache(){

	struct web_cache *curr = global_Cache;

	int c=1;

    while(curr!=NULL){

		printf("%d. Cached site: %s\n", c, curr->url);

		curr = curr->next;

		c++;

	}

	return;

}

void addToCache(struct web_cache* newEntry ){

	if(checkInCache(newEntry -> url, 0, 0) )return;

	int size = newEntry->size;

	if(size + strlen(newEntry->url) + sizeof(newEntry->next)  >= Max_Cache_Size){

		printf("Cache Too Small for this Responsen\n");

		return;

	}

	if(global_Cache == NULL){

		global_Cache = newEntry;

		cacheSpace -= size + strlen(newEntry->url) + sizeof(newEntry->next);

		return;

	}

	while((size + strlen(newEntry->url) + sizeof(newEntry->next)) >= cacheSpace){

		printf("Making Room for Entry\n");

		removeStaleEntry(/*newEntry*/);

	}

	if((size + strlen(newEntry->url) + sizeof(newEntry->next))< cacheSpace){

		cacheSpace = cacheSpace - size + strlen(newEntry->url) + sizeof(newEntry->next);

		printf("Remaining Cache Size: %d\n", cacheSpace);

		struct web_cache *temp = global_Cache;

		newEntry->next = temp;

		global_Cache = newEntry;

	//	prnitf("Cache Entry Added: %s\n", url);

		return;

	}

}

void removeStaleEntry(/*struct web_cache newEntry*/){

	struct web_cache *prev = (struct web_cache*)malloc(sizeof (struct web_cache));

	struct web_cache *last = global_Cache;

	while(last->next!=NULL){

		prev = last;

		last = last->next;

	}

	int sizeOfLastEntry = sizeof(last->next)+strlen(last->url)+strlen(last->contents);

	cacheSpace = cacheSpace + sizeOfLastEntry;

	prev->next = NULL;

//	free(last);

	// addToCache(newEntry);

	return;

}
