#include "private_key.hpp"
#include "sha256.hpp"

#include <string.h>
#include <stdio.h>
#include "sign.hpp"

using digest_type = fc::sha256;
using private_key_type = fc::crypto::private_key;
using signature_type = fc::crypto::signature;

int do_sign(const digest_type& digest, const char *prikey, char *sign_out)
{
    std::string str_prikey = prikey;
    private_key_type pkey(str_prikey);
    printf("key: %s\n", str_prikey.c_str());
    signature_type sig = pkey.sign(digest);
    std::string str_sig = std::string(sig);
    printf("sig: %s\n", str_sig.c_str());
    strncpy(sign_out, str_sig.c_str(), str_sig.size());
    return str_sig.size();
}
int sign(const char *data, unsigned int length, const char *prikey, char *sign_out)
{
    if( data && prikey && length > 0 && sign_out ) {
	const digest_type& digest = digest_type::hash(data, length);
        printf("ss: %s\n", std::string(digest).c_str());
        return do_sign(digest, prikey, sign_out);
    }
    return 0;
}

int sign_hash(const char *hash, int length, const char *prikey, char *sign_out)
{
    if( hash && prikey && length > 0 && sign_out ) {
	const digest_type digest(hash, length);
        return do_sign(digest, prikey, sign_out);
    }
    return 0;
}
