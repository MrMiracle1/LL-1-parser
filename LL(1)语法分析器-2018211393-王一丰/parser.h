#pragma once
#include"grammar.h"

class parser //LL(1)语法分析器类
{
private:
	grammar g;		//分析文法
	map<token, map<token, int>> parser_table;//预测分析表
	vector<token> token_stream;//输入字符流
public:
	void input_token();//通过文件读入字符流
	void table_derivation();//预测分析表推导
	void output_parser_table();//输出预测分析表
	void predict();//预测分析推导
};