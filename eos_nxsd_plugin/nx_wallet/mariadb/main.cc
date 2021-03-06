#include <iostream>

#include "utilities.hpp"
#include "src/worker.hpp"

using mariadb::worker;
using mariadb::handle;
using mariadb::account_ref;
using mariadb::account;
using  namespace mariadb::concurrency;
using namespace utils;

int main(int argc, char* argv[]){
	std::string str_host("localhost");
	std::string str_userName("applocal");
	std::string str_userPWD("123");
	std::string str_dbTableName("nwdb_wallet.nw_invite_code_store");
	std::string str_tableFields("");
	int uPort = 3306;
	long lCount = 100;
	int iCodeLen = 6;

	int iOptsCunt = 0;
	iOptsCunt = get_opts(argc, argv, str_host, uPort, lCount, iCodeLen, str_userName, str_userPWD, str_dbTableName, str_tableFields);

	if( 0 == iOptsCunt ){
		printf("Usage: %s -h<host> -o<port> -c<count> -u<user> -p<password> -t<table_name> -f<table_fields(s:name:value,i:name:value)> -l<invite_code_len>\n", argv[0]);
		return 1;
	} else if( -1 == iOptsCunt ) {
		printf("Port or Count parameter's value must be more than 0!\n");
		return 1;
	} else if( str_tableFields.empty() ) {
                printf("Please specify the table fields' name and initial value!\n");
                return 1;
        }

	printf("***************************************\n"
			"Host:\t%s\n"
			"Port:\t%u\n"
			"Table:\t%s\n"
			"User:\t%s\n"
			"Passwd:\t%s\n"
			"Count:\t%ld\n"
			"****************************************\n",
			str_host.c_str(), uPort, str_dbTableName.c_str(), str_userName.c_str(), str_userPWD.c_str(), lCount );

	account_ref acc_ref = account::create(str_host, str_userName, str_userPWD, "", uPort);
	worker wk(acc_ref, 23, false, mariadb::command::insert);

	vector<string> invited_codes;
	const long code_count = generate_code(lCount, iCodeLen, invited_codes);

	long i = 0;
	const unsigned debug_num = 1000;

	string query_sql("");
	combine_insert_sql(str_dbTableName, str_tableFields, query_sql);

	while( i < code_count ){
		char csQuery[1024] = {'\0'};
		snprintf(csQuery, 1023, query_sql.c_str(), invited_codes[i].c_str());

		wk.set_query(csQuery);
		wk.execute();

		if( status::failed == wk.status()) return 1;
		if( 0 == i % debug_num && 0 != i  ) printf("Inserted: %ld\n", i);

		++i;
	}

	if ( code_count < debug_num ||  0 == code_count % debug_num) printf("Inserted: %ld\n", code_count);
	printf("[DONE]\n\n");

	return 0;
}
