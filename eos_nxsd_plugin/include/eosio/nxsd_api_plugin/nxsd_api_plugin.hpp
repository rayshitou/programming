/**
 *  @file
 *  @copyright by nongxiang times company
 */
#pragma once
#include <appbase/application.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/wallet_plugin/wallet_plugin.hpp>

namespace eosio {

using namespace appbase;

class nxsd_api_plugin : public appbase::plugin<nxsd_api_plugin> {
public:
   nxsd_api_plugin();
   nxsd_api_plugin(const nxsd_api_plugin&) = delete;
   nxsd_api_plugin(nxsd_api_plugin&&) = delete;
   nxsd_api_plugin& operator=(const nxsd_api_plugin&) = delete;
   nxsd_api_plugin& operator=(nxsd_api_plugin&&) = delete;
   ~nxsd_api_plugin();

   APPBASE_PLUGIN_REQUIRES((http_plugin) (chain_plugin) (wallet_plugin))
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   std::unique_ptr<struct nxsd_api_plugin_impl> my;
};

}