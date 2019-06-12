
#ifndef __AQUA__SDL2_KOS_SOCKET_SOCKET_H
	#define __AQUA__SDL2_KOS_SOCKET_SOCKET_H
	
	#define SOCKET_AF_INET 1
	#define SOCKET_SOCK_STREAM 2
	
	#define SOCKET_IP_LOCALHOST 0
	#define SOCKET_IP_INTERNAL 1
	#define SOCKET_IP_EXTERNAL 2
	
	#define SOCKET_CLIENT 0
	#define SOCKET_SERVER 1
	#define SOCKET_MIXED 2
	
	#define SOCKET_REUSE_ADDRESS ((void*) 0)
	#define SOCKET_REUSE_PORT -1
	
	#define SOCKET_DEFAULT_BUFFER_SIZE 1024
	
	typedef unsigned long long* ip_address_t;
	
	typedef struct {
		unsigned long long type; // either SOCKET_SERVER, SOCKET_CLIENT or SOCKET_MIXED (default)
		unsigned long long port;
		void* __internal_pointer; // this is handled by the KOS; the programmer should not use this
		
	} socket_t;
	
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	
	ssize_t read(int fd, void *buf, size_t count); // from <unistd.h>
	
	typedef struct {
		signed long long type;
		signed long long socket;
		
		struct sockaddr_in address;
		signed long long   address_length;
		
		char* buffer;
		char __padding__[SOCKET_DEFAULT_BUFFER_SIZE];
		
	} __internal_socket_t;
	
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
	
	unsigned long long socket_client(unsigned long long ____this, unsigned long long __host_ip, unsigned long long port) {
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
		
		return 0;
		
		error: {
			socket_close((unsigned long long) __this);
			return 1;
			
		}
		
	}
	
	unsigned long long socket_server(unsigned long long ____this, unsigned long long __host_ip, unsigned long long port) {
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
		
		return 0;
		
		error: {
			socket_close((unsigned long long) __this);
			return 1;
			
		}
		
	}
	
	unsigned long long socket_send(unsigned long long ____this, unsigned long long __data, unsigned long long bytes) {
		const char* data = (const char*) __data;
		socket_t* __this = (socket_t*) ____this;
		send((int) ((__internal_socket_t*) __this->__internal_pointer)->socket, data, bytes, 0);
		return 0;
		
	}
	
	char* socket_receive(unsigned long long ____this, unsigned long long bytes) {
		socket_t* __this = (socket_t*) ____this;
		__internal_socket_t* sock = (__internal_socket_t*) __this->__internal_pointer;
		
		memset      (sock->buffer, '\0',         bytes);
		read  ((int) sock->socket, sock->buffer, bytes);
		
		return sock->buffer;
		
	}
	
	static void socket_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		
		if      (command[0] == 's') kos_bda_implementation.temp_value =                      socket_send   (command[1], command[2], command[3]);
		else if (command[0] == 'r') kos_bda_implementation.temp_value = (unsigned long long) socket_receive(command[1], command[2]);
		
		else if (command[0] == 'v') kos_bda_implementation.temp_value = socket_server(command[1], command[2], command[3]);
		else if (command[0] == 'l') kos_bda_implementation.temp_value = socket_client(command[1], command[2], command[3]);
		
		else if (command[0] == 'c') socket_close(command[1]);
		
		else KOS_DEVICE_COMMAND_WARNING("socket")
		*result = &kos_bda_implementation.temp_value;
		
	}
	
#endif
