file(GLOB HEADERS "include/eosio/nxsd_api_plugin/*.hpp")
add_library( nxsd_api_plugin
             nxsd_api_plugin.cpp
	     nxsd_wallet_manager.cpp
             pke.cpp
             httpc.cpp
             ${HEADERS} )

target_link_libraries( nxsd_api_plugin  appbase fc http_plugin chain_plugin wallet_plugin )
target_include_directories( nxsd_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
