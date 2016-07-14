#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <mysql.h>
#include <assert.h>

#pragma comment(lib, "Ws2_32.lib")

//globals
static const char MYSQL_HOST[]  = "localhost";

#include "serverHeaders.h"

#include "setupDatabaseHelpers.c"

#include "clientInteractionHelpers.c"

// main entry for Server
int main(int argc, char *argv[]){

	
	int  server_port;
	char mysql_user[100];
	char mysql_password[100];
	
	
	MYSQL *connection = NULL;
	
	if(argc < 4){
		printf("Server is run like this ->   server.exe server_port mysql_user mysql_password");
		exit(1);
	}else{
		
		server_port = atoi(argv[1]);
		strcpy(mysql_user, argv[2]);
		strcpy(mysql_password, argv[3]);
	}

	printf("Setting up mysql database -> [name: %s, table: %s, user: %s, password %s, csv: %s]\n", "Props", "posts", mysql_user, mysql_password, "data.csv");

	connection = setupDatabase(connection, mysql_user, mysql_password);
	
	listenforClients(server_port, connection);
	
	mysql_close(connection);
	
	return 0;
}