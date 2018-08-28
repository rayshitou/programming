#include <stdio.h>
#include <string.h>
#include "sign.hpp"
#include "exception.hpp"

int main(int argc, char **argv){
        char sig[100] = {'\0'};
	char *data = "you and me";
        char *privkey = "5KYZdUEo39z3FPrtuX2QbbwGnNP5zTd7yyr2SC1j299sBCnWjss";
        try {
	 sign(data, strlen(data), privkey, sig);
        } catch( const fc::exception& e ) {
          printf("err msg: %s\n", e.to_detail_string().c_str());
          return 1;
	}
        printf("signature: %s\n", sig);
	return 0;
}
