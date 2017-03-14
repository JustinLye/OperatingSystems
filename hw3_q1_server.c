//CS 4323 OS HW3 Q1 - Justin Lye
//I wrote this program with help from: http://linuxhowtos.org/C_C++/socket.htm

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

// global message constants
const char *SERVER_REPLY_MSG = "server:\tmessage received\n";
const char *SERVER_CLOSE_MSG = "server:\tgoodbye\n";
const char *SERVER_GREETING_MSG = "server:\tHello! I'm read to chat.\n";
const char *EXIT_MSG = "quit";	/* client message to signal the chat is over */

const int MAX_BUFFER_SIZE = 256; /* maximum characters allowed in stream buffer */
const int MAX_CONNECTION_BKLOG = 5; /* maximum number of connections in acceptance queue */

void ErrorExit(const char* err_msg);	/* prints error message the exits abnormally */
void InitServerAddr(					/* initializes server address structure */
	struct sockaddr_in* server_address,
	int port_number,
	size_t address_size);	
void WriteToSocket(int socket_fd, const char* msg);						/* write to a socket. will terminate program if write fails */
void ReadFromSocket(int socket_fd, char* stream_buffer, int max_size);  /* read from a socket. will terminate program if read fails */
int ShouldClose(const char* stream_buffer);								/* checks if client connection should be closed */
void CreateSocket(int *socket_fd);										/* creates socket and stores file descriptor in *socket_fd. Will terminate program if create socket fails */

int main(int argc, char *argv[]) {
	int socket_fd = 0;													/* file descriptor entry returned by socket() sys. call */
	int client_socket_fd = 0;											/* file descriptor entry returned by accept() sys. call */
	struct sockaddr_in server_address;									/* structure for address of the server */
	struct sockaddr_in client_address;									/* structure for address of the client */
	int client_address_size = sizeof(client_address);					/* the size of the client address */
	char stream_buffer[MAX_BUFFER_SIZE];								/* client input buffer */

	
	//check for appropriate usage
	if(argc < 2)
		ErrorExit("Fatal error! Port number must be passed.\n\tUsage server [port number]\n");

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)		/* attempt to create a internet domain stream socket w/ TCP protocal */
		ErrorExit("Fatal error! Socket creation failed.\n");
	
	InitServerAddr(&server_address, atoi(argv[1]), sizeof(server_address));	/* initialize server address */

	if(bind(socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) /* attempt to bind socket to server address */
		ErrorExit("Fatal error! Socket could not be bound to server address.\n");
	
	listen(socket_fd, MAX_CONNECTION_BKLOG);		/* allow the process to listen for connections on the socket */
	printf("Server is listening on port %d\n", atoi(argv[1]));		/* output message to show server is ready and listening */
	
	if((client_socket_fd = accept(socket_fd, (struct sockaddr*) &client_address, &client_address_size)) < 0)
		ErrorExit("Fatal error! Client connection rejected.\n");

	printf("Client connection has been accepted. Chat is open.\n");
	WriteToSocket(client_socket_fd, SERVER_GREETING_MSG);
	
	//chat loop
	do {
		bzero(stream_buffer, MAX_BUFFER_SIZE);
		ReadFromSocket(client_socket_fd, stream_buffer, MAX_BUFFER_SIZE - 1);
		printf("client:\t%s", stream_buffer);
		WriteToSocket(client_socket_fd, SERVER_REPLY_MSG);
	} while(!ShouldClose(stream_buffer));
	
	WriteToSocket(client_socket_fd, SERVER_CLOSE_MSG);
	close(client_socket_fd);
	close(socket_fd);

	return 0;
}

void ErrorExit(const char* err_msg) {
	perror(err_msg);
	exit(1);
}

void InitServerAddr(struct sockaddr_in* server_address, int port_number, size_t address_size) {
	bzero((char *)server_address, address_size);	/* initialize elements of server address structure to zero */
	server_address->sin_family = AF_INET;			/* use internet domain socket */
	server_address->sin_port = htons(port_number);  /* convert port_number to network byte order */
	server_address->sin_addr.s_addr = INADDR_ANY;   /* use IP address of the machine on which the server is running */
}

void WriteToSocket(int socket_fd, const char* msg) {
	if(write(socket_fd, msg, strlen(msg)) < 0)
		ErrorExit("Fatal error! Problem occurred writing to socket.\n");
}

void ReadFromSocket(int socket_fd, char* stream_buffer, int max_size) {
	if(read(socket_fd, stream_buffer, max_size) < 0)
		ErrorExit("Fatal error! Problem occurred reading from socket.\n");
}

int ShouldClose(const char* stream_buffer) {
	if(strncmp(stream_buffer, EXIT_MSG, strlen(EXIT_MSG)) == 0)
		return 1;
	else
		return 0;
}