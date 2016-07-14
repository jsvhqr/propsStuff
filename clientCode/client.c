#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "clientHeaders.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[]){
	
	int port;
	char server_address[100];
	
	SOCKET sock; 
    struct sockaddr_in echoServAddr;                 
    char *to_send;               
    char server_response[1024] = "";     
    int response_length;         
    WSADATA wsaData;
	int length_to_send;

	
	if(argc < 3){
		printf("Client is run like this ->   client.exe server_port server_address ");
		exit(1);
	}else{
		
		port = atoi(argv[1]);
		strcpy(server_address, argv[2]);
	}
	
	printf("Client started on with Server port %d and address: %s\n" , port, server_address);
	
	printf("Trying to get posts with date, 2008-11-24\n");
	
	to_send = "'2008-11-24','*','380625*','1','?','0'";
	
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) 
		{
			fprintf(stderr, "WSAStartup() failed");
			exit(1);
		}
		
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
			printf(" socket failed with error code : %d" , WSAGetLastError());
			exit(1);
		}
		
		
		memset(&echoServAddr, 0, sizeof(echoServAddr));     
		echoServAddr.sin_family      = AF_INET;             
		echoServAddr.sin_addr.s_addr = inet_addr(server_address);   
		echoServAddr.sin_port        = htons(port); 
		

		if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
			printf(" connect failed with error code : %d" , WSAGetLastError());
			exit(1);
		}else{
			printf("Connected to server ...\n");
		}
		
		if (send(sock, to_send, strlen(to_send) + 1, 0) != (strlen(to_send) + 1)){
			printf(" send failed with error code : %d" , WSAGetLastError());
			exit(1);
		}else{
			printf("Sent query to server successfully ...\n");
		}
		int i = 0;
		do {

			response_length = recv(sock, server_response, 1024, 0);
			if ( response_length > 0 ){
				 printf("received: %s\n", server_response);
				 i++;
			}else if ( response_length == 0 )
				printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());
			
			
		} while( response_length > 0 );
		
		printf("Got %d all posts \n", i);
		
		
		closesocket(sock);
	
	
	return 0;
}