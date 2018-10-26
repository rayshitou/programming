#include <stdlib.h>
#include <time.h>
#include <set>
#include <algorithm>
#include <sstream>
#include <cstring>

#include "utilities.hpp"

using namespace utils;
using std::atoi;
using std::atol;
using std::set;

void utils::combine_insert_sql(const string& table_name, const string& table_fields, string& query_sql){
	string str_fields("");
	string str_values("");

	vector<string> fields = split(table_fields, ',');
	for( int i = 0; i < fields.size(); ++i) {
		vector<string> fds = split(fields[i], ':');
		if( 3 == fds.size() ){
			str_fields += fds[1];

			if( 0 != fds[2].compare("random") ) {
				if( 0 == fds[0].compare("s") )
					str_values += "'" + fds[2] +"'";
				else if( 0 == fds[0].compare("i") )
					str_values += fds[2];
			} else {
				if( 0 == fds[0].compare("s") )
					str_values += "'%s'";
				else if( 0 == fds[0].compare("i") )
					str_values += "%d";
			}

			if( i != fields.size() - 1 ) {
				str_fields += ",";
				str_values += ",";
			}
		}
	}

	query_sql += "insert into " + table_name + "(" + str_fields + ") values(" + str_values + ")";

	return;
}

vector<string> utils::split(const std::string &s, char delim) {
    vector<string> elems;
    // Check to see if emtpy string, give consistent result
    if(s == "")
        elems.emplace_back("");
    else {
        std::stringstream ss;
        ss.str(s);
        string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
    return elems;
}

int utils::get_opts(const int argc, char* argv[], string& host, int& port, long& count, int& code_len, string& user, string& passwd, string& table_name, string& fields){
	int i = 1;
	while( i < argc ){
		if( '-' == argv[i][0] ){
			switch( argv[i][1] ){
				case 'h':
					host = (2 == std::strlen(argv[i]) ? argv[i+1] : argv[i] + 2);
					break;
				case 'o':
					port = (2 == std::strlen(argv[i]) ? atoi(argv[i+1]) : atoi(argv[i] + 2));
					if( port <= 0 ) return -1;
					break;
				case 'c':
					count = (2 == std::strlen(argv[i]) ? atoi(argv[i+1]) : atoi(argv[i] + 2));
					if( count <= 0 ) return -1;
					break;
				case 'l':
					code_len = (2 == std::strlen(argv[i]) ? atoi(argv[i+1]) : atoi(argv[i] + 2));
					break;
				case 'u':
					user = (2 == std::strlen(argv[i]) ? argv[i+1] : argv[i] + 2);
					break;
				case 'p':
					passwd = (2 == std::strlen(argv[i]) ? argv[i+1] : argv[i] + 2);
					break;
				case 't':
					table_name = (2 == std::strlen(argv[i]) ? argv[i+1] : argv[i] + 2);
					break;
				case 'f':
					fields = (2 == std::strlen(argv[i]) ? argv[i+1] : argv[i] + 2);
					break;
				default:
					break;
			}
		}
		++i;
	}
	return argc - 1;
}

int utils::generate_code(const long count, const int codeLen, vector<string>& v_codes){
	const char* code_elements = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
	const int i_elements_len = std::strlen(code_elements);

	srand (time(NULL));
	set<string> codes;
	while( codes.size() < count ){
		string code("");
		while( code.size() < codeLen ){
			int iIndex = rand() % i_elements_len;
			code += code_elements[iIndex];
		}
		codes.emplace(code);
	}

	v_codes.assign(codes.begin(), codes.end());

	return v_codes.size();
}
