#include "socketserver.h"
using namespace std;
int main() {
	SocketServe sServer;
	sServer.newsocket();
	sServer.bindsocket();
	sServer.listensoket();
	SOCKET sClient = sServer.acceptsocket();
	sServer.beginserve(sClient,"index.html");
	sServer.closeserve();
}