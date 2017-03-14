// CS 4323 OS HW3 Q1 - Justin Lye
//I wrote this program with help from: http://linuxhowtos.org/C_C++/socket.htm

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

const int MAX_BUFFER_SIZE = 256;
const char* EXIT_SIGNAL = "quit";

void ErrorExit(const char* err_msg);																	/* prints error message the exits abnormally. */
void InitHostServer(struct sockaddr_in *server_address, size_t server_addr_size, struct hostent *host_server, int port_number);	/* initializes connection to host server */
void WriteToSocket(int socket_fd, const char* msg);
void ReadFromSocket(int socket_fd, char* stream_buffer, int max_size);
int ShouldClose(const char* stream_buffer);

int main(int argc, char* argv[]) {
	int socket_fd = 0;						/* socket file descriptor entry returned by socket() sys. call */
	struct sockaddr_in server_address;		/* server address structure */
	struct hostent *host_server;			/* definition of host computer on internet */
	char input_buffer[MAX_BUFFER_SIZE];					/* client input buffer */
	int input_size = 0;						/* number of characters in client input buffer */

	if(argc < 3)		/* check for valid number of arguments */
		ErrorExit("Fatal error! Invalid arguments.\n\tUsage: client [hostname] [port]\n");

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)				/* attempt to create socket */
		ErrorExit("Fatal error! Socket could not be created.\n");
	
	if((host_server = gethostbyname(argv[1])) == NULL)				/* search for valid host */
		ErrorExit("Fatal error! No such host was found.\n");
		
	InitHostServer(&server_address, sizeof(server_address), host_server, atoi(argv[2]));

	if(connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)		/* attempt to connect to host server. */
		ErrorExit("Fatal error! Connection to host failed.\n");
	
	do {
		bzero(input_buffer, MAX_BUFFER_SIZE);
		ReadFromSocket(socket_fd, input_buffer, MAX_BUFFER_SIZE - 1);
		printf("%s\n", input_buffer);
		bzero(input_buffer, MAX_BUFFER_SIZE);
		printf("client:\t");
		fgets(input_buffer, MAX_BUFFER_SIZE - 1, stdin);
		WriteToSocket(socket_fd, input_buffer);
	} while(!ShouldClose(input_buffer));
	bzero(input_buffer, MAX_BUFFER_SIZE);
	ReadFromSocket(socket_fd, input_buffer, MAX_BUFFER_SIZE - 1);
	printf("%s\n", input_buffer);
	bzero(input_buffer, MAX_BUFFER_SIZE);
	ReadFromSocket(socket_fd, input_buffer, MAX_BUFFER_SIZE - 1);
	printf("%s\n", input_buffer);
	close(socket_fd);
	return 0;
}

void ErrorExit(const char* err_msg) {
	perror(err_msg);
	exit(0);
}

void InitHostServer(
	struct sockaddr_in *server_address,
	size_t server_addr_size,
	struct hostent *host_server,
	int port_number) {
		bzero((char*)server_address, server_addr_size);
		server_address->sin_family = AF_INET;
		server_address->sin_port = htons(port_number);
		bcopy((char *)host_server->h_addr, (char *)&(server_address->sin_addr.s_addr), host_server->h_length);
	}
	
void WriteToSocket(int socket_fd, const char* msg) {
	if(write(socket_fd, msg, strlen(msg)) < 0)
		ErrorExit("Fatal error! Problem occurred when writing to socket.\n");
}

void ReadFromSocket(int socket_fd, char* stream_buffer, int max_size) {
	if(read(socket_fd, stream_buffer, max_size) < 0)
		ErrorExit("Fatal error! Problem occurred when reading from socket.\n");
}

int ShouldClose(const char* stream_buffer) {
	if(strncmp(stream_buffer, EXIT_SIGNAL, strlen(EXIT_SIGNAL)) == 0)
		return 1;
	else
		return 0;
}