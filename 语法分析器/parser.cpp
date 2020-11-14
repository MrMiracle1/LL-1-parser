#include"parser.h"
#include<iostream>
#include<stack>
#include <iomanip>
void parser::input_token()
{
	FILE *fptr;
	fptr = fopen("token_stream.in", "r");
	string num="";
	while (!feof(fptr))
	{
		char c = fgetc(fptr);
		if (c == -1)
			break;
		if (c > '9' || c < '0')
		{
			if (num.size())
			{
				token_stream.emplace_back("n");
				num = "";
			}
			token t(1,c);
			token_stream.emplace_back(t);
		}
		else
		{
			num += c;
		}
	}
	token_stream.emplace_back("$");
	
}

void parser::table_derivation()
{
	g.remove_recursion();
	cout << "������ݹ�����в���ʽ��" << endl;
	g.output_productions();
	g.output_first();
	g.output_follow();
	cout <<endl<<"first_follow����" << endl;
	g.output_first_follow();
	g.terminators.erase(find(g.terminators.begin(), g.terminators.end(), "��"));
	cout << endl << "Ԥ�������" << endl;
	for (token i : g.non_terminators)
	{
		for (int j : g.productions_class[i])
		{
			bool is_epsilon = false;
			for (token t : g.first_of_production[j])
			{
				if (t != "��")
					parser_table[i][t] = j;
				else
					is_epsilon = true;
			}
			if (is_epsilon)
			{
				for (token t : g.follow[i])
				{
					parser_table[i][t] = j;
				}
			}
		}
		for (token t : g.follow[i])
		{
			if(parser_table[i].find(t)== parser_table[i].end())
				parser_table[i][t] = -1;
		}
	}
	for (token i : g.non_terminators)
	{
		for (token j : g.terminators)
		{
			if (parser_table[i].find(j) == parser_table[i].end())
				parser_table[i][j] = -2;
		}
	}
}

void parser::output_parser_table()
{

	//for (token i : g.non_terminators)
	//{
	//	for (token j : g.terminators)
	//	{
	//		cout << parser_table[i][j] << "	";
	//	}
	//	cout << endl;
	//}
	cout << "-------------------------------------------------------------------------------------------" << endl;
	cout << setiosflags(ios::left) << setw(10)<<" ";
	for (token i : g.terminators)
	{
		cout << setw(10) <<i;
	}
	cout << endl;
	cout << "-------------------------------------------------------------------------------------------" << endl;
	for (token i : g.non_terminators)
	{
		cout << setw(10) <<i;
		for (token j : g.terminators)
		{
			int temp = parser_table[i][j];
			if (temp == -1)
			{
				cout <<setw(10) << "synch";
				continue;
			}
			else if(temp==-2)
			{
				cout <<setw(10)<<" ";
				continue;
			}
			token s= g.productions[temp].left+"->";
			for (token t : g.productions[temp].right)
			{
				s = s+t;
			}
			cout << setw(10)<<s;
			
		}
		cout << endl;
	}
}

void parser::predict()
{
	cout << endl << "Ԥ����̣�" << endl;
	cout << "-----------------------------------------------" << endl;
	cout << setiosflags(ios::left) << setw(10)<<"���"<<setw(30) << "�����"
		<< setw(20) << "������" << setw(20)<<"���" << endl;
	cout << "-----------------------------------------------" << endl;
	stack<token> parser_stack;
	pair<deque<token>, deque<token>> left_pattern{ {},{g.start_token} };
	parser_stack.push("$");
	parser_stack.push(g.start_token);
	vector<token>::iterator iter = token_stream.begin();
	for (int d=1;iter != token_stream.end();d++)
	{
		cout << setw(10) << d;
		string s = "";
		for (token t : left_pattern.first)
		{
			s+=t;
		}
		for (token t : left_pattern.second)
		{
			s+= t;
		}
		cout << setw(30) << s;
		vector<token>::iterator iter2 = iter;
		s = "";
		for (;iter2 != token_stream.end();iter2++)
		{
			s+=*iter2;
		}
		cout << setw(20)<<s;
		s = "";
		if (find(g.terminators.begin(), g.terminators.end(), parser_stack.top()) != g.terminators.end())//ջ��Ϊ���ս��
		{
			if (*iter == parser_stack.top())
			{
				s += "ƥ��ɹ���" + *iter;
				iter++;			
			}
			else
			{
				s+="error:" + parser_stack.top() + " expected";
			}
			parser_stack.pop();
			if (left_pattern.second.size())
			{
				left_pattern.first.push_back(left_pattern.second.front());
				left_pattern.second.pop_front();
			}
		}
		else//ջ��Ϊ�ս��
		{
			int p = parser_table[parser_stack.top()][*iter];
			if (p == -2)
			{
				s+= "error:" + *iter + " unexpected";
			}
			else if (p == -1)
			{
				s+="error:"+parser_stack.top()+"expected";
				parser_stack.pop();
			}
			else
			{
				parser_stack.pop();
				left_pattern.second.pop_front();
				s+=g.productions[p].left+"->";
				for (string j : g.productions[p].right)
					s+=j;
				if (g.productions[p].right.front()!= "��")
				{
					for (int i = g.productions[p].right.size() - 1;i >= 0;i--)
					{
						parser_stack.push(g.productions[p].right[i]);
					}
					left_pattern.second.insert(left_pattern.second.begin(), g.productions[p].right.begin(), g.productions[p].right.end());
				}				
			}
		}
		cout <<setw(20)<< s << endl;
	}
}