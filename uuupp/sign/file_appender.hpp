#pragma once

#include "filesystem.hpp"
#include "appender.hpp"
#include "logger.hpp"
#include "time.hpp"

namespace fc {

class file_appender : public appender {
    public:
         struct config {
            config( const fc::path& p = "log.txt" );

            fc::string                         format;
            fc::path                           filename;
            bool                               flush = true;
            bool                               rotate = false;
            microseconds                       rotation_interval;
            microseconds                       rotation_limit;
         };
         file_appender( const variant& args );
         ~file_appender();
         virtual void log( const log_message& m )override;

      private:
         class impl;
         fc::shared_ptr<impl> my;
   };
} // namespace fc

#include "reflect.hpp"
FC_REFLECT( fc::file_appender::config,
            (format)(filename)(flush)(rotate)(rotation_interval)(rotation_limit) )
