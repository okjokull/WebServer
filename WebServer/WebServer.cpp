// WebServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"
#include <iostream>
#include <process.h>
#include <fstream>

unsigned int __stdcall connection(void *p)
{
	using namespace std;
	MySocket *accept_socket = (MySocket *)p;
	std::ifstream file("1.txt");
	string buf;
	while (getline(file,buf))
	{
		accept_socket->SendLine(buf);
	}
	while (true)
	{
		std::string buffer = accept_socket->ReceiveLine();
		if (buffer.empty())
			break;
		std::cout << buffer;
	}
	delete accept_socket;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	try
	{
		SocketServer web_server(80, 10);
		while (true)
		{
			MySocket* accept_socket = web_server.Accept();
			CloseHandle((HANDLE)_beginthreadex(nullptr, 0, connection, accept_socket, 0, nullptr));
		}
	   
	}
	catch (const char * e)
	{
		std::cout << e << std::endl;

	}
	catch (...)
	{
		std::cout << "Unknow Error" << std::endl;
	}



	return 0;
}

