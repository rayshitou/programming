#include <eosio/nxsd_api_plugin/nxsd_wallet_manager.hpp>
#include <eosio/wallet_plugin/wallet_manager.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace eosio {

nxsd_wallet_manager::nxsd_wallet_manager(application& app, bool has_pub_key)
: _app(app),
  _has_pub_key(has_pub_key)
{}

nxsd_wallet_manager::~nxsd_wallet_manager()
{
    auto& w_plugin = _app.get_plugin<wallet_plugin>();
    w_plugin.get_wallet_manager().lock(_wallet);
}

void nxsd_wallet_manager::open(const std::string& wallet, const std::string& w_passwd, const fc::public_key pub_key)
{

    auto& w_plugin = _app.get_plugin<wallet_plugin>();
    // begin, 防止已解锁的钱包再解锁，导致异常程序退出
    w_plugin.get_wallet_manager().open(wallet); 
    w_plugin.get_wallet_manager().lock(wallet); 
    // end
    w_plugin.get_wallet_manager().unlock(wallet, RSA_pub_decrypt(w_passwd, pub_key));
}

string nxsd_wallet_manager::RSA_pub_decrypt(const string& base64_en, const fc::public_key pub_key){
    // 如果公钥、私钥、钱包密码是没有加密的或没有解密的公钥，则不处理，直接返回原字符串
    if( !_has_pub_key || 
        boost::algorithm::starts_with(base64_en, "EOS") || // 没有加密的公钥
        boost::algorithm::starts_with(base64_en, "PUB") || // 没有加密的公钥
        boost::algorithm::starts_with(base64_en, "PW") ||  // 没有加密的钱包密码
        51 == base64_en.size()/*没有加密的私钥*/ )
        return base64_en; 

    string str_en = fc::base64_decode( base64_en );
    fc::bytes bytes_decrypted = pub_key.decrypt(fc::bytes(str_en.begin(), str_en.end()));
    return string(bytes_decrypted.begin(), bytes_decrypted.end());
}

} // namespace eosio