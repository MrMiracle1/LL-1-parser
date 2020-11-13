#include"parser.h"
#include<iostream>
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
			else if(temp==0)
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