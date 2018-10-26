#pragma once

#include <eosiolib/eosio.hpp>
#include "fc.hpp"

#include <string>
#include <map>
#include <set>

namespace nxsd {

    using std::set;
    using std::map;
    using std::string;
    using eosio::contract;

    typedef std::map<string, string> fv_map;

    class farm : public contract {
      public:
          explicit farm( action_name self )
          :contract(self) {}

          void insert( string table_name,
                      string fields // 逗号分隔的表列值，如：fvalue1,fvalue2,fvalue3
                     );

          void update( string table_name, 
                       string condition // 表记录更新条件，如: name=Lina when id=xxxx
                      );

          void del( string table_name,
        		  string condition  // 删除记录条件，如：id=someid
				  );

          void query( string id );  // 查询记录条件，如：id=someid

      private:
        void farm_insert( string id,
                      string name,
                      string create_time,
                      string owner,
                      string on_block_time );

        void xbean_insert( string id,
                      string from,
                      string to,
                      string quantity,
                      string create_time,
                      string trx_time,
                      string on_block_time );

        void quantumchook_insert( string id,
                      string dna,
                      string owner,
                      string qualification,
                      string fattening_time,
                      string on_block_time );

        void quantumegg_insert( string id,
                      string owner,
                      string lay_egg_time,
                      string on_block_time );

        void chook_insert( string id,
                      string farm_name,
                      string state,
                      string description,
                      string owner,
                      string fattening_time,
                      string hatch_time,
                      string on_block_time );

        void fatten_insert( string id,
                      string farm_name,
                      string contact_info,
                      string shipping_address,
                      string receiver,
                      string shipping_time,
                      string fattening_time,
                      string killer_time,
                      string on_block_time );

        void chookfeeding_insert( string id,
                      string food,
                      string location,
                      string feeding_time,
                      string on_block_time );

        void disinfect_insert( string id,
                      string event,
                      string description,
                      string location,
                      string disinfect_time,
                      string on_block_time );

          void farm_update( const fv_map& updated, const string& condition );
          void xbeans_update( const fv_map& updated, const string& condition );
          void quantumchook_update( const fv_map& updated, const string& condition );
          void quantumegg_update( const fv_map& updated, const string& condition );
          void chook_update( const fv_map& updated, const string& condition );
          void chookfeeding_update( const fv_map& updated, const string& condition );
          void disinfect_update( const fv_map& updated, const string& condition );
          void fatten_update( const fv_map& updated, const string& condition );

          void farm_del( string condition );
          void xbeans_del( string condition );
          void quantumchook_del( string condition );
          void quantumegg_del( string condition );
          void chook_del( string condition );
          void chookfeeding_del( string condition );
          void disinfect_del( string condition );
          void fatten_del( string condition );
      
      private:
          // @abi table farmstat
          struct farm_stat {
            uint64_t  id;
            string    farm_id;
            string    name;
            string    create_time;
            string    owner;
            string    on_block_time; // the time when record is stored on blockchain

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( farm_stat, (id)(farm_id)(name)(create_time)(owner)(on_block_time) )
          };

          const string farmstat_fields[6] = {"id", "farm_id", "name", "create_time", "owner", "on_block_time"};

          typedef eosio::multi_index<N(farmstat), farm_stat> farm_stats;

          // @abi table xbeantrx
          struct xbean_trx {
            uint64_t  id;
            string    trx_id;
            string    create_time;
            string    from;
            string    on_block_time;
            string    to;
            string    quantity;
            string    trx_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( xbean_trx, (id)(trx_id)(create_time)(from)(on_block_time)(to)(quantity)(trx_time) )
          };

          const string xbeantrx_fields[8] = {"id", "trx_id", "from", "create_time", "to", "on_block_time", "quantity", "trx_time"};

          typedef eosio::multi_index<N(xbeantrx), xbean_trx> xbean_trxs;

          // @abi table quantumchook
          struct quantum_chook {
            uint64_t  id;
            string    chook_id;
            string    dna;
            string    qualification;
            string    owner;
            string    fattening_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( quantum_chook, (id)(chook_id)(dna)(qualification)(owner)(fattening_time)(on_block_time) )
          };

          const string quantumchook_fields[7] = {"id", "chook_id", "dna", "qualification", "owner", "fattening_time", "on_block_time"};

          typedef eosio::multi_index<N(quantumchook), quantum_chook> quantum_chooks;

          // @abi table quantumegg
          struct quantum_egg {
            uint64_t  id;
            string    egg_id;
            string    owner;
            string    lay_egg_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( quantum_egg, (id)(egg_id)(owner)(lay_egg_time)(on_block_time) )
          };

          const string quantumegg_fields[5] = {"id", "egg_id", "lay_egg_time", "owner", "on_block_time"};

          typedef eosio::multi_index<N(quantumegg), quantum_egg> quantum_eggs;

          // @abi table chook
          struct chook {
            uint64_t  id;
            string    chook_id;
            string    farm_name;
            string    state;
            string    description;
            string    owner;
            string    fattening_time;
            string    hatch_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( chook, (id)(chook_id)(farm_name)(state)(description)(owner)(fattening_time)(hatch_time)(on_block_time) )
          };

          const string chook_fields[9] = {"id", "chook_id", "farm_name", "state", "fattening_time", "description", "owner", "hatch_time", "on_block_time"};

          typedef eosio::multi_index<N(chook), chook> chooks;

          // @abi table chookfeeding
          struct chook_feeding {
            uint64_t  id;
            string    chook_id;
            string    food;
            string    location;
            string    feeding_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( chook_feeding, (id)(chook_id)(food)(location)(feeding_time)(on_block_time) )
          };

          const string chookfeeding_fields[6] = {"id", "chook_id", "food", "location", "feeding_time", "on_block_time"};

          typedef eosio::multi_index<N(chookfeeding), chook_feeding> chook_feedings;

          // @abi table cdisinfect
          struct cdisinfect {
            uint64_t  id;
            string    chook_id;
            string    event;
            string    description;
            string    location;
            string    disinfect_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( cdisinfect, (id)(chook_id)(event)(description)(location)(disinfect_time)(on_block_time) )
          };

          const string cdisinfect_fields[7] = {"id", "chook_id", "event", "location", "description", "disinfect_time", "on_block_time"};

          typedef eosio::multi_index<N(cdisinfect), cdisinfect> cdisinfects;

          // @abi table cfatten
          struct cfatten {
            uint64_t  id;
            string    chook_id;
            string    farm_name;
            string    contact_info;
            string    shipping_address;
            string    receiver;
            string    shipping_time;
            string    fattening_time;
            string    killer_time;
            string    on_block_time;

            uint64_t primary_key()const {return id;}

            EOSLIB_SERIALIZE( cfatten, (id)(chook_id)(farm_name)(contact_info)(shipping_address)(receiver)(shipping_time)(fattening_time)(killer_time)(on_block_time) )
          };

          const string cfatten_fields[10] = {"id", "chook_id", "farm_name", "contact_info", "shipping_address", "receiver", "shipping_time", "fattening_time", "killser_time", "on_block_time"};

          typedef eosio::multi_index<N(cfatten), cfatten> cfattens;

          const map<string, set<string>> m_tables{
            {"cfatten", set<string>(cfatten_fields, cfatten_fields + 10)},
            {"cdisinfect", set<string>(cdisinfect_fields, cdisinfect_fields + 7)},
            {"chookfeeding", set<string>(chookfeeding_fields, chookfeeding_fields + 6)},
            {"chook", set<string>(chook_fields, chook_fields + 9)},
            {"quantumegg", set<string>(quantumegg_fields, quantumegg_fields + 5)},
            {"quantumchook", set<string>(quantumchook_fields, quantumchook_fields + 7)},
            {"xbeantrx", set<string>(xbeantrx_fields, xbeantrx_fields + 8)},
            {"farmstat", set<string>(farmstat_fields, farmstat_fields + 6)}
          };
   };

} // namespace nxsd
