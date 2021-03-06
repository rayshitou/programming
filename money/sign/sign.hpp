
#ifndef SIGN_HPP_
#define SIGN_HPP_

#ifdef __cplusplus
extern "C" {
#endif

int sign(const char *data, unsigned int length, const char *prikey, char *sign_out);

int sign_hash(const char *hash, int length, const char *prikey, char *sign_out);

#ifdef __cplusplus
}
#endif

#endif /* SIGN_HPP_ */
