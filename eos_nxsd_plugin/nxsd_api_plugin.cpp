/**
 *  @file
 *  @copyright by nongxiang times company
 */
#include <eosio/chain/config.hpp>
#include <eosio/nxsd_api_plugin/nxsd_api_plugin.hpp>
#include <eosio/wallet_plugin/wallet_manager.hpp>
#include <eosio/utilities/key_conversion.hpp>
#include <eosio/nxsd_api_plugin/httpc.hpp>
#include <eosio/nxsd_api_plugin/pke.hpp>
#include <eosio/nxsd_api_plugin/nxsd_wallet_manager.hpp>

#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/exception/exception.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <boost/format.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>    
#include <boost/process/spawn.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>

#include <utility>
#include <regex>

using namespace eosio::utilities;
using namespace boost::filesystem;

namespace eosio { namespace detail {

    struct nxsd_api_empty {};

    struct nxsd_api_keys {
        std::string pub_key;    // 公钥
        std::string prv_key;    // 私钥
    };

    struct nxsd_api_rpc_account_params {
        std::string name; // RPC接口调用用户名
        std::string pw;   // 用户的密码
    };

    struct nxsd_api_transfer_args {
        std::string from;       // 代币转出账号
        std::string to;         // 代币接收账号
        std::string quantity;   // 转账数量，如：1.0000 EOS，一定是4位小数
        uint64_t    expiration; // 交易过期时长，秒为单位
        std::string memo;       // 备注
    };

    struct nxsd_api_transfer_params {
        std::string wallet;             // 钱包名称
        std::string ppwww;              // 钱包密码
        std::string code;               // 交易合约所在的账户，如：eosio.token
        nxsd_api_transfer_args args;
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号 
    };

    struct nxsd_api_get_balance_params {
        std::string code;       // 拥有查询余额合约的账号，如：eosio.token
        std::string account;    // 需要查询余额的账号
        std::string symbol;     // 代币符号，ALL表示查询账号拥有的所有代币余额
    };

     struct nxsd_api_error_msg {
        int32_t code;          // 错误码
        std::string message;    // 描述信息
    };

    struct nxsd_api_transfer_pushed_results{
        std::string trx_status;     // 交易结果状态，成功：succeed，失败：failed
        std::string trx_id;         // 交易序列号
    };

    struct nxsd_api_transfer_error {
        nxsd_api_error_msg error;
    };

    struct nxsd_api_buyram_params {
        std::string wallet;     // 钱包名称
        std::string ppwww;      // 钱包密码
        std::string payer;      // 付费账号
        std::string receiver;   // 内存的接收账号
        std::string quant;      // 内存数量，代币为单位
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_delegatebw_params {
        std::string wallet;     // 钱包名称
        std::string ppwww;      // 钱包密码
        std::string from;                   // 资源抵押账号
        std::string receiver;               // 抵押的资源接收账号
        std::string stake_net_quantity;     // 抵押的网络带宽，代币为单位，如：1.0000 EOS，一定是4位小数
        std::string stake_cpu_quantity;     // 抵押的CPU带宽，代币为单位，如：1.0000 EOS，一定是4位小数
        bool transfer;                      // 是否允许接收方账号释放抵押的资源，允许：true，禁止：false
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_undelegatebw_params {
        std::string wallet;     // 钱包名称
        std::string ppwww;      // 钱包密码
        std::string from;                   // 释放资源的账号
        std::string receiver;               // 接受资源的账号
        std::string unstake_net_quantity;     // 释放的网络带宽，代币为单位，如：1.0000 EOS，一定是4位小数
        std::string unstake_cpu_quantity;     // 释放的CPU带宽，代币为单位，如：1.0000 EOS，一定是4位小数
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_sellram_params {
        std::string wallet;     // 钱包名称
        std::string ppwww;      // 钱包密码
        string receiver; // 出售内存代币的接收账号
        int64_t amount; // 数量，字节为单位
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_account_keys {
        std::string owner;    // 拥有者权限的公钥
        std::string active;   // active权限的公钥
    };

    struct nxsd_api_create_account_params {
        std::string wallet;     // 钱包名称
        std::string ppwww;      // 钱包密码
        string creator;                 // 账号的创建者账号
        string account_name;            // 被创建的账号
        string stake_net;               // 为使用带宽而抵押的代币数量，如：1.0000 EOS，一定是4位小数
        string stake_cpu;               // 为使用cpu而抵押的代币数量，如：1.0000 EOS，一定是4位小数
        string buy_ram_eos;             // 创建者为新账号购买内存的代币数量，如：1.0000 EOS，一定是4位小数
        bool   transfer;                // 是否允许接收方账号释放抵押的资源，允许：true，禁止：false
        nxsd_api_account_keys keys;     // owner与active权限对于的公钥
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_create_wallet_params {
        std::string wallet_name;                 // 钱包名称
        nxsd_api_rpc_account_params rpc_account; // rpc接口账号
    };

    struct nxsd_api_create_wallet_result {
        std::string wallet_pw;    // 钱包密码，如果有RSA私钥，则加密；否则，密码不加密
    };

    struct nxsd_api_create_rpc_account_params {
        nxsd_api_rpc_account_params creator;   // RPC系统账号
        nxsd_api_rpc_account_params account;   // 用户账号
    };

    struct nxsd_api_list_rpc_account_params{
        std::size_t ncount;
        std::vector<nxsd_api_rpc_account_params> accounts;
    };

   struct nxsd_api_delete_rpc_account_params {
        nxsd_api_rpc_account_params admin;   // RPC系统账号和密码
        std::string account_name;             // 用户账号
    };

   struct nxsd_api_table_action_params {
          std::string wallet;      // 钱包名称
          std::string ppwww;       // 钱包密码
	   	  string contract_account; // 部署了合约的账号
	      string action;           // action名
	      string table;            // 表名
	      string data;             // 表记录数据
          nxsd_api_rpc_account_params rpc_account; // rpc接口账号
   };
}}

FC_REFLECT(eosio::detail::nxsd_api_empty, );
FC_REFLECT(eosio::detail::nxsd_api_table_action_params, (wallet)(ppwww)(contract_account)(action)(table)(data)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_list_rpc_account_params, (ncount)(accounts));
FC_REFLECT(eosio::detail::nxsd_api_rpc_account_params, (name)(pw));
FC_REFLECT(eosio::detail::nxsd_api_delete_rpc_account_params, (admin)(account_name));
FC_REFLECT(eosio::detail::nxsd_api_create_rpc_account_params, (creator)(account));
FC_REFLECT(eosio::detail::nxsd_api_create_wallet_params, (wallet_name)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_create_wallet_result, (wallet_pw));
FC_REFLECT(eosio::detail::nxsd_api_account_keys, (owner)(active));
FC_REFLECT(eosio::detail::nxsd_api_create_account_params, (wallet)(ppwww)(creator)(account_name)(stake_net)(stake_cpu)(buy_ram_eos)(keys)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_sellram_params, (wallet)(ppwww)(receiver)(amount)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_keys, (pub_key)(prv_key));
FC_REFLECT(eosio::detail::nxsd_api_transfer_args, (from)(to)(quantity)(expiration)(memo));
FC_REFLECT(eosio::detail::nxsd_api_transfer_params, (wallet)(ppwww)(code)(args)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_get_balance_params, (code)(account)(symbol));
FC_REFLECT(eosio::detail::nxsd_api_error_msg, (code)(message));
FC_REFLECT(eosio::detail::nxsd_api_transfer_pushed_results, (trx_status)(trx_id));
FC_REFLECT(eosio::detail::nxsd_api_transfer_error, (error));
FC_REFLECT(eosio::detail::nxsd_api_buyram_params, (wallet)(ppwww)(payer)(receiver)(quant)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_undelegatebw_params, (wallet)(ppwww)(from)(receiver)(unstake_net_quantity)(unstake_cpu_quantity)(rpc_account));
FC_REFLECT(eosio::detail::nxsd_api_delegatebw_params, (wallet)(ppwww)(from)(receiver)(stake_net_quantity)(stake_cpu_quantity)(transfer)(rpc_account));

using namespace eosio::client::http;

bool   no_verify = false;       // don't verify peer certificate when using HTTPS
bool   print_request = false;   // Debug显示请求内容
bool   print_response = false;  // Debug显示应答内容
http_context context = eosio::client::http::create_http_context();
vector<string> headers;         // pass specific HTTP header; repeat this option to pass multiple headers
//string url = "http://101.124.2.210:8888/"; // 交易广播的服务器地址

template<typename T>
fc::variant call( const std::string& url,
                  const std::string& path,
                  const T& v ) {
   try {
      connection_param *cp = new connection_param(context, eosio::client::http::parse_url(url) + path,
              no_verify ? false : true, headers);

      return do_http_call( *cp, fc::variant(v), print_request, print_response );
   }
   catch(boost::system::system_error& e) {
      throw connection_exception(fc::log_messages{FC_LOG_MESSAGE(error, e.what())});
   }
}
                  
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

    auto         tx_expiration = fc::seconds(60);  // 交易默认超时时间
    const bool   tx_force_unique = false;               
    const bool   tx_dont_broadcast = false;             //  不广播交易
    const bool   tx_skip_sign = false;                  // 交易跳过签名

    struct nxsd_api_plugin_impl{
        enum error_code {
            succeeded = 200,
            key_created = 201,
            key_generated_failed = 419,
            transfer_quantity_invalid = 420,
            transfer_block_id_invalid = 421,
            transfer_not_enough_quantity = 422,
            failed = 423,
            rpc_account_exist = 424,
            rpc_sys_account_nonexist = 425,
            rpc_sys_acount_pwd_invalid = 426,
            rpc_account_nonexist = 427,
            rpc_acount_pwd_invalid = 428,
            rpc_acount_or_pwd_empty = 429,
            unable_open_file = 430,
            no_more_accounts = 431,
            rpc_account_or_pwd_invalid = 432,
            rpc_account_start_with_sys = 433
        };

        nxsd_api_plugin_impl(appbase::application& app)
        : _app(app)
        {}

        // 数据上链及查询
        results_pair handle_table(const std::string& body){
            const eosio::detail::nxsd_api_table_action_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_table_action_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            vector<string> tx_permission;
            tx_permission.push_back(str_params.contract_account);
            auto accountPermissions = get_account_permissions(tx_permission);

            fc::variant result;
            string str_data("");
            fc::variant action_args_var;

            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            w_plugin.get_wallet_manager().open(str_params.wallet); 
            w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // end
            w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));

            if( 0 == str_params.action.compare("query") || 0 == str_params.action.compare("QUERY") ){
                str_data = "[\"" + str_params.table + "\"" + "]";
                action_args_var = fc::json::from_string(str_params.table, fc::json::relaxed_parser);

                //result = send_actions({chain::action{accountPermissions, str_params.contract_account, str_params.action, variant_to_bin( str_params.contract_account, str_params.action, action_args_var ) }});

                result = query_table(str_params);
            } else {
                string cur_time = get_cur_local_time();

                if( 0 == str_params.action.compare("insert") || 0 == str_params.action.compare("add") ){
                    str_data = "[\"" + str_params.table + "\"" + "," + "\"" + str_params.data + "," + cur_time + "\"" + "]";
                } else {
                    str_data = "[\"" + str_params.table + "\"" + "," + "\"" + str_params.data + "\"" + "]";
                }
                
                if( !str_data.empty() ) {
                    try {
                        action_args_var = fc::json::from_string(str_data, fc::json::relaxed_parser);
                    } EOS_RETHROW_EXCEPTIONS(action_type_exception, "Fail to parse action JSON data='${data}'", ("data", str_data))
                }
                
                result = send_actions({chain::action{accountPermissions, str_params.contract_account, str_params.action, variant_to_bin( str_params.contract_account, str_params.action, action_args_var ) }});
            }

            w_plugin.get_wallet_manager().lock(str_params.wallet);

            return { succeeded, result };
        }

        // 创建RPC API使用账号
        results_pair create_rpc_account(const std::string& body){
            const eosio::detail::nxsd_api_create_rpc_account_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_create_rpc_account_params>();

            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = save_rpc_account_to_file(str_params);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);

            return {err_msg.code, fc::variant(err_msg)};
        }

        //  删除RPC API使用账号
        results_pair delete_rpc_account(const std::string& body){
            const eosio::detail::nxsd_api_delete_rpc_account_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_delete_rpc_account_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = del_rpc_account(str_params);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);

            if ( succeeded != err_msg.code){
                return {err_msg.code, fc::variant(err_msg)};
            }

            err_msg.code =  write_rpc_account_to_file();
            err_msg.message = error_code_str(ret_code);

            return {err_msg.code, fc::variant(err_msg)};
        }

        // 显示RPC API使用账号
        results_pair list_rpc_account(const std::string& body){
            eosio::detail::nxsd_api_rpc_account_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_rpc_account_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            eosio::detail::nxsd_api_list_rpc_account_params accts;
            accts.ncount = map_rpc_acc.size();
            for(const auto& acc: map_rpc_acc){
                if( boost::algorithm::starts_with(acc.first, "sys") ){
                    --accts.ncount;
                    continue;
                }
                
                accts.accounts.push_back(eosio::detail::nxsd_api_rpc_account_params{acc.first, acc.second});
            }
            return {succeeded, fc::variant(accts)};
        }

        // 创建EOS钱包
        results_pair create_wallet(const std::string& body){
            const eosio::detail::nxsd_api_create_wallet_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_create_wallet_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            string wallet_pw = w_plugin.get_wallet_manager().create(str_params.wallet_name); 

            eosio::detail::nxsd_api_create_wallet_result wallet_ret{RSA_priv_encrypt(wallet_pw)};

            //std::cout << "Plain wallet password: " << wallet_pw << std::endl;
            return {err_msg.code, fc::variant(wallet_ret)};
        }

        // 创建EOS账号
        results_pair create_account(const std::string& body){
            const eosio::detail::nxsd_api_create_account_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_create_account_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            public_key_type owner_key, active_key;
            try {
                owner_key = public_key_type(RSA_pub_decrypt(str_params.keys.owner));
            } EOS_RETHROW_EXCEPTIONS(public_key_type_exception, "Invalid owner public key: ${public_key}", ("public_key", str_params.keys.owner));
            try {
                active_key = public_key_type(RSA_pub_decrypt(str_params.keys.active));
            } EOS_RETHROW_EXCEPTIONS(public_key_type_exception, "Invalid active public key: ${public_key}", ("public_key", str_params.keys.active));
            
            
            action create = create_newaccount(str_params.creator, str_params.account_name, owner_key, active_key);
            
            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            w_plugin.get_wallet_manager().open(str_params.wallet); 
            w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // end
            w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));
            //nxsd_wallet_manager nxsd_wt_mgr(_app, has_pub_key);
            //nxsd_wt_mgr.open(str_params.wallet, str_params.ppwww, pub_key);

            fc::variant result;
            if( !str_params.buy_ram_eos.empty() ){
                asset ast_ram;
                to_asset(str_params.buy_ram_eos , ast_ram);
                action buyram = create_buyram(str_params.creator, str_params.account_name, ast_ram);
            
                asset ast_cpu, ast_net;
                to_asset(str_params.stake_net, ast_net);
                to_asset(str_params.stake_cpu, ast_cpu);
                if ( ast_net.get_amount() != 0 || ast_cpu.get_amount() != 0 ) {
                    action delegate = create_delegate( str_params.creator, str_params.account_name, ast_net, ast_cpu, str_params.transfer);
                    result = send_actions( { create, buyram, delegate } );
                }
                else {
                    result = send_actions( { create, buyram } );
                }
            } else {
               result = send_actions( { create } );
            }

            w_plugin.get_wallet_manager().lock(str_params.wallet); 

            return { succeeded, result };
        }

        // 卖EOS内存
        results_pair sell_ram(const std::string& body){
            const eosio::detail::nxsd_api_sellram_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_sellram_params>();

            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            int64_t amount = str_params.amount;
            if (0 == amount) // 卖出账户剩余的所有内存
            {
                auto& c_plugin = _app.get_plugin<chain_plugin>();

                using account_params = eosio::chain_apis::read_only::get_account_params;
                const account_params params{eosio::string_to_name(str_params.receiver.c_str())};
                
                const auto& account_ret = c_plugin.get_read_only_api().get_account(params);

                if (-1 == account_ret.ram_quota){
                    eosio::detail::nxsd_api_transfer_error err;
                    err.error.code = transfer_not_enough_quantity;
                    err.error.message = "Not enough quantity";

                    return std::make_pair(transfer_not_enough_quantity, fc::variant(err));
                }

                amount = account_ret.ram_quota - account_ret.ram_usage;
            }

            fc::variant act_payload = fc::mutable_variant_object()
               ("account", str_params.receiver)
               ("bytes", amount);

            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            w_plugin.get_wallet_manager().open(str_params.wallet); 
            w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // end
            w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));

            const auto& result = send_actions({create_action({permission_level{str_params.receiver,config::active_name}}, config::system_account_name, N(sellram), act_payload)});
            
            w_plugin.get_wallet_manager().lock(str_params.wallet); 

            return { succeeded, result };
        }

        // 解抵押CPU、网络
        results_pair undelegatebw(const std::string& body){
            const eosio::detail::nxsd_api_undelegatebw_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_undelegatebw_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            asset ast_cpu, ast_net;
            to_asset(str_params.unstake_net_quantity, ast_net);
            to_asset(str_params.unstake_cpu_quantity, ast_cpu);

            fc::variant act_payload = fc::mutable_variant_object()
               ("from", str_params.from)
               ("receiver", str_params.receiver)
               ("unstake_net_quantity", ast_net)
               ("unstake_cpu_quantity", ast_cpu);
            
            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            w_plugin.get_wallet_manager().open(str_params.wallet); 
            w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // end
            w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));

            const auto& result = send_actions({create_action({permission_level{str_params.from,config::active_name}}, config::system_account_name, N(undelegatebw), act_payload)});
            
            w_plugin.get_wallet_manager().lock(str_params.wallet);

            return { succeeded, result };
        }

        // 抵押CPU、网络
        results_pair delegatebw(const std::string& body){
            const eosio::detail::nxsd_api_delegatebw_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_delegatebw_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            asset ast_cpu, ast_net;
            to_asset(str_params.stake_net_quantity, ast_net);
            to_asset(str_params.stake_cpu_quantity, ast_cpu);
            
            auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            w_plugin.get_wallet_manager().open(str_params.wallet); 
            w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // end
            w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));
            
            const auto& result = send_actions({create_delegate(str_params.from, str_params.receiver, ast_net, ast_cpu, str_params.transfer)});
            
            w_plugin.get_wallet_manager().lock(str_params.wallet);

            return { succeeded, result };
        }

        // 创建公私钥
        results_pair create_key(const std::string& body){
            auto pk    = private_key_type::generate();

            string en_privs = RSA_priv_encrypt(string(pk));
            string en_pubs = RSA_priv_encrypt(string(pk.get_public_key()));

            eosio::detail::nxsd_api_keys keys{en_pubs, en_privs};  
            return { key_created, fc::variant(keys) };
        }

        // 获取账号的余额
        results_pair get_balance(const std::string& body){
            const eosio::detail::nxsd_api_get_balance_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_get_balance_params>();

            vector<string> b_vec;
            do_get_balance(str_params.code, str_params.account, str_params.symbol, b_vec);

            return { succeeded, fc::variant(b_vec) };
        }

        // 转账
        results_pair token_transfer(const std::string& body){
            const eosio::detail::nxsd_api_transfer_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_transfer_params>();

            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            std::string memo_none = str_params.args.memo;
            tx_expiration = fc::seconds(str_params.args.expiration);

            asset ast;
            std::string ast_err("");
            ast_err = to_asset(str_params.args.quantity, ast);              
            if(ast_err.empty())
            {
                fc::variant ret_variant = pre_create_transfer(str_params.wallet, str_params.ppwww, str_params.code, str_params.args.from, str_params.args.to, ast, memo_none);
                return { succeeded, ret_variant };
            }
            
            eosio::detail::nxsd_api_transfer_error err;
            err.error.code = transfer_quantity_invalid;
            err.error.message = ast_err;

            return std::make_pair(transfer_quantity_invalid, fc::variant(err));
        }

        // 买EOS内存
        results_pair buy_ram(const std::string& body){
            const eosio::detail::nxsd_api_buyram_params str_params = fc::json::from_string(body).as<eosio::detail::nxsd_api_buyram_params>();
            
            eosio::detail::nxsd_api_error_msg err_msg;
            error_code ret_code = verify_rpc_account(str_params.rpc_account);
            err_msg.code =  ret_code;
            err_msg.message = error_code_str(ret_code);
            
            if( succeeded != err_msg.code ){
                return {err_msg.code, fc::variant(err_msg)};
            }

            asset ast;
            to_asset(str_params.quant, ast);
            fc::variant act_payload = fc::mutable_variant_object()
               ("payer", str_params.payer)
               ("receiver", str_params.receiver)
               ("quant", ast);
            
            nxsd_wallet_manager nxsd_wt_mgr(_app, has_pub_key);
            nxsd_wt_mgr.open(str_params.wallet, str_params.ppwww, pub_key);
            // auto& w_plugin = _app.get_plugin<wallet_plugin>();
            // // begin, 防止已解锁的钱包再解锁，导致异常程序退出
            // w_plugin.get_wallet_manager().open(str_params.wallet); 
            // w_plugin.get_wallet_manager().lock(str_params.wallet); 
            // // end
            // w_plugin.get_wallet_manager().unlock(str_params.wallet, RSA_pub_decrypt(str_params.ppwww));
            const auto& result = send_actions({create_action({permission_level{str_params.payer,config::active_name}}, config::system_account_name, N(buyram), act_payload)});
            //w_plugin.get_wallet_manager().lock(str_params.wallet); 

            return { succeeded, result };
        }

        // @return: the number of RPC accounts
        uint32_t get_rpc_account_from_file(){
            std::ifstream rpc_acc_in(rpc_account_file.c_str());

            std::string str_rpc_accounts("");
            if( rpc_acc_in.is_open() ){
                std::string str_base64 = read_all_from_file(rpc_acc_in);
                boost::algorithm::erase_all(str_base64, "\n");
                str_rpc_accounts = RSA_pub_decrypt(str_base64);
                rpc_acc_in.close();

                if( !str_rpc_accounts.empty() ){
                    vector<string> vec_accounts;
                    boost::split( vec_accounts, str_rpc_accounts, boost::is_any_of( ",:" ), boost::token_compress_on );
                    for (int i = 0; i < vec_accounts.size() / 2; ++i){
                        map_rpc_acc[ vec_accounts[i * 2] ] = vec_accounts[i * 2 + 1];
                    }

                    return map_rpc_acc.size();
                }
            }

            return 0;
        }

        // 代币增发，频率可以是每年、每月、每星期、每天或每小时
        void token_ticker() {
            token_timer->expires_from_now (token_interval);
            token_timer->async_wait ([this](boost::system::error_code ec) {

            token_ticker ();

            if (ec) {
               wlog ("Peer token ticked sooner than expected: ${m}", ("m", ec.message()));
            }
            
            token_inc_transfer();
         });
        }

        void set_cur_inc_count()
        {
            token_have_inc_count = 0;

            std::ifstream token_inc_in(str_token_inc_file.c_str());

            std::string str_token_inc("");
            if( token_inc_in.is_open() ){
                str_token_inc = read_all_from_file(token_inc_in);

                const char* file_buf_tmp = str_token_inc.c_str(); 
                while (*file_buf_tmp != '\0')
                {
                    const char* p = strchr(file_buf_tmp, '\n');
                    if (p == NULL)
                    {
                        ++token_have_inc_count;
                        break;
                    }
                    token_have_inc_count += p - file_buf_tmp > 0;
                    file_buf_tmp += p - file_buf_tmp + 1;
                }
            }
        }

        public:
            unique_ptr<boost::asio::steady_timer> token_timer;

        private:
            boost::asio::steady_timer::duration   token_interval{std::chrono::seconds{30}};
            int64_t token_have_inc_count;

        private:
            // 按条件查找链上表中的数据
            fc::variant query_table(const eosio::detail::nxsd_api_table_action_params& str_params){
                const auto& c_plugin = _app.get_plugin<chain_plugin>();
                const auto& db = c_plugin.chain().db();

                const auto& code_account = db.get<account_object, by_name>( eosio::string_to_name(str_params.contract_account.c_str()) );

                const fc::microseconds abi_serializer_max_time(10 * 1000);

                vector<fc::variant> rows;
                abi_def abi;
                if( abi_serializer::to_abi(code_account.abi, abi) ) {
                    abi_serializer abis( abi, abi_serializer_max_time );

                    const auto* t_id = db.find<chain::table_id_object, chain::by_code_scope_table>(boost::make_tuple( eosio::string_to_name(str_params.contract_account.c_str()), N(nxsd), eosio::string_to_name(str_params.table.c_str()) ));
                    if (t_id != nullptr) {
                        const auto &idx = db.get_index<key_value_index, by_scope_primary>();
                        vector<string> vec_con;
                        boost::split( vec_con, str_params.data , boost::is_any_of( "=" ), boost::token_compress_on );
                        string str_data = vec_con.size() >= 2 ? vec_con[1] : vec_con[0];
                        auto it = idx.find(boost::make_tuple( t_id->id, eosio::string_to_name(str_data.c_str()) ));
                        if ( it != idx.end() ) {
                            vector<char> data;
                            eosio::chain_apis::read_only::copy_inline_row(*it, data);
                            rows.emplace_back(abis.binary_to_variant(abis.get_table_type(eosio::string_to_name(str_params.table.c_str())), data, abi_serializer_max_time));
                        }
                    }
                }

                return fc::variant(rows);
            }

            string get_cur_local_time(const char* format = "%F %T", const tm* cur = NULL)
            {
                const tm *tm_cur = cur;

                if ( !cur ){
                    time_t rawtime = time(NULL);
                    tm_cur = localtime(&rawtime);
                }

                char cstr_time[21]{'\0'};
                strftime(cstr_time, 20, format, tm_cur);

                return string(cstr_time);
            }

            bytes variant_to_bin( const account_name& account, const action_name& action, const fc::variant& action_args_var ) {
                static unordered_map<account_name, std::vector<char> > abi_cache;
                auto it = abi_cache.find( account );

                if ( it == abi_cache.end() ) {
                    if( is_localhost() ){
                        using code_abi_params = eosio::chain_apis::read_only::get_raw_code_and_abi_params;
                        code_abi_params params{account};
                        auto& c_plugin = _app.get_plugin<chain_plugin>();
                        const auto result = c_plugin.get_read_only_api().get_raw_code_and_abi(params);
                        std::tie( it, std::ignore ) = abi_cache.emplace( account, result.abi.data );
                    } else {
                        const auto result = call(url, get_raw_code_and_abi_func, fc::mutable_variant_object("account_name", account));
                        std::tie( it, std::ignore ) = abi_cache.emplace( account, result["abi"].as_blob().data );
                    }
                }

                const std::vector<char>& abi_v = it->second;

                abi_def abi;
                if( abi_serializer::to_abi(abi_v, abi) ) {
                    abi_serializer abis( abi, fc::seconds(10) );
                    auto action_type = abis.get_action_type(action);
                    FC_ASSERT(!action_type.empty(), "Unknown action ${action} in contract ${contract}", ("action", action)("contract", account));
                    return abis.variant_to_binary(action_type, action_args_var, fc::seconds(10));
                } else {
                    FC_ASSERT(false, "No ABI found for ${contract}", ("contract", account));
                }
            }

            void do_get_balance(const string& code, const string& account, const string& symbol, vector<string>& b_vec){
                if( is_localhost() ){
                    using balance_params = eosio::chain_apis::read_only::get_currency_balance_params;
                    auto& c_plugin = _app.get_plugin<chain_plugin>();
                    vector<asset> asset_ret;

                    const balance_params params{
                        eosio::string_to_name(code.c_str()),
                        eosio::string_to_name(account.c_str()),
                        ( strcasecmp(symbol.c_str(), "ALL") != 0 ) ? symbol : fc::optional<string>()
                    };
                    
                    asset_ret = c_plugin.get_read_only_api().get_currency_balance(params);

                    for(const auto& ast: asset_ret){
                        b_vec.push_back(ast.to_string());
                    }
                } else {
                    auto result = call(url, get_currency_balance_func, fc::mutable_variant_object
                        ("account", account)
                        ("code", code)
                        ("symbol", (strcasecmp(symbol.c_str(), "ALL") == 0) ? fc::variant() : symbol)
                    );

                    const auto& rows = result.get_array();
                    for(const auto& r: rows){
                        b_vec.push_back(r.as_string());
                    }   
                }
            }

            double round(double number, unsigned int bits) {
                long long ll = (long long)(number * pow(10, bits));
                double dd = (ll + 0.5) / pow(10, bits);
                return dd;
            }

            bool is_on_token_time(const string& str_frequency, struct tm **cur_tm){
                bool b_is = false;

                time_t rawtime = time(NULL);
                struct tm *timeinfo = localtime(&rawtime);//gmtime(&rawtime);

                uint8_t flag = 0x00;

                if( 0 == timeinfo->tm_min ) 
                    flag = flag | 0x01;
                if( 0 == timeinfo->tm_hour ) 
                    flag = flag | 0x02;
                if( 1 == timeinfo->tm_mday ) 
                    flag = flag | 0x04;
                if( 5 == timeinfo->tm_wday )
                    flag = flag | 0x08;
                if( 0 == timeinfo->tm_mon )
                    flag = flag | 0x10;
                if( 0 <= timeinfo->tm_sec && timeinfo->tm_sec < 30 )
                    flag = flag | 0x20;

                /*
                struct tm {
                    int tm_sec; 秒 – 取值区间为[0,59] 
                    int tm_min; 分 - 取值区间为[0,59] 
                    int tm_hour;  时 - 取值区间为[0,23] 
                    int tm_mday;  一个月中的日期 - 取值区间为[1,31] 
                    int tm_mon; 月份（从一月开始，0代表一月） - 取值区间为[0,11] 
                    int tm_year; 年份，其值等于实际年份减去1900 
                    int tm_wday;  星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 
                    int tm_yday;  从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 
                    int tm_isdst; 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的时候，tm_isdst为0；不了解情况时，tm_isdst()为负。
                    long int tm_gmtoff; 指定了日期变更线东面时区中UTC东部时区正秒数或UTC西部时区的负秒数
                    const char *tm_zone;  当前时区的名字(与环境变量TZ有关)
                };
                */
                if ( strcasecmp(str_frequency.c_str(), "h") == 0 && (flag & 0x01) == 0x01 ) // 按小时
                    b_is = true;
                else if ( strcasecmp(str_frequency.c_str(), "d") == 0 && (flag & 0x03) == 0x03 ) // 按天
                    b_is = true;
                else if ( strcasecmp(str_frequency.c_str(), "w") == 0 && (flag & 0x0b) == 0x0b ) // 按周
                    b_is = true;
                else if ( strcasecmp(str_frequency.c_str(), "m") == 0 && (flag & 0x07) == 0x07 ) // 按月
                    b_is = true;
                else if ( strcasecmp(str_frequency.c_str(), "y") == 0 && (flag & 0x17) == 0x17 ) // 按年
                    b_is = true;
                else if ( strcasecmp(str_frequency.c_str(), "n") == 0 && (flag & 0x20) == 0x20 ) // 按分钟
                    b_is = true;

                *cur_tm = timeinfo;

                return b_is;
            }

            void append_plain_text_to_file(const string& line){
                std::fstream out_w;
                out_w.open(str_token_inc_file.c_str(), std::ios::out | std::ios::app );
                if( out_w.is_open() ){
                    out_w << line << std::endl;
                    out_w.flush();
                    out_w.close();
                }
            }

            int32_t token_inc_transfer(){
                string wallet, wlt_pw, sender, recipient, str_ast, memo, symbol, code_acc("eosio.token"), frequency;
                int64_t amount, inc_count;
                double d_ratio;
                vector<string> vec_meta;
                boost::split( vec_meta, str_token_inc_meta , boost::is_any_of( ":" ), boost::token_compress_on );
                if( vec_meta.size() >= 9 ){
                    wallet = vec_meta[0];
                    wlt_pw = vec_meta[1];
                    sender = vec_meta[2];
                    recipient = vec_meta[3];
                    amount = boost::lexical_cast<int64_t>(vec_meta[4]);
                    symbol = vec_meta[5];
                    d_ratio = boost::lexical_cast<double>(vec_meta[6]);
                    frequency = vec_meta[7];
                    inc_count = boost::lexical_cast<int64_t>(vec_meta[8]);

                    char cstr_amount[50]{'\0'};
                    snprintf(cstr_amount, 49, "%.4f", round(amount * d_ratio / 100, 4));

                    str_ast = string(cstr_amount) + " " + symbol;
                    memo = sender + "->" + recipient + "-" + str_ast;
                } else {
                    wlog("[nxsd-log] Please specify the ICO increment rule.(cur rule: ${r})", ("r", str_token_inc_meta));
                    return 1;
                }

                struct tm *cur_tm = NULL;
                if( !is_on_token_time(frequency, &cur_tm)){
                    return 0;
                }

                asset ast;
                std::string ast_err("");
                ast_err = to_asset(str_ast, ast); 

                if(ast_err.empty())
                {
                    if( token_have_inc_count < inc_count ){
                        char cstr[21]{'\0'};
                        strftime(cstr, 20, "%F %T", cur_tm);
                        string str_time(cstr);

                        vector<string> vec_sender_ast_before;
                        do_get_balance("eosio.token", sender, symbol, vec_sender_ast_before);

                        vector<string> vec_recipient_ast_before;
                        do_get_balance("eosio.token", recipient, symbol, vec_recipient_ast_before);

                        pre_create_transfer(wallet, wlt_pw, code_acc, sender, recipient, ast, memo);
                        ++token_have_inc_count;

                        vector<string> vec_sender_ast_after;
                        do_get_balance("eosio.token", sender, symbol, vec_sender_ast_after);

                        vector<string> vec_recipient_ast_after;
                        do_get_balance("eosio.token", recipient, symbol, vec_recipient_ast_after);

                        const string sep = "@";
                        const string sep_sub = ",";
                        std::stringstream ss_out;
                        ss_out << "[" << sender << "->" << recipient << sep_sub << str_ast<< "]" << sep
                        << str_time << sep
                        << "(" << sender << sep_sub << vec_sender_ast_before[0] << sep_sub << vec_sender_ast_after[0] << ")" << "->"
                        << "(" << recipient << sep_sub << vec_recipient_ast_before[0] << sep_sub << vec_recipient_ast_after[0] << ")";

                        append_plain_text_to_file(ss_out.str());
                    }
                } else {
                    wlog("[nxsd-log] The asset is invalid, please confirm it.(cur asset: ${t})", ("t", str_ast));
                    return 2;
                }

                return 0;
            }

            fc::variant pre_create_transfer(const string& wallet, const string& wlt_pw,
                                            const string& contract, const string& sender, const string& recipient,
                                            asset amount, const string& memo )
            {
                auto& w_plugin = _app.get_plugin<wallet_plugin>();

                // begin, 防止已解锁的钱包再解锁，导致异常程序退出
                w_plugin.get_wallet_manager().open(wallet); 
                w_plugin.get_wallet_manager().lock(wallet); 
                // end
                w_plugin.get_wallet_manager().unlock(wallet, RSA_pub_decrypt(wlt_pw));

                fc::variant ret_variant = send_actions({create_transfer(contract, sender, recipient, amount, memo)});
                
                w_plugin.get_wallet_manager().lock(wallet);

                return ret_variant;
            }

            bool is_localhost(void){
                std::cmatch m;
                return std::regex_search(url.c_str(), m, std::regex("localhost|127.0.0.1"));
            }

            error_code verify_rpc_account(eosio::detail::nxsd_api_rpc_account_params rpc_account){
                if( 0 == map_rpc_acc.count(rpc_account.name) ||
                    map_rpc_acc[rpc_account.name] != rpc_account.pw ){
                    return rpc_account_or_pwd_invalid;
                }

                return succeeded;
            }

            std::string error_code_str(error_code code){
                switch( code ){
                    case rpc_acount_or_pwd_empty : return "The password is empty.";
                    case rpc_account_exist : return "RPC account is already exist.";
                    case rpc_sys_acount_pwd_invalid : return "The password of the RPC system account is invalid.";
                    case unable_open_file : return "No file to save the account info, please specify via 'rpc-account-file' option.";
                    case no_more_accounts : return "More than accounts limits, can not create.";
                    case rpc_sys_account_nonexist : return "RPC system account is invalid.";
                    case rpc_account_nonexist : return "RPC account is not exist.";
                    case rpc_account_or_pwd_invalid : return "RPC account or password invalid.";
                    case rpc_account_start_with_sys : return "RPC account name can not start with 'sys'.";
                    case succeeded : return "Succeed.";
                    case failed : return "Failed.";
                    default: return "some crazy reason.";
                }   
            }

            std::string rpc_account_string_from_map(){
                std::string str_rpc_accounts("");
                int iLoop = 1;
                for( const auto& acc: map_rpc_acc ){
                    if( 1 == iLoop ){
                        str_rpc_accounts += acc.first + ":" + acc.second;

                    }else{
                        str_rpc_accounts += "," + acc.first + ":" + acc.second;
                    }
                    ++iLoop;
                }

                return str_rpc_accounts;
            }

            error_code write_rpc_account_to_file(){
                std::ofstream out_acc(rpc_account_file.c_str());
                if( out_acc.is_open() ){
                    out_acc << RSA_priv_encrypt(rpc_account_string_from_map());
                    out_acc << std::endl;
                    out_acc.close();

                    return succeeded;
                }
                return unable_open_file;
            }

            error_code del_rpc_account(const eosio::detail::nxsd_api_delete_rpc_account_params& del_rpc_account_params){
                if( map_rpc_acc.count(del_rpc_account_params.admin.name) &&
                    map_rpc_acc[del_rpc_account_params.admin.name] == del_rpc_account_params.admin.pw ){
                    std::size_t ret_code = map_rpc_acc.erase(del_rpc_account_params.account_name);

                    if( 0 == ret_code)
                        return rpc_account_nonexist;
                } else if( !map_rpc_acc.count(del_rpc_account_params.admin.name) ){
                    return rpc_sys_account_nonexist;
                }
                else{
                    return rpc_sys_acount_pwd_invalid;
                }

                return succeeded;
            }

            error_code save_rpc_account_to_file(const eosio::detail::nxsd_api_create_rpc_account_params& rpc_accounts_params){
                std::string str_rpc_accounts = rpc_account_string_from_map();
                std::size_t ncount = str_rpc_accounts.size()
                                    + rpc_accounts_params.account.name.size()
                                    + rpc_accounts_params.account.pw.size() + 2;
                if( 256 < ncount) // RSA的2048位秘钥加密，账号、密码及间隔符的总字符个数不能超过256个字节
                    return no_more_accounts;

                if( rpc_accounts_params.account.name.empty() || rpc_accounts_params.account.pw.empty() ){
                    return rpc_acount_or_pwd_empty;
                }

                if( map_rpc_acc.count(rpc_accounts_params.account.name) ){
                    return rpc_account_exist;
                }

                if( boost::algorithm::starts_with(rpc_accounts_params.account.name, "sys") && 0 < map_rpc_acc.size() ){
                    return rpc_account_start_with_sys;
                }
                
                if( (boost::algorithm::starts_with(rpc_accounts_params.account.name, "sys") && 0 == map_rpc_acc.size()) ||
                    (map_rpc_acc.count(rpc_accounts_params.creator.name) &&  map_rpc_acc[rpc_accounts_params.creator.name] == rpc_accounts_params.creator.pw)
                    ){                       
                        map_rpc_acc[rpc_accounts_params.account.name] = rpc_accounts_params.account.pw;
                } else if( !map_rpc_acc.count(rpc_accounts_params.creator.name) ){
                    return rpc_sys_account_nonexist;
                }
                else{
                    return rpc_sys_acount_pwd_invalid;
                }

                error_code ret = write_rpc_account_to_file();

                return ret;
            }

            chain::action create_buyram(const string& creator, const string& newaccount, const asset& quantity) {
                fc::variant act_payload = fc::mutable_variant_object()
                        ("payer", creator)
                        ("receiver", newaccount)
                        ("quant", quantity.to_string());
                return create_action(vector<chain::permission_level>{{eosio::string_to_name(creator.c_str()),config::active_name}},
                                        config::system_account_name, N(buyram), act_payload);
            }

            chain::action create_newaccount(const string& creator, const string& newaccount, public_key_type owner, public_key_type active) {
                return action {
                    vector<chain::permission_level>{{eosio::string_to_name(creator.c_str()),config::active_name}},
                    eosio::chain::newaccount{
                        .creator      = eosio::string_to_name(creator.c_str()),
                        .name         = eosio::string_to_name(newaccount.c_str()),
                        .owner        = eosio::chain::authority{1, {{owner, 1}}, {}},
                        .active       = eosio::chain::authority{1, {{active, 1}}, {}}
                    }
                };
            }

            chain::action create_delegate(const string& from, const string& receiver, const asset& net, const asset& cpu, bool transfer) {
                fc::variant act_payload = fc::mutable_variant_object()
                        ("from", from)
                        ("receiver", receiver)
                        ("stake_net_quantity", net.to_string())
                        ("stake_cpu_quantity", cpu.to_string())
                        ("transfer", transfer);
                return create_action(vector<chain::permission_level>{permission_level{eosio::string_to_name(from.c_str()),config::active_name}},
                                        config::system_account_name, N(delegatebw), act_payload);
            }

            chain::action create_action(const vector<permission_level>& authorization, const account_name& code, const action_name& act, const fc::variant& args) {
                eosio::chain_apis::read_only::abi_json_to_bin_params params;

                params.code = code;
                params.action = act;
                params.args = args;

                auto& plugin = _app.get_plugin<chain_plugin>();
                auto result = plugin.get_read_only_api().abi_json_to_bin(params);

                wdump((result)(args));
                return chain::action{authorization, code, act, result.binargs};
            }

            chain::action generate_nonce_action() {
                return chain::action( {}, config::null_account_name, "nonce", fc::raw::pack(fc::time_point::now().time_since_epoch().count()));
            }

            eosio::chain_apis::read_only::get_required_keys_result determine_required_keys(const signed_transaction& trx) {
                auto& w_plugin = _app.get_plugin<wallet_plugin>();
                const auto& public_keys = w_plugin.get_wallet_manager().get_public_keys();

                if( is_localhost() ){                   
                    eosio::chain_apis::read_only::get_required_keys_params keys_params;
                    keys_params.transaction = (transaction)trx;
                    keys_params.available_keys = public_keys;

                    auto& c_plugin = _app.get_plugin<chain_plugin>();
                    const auto& required_keys = c_plugin.get_read_only_api().get_required_keys(keys_params);
                   
                    return required_keys;
                } else {
                    typedef flat_set<public_key_type> pub_flat_set;
                    //const auto& public_keys = call(wallet_url, wallet_public_keys);
                    auto get_arg = fc::mutable_variant_object
                            ("transaction", (transaction)trx)
                            ("available_keys", public_keys);

                    const auto& required_keys = call(url, get_required_keys, get_arg);

                    auto required_keys_set = required_keys.get_object()["required_keys"].as<pub_flat_set>();
                    eosio::chain_apis::read_only::get_required_keys_result result;
                    result.required_keys = required_keys_set;    

                    return result;
                }
            }

            eosio::chain_apis::read_only::get_info_results get_info() {
                if( is_localhost() ){
                    auto& plugin = _app.get_plugin<chain_plugin>();
                    return plugin.get_read_only_api().get_info(eosio::chain_apis::read_only::get_info_params());
                } else {
                    return call(url, get_info_func, fc::variant()).as<eosio::chain_apis::read_only::get_info_results>();
                }
            }

            void sign_transaction(signed_transaction& trx, const eosio::chain_apis::read_only::get_required_keys_result& required_keys, const chain_id_type& chain_id) {
                auto& w_plugin = _app.get_plugin<wallet_plugin>();
                const auto& signed_trx = w_plugin.get_wallet_manager().sign_transaction(trx, required_keys.required_keys, chain_id);
                trx = signed_trx;
            }

            fc::variant push_transaction( signed_transaction& trx, int32_t extra_kcpu = 1000, packed_transaction::compression_type compression = packed_transaction::none ) {
                auto info = get_info();
                trx.expiration = info.head_block_time + tx_expiration;

                auto& plugin = _app.get_plugin<chain_plugin>();                   

                // Set tapos, default to last irreversible block if it's not specified by the user
                block_id_type ref_block_id = info.last_irreversible_block_id;
                /*try {
                    fc::variant ref_block;
                    if (!tx_ref_block_num_or_id.empty()) {
                        eosio::chain_apis::read_only::get_block_params blk_params{tx_ref_block_num_or_id};
                        ref_block = plugin.get_read_only_api().get_block(blk_params);
                        ref_block_id = ref_block["id"].as<block_id_type>();
                    }
                } EOS_RETHROW_EXCEPTIONS(invalid_ref_block_exception, "Invalid reference block num or id: ${block_num_or_id}", ("block_num_or_id", tx_ref_block_num_or_id)); 
                */
                trx.set_reference_block(ref_block_id);

                if (tx_force_unique) {
                    trx.context_free_actions.emplace_back( generate_nonce_action() );
                }

                trx.max_cpu_usage_ms = 0;
                trx.max_net_usage_words = (0 + 7)/8;

                if (!tx_skip_sign) {
                    auto required_keys = determine_required_keys(trx);
                    sign_transaction(trx, required_keys, info.chain_id);
                }

                if (!tx_dont_broadcast) {
                    if( is_localhost() ){
                        fc::variant var_packed_tx(packed_transaction(trx, compression));
                        std::string str_packed_tx(fc::json::to_string( var_packed_tx ));

                        using push_params = eosio::chain_apis::read_write::push_transaction_params;
                        push_params var_obj_params = fc::json::from_string(str_packed_tx).as<push_params>();

                        using push_result_params = eosio::chain_apis::read_write::push_transaction_results;
                        auto rw_api = _app.get_plugin<chain_plugin>().get_read_write_api();

                        eosio::detail::nxsd_api_transfer_pushed_results push_ret;
                        fc::variant var_result;
                                                        
                        rw_api.push_transaction(var_obj_params, 
                            [&var_result](const fc::static_variant<fc::exception_ptr, push_result_params>& result){
                                to_variant(result,var_result);
                            });
                        return var_result[1];
                    } else{
                        return call(url, push_txn_func, packed_transaction(trx, compression));
                    }
                } else {
                    return fc::variant(trx);
                }
            }

            fc::variant push_actions(std::vector<chain::action>&& actions, int32_t extra_kcpu, packed_transaction::compression_type compression = packed_transaction::none ) {
                signed_transaction trx;
                trx.actions = std::forward<decltype(actions)>(actions);

                return push_transaction(trx, extra_kcpu, compression);
            }       

            std::string to_asset( const string& code, const string& s, asset& ast) {
                static map<eosio::chain::symbol_code, eosio::chain::symbol> cache;
                auto a = asset::from_string( s );
                eosio::chain::symbol_code sym = a.get_symbol().to_symbol_code();
                auto it = cache.find( sym );
                auto sym_str = a.symbol_name();

                char c_err[100] = {'\0'};
                if ( it == cache.end() ) {
                    eosio::chain_apis::read_only::get_currency_stats_params sts_params{eosio::string_to_name(code.c_str()), sym_str};
                    auto& plugin = _app.get_plugin<chain_plugin>();
                    auto json = plugin.get_read_only_api().get_currency_stats(sts_params);
                    auto obj = json.get_object();
                    auto obj_it = obj.find( sym_str );
                    if (obj_it != obj.end()) {
                        auto result = obj_it->value().as<eosio::chain_apis::read_only::get_currency_stats_result>();
                        auto p = cache.insert(make_pair( sym, result.max_supply.get_symbol() ));
                        it = p.first;
                    } else {
                        snprintf(c_err, 99, "Symbol %s is not supported by token contract %s", sym_str.c_str(), code.c_str());
                        return c_err;
                    }
                }
                auto expected_symbol = it->second;
                if ( a.decimals() < expected_symbol.decimals() ) {
                    auto factor = expected_symbol.precision() / a.precision();
                    auto a_old = a;
                    a = asset( a.get_amount() * factor, expected_symbol );
                } else if ( a.decimals() > expected_symbol.decimals() ) {
                    snprintf(c_err, 99, "Too many decimal digits in %s, only %c supported", a.to_string().c_str(), expected_symbol.decimals());
                    return c_err;
                } // else precision matches
                
                ast = a;

                return "";
            }

            inline std::string to_asset( const string& s, asset& ast ) {
                return to_asset( "eosio.token", s, ast );
            }
            
            chain::action create_transfer(const string& contract, const name& sender, const name& recipient, asset amount, const string& memo ) {
                auto transfer = fc::mutable_variant_object
                    ("from", sender)
                    ("to", recipient)
                    ("quantity", amount)
                    ("memo", memo);

                vector<char> abi_bytes;

                if( is_localhost() ){
                    eosio::chain_apis::read_only::abi_json_to_bin_params params;
                    params.code = eosio::string_to_name(contract.c_str());
                    params.action = N(transfer);
                    to_variant(transfer, params.args);

                    auto& plugin = _app.get_plugin<chain_plugin>();
                    auto result = plugin.get_read_only_api().abi_json_to_bin(params);
                    abi_bytes = result.binargs;
                } else {
                    auto args = fc::mutable_variant_object
                        ("code", contract)
                        ("action", "transfer")
                        ("args", transfer);

                    auto result = call(url, json_to_bin_func, args);
                    abi_bytes = result.get_object()["binargs"].as<bytes>();
                }

                return action {
                    tx_permission.empty() ? vector<chain::permission_level>{{sender,config::active_name}} : get_account_permissions(tx_permission),
                    contract, "transfer", abi_bytes
                };
            }

            fc::variant send_actions(std::vector<chain::action>&& actions, int32_t extra_kcpu = 1000, packed_transaction::compression_type compression = packed_transaction::none ) {
                auto result = push_actions( move(actions), extra_kcpu, compression);

                return result;
            }

            vector<chain::permission_level> get_account_permissions(const vector<string>& permissions) {
                auto fixedPermissions = permissions | boost::adaptors::transformed([](const string& p) {
                    vector<string> pieces;
                    split(pieces, p, boost::algorithm::is_any_of("@"));
                    if( pieces.size() == 1 ) pieces.push_back( "active" );
                    return chain::permission_level{ .actor = pieces[0], .permission = pieces[1] };
                });
                vector<chain::permission_level> accountPermissions;
                boost::range::copy(fixedPermissions, back_inserter(accountPermissions));
                return accountPermissions;
            }

        public:
            string RSA_priv_encrypt(const string& str_plain){
                if( !has_priv_key)
                    return str_plain;

                fc::bytes bytes_encrypted = priv_key.encrypt(fc::bytes(str_plain.begin(), str_plain.end()));
                return fc::base64_encode( (const unsigned char*)bytes_encrypted.data(), bytes_encrypted.size() );
            }

            string RSA_pub_decrypt(const string& base64_en){
                // 如果公钥、私钥、钱包密码是没有加密的或没有解密的公钥，则不处理，直接返回原字符串
                if( !has_pub_key || 
                    boost::algorithm::starts_with(base64_en, "EOS") || // 没有加密的公钥
                    boost::algorithm::starts_with(base64_en, "PUB") || // 没有加密的公钥
                    boost::algorithm::starts_with(base64_en, "PW") ||  // 没有加密的钱包密码
                    51 == base64_en.size()/*没有加密的私钥*/ )
                    return base64_en; 

                string str_en = fc::base64_decode( base64_en );
                fc::bytes bytes_decrypted = pub_key.decrypt(fc::bytes(str_en.begin(), str_en.end()));
                return string(bytes_decrypted.begin(), bytes_decrypted.end());
            }

            string read_all_from_file(const std::ifstream& in){
                std::stringstream ss_content;
                ss_content << in.rdbuf();
                std::string str_content(ss_content.str());
                
                //boost::algorithm::erase_all(str_content, "\n");

                return str_content;
            }

        public:
                vector<string> tx_permission;  
                appbase::application& _app; 
                string url; // 交易广播的服务器地址
                bool has_priv_key;
                bool has_pub_key;
                fc::public_key pub_key;
                fc::private_key priv_key;
                std::string rpc_account_file;
                std::map<string, string> map_rpc_acc;
                string str_token_inc_file;
                string str_token_inc_meta;
    };

    void nxsd_api_plugin::set_program_options(options_description&, options_description& cfg) {
        cfg.add_options()
            ("trx-broadcast-url", bpo::value<string>()->default_value("http://127.0.0.1:8888/"),
            "The url for broadcasting transaction to blocks")
            ("rsa-priv-file", bpo::value<string>()->default_value("/temp/priv.pem"),
            "The RSA private pem file for encrypting in nxsd_api plugin (absolute file path)")
            ("rsa-pub-file", bpo::value<string>()->default_value("/temp/pub.pem"),
            "The RSA public pem file for decrypting in nxsd_api plugin (absolute file path)")
            ("rpc-account-file", bpo::value<string>()->default_value("/temp/rpc_account.dat"),
            "The RPC acount for calling RPC API (absolute file path)")
            ("token-inc-meta", bpo::value<string>()->default_value(""),
            "The currency amount for the ICO increment delivering. the format is <wallet>:<wallet-passwd>:<sender>:<to>:<total>:<token_symbol>:<inc_ratio>:<y|m|w|d|h|n>:<inc_num>")
            ;
    }

    void nxsd_api_plugin::plugin_initialize(const variables_map& options) {
        if( options.count("token-inc-meta") ) {
            my->str_token_inc_meta = options.at("token-inc-meta").as<string>();
        } 

        my->url = "http://127.0.0.1:8888/";
        if( options.count("trx-broadcast-url") ) {
            my->url = options.at("trx-broadcast-url").as<string>();
        } 
        if( my->url[my->url.size() - 1] != '/' ){
            my->url += "/";
        }

        my->has_priv_key = false;
        my->has_pub_key = false;
        string rsa_file;
        string rsa_contents;
        if( options.count("rsa-priv-file") ) {
            rsa_file = options.at("rsa-priv-file").as<string>();
            if( boost::filesystem::is_regular_file(rsa_file) ){
                my->has_priv_key = true;
                fc::private_key priv(rsa_file);
                my->priv_key = priv;
            }
        }

        if( options.count("rsa-pub-file") ) {
            rsa_file = options.at("rsa-pub-file").as<string>();
            if( boost::filesystem::is_regular_file(rsa_file) ){
                my->has_pub_key = true;
                fc::public_key pub(rsa_file);
                my->pub_key = pub;
            }
        }

        // 这个需要在私钥、公钥加载完后执行，因为get_rpc_account_from_file函数用到了RSA加解密
        if( options.count("rpc-account-file") ) {
            my->rpc_account_file = options.at("rpc-account-file").as<string>();
            if( !my->rpc_account_file.empty() ){
                my->get_rpc_account_from_file();

                boost::filesystem::path ff(my->rpc_account_file);
                my->str_token_inc_file = ff.parent_path().string() + "/token_inc.dat";
                my->set_cur_inc_count();
            }
        }

        my->token_timer.reset( new boost::asio::steady_timer( app().get_io_service()));
        my->token_ticker();
    }

    void nxsd_api_plugin::plugin_startup(){
        ilog( "starting nxsd_api_plugin" );
        app().get_plugin<http_plugin>().add_api({
        CALL( nxsd, my, create_key, nxsd_api_plugin_impl::create_key ),
        CALL( nxsd, my, token_transfer, nxsd_api_plugin_impl::token_transfer),
        CALL( nxsd, my, get_balance, nxsd_api_plugin_impl::get_balance),
        CALL( nxsd, my, delegatebw, nxsd_api_plugin_impl::delegatebw),
        CALL( nxsd, my, undelegatebw, nxsd_api_plugin_impl::undelegatebw),
        CALL( nxsd, my, sell_ram, nxsd_api_plugin_impl::sell_ram),
        CALL( nxsd, my, create_wallet, nxsd_api_plugin_impl::create_wallet),
        CALL( nxsd, my, buy_ram, nxsd_api_plugin_impl::buy_ram),
        CALL( nxsd, my, create_rpc_account, nxsd_api_plugin_impl::create_rpc_account),
        CALL( nxsd, my, delete_rpc_account, nxsd_api_plugin_impl::delete_rpc_account),
        CALL( nxsd, my, list_rpc_account, nxsd_api_plugin_impl::list_rpc_account),
        CALL( nxsd, my, handle_table, nxsd_api_plugin_impl::handle_table),
        CALL( nxsd, my, create_account, nxsd_api_plugin_impl::create_account)
        });
    }

    void nxsd_api_plugin::plugin_shutdown(){

    }

    nxsd_api_plugin::nxsd_api_plugin()
    : my(new nxsd_api_plugin_impl(app()))
    {

    }

    nxsd_api_plugin::~nxsd_api_plugin(){
        
    }

}
