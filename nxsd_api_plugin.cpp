/**
 *  @file
 *  @copyright by nongxiang times company
 */
#include <eosio/nxsd_api_plugin/nxsd_api_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/utilities/key_conversion.hpp>

#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/variant.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/clamp.hpp>

#include <utility>

namespace eosio { namespace detail {
  struct nxsd_api_empty {};

  struct nxsd_api_keys {
     std::string pub_key;
     std::string prv_key;
  };
}}

FC_REFLECT(eosio::detail::nxsd_api_empty, );
FC_REFLECT(eosio::detail::nxsd_api_keys, (pub_key)(prv_key));

namespace eosio {

static appbase::abstract_plugin& _nxsd_api_plugin = app().register_plugin<nxsd_api_plugin>();

using namespace eosio::chain;
using key_pair = std::pair<std::string, std::string>;
using results_pair = std::pair<uint32_t,fc::variant>;

#define CALL(api_name, api_handle, call_name, invoke_cb) \
{std::string("/v1/" #api_name "/" #call_name), \
   [this](string, string body, url_response_callback response_cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             const auto result = api_handle->invoke_cb(body); \
             response_cb(result.first, fc::json::to_string(result.second)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, response_cb); \
          } \
       }}

struct nxsd_api_plugin_impl{
    enum http_return_codes {
      key_created = 201,
      key_generated_failed = 419
   };

   results_pair create_key(const std::string& body){
      auto pk    = private_key_type::generate();
      auto privs = string(pk);
      auto pubs  = string(pk.get_public_key());
      eosio::detail::nxsd_api_keys keys{pubs, privs};
      
      return { key_created, fc::variant(keys) };
   }
};

void nxsd_api_plugin::set_program_options(options_description&, options_description& cfg) {}

void nxsd_api_plugin::plugin_initialize(const variables_map&) {}

void nxsd_api_plugin::plugin_startup(){
    ilog( "starting nxsd_api_plugin" );
    app().get_plugin<http_plugin>().add_api({
      CALL( nxsd, my, create_key, nxsd_api_plugin_impl::create_key )
   });
}

void nxsd_api_plugin::plugin_shutdown(){

}

nxsd_api_plugin::nxsd_api_plugin()
: my(new nxsd_api_plugin_impl())
{}

nxsd_api_plugin::~nxsd_api_plugin(){}
}