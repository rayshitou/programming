#include "fc/crypto/private_key.hpp"
#include "fc/crypto/sha256.hpp"

#include "sign.hpp"

using digest_type = fc::sha256;

int sign(const char *data, unsigned int length, const char *prikey, char *signature)
{
	digest_type digest = digest_type::hash(data, length);

  return 0;
}

int sign_hash(const char *hash, int length, const char *prikey, char *signature)
{
	return 0;
}
