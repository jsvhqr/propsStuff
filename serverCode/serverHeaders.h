void dbError(MYSQL *connection);

void connectionError(void);

char** str_split(char* a_str, const char a_delim);

void replaceWildcards(char *str);

int hasWildcards(char *str);

MYSQL* setupDatabase(MYSQL *connection, char user[], char password[]);

void listenforClients(int port, MYSQL* connection);

void handleClientRequest(SOCKET s, MYSQL *connection);

char * buildSQLQuery(char queires []);

void sendResponseToQuery(char query[], MYSQL* connection,  SOCKET client_socket);

char * appendStrings(char *str1, char *str2);
