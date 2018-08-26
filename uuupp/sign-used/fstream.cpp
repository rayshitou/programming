#include <fstream>
#include <sstream>

#include "filesystem.hpp"
#include "exception.hpp"
#include "fstream.hpp"
#include "logger.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fc {

   void read_file_contents( const fc::path& filename, std::string& result )
   {
      const boost::filesystem::path& bfp = filename;
      boost::filesystem::ifstream f( bfp, std::ios::in | std::ios::binary );
      // don't use fc::stringstream here as we need something with override for << rdbuf()
      std::stringstream ss;
      ss << f.rdbuf();
      result = ss.str();
   }
  
} // namespace fc 
