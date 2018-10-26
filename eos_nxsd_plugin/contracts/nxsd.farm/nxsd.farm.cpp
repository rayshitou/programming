
#include "nxsd.farm.hpp"
#include "fc.cpp"

#include <eosiolib/print.hpp>

#define ASSERT_FIELDS_COUNT(num1, num2) eosio_assert( num1 == num2, "the fileds count is not equal colums of fields" );

#define DELETE_ONE_RECORD(table_obj, del_condition) \
			table_obj t_obj(_self, eosio::string_to_name(table_scope)); \
			auto id_index = eosio::string_to_name(del_condition.c_str()); \
			auto existing = t_obj.find( id_index ); \
			eosio_assert( existing != t_obj.end(), "the record is not exist" ); \
			t_obj.erase(existing);

namespace nxsd {

  const char *table_scope = "nxsd";

  //@abi action
  void farm::insert( string table_name, string fields ){
      eosio_assert( 1 == m_tables.count(table_name), "the table is not exist" );

      std::vector<std::string> field_values = nxsd::split(fields, ",");

      if( 0 == table_name.compare("farmstat") ){
          ASSERT_FIELDS_COUNT(5, field_values.size())
          
          farm_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4]);
      } else if( 0 == table_name.compare("xbeantrx") ){
          ASSERT_FIELDS_COUNT(7, field_values.size())
          
          xbean_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4], field_values[5], field_values[6]);
      } else if( 0 == table_name.compare("quantumchook") ){
          ASSERT_FIELDS_COUNT(6, field_values.size())
          
          quantumchook_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4], field_values[5]);
      } else if( 0 == table_name.compare("quantumegg") ){
          ASSERT_FIELDS_COUNT(4, field_values.size())
          
          quantumegg_insert(field_values[0], field_values[1], field_values[2], field_values[3]);
      } else if( 0 == table_name.compare("chook") ){
          ASSERT_FIELDS_COUNT(8, field_values.size())
          
          chook_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4], field_values[5], field_values[6], field_values[7]);
      } else if( 0 == table_name.compare("chookfeeding") ){
          ASSERT_FIELDS_COUNT(5, field_values.size())
          
          chookfeeding_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4]);
      } else if( 0 == table_name.compare("cdisinfect") ){
          ASSERT_FIELDS_COUNT(6, field_values.size())
          
          disinfect_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4], field_values[5]);
      } else if( 0 == table_name.compare("cfatten") ){
          ASSERT_FIELDS_COUNT(9, field_values.size())
          
          fatten_insert(field_values[0], field_values[1], field_values[2], field_values[3], field_values[4], field_values[5], field_values[6], field_values[7], field_values[8]);
      }
      
  }

  //@abi action
  void farm::update( string table_name, string condition ){
      eosio_assert( 1 == m_tables.count(table_name), "the table is not exist" );

      fv_map map_updated, map_condition;
      bool all_fields_in_table = true;
      string str_con;
      std::vector<std::string> vec_when = nxsd::split(condition, "when");
      if( vec_when.size() == 2 )
      {
          std::vector<std::string> vec_con = nxsd::split(vec_when[1], "=");
          if( vec_con.size() == 2 )
          {
              map_condition[vec_con[0]] = vec_con[1];
              str_con = vec_con[1];
          }

          std::vector<std::string> vec_set = nxsd::split(vec_when[0], "set");
          if( vec_set.size() >= 1 )
          {
              std::vector<std::string> vec_updated = nxsd::split(vec_set[vec_set.size() - 1], ",");
              for( std::string::size_type i =0; i < vec_updated.size(); ++i)
              {
                  std::vector<std::string> vec_fv = nxsd::split(vec_updated[i], "=");
                  if( vec_fv.size() == 2 )
                  {
                      map_updated[vec_fv[0]] = vec_fv[1];
                  }

                  map<string, set<string>>::const_iterator iter = m_tables.find(table_name);
                  if( iter->second.find(vec_fv[0]) == iter->second.end() ){
                      all_fields_in_table = false;
                  }
              }
          }
      }

      if( map_updated.size() > 0 && map_condition.size() > 0 && all_fields_in_table ){
          if( 0 == table_name.compare("farmstat") ){                
              farm_update(map_updated, str_con);
          } else if( 0 == table_name.compare("xbeantrx") ){
              xbeans_update(map_updated, str_con);
          } else if( 0 == table_name.compare("quantumchook") ){
              quantumchook_update(map_updated, str_con);
          } else if( 0 == table_name.compare("quantumegg") ){
              quantumegg_update(map_updated, str_con);
          } else if( 0 == table_name.compare("chook") ){
              chook_update(map_updated, str_con);
          } else if( 0 == table_name.compare("chookfeeding") ){
              chookfeeding_update(map_updated, str_con);
          } else if( 0 == table_name.compare("cdisinfect") ){
              disinfect_update(map_updated, str_con);
          } else if( 0 == table_name.compare("cfatten") ){
              fatten_update(map_updated, str_con);
          }
      }
  }

  //@abi action
  void farm::del( string table_name, string condition ){
      eosio_assert( 1 == m_tables.count(table_name), "the table is not exist" );

      std::vector<std::string> vec_con = nxsd::split(condition, "=");

      if( vec_con.size() >= 2 ){
          if( 0 == table_name.compare("farmstat") ){
              farm_del(vec_con[1]);
          } else if( 0 == table_name.compare("xbeantrx") ){
              xbeans_del(vec_con[1]);
          } else if( 0 == table_name.compare("quantumchook") ){
              quantumchook_del(vec_con[1]);
          } else if( 0 == table_name.compare("quantumegg") ){
              quantumegg_del(vec_con[1]);
          } else if( 0 == table_name.compare("chook") ){
              chook_del(vec_con[1]);
          } else if( 0 == table_name.compare("chookfeeding") ){
              chookfeeding_del(vec_con[1]);
          } else if( 0 == table_name.compare("cdisinfect") ){
              disinfect_del(vec_con[1]);
          } else if( 0 == table_name.compare("cfatten") ){
              fatten_del(vec_con[1]);
          }
      }
  }

  // @abi action
  void farm::query( string table_name ){
	  eosio_assert( 1 == m_tables.count(table_name), "the table is not exist" );
  }

  void farm::farm_del( string condition )
  {
	  DELETE_ONE_RECORD(farm_stats, condition)
  }

  void farm::xbeans_del( string condition )
  {
      DELETE_ONE_RECORD(xbean_trxs, condition)
  }

  void farm::quantumchook_del( string condition )
  {
      DELETE_ONE_RECORD(quantum_chooks, condition)
  }

  void farm::quantumegg_del( string condition )
  {
      DELETE_ONE_RECORD(quantum_eggs, condition)
  }

  void farm::chook_del( string condition )
  {
      DELETE_ONE_RECORD(chooks, condition)
  }

  void farm::chookfeeding_del( string condition )
  {
      DELETE_ONE_RECORD(chook_feedings, condition)
  }

  void farm::disinfect_del( string condition )
  {
      DELETE_ONE_RECORD(cdisinfects, condition)
  }

  void farm::fatten_del( string condition )
  {
      DELETE_ONE_RECORD(cfattens, condition)
  }

	void farm::farm_insert(
                            string id,
                            string name,
                            string create_time,
                            string owner,
                            string on_block_time
                          )
  {
	  farm_stats farm_table(_self, eosio::string_to_name(table_scope));
	  auto id_index = eosio::string_to_name(id.c_str());
	  auto existing = farm_table.find( id_index );
  	eosio_assert( existing == farm_table.end(), "the farm is already exist" );

      farm_table.emplace(_self, [&](auto &new_farm) {
          new_farm.id = id_index;
          new_farm.farm_id = id;
          new_farm.name = name;
          new_farm.create_time = create_time;
          new_farm.owner = owner;
          new_farm.on_block_time = on_block_time;
      });
  }

  void farm::xbean_insert(
                              string id,
                              string from,
                              string to,
                              string quantity,
                              string create_time,
                              string trx_time,
                              string on_block_time 
                            )
  {
      xbean_trxs xbean_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = xbean_table.find( id_index );
      eosio_assert( existing == xbean_table.end(), "the xbean transaction is already exist" );

      xbean_table.emplace(_self, [&](auto &new_xbean) {
          new_xbean.id = id_index;
          new_xbean.trx_id = id;
          new_xbean.from = from;
          new_xbean.to = to;
          new_xbean.quantity = quantity;
          new_xbean.create_time = create_time;
          new_xbean.trx_time = trx_time;
          new_xbean.on_block_time = on_block_time;
      });
  }

  void farm::quantumchook_insert(
                                      string id,
                                      string dna,
                                      string owner,
                                      string qualification,
                                      string fattening_time,
                                      string on_block_time 
                                 )
  {
      quantum_chooks qc_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = qc_table.find( id_index );
      eosio_assert( existing == qc_table.end(), "the quantum chook is already exist" );

      qc_table.emplace(_self, [&](auto &new_qc) {
          new_qc.id = id_index;
          new_qc.chook_id = id;
          new_qc.dna = dna;
          new_qc.owner = owner;
          new_qc.qualification = qualification;
          new_qc.fattening_time = fattening_time;
          new_qc.on_block_time = on_block_time;
      });
  }

  void farm::quantumegg_insert(
                                  string id,
                                  string owner,
                                  string lay_egg_time,
                                  string on_block_time
                              )
  {
      quantum_eggs qe_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = qe_table.find( id_index );
      eosio_assert( existing == qe_table.end(), "the quantum egg is already exist" );

      qe_table.emplace(_self, [&](auto &new_qe) {
          new_qe.id = id_index;
          new_qe.egg_id = id;
          new_qe.owner = owner;
          new_qe.lay_egg_time = lay_egg_time;
          new_qe.on_block_time = on_block_time;
      });
  }

  void farm::chook_insert(
                            string id,
                            string farm_name,
                            string state,
                            string description,
                            string owner,
                            string fattening_time,
                            string hatch_time,
                            string on_block_time
                         )
  {
      chooks chook_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = chook_table.find( id_index );
      eosio_assert( existing == chook_table.end(), "the chook is already exist" );

      chook_table.emplace(_self, [&](auto &new_chook) {
          new_chook.id = id_index;
          new_chook.chook_id = id;
          new_chook.farm_name = farm_name;
          new_chook.owner = owner;
          new_chook.state = state;
          new_chook.description = description;
          new_chook.hatch_time = hatch_time;
          new_chook.fattening_time = fattening_time;
          new_chook.on_block_time = on_block_time;
      });
  }

  void farm::fatten_insert(
                            string id,
                            string farm_name,
                            string contact_info,
                            string shipping_address,
                            string receiver,
                            string shipping_time,
                            string fattening_time,
                            string killer_time,
                            string on_block_time
                          )
  {
      cfattens fatten_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = fatten_table.find( id_index );
      eosio_assert( existing == fatten_table.end(), "the chook is already exist" );

      fatten_table.emplace(_self, [&](auto &new_fatten) {
          new_fatten.id = id_index;
          new_fatten.chook_id = id;
          new_fatten.farm_name = farm_name;
          new_fatten.contact_info = contact_info;
          new_fatten.shipping_address = shipping_address;
          new_fatten.receiver = receiver;
          new_fatten.killer_time = killer_time;
          new_fatten.shipping_time = shipping_time;
          new_fatten.fattening_time = fattening_time;
          new_fatten.on_block_time = on_block_time;
      });
  }

  void farm::chookfeeding_insert(
                                    string id,
                                    string food,
                                    string location,
                                    string feeding_time,
                                    string on_block_time
                                )
  {
      chook_feedings feeding_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = feeding_table.find( id_index );
      eosio_assert( existing == feeding_table.end(), "the feeding record is already exist" );

      feeding_table.emplace(_self, [&](auto &new_feeding) {
          new_feeding.id = id_index;
          new_feeding.chook_id = id;
          new_feeding.food = food;
          new_feeding.location = location;
          new_feeding.feeding_time = feeding_time;
          new_feeding.on_block_time = on_block_time;
      });
  }

  void farm::disinfect_insert(
                                string id,
                                string event,
                                string description,
                                string location,
                                string disinfect_time,
                                string on_block_time
                              )
  {
      cdisinfects disinfect_table(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(id.c_str());
      auto existing = disinfect_table.find( id_index );
      eosio_assert( existing == disinfect_table.end(), "the disinfecting record is already exist" );

      disinfect_table.emplace(_self, [&](auto &new_disinfect) {
          new_disinfect.id = id_index;
          new_disinfect.chook_id = id;
          new_disinfect.event = event;
          new_disinfect.description = description;
          new_disinfect.location = location;
          new_disinfect.disinfect_time = disinfect_time;
          new_disinfect.on_block_time = on_block_time;
      });
  }

  void farm::farm_update( const fv_map& updated, const string& condition )
  {
  	farm_stats farm_table(_self, eosio::string_to_name(table_scope));
   	auto id_index = eosio::string_to_name(condition.c_str());
	  auto existing = farm_table.find( id_index );
   	eosio_assert( existing != farm_table.end(), "the farm is not exist" );

   	farm_table.modify(existing, _self, [&](auto &modifiable_farm) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
 		      if( 0 == it->first.compare("on_block_time") && !it->second.empty())
         	  modifiable_farm.on_block_time = it->second;
          else if( 0 == it->first.compare("create_time") && !it->second.empty())
            modifiable_farm.create_time = it->second;
          else if( 0 == it->first.compare("owner") && !it->second.empty())
            modifiable_farm.owner = it->second;
          else if( 0 == it->first.compare("name") && !it->second.empty())
            modifiable_farm.name = it->second;
       }
     });
  }

  void farm::xbeans_update( const fv_map& updated, const string& condition )
  {
      xbean_trxs tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the xbean transaction is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("create_time") && !it->second.empty())
            new_tb.create_time = it->second;
          else if( 0 == it->first.compare("from") && !it->second.empty())
            new_tb.from = it->second;
          else if( 0 == it->first.compare("to") && !it->second.empty())
            new_tb.to = it->second;
          else if( 0 == it->first.compare("trx_time") && !it->second.empty())
            new_tb.trx_time = it->second;
          else if( 0 == it->first.compare("quantity") && !it->second.empty())
            new_tb.quantity = it->second;
       }
     });
  }

  void farm::quantumchook_update( const fv_map& updated, const string& condition )
  {
      quantum_chooks tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the quantum chook is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("fattening_time") && !it->second.empty())
            new_tb.fattening_time = it->second;
          else if( 0 == it->first.compare("owner") && !it->second.empty())
            new_tb.owner = it->second;
          else if( 0 == it->first.compare("qualification") && !it->second.empty())
            new_tb.qualification = it->second;
          else if( 0 == it->first.compare("dna") && !it->second.empty())
            new_tb.dna = it->second;
       }
     });
  }

  void farm::quantumegg_update( const fv_map& updated, const string& condition )
  {
      quantum_eggs tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the quantum egg is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("lay_egg_time") && !it->second.empty())
            new_tb.lay_egg_time = it->second;
          else if( 0 == it->first.compare("owner") && !it->second.empty())
            new_tb.owner = it->second;
       }
     });
  }

  void farm::chook_update( const fv_map& updated, const string& condition )
  {
      chooks tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the chook is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("fattening_time") && !it->second.empty())
            new_tb.fattening_time = it->second;
          else if( 0 == it->first.compare("owner") && !it->second.empty())
            new_tb.owner = it->second;
          else if( 0 == it->first.compare("farm_name") && !it->second.empty())
            new_tb.farm_name = it->second;
          else if( 0 == it->first.compare("state") && !it->second.empty())
            new_tb.state = it->second;
          else if( 0 == it->first.compare("description") && !it->second.empty())
            new_tb.description = it->second;
          else if( 0 == it->first.compare("hatch_time") && !it->second.empty())
            new_tb.hatch_time = it->second;
       }
     });
  }

  void farm::chookfeeding_update( const fv_map& updated, const string& condition )
  {
      chook_feedings tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the record of chook feeding is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("feeding_time") && !it->second.empty())
            new_tb.feeding_time = it->second;
          else if( 0 == it->first.compare("location") && !it->second.empty())
            new_tb.location = it->second;
          else if( 0 == it->first.compare("food") && !it->second.empty())
            new_tb.food = it->second;
       }
     });
  }

  void farm::disinfect_update( const fv_map& updated, const string& condition )
  {
      cdisinfects tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the record of chook disinfecting is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("disinfect_time") && !it->second.empty())
            new_tb.disinfect_time = it->second;
          else if( 0 == it->first.compare("location") && !it->second.empty())
            new_tb.location = it->second;
          else if( 0 == it->first.compare("event") && !it->second.empty())
            new_tb.event = it->second;
          else if( 0 == it->first.compare("description") && !it->second.empty())
            new_tb.description = it->second;
       }
     });
  }

  void farm::fatten_update( const fv_map& updated, const string& condition )
  {
      cfattens tb(_self, eosio::string_to_name(table_scope));
      auto id_index = eosio::string_to_name(condition.c_str());
      auto existing = tb.find( id_index );
      eosio_assert( existing != tb.end(), "the record of chook fattening is not exist" );

      tb.modify(existing, _self, [&](auto &new_tb) {
      std::map<string, string>::const_iterator it;
      for( it = updated.begin(); it != updated.end(); it++ ){
          if( 0 == it->first.compare("on_block_time") && !it->second.empty())
            new_tb.on_block_time = it->second;
          else if( 0 == it->first.compare("killer_time") && !it->second.empty())
            new_tb.killer_time = it->second;
          else if( 0 == it->first.compare("fattening_time") && !it->second.empty())
            new_tb.fattening_time = it->second;
          else if( 0 == it->first.compare("shipping_time") && !it->second.empty())
            new_tb.shipping_time = it->second;
          if( 0 == it->first.compare("receiver") && !it->second.empty())
            new_tb.receiver = it->second;
          else if( 0 == it->first.compare("shipping_address") && !it->second.empty())
            new_tb.shipping_address = it->second;
          else if( 0 == it->first.compare("contact_info") && !it->second.empty())
            new_tb.contact_info = it->second;
          else if( 0 == it->first.compare("farm_name") && !it->second.empty())
            new_tb.farm_name = it->second;
       }
     });      
  }

} // nxsd

EOSIO_ABI( nxsd::farm, (insert)(update)(del)(query) )
