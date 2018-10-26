#ifndef UNP_CHAT_H
#define UNP_CHAT_H

#include "unp.h"
#include <map>
#include <string>

using namespace std;

class IChat{
public:
	virtual void CT_Socket() = 0;
	virtual void CT_Listen() {};
	virtual void CT_Connect() {};
	virtual void CT_Bind() {};
	virtual void CT_Start() = 0;
	virtual ~IChat() {}

	IChat(const char* ip, int port);
protected:
	struct sockaddr_in      inet_addr;
};

class SChat: public IChat{
public:
	void CT_Socket();
	void CT_Listen();
	void CT_Start();
	void CT_Bind();

	SChat(const char* ip = NULL, int port = 9999);
private:
	void Chat(const int sk, const sockaddr_in& addr_cli);
	void User_Logout(const int sk, const char* user);
	void Do_UserLogout(const int sk, const char* user);
	void User_List(const int sk);
	void Do_UserList(const int sk);
	void User_Login(const char* cmd, const int sk, const sockaddr_in& addr_cli, const int pipe_w);
	string Do_UserLogin(const char* cmd, const int sk, const int pipe_w, const sockaddr_in& addr_cli);
	string Combine_IPandPort(const sockaddr_in& addr_cli);
private:
	typedef struct {
		string ip;
		int port;
	}IPORT;

	int sock_listen;
	map<string, string> map_n_iport;
};

class CChat{};

#endif
