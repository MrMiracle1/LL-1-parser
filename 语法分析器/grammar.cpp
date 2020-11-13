#include"grammar.h"
#include<iostream>
#include<algorithm>
void grammar::remove_recursion()//消除左递归
{
	int t = 0;//productions_class的初始化
	for (production i : productions)
	{
		productions_class[i.left].insert(t);
		t++;
	}

	set<token> no_recursion;//已消左递归的非终结符
	vector<token> new_non_terminators;//产生的新终结符
	for (const auto& non_terminator : non_terminators)//遍历所有终结符
	{
		set<int> new_production_class = productions_class[non_terminator];//该终结符的所有新产生式
		for (int i : productions_class[non_terminator])//遍历该终结符的所有产生式
		{
			token first_token = productions[i].right.front();//提取产生式第一个字符
			if (no_recursion.find(first_token) != no_recursion.end())//判断是否已消除左递归
			{
				new_production_class.erase(i);//删除在目前非终结符下的产生式
				productions[i].right.pop_front();//删除产生式第一个字符
				for (int j : productions_class[first_token])//遍历该字符对应的所有产生式
				{
					new_production_class.insert(productions.size());//向目前终结符加入一个产生式
					production new_production = productions[i];//一个新产生式复制原产生式
					new_production.right.insert(new_production.right.begin(), productions[j].right.begin(), productions[j].right.end());//原第一个字符的产生式右端插入原生产式前端
					productions.emplace_back(new_production);//加入该新产生式
				}
			}
		}
		productions_class[non_terminator] = new_production_class;//更新该非终结符的所有产生式
		vector<int> left_recursion_productions;//含左递归的产生式集合
		for (int i : productions_class[non_terminator])//遍历该非终结符的所有产生式
		{
			const token& first_token = productions[i].right.front();//提取产生式第一个字符
			if (first_token == non_terminator)//第一个字符为该非终结符
			{
				new_production_class.erase(i);//去除该产生式在非终结符下的记录
				left_recursion_productions.emplace_back(i);//将更改产生式放入左递归产生式集合
			}
		}
		if (left_recursion_productions.size())//该非终结符有左递归
		{
			productions_class[non_terminator] = new_production_class;//更新该非终结符的所有产生式
			token new_non_terminator=non_terminator + "\'";//定义新非终结符
			new_non_terminators.emplace_back(new_non_terminator);//加入非终结符集合
			for (int i : productions_class[non_terminator])//遍历该非终结符下所有产生式
			{
				productions[i].right.emplace_back(new_non_terminator);//产生式尾部加入新非终结符
			}
			productions_class[non_terminator] = new_production_class;//更新该非终结符的所有产生式
			productions_class[new_non_terminator].insert(productions.size());//增加新非终结符下一个产生式
			production new_production;//新产生式定义
			new_production.left = new_non_terminator;//新产生式左侧为新非终结符
			new_production.right = deque<token>{ "0" };//新产生式右侧为空
			productions.emplace_back(new_production);//加入该新产生式
			/*productions.emplace_back(new_non_terminator, deque<token>{NULL});*/
			for (int i : left_recursion_productions)//遍历所有左递归产生式
			{
				productions[i].left = new_non_terminator;//产生式左端为新非终结符
				productions[i].right.pop_front();//右端首字符弹出
				productions[i].right.emplace_back(new_non_terminator);//右端末尾加入新非终结符 
				productions_class[new_non_terminator].insert(i);
			}
		}
		no_recursion.insert(non_terminator);
	}
	for (auto &new_non_terminator : new_non_terminators)
	{
		non_terminators.emplace_back(new_non_terminator);
	}
}

void grammar::first_derivation(const token& non_terminator)//计算first集
{
	for (int i : productions_class[non_terminator])
	{
		production current_production = productions[i];
		bool all_epsilon = true;
		for (const auto& j : current_production.right)
		{
			if (find(terminators.begin(), terminators.end(), j) != terminators.end())
			{
				all_epsilon = false;
				first_of_production[i].insert(j);
				break;
			}
			if (first_over[j] == false)
			{
				first_derivation(j);
			}
			first_of_production[i].insert(first[j].begin(), first[j].end());
			if (first[j].find("0") == first[j].end())
			{
				all_epsilon = false;
				break;
			}
			else
			{
				first_of_production[i].erase("0");
			}
		}
		if (all_epsilon)
		{
			first_of_production[i].insert("0");
		}
		first[non_terminator].insert(first_of_production[i].begin(), first_of_production[i].end());
	}
	first_over[non_terminator] = true;
}

void grammar::follow_derivation(const token& non_terminator) 
{
	for (const auto& i : non_terminators)
	{
		for (int j : productions_class[i])
		{
			const auto& current_production = productions[j];
			for (int t = 0,k;t < current_production.right.size();t++)
			{
				if (current_production.right[t] == non_terminator)
				{
					for (k = t + 1;k < current_production.right.size();k++)
					{
						const auto& temp_token=current_production.right[k];
						if (find(terminators.begin(), terminators.end(), temp_token) != terminators.end())
						{
							follow[non_terminator].insert(temp_token);
							break;
						}
						follow[non_terminator].insert(first[temp_token].begin(), first[temp_token].end());
						if (first[temp_token].find("0") == first[temp_token].end())
						{
							break;
						}
						else
						{
							follow[non_terminator].erase("0");
						}
					}
					if (k == current_production.right.size() && current_production.left != non_terminator)
					{
						const auto& non_terminator2 = current_production.left;
						follow_include[non_terminator][non_terminator2] = true;
						if (!follow_include[non_terminator2][non_terminator])
						{
							if (!(follow_over[non_terminator2]))
							{
								follow_derivation(non_terminator2);
							}
							follow[non_terminator].insert(follow[non_terminator2].begin(), follow[non_terminator2].end());
						}
					}
				}
			}
		}
	}
	follow_over[non_terminator] = true;
}

void grammar::output_productions()
{
	for (production i : productions)
	{
		cout << i.left << "=";
		for(string j:i.right)
			cout<<j;
		cout << endl;
	}
}

void grammar::output_first()
{
	for (const auto& i : productions)
	{
		first_of_production.push_back({});
	}
	for (token i : non_terminators)
	{
		first_derivation(i);
		cout << i << ": ";
		for (token j : first[i])
		{
			cout << j << " ";
		}
		cout << endl;
	}
}

void grammar::output_follow()
{
	follow[start_token].insert("dollar");
	for (token i : non_terminators)
	{
		if(!follow_over[i])
			follow_derivation(i);
	}
	for (token i:non_terminators)
	{
		for (token j: non_terminators)
		{
			if (follow_include[i][j] && follow_include[j][i])
			{
				follow[i].insert(follow[j].begin(), follow[j].end());
				follow[j] = follow[i];
				follow_include[i][j] = follow_include[j][i] = false;
			}
		}
	}
	for (token i : non_terminators)
	{
		cout << i << ": ";
		for (token j : follow[i])
		{
			cout << j << " ";
		}
		cout << endl;
	}
}

grammar::grammar():
	non_terminators({ "E", "T", "F" }),
	terminators(
		{
			"+", "-", "*", "/", "(", ")","n","0","dollar"
		}
	),
	start_token("E"),
	productions( 
		{
			{
				"E", {"E","+","T"}
			},
			{
				"E", {"E","-","T"}
			},
			{

				"E", {"T"}

			},
			{
				"T", {"T","*","F"}
			},
			{
				"T", {"T","/","F"}
			},
			{
				"T", {"F"}
			},
			{
				"F", {"(","E",")"}
			},
			{
				"F", {"n"}
			}
		}
	)
{
	for (token i : non_terminators)
	{
		first_over.insert(pair<token, bool>(i, false));
	}
}