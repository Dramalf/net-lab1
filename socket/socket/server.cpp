#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include "socketserver.h"
using namespace std;
SocketServe::SocketServe() {
  WORD sockVersion = MAKEWORD(2, 2);
  WSAData wasData;
  if (0 != WSAStartup(sockVersion, &wasData))
  {
    cout<<"初始化socket失败！"<<endl;
    WSACleanup();
  }
}

SocketServe::~SocketServe(){
    closeserve();
}

bool SocketServe::newsocket(){
    //创建套接字
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //如果为空
    if (slisten == INVALID_SOCKET)
    {
        cout<<"套接字创建失败"<<endl;

        return false;
    }
    else{
        cout<<"套接字创建成功"<<endl;
        return true;
    }
}

bool SocketServe::bindsocket(){
    sockaddr_in sin;             //addr地址
    sin.sin_family = AF_INET;         //ipv4
    sin.sin_port = htons(PORT);         //设置端口
    sin.sin_addr.S_un.S_addr = INADDR_ANY; //任意地址都可以访问  
    //绑定
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        cout<<"绑定地址失败"<<endl;
        return false;
    }
    return true;
}

bool SocketServe::listensoket(){
    if (listen(slisten, 5) != SOCKET_ERROR) //可以监听的数量
    {
        cout<<"监听成功"<<endl;
        return true;
    }
    cout<<"监听失败"<<endl;
    return false;
}
SOCKET SocketServe::acceptsocket(){
    SOCKET sclient;      //接收套接字
    sockaddr_in remoteAddr; //对方地址
    int nAddrlen = sizeof(remoteAddr);
    cout<<"等待连接..."<<endl;
     //存储通信的socket
    sclient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
    if (sclient == INVALID_SOCKET)
      {
          cout<<"连接失败"<<endl;
          return INVALID_SOCKET;
      }
    else{
        string clientAddress;
		sockaddr_in clientAddr;
		int nameLen, rtn;
		nameLen = sizeof clientAddress;
		rtn = getpeername(sclient, (LPSOCKADDR)&clientAddr, &nameLen);
		if (rtn != SOCKET_ERROR)
		{
    		clientAddress += inet_ntoa(clientAddr.sin_addr);
			cout << "请求来源IP:" << clientAddress << endl;
			unsigned short ulPPort = clientAddr.sin_port;
			cout << "请求来源端口号:" << ulPPort << endl;
		}
        return sclient;
    }    
}

bool SocketServe::sendsocket(SOCKET& sclient, const string& file_path){
    char buf[SIZE];
	ifstream is(file_path, ios::binary);
	if (!is)
	{
		cout << "请求的文件\"" << file_path << "\"不存在！" << endl;
		sprintf_s(buf, "HTTP/1.1 404 Not Found\nContent-Length: %d\nContent-Type: %s\n\n\0", file_len("404.html"), find_type("404.html").c_str());
		send(sclient, buf, strlen(buf), 0);
		is.open("404.html", ios::binary);
	}
	else
	{
		cout << "发送文件["  << file_path << " ]！" << endl;
		sprintf_s(buf, "HTTP/1.1 200 OK\nContent-Length: %d\nContent-Type: %s\n\n\0", file_len(file_path), find_type(file_path).c_str());
		cout << buf << endl;
		send(sclient, buf, strlen(buf), 0);
	}
	do {
		is.read(buf, SIZE);
		send(sclient, buf, is.gcount(), 0);
	} while (is.gcount() == SIZE);
	is.close();
	return true;
}

bool recvsocket(SOCKET& clientSocket){
    return true;
}



bool SocketServe::closeserve(){
    if (slisten != INVALID_SOCKET)
		closesocket(slisten);
	slisten = INVALID_SOCKET;
	WSACleanup();
	return false;
}
bool SocketServe::beginserve(SOCKET sclient, const string& response_path){
    int length;
	char buf[SIZE];
	FD_SET set;//设置阻塞参数
	timeval tv{ 15,0 };
	while (true)
	{	
		FD_ZERO(&set);
		FD_SET(sclient, &set);
		switch (select(0, &set, NULL, NULL, &tv)) {//检查阻塞
		case SOCKET_ERROR:
			cout << "[ERROR] select error" << endl;
		//	return false;
		case 0:
			if (socket_error == 1)
				cout << endl << "[INFO] Time out..." << endl;
			else
				cout << endl << "[INFO] Send finished..." << endl;
		default:
			break;
		}
		string clientIP;
		sockaddr_in clientAddr;
		int clientLen = sizeof(clientIP);
		int rtn = getpeername(sclient, (LPSOCKADDR)&clientAddr, &clientLen);
		if (rtn != SOCKET_ERROR) {
			clientIP += inet_ntoa(clientAddr.sin_addr);
			cout << "[INFO] Client Address: " << clientIP << ":" << clientAddr.sin_port << endl;
		}
		length = recv(sclient, buf, SIZE, 0);
		if (length <= 0 && GetLastError() != EINTR)
			break;
		buf[length] = '\0';
		string filePath = homepath+reqpath(buf,response_path);
		if (filePath == "\\")
			filePath = "\\index.html";

		sendsocket(sclient, filePath);
	}
	
	return true;
}

string SocketServe::find_type(const string& file_path){
    string t;
    int p=file_path.rfind('.',file_path.size() - 1);
    string postfix = file_path.substr(p+1); 
    if(postfix=="html"||postfix=="htm")
        t="text/html";
    else if (postfix=="txt") 
		t = "text/plain";
	else if (postfix=="jpg"|| postfix==".jpeg" || postfix== ".jpe") 
		t = "image/jpeg";
	else if (postfix==".gif") 
		t = "image/gif";
	
	else if (postfix==".bmp") 
		t = "image/bmp";
	else 
		t = postfix;
	return t;
}

string SocketServe::reqpath(const string& res, const string& default_path){
    int pos1, pos2;
	pos1 = res.find('/', 0);
	if (pos1 == string::npos)
		return NULL;
	pos2 = res.find(' ', pos1);
	string filepath = res.substr(pos1, pos2 - pos1);
	if (filepath == "/")
		filepath = "/"+default_path;
	string::size_type t = filepath.size();
	for (int i = 0; i < t; i++)
		if (filepath[i] == '/')
			filepath[i] = '\\';
	return filepath;
}

int SocketServe::file_len(const string& file_path){
    ifstream ifs(file_path, ios::binary | ios::ate);
	if (!ifs)
	{
		std::cout << "文件打开失败:" << file_path <<endl;
		return -1;
	}
	int length = ifs.tellg();
	ifs.close();
	return length;
}