#pragma once
#include<iostream>
#include<fstream>
#include<winSock2.h>
#include<windows.h>
#include<string>
#include"constpara.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class SocketServe {

public:
	SocketServe();
	~SocketServe();

	bool newsocket();//创建一个socket
	bool bindsocket();//建立链接
	bool listensoket();//设置监听
	SOCKET acceptsocket();//连接并生成会话
	bool sendsocket(SOCKET& clientSocket, const string& file_path);//调用send进行会话
	bool recvsocket(SOCKET& clientSocket);//调用recv进行会话
	bool closeserve();//关闭socket
	bool beginserve(SOCKET sclient, const string& response_path);

private:
	string reqpath(const string& res, const string& default_path);
	string find_type(const string& file_path);
	int socket_error;
	int file_len(const string& file_path);
	SOCKET slisten;
	string homepath = HOME_PATH;
};