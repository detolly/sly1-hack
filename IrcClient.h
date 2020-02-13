#pragma once

#include "stdio.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

//making the small menus 00134DAC 00170E04

class IrcClient {
private:
	const char* username, oauth;
	char** channel;
	bool isInChannel;
	int sockethandle;
	struct sockaddr_in serv_addr;

public:

	IrcClient(const char*, const char*);
	void Join(char** channel);
	void Part();
	void sendMessage(const char*);


};