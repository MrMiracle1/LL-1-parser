#include"parser.h"
#include<iostream>
#include<stack>
void parser::table_derivation()
{
	g.remove_recursion();
	g.output_productions();
	g.output_first();
	g.output_follow();
	for (token i : g.non_terminators)
	{
		for (int j : g.productions_class[i])
		{
			bool is_epsilon = false;
			for (token t : g.first_of_production[j])
			{
				if (t != "0")
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

	cout << "	";
	for (token i : g.terminators)
	{
		cout <<i<< "		";
	}
	cout << endl;
	for (token i : g.non_terminators)
	{
		cout <<i<<"	";
		for (token j : g.terminators)
		{
			int temp = parser_table[i][j];
			if (temp == -1)
			{
				cout << "synch	";
				continue;
			}
			else if(temp==-2)
			{
				cout << "		";
				continue;
			}
			cout << g.productions[temp].left << "->";
			for (token t : g.productions[temp].right)
			{
				cout << t ;
			}
			cout << "	";
		}
		cout << endl;
	}
}

void parser::predict()
{
	stack<token> parser_stack;
	pair<deque<token>, deque<token>> left_pattern{ {},{g.start_token} };
	parser_stack.push("dollar");
	parser_stack.push(g.start_token);
	vector<token>::iterator iter = token_stream.begin();
	for (;iter != token_stream.end();)
	{
		for (token t : left_pattern.first)
		{
			cout << t;
		}
		for (token t : left_pattern.second)
		{
			cout << t;
		}
		cout << "			";
		vector<token>::iterator iter2 = iter;
		for (;iter2 != token_stream.end();iter2++)
		{
			cout << *iter2;
		}
		cout << "			";
		if (find(g.terminators.begin(), g.terminators.end(), parser_stack.top()) != g.terminators.end())//Õ»¶¥Îª·ÇÖÕ½á·û
		{
			if (*iter == parser_stack.top())
			{
				iter++;
			}
			else
			{
				cout << "error:" << parser_stack.top() << " expected"<<endl;
			}
			parser_stack.pop();
			if (left_pattern.second.size())
			{
				left_pattern.first.push_back(left_pattern.second.front());
				left_pattern.second.pop_front();
			}
		}
		else//Õ»¶¥ÎªÖÕ½á·û
		{
			int p = parser_table[parser_stack.top()][*iter];
			if (p == -2)
			{
				cout << "error:" << *iter << " unexpected" << endl;
			}
			else if (p == -1)
			{
				cout << "error:"<<parser_stack.top()<<"expected"<<endl;
				parser_stack.pop();
			}
			else
			{
				parser_stack.pop();
				left_pattern.second.pop_front();
				cout << g.productions[p].left << "=";
				for (string j : g.productions[p].right)
					cout << j;
				cout << endl;
				if (g.productions[p].right.front()!= "0")
				{
					for (int i = g.productions[p].right.size() - 1;i >= 0;i--)
					{
						parser_stack.push(g.productions[p].right[i]);
					}
					left_pattern.second.insert(left_pattern.second.begin(), g.productions[p].right.begin(), g.productions[p].right.end());
				}				
			}
		}
	}
}