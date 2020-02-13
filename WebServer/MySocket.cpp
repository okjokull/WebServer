#include "stdafx.h"
#include "MySocket.h"

#pragma comment(lib,"ws2_32.lib")

bool MySocket::init_winsocket = false;

MySocket::MySocket()
{
   if (!init_winsocket)
   { 

	   WSADATA info;
	   if (WSAStartup(MAKEWORD(2, 2), &info))
		   throw "Coule not start WSA";
	   init_winsocket = true;
   }
   socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (INVALID_SOCKET == socket_ )
   {
	   throw"Invalid socket";
   }
}


MySocket::MySocket(SOCKET new_socket)
{
	socket_ = new_socket;

}

std::string MySocket::ReceiveLine()
{
	std::string ret;
	while (true)
	{
		char buf;
		switch (recv(socket_,&buf,1,0))
		{
		case 0:
			return ret;
		case -1:
			return "";
		}
		ret += buf;
		if (buf == '\n')
		{
			break;
		}
	}
	return ret;
}

bool MySocket::SendLine(std::string buffer)
{
	buffer += '\n';
	return SOCKET_ERROR  != send(socket_, buffer.c_str(), buffer.length(), 0);
}

void MySocket::Close()
{
	closesocket(socket_);
}

MySocket::~MySocket()
{
}

SocketClient::SocketClient(const std::string &host, int port)
{
	bool isok = false;
	do 
	{
		//shibai
		hostent  *he = gethostbyname(host.c_str());
		if (!he)
		{
			break;
		}
		sockaddr_in  addr = { 0 };
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr = *((in_addr*)he->h_addr);

		if (  SOCKET_ERROR ==  connect(socket_,(sockaddr *)&addr,sizeof(sockaddr)))
		{
			break;
		}
		isok = true;
	} while (FALSE);
	if (!isok)
	{
		char error_msg[MAXBYTE] = { 0 };
		DWORD  erroc_code = WSAGetLastError();
		strerror_s(error_msg, WSAGetLastError());
		throw error_msg;
	}


	
}

SocketServer::SocketServer(int port, /*端口 */ int connectios, /*接受的连接数 */ TypeSocket type /*= BlockingSocket*/)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	if (type == UnBlockingSocket)
	{
		unsigned long arg = 1;
		ioctlsocket(socket_, FIONBIO, &arg);
	}
	if (SOCKET_ERROR == bind(socket_,
		(sockaddr*)&addr,sizeof(sockaddr)))
	{

		Close();
		throw "Bind Error";
	}
	listen(socket_, connectios);
}

MySocket* SocketServer::Accept()
{
	MySocket * accept_socket = new MySocket
		(accept(socket_, nullptr, nullptr));
	if (*accept_socket == INVALID_SOCKET)
	{
		throw "Invalid socket";
	}
	return accept_socket;

}

