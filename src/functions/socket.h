
#ifndef __AQUA__SDL2_KOS_SOCKET_SOCKET_H
	#define __AQUA__SDL2_KOS_SOCKET_SOCKET_H
	
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	
	#include "../lib/macros.h"
	#include "../lib/structs.h"
	
	ssize_t read(int fd, void *buf, size_t count); // from <unistd.h>
	
	typedef struct {
		signed long long type;
		signed long long socket;
		
		struct sockaddr_in address;
		signed long long   address_length;
		
		char* buffer;
		char __padding__[SOCKET_DEFAULT_BUFFER_SIZE];
		
	} __internal_socket_t;
	
	unsigned long long socket_support(void) {
		return 1;
		
	}
	
	static void socket_socket(unsigned long long ____this) {
		socket_t* __this = (socket_t*) ____this;
		__this->__internal_pointer =                   (__internal_socket_t*) malloc(sizeof(__internal_socket_t));
		
		((__internal_socket_t*) __this->__internal_pointer)->buffer = (char*) malloc(SOCKET_DEFAULT_BUFFER_SIZE);
		memset(((__internal_socket_t*) __this->__internal_pointer)->buffer, '\0',    SOCKET_DEFAULT_BUFFER_SIZE);
		
	}
	
	static int socket_actor(unsigned long long ____this, int type) {
		socket_t* __this = (socket_t*) ____this;
		__internal_socket_t* sock = (__internal_socket_t*) __this->__internal_pointer;
		
		__this->type = (unsigned long long) type;
		sock->type   =                      type;
		
		sock->address_length = sizeof(sock->address);
		sock->socket         = socket(AF_INET, SOCK_STREAM, 0);
		
		if (sock->socket < 0) {
			printf("WARNING Failed to open socket (%d)\n", (int) sock->socket);
			return 1;
			
		} else {
			return 0;
			
		}
		
	}
	
	void socket_close(unsigned long long ____this) {
		socket_t* __this = (socket_t*) ____this;
		
		free(__this->__internal_pointer                                  /* sizeof(__internal_socket_t) */);
		free(((__internal_socket_t*) __this->__internal_pointer)->buffer /* SOCKET_DEFAULT_BUFFER_SIZE  */);
		
	}
	
	void socket_client(unsigned long long ____this, unsigned long long __host_ip, unsigned long long port) {
		ip_address_t host_ip = (ip_address_t) __host_ip;
		
		socket_t* __this = (socket_t*) ____this;
		__this->port = port;
		
		socket_socket((unsigned long long) __this);
		__internal_socket_t* sock = (__internal_socket_t*) __this->__internal_pointer;

		int temp_error;
		if (socket_actor((unsigned long long) __this, SOCKET_CLIENT)) {
			goto error;
			
		}
		
		bzero(&sock->address, sizeof(sock->address));
		
		sock->address.sin_family = AF_INET;
		sock->address.sin_port   = htons(port);
		
		if (inet_aton((const char*) host_ip, (struct in_addr*) &sock->address.sin_addr.s_addr) == 0) {
			printf("WARNING Failed to convert host IP address\n");
			goto error;
			
		}
		
		temp_error = connect((int) sock->socket, (struct sockaddr*) &sock->address, sizeof(sock->address));
		if (temp_error != 0) {
			printf("WARNING Failed to connect (%d)\n", temp_error);
			goto error;
			
		}
		
		return;
		
		error: {
			socket_close((unsigned long long) __this);
			
			__this->error = 1;
			return;
			
		}
		
	}
	
	void socket_server(unsigned long long ____this, unsigned long long __host_ip, unsigned long long port) {
		ip_address_t host_ip = (ip_address_t) __host_ip;
		
		socket_t* __this = (socket_t*) ____this;
		__this->port = port;
		
		socket_socket((unsigned long long) __this);
		__internal_socket_t* sock = (__internal_socket_t*) __this->__internal_pointer;

		int option = 1;

		int temp_error;
		if (socket_actor((unsigned long long) __this, SOCKET_SERVER)) {
			goto error;
			
		}

		temp_error = setsockopt((int) sock->socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
		
		if (temp_error) {
			printf("WARNING Failed to set socket options (%d)\n", temp_error);
			goto error;
			
		}
		
		sock->address.sin_family      = AF_INET;
		sock->address.sin_addr.s_addr = INADDR_ANY;
		sock->address.sin_port        = htons(port);
		
		temp_error = bind((int) sock->socket, (struct sockaddr*) &sock->address, sizeof(sock->address));
		if (temp_error < 0) {
			printf("WARNING Failed to bind socket (%d)\n", temp_error);
			goto error;
			
		}
		
		temp_error = listen((int) sock->socket, 3);
		if (temp_error < 0) {
			printf("WARNING Socket failed to listen (%d)\n", temp_error);
			goto error;
			
		}
		
		sock->socket = accept((int) sock->socket, (struct sockaddr*) &sock->address, (socklen_t*) &sock->address_length);
		if (sock->socket < 0) {
			printf("WARNING Socket failed to accept (%lld)\n", sock->socket);
			goto error;
			
		}
		
		return;
		
		error: {
			socket_close((unsigned long long) __this);
			
			__this->error = 1;
			return;
			
		}
		
	}
	
	void socket_send(unsigned long long ____this, const char* data, unsigned long long bytes) {
		socket_t* __this = (socket_t*) ____this;
		send((int) ((__internal_socket_t*) __this->__internal_pointer)->socket, data, bytes, 0);
		
	}
	
	char* socket_receive(unsigned long long ____this, unsigned long long bytes) {
		socket_t* __this = (socket_t*) ____this;
		__internal_socket_t* sock = (__internal_socket_t*) __this->__internal_pointer;
		
		memset      (sock->buffer, '\0',         bytes);
		read  ((int) sock->socket, sock->buffer, bytes);
		
		return sock->buffer;
		
	}
	
	#define SOCKET_FUNCTIONS 1
	
#endif
