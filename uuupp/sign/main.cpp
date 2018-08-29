#include <stdio.h>
#include <string.h>
#include "sign.hpp"
#include "exception.hpp"

int main(int argc, char **argv){
        char sig[100] = {'\0'};
	char *data = "you and me";
        // public key: EOS8Jwucnpgfxndtxycfv8JYraQHBvuHgr3Hn38VcdrQ7koDiBCmz
        char *privkey = "5JneHPiz1Ci5yqjAMBcXgDVaecaXd9hSLSap3Aabj6C6M6vtWYv";
        try {
	 sign(data, strlen(data), privkey, sig);
        } catch( const fc::exception& e ) {
          printf("err msg: %s\n", e.to_detail_string().c_str());
          return 1;
	}
        printf("data: %s\n", data);
        printf("prikey: %s\n", privkey);
        printf("signature: %s\n", sig);
	return 0;
}
