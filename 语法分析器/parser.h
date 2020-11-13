#pragma once
#include"grammar.h"

class parser 
{
private:
	grammar g;
	map<token, map<token, int>> parser_table;
public:
	void table_derivation();
	void output_parser_table();
};