#include "private_key.hpp"
#include "sha256.hpp"

#include <string.h>
#include "sign.hpp"

#include "exception.hpp"

using digest_type = fc::sha256;
using private_key_type = fc::crypto::private_key;
using signature_type = fc::crypto::signature;

int do_sign(const digest_type& digest, const char *prikey, char *sign_out)
{
    std::string str_prikey = prikey;
    private_key_type pkey(str_prikey);
    std::string str_sig("");
    int itry_times = 1;
    while(itry_times <= 23){
      try{
    	signature_type sig = pkey.sign(digest);
    	str_sig = std::string(sig);
    	strncpy(sign_out, str_sig.c_str(), str_sig.size());
        break;
      } catch( const fc::exception& e ) {
          ++itry_times;
      }
    }
    return str_sig.size();
}
int sign(const char *data, unsigned int length, const char *prikey, char *sign_out)
{
    if( data && prikey && length > 0 && sign_out ) {
	const digest_type& digest = digest_type::hash(data, length);
        return do_sign(digest, prikey, sign_out);
    }
    return 0;
}

int sign_hash(const char *hash, int length, const char *prikey, char *sign_out)
{
    if( hash && prikey && length > 0 && sign_out ) {
	try{
          std::string str_hash_hex(hash, length);
	  char cstr_hash[32];
	  std::size_t h_len = fc::from_hex(str_hash_hex, cstr_hash, 32);
	  const digest_type digest(cstr_hash, h_len);
          return do_sign(digest, prikey, sign_out);
	} catch( const fc::exception& e ) {
          printf("%s\n", e.to_detail_string().c_str());
        }
    }
    return 0;
}
