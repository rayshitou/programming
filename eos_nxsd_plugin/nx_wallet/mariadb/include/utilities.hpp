#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

#ifndef INC_UTILITIES_H
#define INC_UTILITIES_H

namespace utils {

extern int get_opts(const int argc, char* argv[], string& host, int& port, long& count, int& code_len, string& user, string& passwd, string& table_name, string& fields);
extern int generate_code(const long count, const int codeLen, vector<string>& codes);
extern vector<string> split(const string &s, char delim);
// @param: table_fields format is like: s:field_name:value,i:field_name:value
extern void combine_insert_sql(const string& table_name, const string& table_fields, string& query_sql);

}

#endif
