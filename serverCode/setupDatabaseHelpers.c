// handle any database error
void dbError(MYSQL *connection){
	fprintf(stderr, "%s\n", mysql_error(connection));
	mysql_close(connection);
	exit(1);
}

// create Props database, create posts table, create columns in posts and populate the columns with data from data.csv
MYSQL * setupDatabase(MYSQL *connection, char user[], char password[]){

	printf("MySQL client version: %s\n", mysql_get_client_info());
	
	connection = mysql_init(NULL);	
	
	if(connection == NULL){
		dbError(connection);
	}else{
		printf("Created connection pointer ... \n");
	}
	if(mysql_real_connect(connection, MYSQL_HOST, user, password, NULL, 0, NULL, 0) == NULL){
		dbError(connection);
	}else{
		printf("Connected to mysql at host: %s\n", MYSQL_HOST);
	} 
	
	if(mysql_query(connection, "CREATE DATABASE Props")){
		dbError(connection);
	}else{
		printf("Created empty database Props ...\n");
	}
	
	if (mysql_query(connection, "USE Props")) {      
		dbError(connection);
	}else{
		printf("Selected Props ...\n");
	}
	
	if (mysql_query(connection, "CREATE TABLE posts (date varchar(12) not null, external_id varchar(20) not null, number varchar(12) not null, constant varchar(1) not null, digits varchar(1) not null, decimals varchar(1) not null)")) {      
		dbError(connection);
	}else{
		printf("Created columns in Props ...\n");
	}
	
	if (mysql_query(connection, "LOAD DATA LOCAL INFILE 'data.csv' INTO TABLE Props.posts")){      
		dbError(connection);
	}else{
		printf("Populated database Props with data from csv file ...\n");
	}
	
	return connection;
}