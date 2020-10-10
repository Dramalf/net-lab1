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

	bool newsocket();//����һ��socket
	bool bindsocket();//��������
	bool listensoket();//���ü���
	SOCKET acceptsocket();//���Ӳ����ɻỰ
	bool sendsocket(SOCKET& clientSocket, const string& file_path);//����send���лỰ
	bool recvsocket(SOCKET& clientSocket);//����recv���лỰ
	bool closeserve();//�ر�socket
	bool beginserve(SOCKET sclient, const string& response_path);

private:
	string reqpath(const string& res, const string& default_path);
	string find_type(const string& file_path);
	int socket_error;
	int file_len(const string& file_path);
	SOCKET slisten;
	string homepath = HOME_PATH;
};