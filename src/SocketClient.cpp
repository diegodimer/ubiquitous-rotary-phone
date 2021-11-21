#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <inc/Message.hpp>
#include <inc/Socket.hpp>
#include <thread>
#include <poll.h>
#define PORT 4050

using namespace std;

SocketClient::SocketClient(char _hostname[], int _port)
{
	strcpy(m_hostname, _hostname);
	m_port = _port;
	connect_to_server();
};

SocketClient::SocketClient(int _socket)
{
	m_socket = _socket;
};

int SocketClient::send_message(Message _msg)
{
	int n = send(m_socket, &_msg, sizeof(Message), 0);
	if (n < 0)
		printf("ERROR writing to socket");
	return n;
};

Message *SocketClient::receive_message()
{
	Message *message = new Message();
	memset(message, 0, sizeof(Message));
	int n = read(m_socket, message, sizeof(Message)); 
	string empyMessage("");
	if (n <= 0)
	{
		return new Message(Type::SHUTDOWN_REQ, empyMessage.c_str() ); // if couldn't read from server, assume lost connection and closes 
	}

	return message;
};

int SocketClient::connect_to_server()
{
	int n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	server = gethostbyname(m_hostname);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("ERROR opening socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(m_port);
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);

	if (connect(m_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting\n");
		exit(0);
	}

	return 0;
}

void SocketClient::close_connection()
{
	close(m_socket);
}

