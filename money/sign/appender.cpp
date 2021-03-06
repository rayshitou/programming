#include "appender.hpp"
#include "logger.hpp"
#include <unordered_map>
#include <string>
#include "console_appender.hpp"
#include "file_appender.hpp"
#include "gelf_appender.hpp"
#include "variant.hpp"
#include <mutex>
#include "console_defines.h"


namespace fc {

   std::unordered_map<std::string,appender::ptr>& get_appender_map() {
     static std::unordered_map<std::string,appender::ptr> lm;
     return lm;
   }
   std::unordered_map<std::string,appender_factory::ptr>& get_appender_factory_map() {
     static std::unordered_map<std::string,appender_factory::ptr> lm;
     return lm;
   }
   appender::ptr appender::get( const fc::string& s ) {
      static std::mutex appender_mutex;
      std::lock_guard<std::mutex> lock(appender_mutex);
      return get_appender_map()[s];
   }
   bool  appender::register_appender( const fc::string& type, const appender_factory::ptr& f )
   {
      get_appender_factory_map()[type] = f;
      return true;
   }
   appender::ptr appender::create( const fc::string& name, const fc::string& type, const variant& args  )
   {
      auto fact_itr = get_appender_factory_map().find(type);
      if( fact_itr == get_appender_factory_map().end() ) {
         //wlog( "Unknown appender type '%s'", type.c_str() );
         return appender::ptr();
      }
      auto ap = fact_itr->second->create( args );
      get_appender_map()[name] = ap;
      return ap;
   }

   static bool reg_console_appender = appender::register_appender<console_appender>( "console" );
   //static bool reg_file_appender = appender::register_appender<file_appender>( "file" );
   static bool reg_gelf_appender = appender::register_appender<gelf_appender>( "gelf" );

} // namespace fc
