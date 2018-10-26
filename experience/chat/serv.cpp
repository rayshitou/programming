#include "chat.hpp"

int main(int argc, char* argv[]){
	SChat serv;
	serv.CT_Listen();
	serv.CT_Start();

	pthread_exit(0);
}
