#pragma once
#include"grammar.h"

class parser 
{
private:
	grammar g;
	map<token, map<token, int>> parser_table;
	vector<token> token_stream;
public:
	void input_token();
	void table_derivation();
	void output_parser_table();
	void predict();
};