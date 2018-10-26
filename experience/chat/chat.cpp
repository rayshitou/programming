#include "chat.hpp"


IChat::IChat(const char* ip, int port){
	inet_addr.sin_family = AF_INET;
	inet_addr.sin_port   = htons(port);

	if( ip ) {
		inet_pton(AF_INET, ip, &inet_addr.sin_addr);
	} else {
		inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
}

SChat::SChat(const char* ip, int port)
: IChat(ip, port)
{

}

void SChat::CT_Listen() {
	CT_Socket();
	CT_Bind();
	Listen(sock_listen, LISTENQ);
}

string SChat::Combine_IPandPort(const sockaddr_in& addr_cli) {
	string str_out("");
	char ipstr[20] = {'\0'};
	char  portstr[8] = {'\0'};
	if( inet_ntop(AF_INET, &addr_cli.sin_addr, ipstr, sizeof(ipstr)) != NULL) {
		str_out += ipstr;
		int sport = ntohs(addr_cli.sin_port);
		if (sport != 0) {
				snprintf(portstr, sizeof(portstr), ":%d", sport);
				str_out += portstr;
		}
	}

	return str_out;
}

void SChat::User_List(const int sk) {
	Do_UserList(sk);
	//Close(sk);
	return;
}

void SChat::Do_UserList(const int sk) {
	string str_out("");

	map<string, string>::iterator it = map_n_iport.begin();
	while(it != map_n_iport.end())
	{
		str_out += it->first + ":";
		it++;
	}
	Write(sk, const_cast<char*>(str_out.c_str()), str_out.size());
	return;
}

void SChat::User_Login(const char* cmd, const int sk, const sockaddr_in& addr_cli, const int pipe_w) {
	//const string str_name(cmd+3);
//	int pipefd[2];
//
//	if (pipe(pipefd) == -1) {
//	   perror("pipe");
//	   exit(EXIT_FAILURE);
//   }
//
//	pid_t cpid = Fork() ;
//
//	if( 0 == cpid ) {
//		Close(sock_listen);
		Do_UserLogin(cmd, sk, pipe_w, addr_cli);
		//exit(0);
//	} else if ( 0 < cpid ) {
//		char buf[513] = {'\0'};
//		read(pipefd[0], &buf, 512);
//		map_n_iport.insert(make_pair(str_name, buf));
//		close(pipefd[0]);
//		close(pipefd[1]);
//	} else if ( -1 == cpid ){
//		perror("fork");
//		exit(EXIT_FAILURE);
//	}
//	Close(sk);
}

string SChat::Do_UserLogin(const char* cmd, const int sk, const int pipe_w, const sockaddr_in& addr_cli) {
	string str_out("");
	const string str_user(cmd+3);

	//string str_iport = Combine_IPandPort(addr_cli);
	str_out += "hello@" + str_user;
	Write(sk, const_cast<char*>(str_out.c_str()), str_out.size());
	write(pipe_w, str_user.c_str(), str_user.size());
	//wait(NULL);

	return str_out;
}

void SChat::User_Logout(const int sk, const char* user) {
	Do_UserLogout(sk, user);
	//Close(sk);
}

void SChat::Do_UserLogout(const int sk, const char* user) {
	char str[128] = {'\0'};
	int n = map_n_iport.erase(user);

	strcat(str, user);

	if( 0 == n )
		strcat(str, " is not existed!");
	else
		strcat(str, " logout.");

	Write(sk, str, strlen(str));
}

void SChat::Chat(const int sk, const sockaddr_in& addr_cli) {
	int pipefd[2];

	if (pipe(pipefd) == -1) {
	   perror("pipe");
	   exit(EXIT_FAILURE);
   }

	pid_t cpid = Fork() ;

	if( 0 == cpid ) {
		Close(sock_listen);
		char	recvline[MAXLINE + 1];
		int n = read(sk, recvline, MAXLINE);
		recvline[n-1] = 0;

		const string str_cmd(recvline, recvline+2);

		if( str_cmd.compare("ls") == 0) {
			User_List(sk);
		} else if( str_cmd.compare("li") == 0 ) {
			User_Login(recvline, sk, addr_cli, pipefd[1]);
		} else if( str_cmd.compare("lo") == 0 ) {
			User_Logout(sk, recvline+3);
		}
		exit(0);
	}  else if ( 0 < cpid ) {
		char buf[513] = {'\0'};
		string str_iport = Combine_IPandPort(addr_cli);
		read(pipefd[0], &buf, 512);
		map_n_iport.insert(make_pair(buf, str_iport));
		close(pipefd[0]);
		close(pipefd[1]);
	} else if ( -1 == cpid ){
		perror("fork");
		exit(EXIT_FAILURE);
	}

	Close(sk);
}

void SChat::CT_Start() {
	for ( ; ; ) {
			int sock_conn;
			socklen_t   len;
			struct sockaddr_in cliaddr;
			sock_conn = Accept(sock_listen, (SA *) &cliaddr, &len);

			Chat(sock_conn, cliaddr);
//			char	recvline[MAXLINE + 1];
//			int n = read(sock_conn, recvline, MAXLINE);
//			recvline[n-1] = 0;
//
//			const string str_cmd(recvline, recvline+2);
//
//			if( str_cmd.compare("ls") == 0) {
//				User_List(sock_conn);
//			} else if( str_cmd.compare("li") == 0 ) {
//				User_Login(recvline, sock_conn, cliaddr);
//			} else if( str_cmd.compare("lo") == 0 ) {
//				User_Logout(sock_conn, recvline+3);
//			}
		}
}

void SChat::CT_Bind() {
	Bind(sock_listen, (SA *) &inet_addr, sizeof(inet_addr));
}

void SChat::CT_Socket() {
	sock_listen = Socket(AF_INET, SOCK_STREAM, 0);
	int on = 1;
	setsockopt(sock_listen, SOL_SOCKET, SO_REUSEPORT, &on, sizeof (on));
}
