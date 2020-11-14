#pragma once
#include"grammar.h"

class parser //LL(1)�﷨��������
{
private:
	grammar g;		//�����ķ�
	map<token, map<token, int>> parser_table;//Ԥ�������
	vector<token> token_stream;//�����ַ���
public:
	void input_token();//ͨ���ļ������ַ���
	void table_derivation();//Ԥ��������Ƶ�
	void output_parser_table();//���Ԥ�������
	void predict();//Ԥ������Ƶ�
};