#include <stdio.h>
#include <string.h>
#include "sign.hpp"

int main(int argc, char **argv){
        char sig[100] = {'\0'};
	char *data = "you and me";
        char *privkey = "5Jtfxa51FnM6QPQejXkwybYcwxDqs5EB1zk444K7eUPtDgk49rp";
	sign(data, strlen(data), privkey, sig);
        printf("signature: %s\n", sig);
	return 0;
}
