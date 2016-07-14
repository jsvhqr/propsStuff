Setup the mysql database
-------------------------------------------

- install mysql http://dev.mysql.com/downloads/installer/ (developer), remember the password you choose for root :).

- If after the installation you have this file (C:\Program Files (x86)\MySQL\MySQL Connector C 6.1\lib\libmysql.lib) then everything is fine (It wasn't for me..)

- If you do not have that file, goto https://dev.mysql.com/downloads/connector/c/ and download+install the 64bit (I presume you have 64 bit.)

- You don't need to enter the csv data into the database, my server reads it in. However, if for some reason you wan't to restart the server, 
  you need to drop the database first. This is done by opening "MYSQL command line client" entering your password, then type "drop database props;".



Compile and Run server code
--------------------------------------------------------

- First navigate (I used Visual Studios Developer Command Promt) to the serverCode folder

- To compile server.c, type "cl /I "C:\Program Files\MySQL\MySQL Server 5.7\include" /MD "C:\Program Files (x86)\MySQL\MySQL Connector C 6.1\lib\libmysql.lib" server.c"

- You need to both run and compile the server inside this folder, as the program needs the "libmysql.dll" inside it.

- To run server.exe type "server.exe 9090 root YourRootPassword", It needs to be 9090, as I hardcoded it in the client.

- If everything goes well it should say something like:
	
	Setting up mysql database -> [name: Props, table: posts, user: root, password props, csv: data.csv]
	MySQL client version: 6.1.6
	Created connection pointer ...
	Connected to mysql at host: localhost
	Created empty database Props ...
	Selected Props ...
	Created columns in Props ...
	Populated database Props with data from csv file ...
	Initialised Windows sockets...
	Created TCP socket ...
	Bound socket to port 9090
	Server is now listening for incomming connections ... 


Compile and Run client code
---------------------------------------------------------

- Navigate to clientCode folder

- To compile, simply run "cl clientGUI.c", don't bother with client.c it's an old testing file that I forgot to remove.

- To run, -> "clientGUI.exe"

- Searching should be obvious, but beware, I do supbar error checking so you can probably crash it if you do something weird :D.

- It supports the '*' and '?' wildcards



Remarks
------------------------------------------------------------
- I'm aware the Server should have been multithreaded, but I haven't used threads in C on windows before, so I thought I'd avoid that :)

- If you search for something like *,*,*,*,*,* then it will ofc return every single entry in the database, it seems like the client doesn't handle it very well.

- I have not looked into why yet and It's because I can't really spend more time on this now..

- If there is anything else you can reach my at johsn@kth.se or 073-9039908

- I'm aware the GUI is awful, but I have never built a GUI in C before :)



