#pragma once

#include <appbase/application.hpp>
#include <eosio/wallet_plugin/wallet_plugin.hpp>
#include <eosio/nxsd_api_plugin/pke.hpp>

namespace eosio {
using namespace appbase;

class nxsd_wallet_manager {
    public:
        nxsd_wallet_manager(application& app, bool has_pub_key);
        ~nxsd_wallet_manager();
        void open(const std::string& wallet, const std::string& w_passwd, const fc::public_key pub_key);
    
    private:
        string RSA_pub_decrypt(const string& base64_en, const fc::public_key pub_key);

    private:
        application& _app;
        std::string _wallet;
        bool _has_pub_key;
}; // class nxsd_wallet_manager

} // namespace eosio