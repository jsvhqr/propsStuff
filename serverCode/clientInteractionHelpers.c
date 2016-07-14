// handle socket problems
void connectionError(void){
	printf("failed with error code : %d" , WSAGetLastError());
	exit(1);
}

// main loop, wait for client to connect, then handle it. This method should have spawned a thread for each connecting client. But I ran out of time.
void listenforClients(int port, MYSQL* connection){
	
	WSADATA wsa;
	SOCKET server_socket , client_socket;
	struct sockaddr_in server_address , client_address;
	
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        connectionError();
    }else{
		printf("Initialised Windows sockets...\n");
	}
     
	
	
	if((server_socket = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP )) == INVALID_SOCKET)
    {
        connectionError();
    }else{
		printf("Created TCP socket ...\n");
	}
 
    /* Construct local address structure */
    memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
    server_address.sin_family = AF_INET;                /* Internet address family */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_address.sin_port = htons(port);      /* Local port */
     
    
    if( bind(server_socket ,(struct sockaddr *)&server_address , sizeof(server_address)) == SOCKET_ERROR)
    {
        connectionError();
    }else{
		printf("Bound socket to port %d\n", port);
	}
	listen(server_socket, 3);
	
	while(1){
		
		int size = sizeof(client_address);
		printf("Server is now listening for incomming connections ... \n");
		if ((client_socket = accept(server_socket, (struct sockaddr *) &client_address, &size)) == SOCKET_ERROR){
			connectionError();
		}
        printf("Connection accepted ...\n");
        handleClientRequest(client_socket, connection);
		closesocket(client_socket);
    }
 
    closesocket(server_socket);
    WSACleanup();
	
}

// gets query from client, builds sql query and responds to client
void handleClientRequest(SOCKET client_socket, MYSQL *connection){
	
	char server_buffer[256];
	int recv_message_size; 
	
        recv_message_size = recv(client_socket, server_buffer, 1024, 0);
        if ( recv_message_size > 0 ){
			printf("received query: %s\n", server_buffer);
			char * sql_query = buildSQLQuery(server_buffer);
			sendResponseToQuery(sql_query, connection, client_socket);
			free(sql_query);
			printf("sent response to client ...\n");
		}else if ( recv_message_size == 0 ){
			printf("Connection closed \n");
		}else{
			printf("recv failed: %d\n", WSAGetLastError());
		}
	
}
// Takes a ready SQL query, promts SQL server and sends responses to client
void sendResponseToQuery(char query[], MYSQL* connection, SOCKET client_socket){
	
	if (mysql_query(connection, "USE Props")){      
		dbError(connection);
	}else{
		printf("Selected Props ...\n");
	}
	
	if (mysql_query(connection, query)){
		printf("Error sql query ...\n");
		dbError(connection);
	}else{
		printf("Executed query succesfully ...\n");
	}
  
	MYSQL_RES *result = mysql_store_result(connection);
  
	if (result == NULL){
		dbError(connection);
	}else{
		printf("Results where found ...\n");
	}

	int num_of_columns = mysql_num_fields(result);
	
	printf("Found %d columns ...\n", num_of_columns);

	MYSQL_ROW row;
  
	while ((row = mysql_fetch_row(result))){
		
		char to_send[100] = "";
		
		for(int i = 0; i < num_of_columns; i++){
				
			
			printf("Column %d has value %s\n", i, row[i]);
			strcat(to_send, " | ");
			strcat(to_send, row[i]);
					
		}
		if (send(client_socket, to_send, strlen(to_send) + 1, 0) != (strlen(to_send) + 1)){
			printf(" send failed with error code : %d" , WSAGetLastError());
			exit(1);
		}else{
			printf("Sent response %s to client successfully ...\n", to_send);
		}
		to_send[0] = '\0';
		
	}
	printf("finished aswering query .. \n");
	
	mysql_free_result(result);

	printf("Freed mysql results ... \n");
}


// Splits commma-seperated list into different queries. Uses these to build a complete SQL query. Makes sure to handle Wildcards like '*' and '?'
// Does appending of strings until complete SQL query is created, if wildcards are found it uses 'LIKE' instead of '=' etc..
char * buildSQLQuery(char queires []){
	
	char * base_sql_query = "SELECT * FROM Posts WHERE ";
	
	char builing_sql_query [2048];
	
	int date = 0;
	int external_id = 1;
	int number = 2;
	int constant = 3;
	int digits = 4;
	int decimals = 5;
	
	const char separator = ',';
	
	char ** split_queries = str_split(queires, separator);
	
	if (split_queries){
        int i;
        for (i = 0; *(split_queries + i); i++){
			if( i == date){
				char * tmp = *(split_queries + i);
				char * base_date_term = "date ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					printf("date=[%s]\n", tmp);
					char * app1 = appendStrings(base_date_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(base_sql_query,app2);
					char * app4 = appendStrings(app3, " AND (");
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					printf("date=[%s]\n", tmp);
					char * app1 = appendStrings(base_date_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(base_sql_query,app2);
					char * app4 = appendStrings(app3, " AND (");
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}else if(i == external_id){
				printf("external_id=[%s]\n", *(split_queries + i));
				char * tmp = *(split_queries + i);
				char * base_external_id_term = "external_id ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					char * app1 = appendStrings(base_external_id_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					char * app1 = appendStrings(base_external_id_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}else if(i == number){
				printf("number=[%s]\n", *(split_queries + i));
				char * tmp = *(split_queries + i);
				char * base_number_term = "number ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					char * app1 = appendStrings(base_number_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					char * app1 = appendStrings(base_number_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}else if(i == constant){
				printf("constant=[%s]\n", *(split_queries + i));
				char * tmp = *(split_queries + i);
				char * base_constant_term = "constant ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					char * app1 = appendStrings(base_constant_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					char * app1 = appendStrings(base_constant_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}else if(i == digits){
				printf("digits=[%s]\n", *(split_queries + i));
				char * tmp = *(split_queries + i);
				char * base_digits_term = "digits ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					char * app1 = appendStrings(base_digits_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					char * app1 = appendStrings(base_digits_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " AND ");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}else if(i == decimals){
				printf("decimals=[%s]\n", *(split_queries + i));
				char * tmp = *(split_queries + i);
				char * base_decimals_term = "decimals ";
				if(hasWildcards(tmp) == 0){
					replaceWildcards(tmp);
					char * app1 = appendStrings(base_decimals_term, "LIKE ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " )");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}else{
					char * app1 = appendStrings(base_decimals_term, "= ");
					char * app2 = appendStrings(app1, tmp);
					char * app3 = appendStrings(app2, " )");
					char * app4 = appendStrings(builing_sql_query, app3);
					strcpy(builing_sql_query, app4);
					free(app4);
					free(app3);
					free(app2);
					free(app1);
					printf("SQL query right now: %s\n", builing_sql_query);
				}
				free(*(split_queries + i));
			}
        }
        printf("\n");
        free(split_queries);
    }
	
	char * to_return =  malloc(strlen(builing_sql_query) + 1);
	strcpy(to_return, builing_sql_query);
	
	
	printf("final sql query: %s\n", to_return);
	return to_return;
	
}	

// splits comma-seperated list
char** str_split(char* a_str, const char a_delim){
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

//replaces wildcars '?' and '*' with corrosponding mysql chars
void replaceWildcards(char *str){
	
	while(*str){
		if(*str == '*'){
			*str = '%';
		}else if(*str == '?'){
			*str = '_';
		}
		str++;
	}
}
// checks if a certain query has a wildcard char
int hasWildcards(char *str){
	
	int to_ret = 1;
	
	while(*str){
		if(*str == '*'){
			to_ret = 0;
		}else if(*str == '?'){
			to_ret = 0;
		}
		str++;
	}
	return to_ret;
}
// appends strings, needs freeing after
char * appendStrings(char *str1, char *str2){
	
	char * new_str ;
		if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
		new_str[0] = '\0';   // ensures the memory is an empty string
		strcat(new_str,str1);
		strcat(new_str,str2);
		} else {
			printf("malloc failed! when trying to append strings %s and &s\n", str1, str2);
			exit(1);
		}
	return new_str;
}
