#include"grammar.h"
#include<iostream>
#include<algorithm>
void grammar::remove_recursion()//������ݹ�
{
	int t = 0;//productions_class�ĳ�ʼ��
	for (production i : productions)
	{
		productions_class[i.left].insert(t);
		t++;
	}

	set<token> no_recursion;//������ݹ�ķ��ս��
	vector<token> new_non_terminators;//���������ս��
	for (const auto& non_terminator : non_terminators)//���������ս��
	{
		set<int> new_production_class = productions_class[non_terminator];//���ս���������²���ʽ
		for (int i : productions_class[non_terminator])//�������ս�������в���ʽ
		{
			token first_token = productions[i].right.front();//��ȡ����ʽ��һ���ַ�
			if (no_recursion.find(first_token) != no_recursion.end())//�ж��Ƿ���������ݹ�
			{
				new_production_class.erase(i);//ɾ����Ŀǰ���ս���µĲ���ʽ
				productions[i].right.pop_front();//ɾ������ʽ��һ���ַ�
				for (int j : productions_class[first_token])//�������ַ���Ӧ�����в���ʽ
				{
					new_production_class.insert(productions.size());//��Ŀǰ�ս������һ������ʽ
					production new_production = productions[i];//һ���²���ʽ����ԭ����ʽ
					new_production.right.insert(new_production.right.begin(), productions[j].right.begin(), productions[j].right.end());//ԭ��һ���ַ��Ĳ���ʽ�Ҷ˲���ԭ����ʽǰ��
					productions.emplace_back(new_production);//������²���ʽ
				}
			}
		}
		productions_class[non_terminator] = new_production_class;//���¸÷��ս�������в���ʽ
		vector<int> left_recursion_productions;//����ݹ�Ĳ���ʽ����
		for (int i : productions_class[non_terminator])//�����÷��ս�������в���ʽ
		{
			const token& first_token = productions[i].right.front();//��ȡ����ʽ��һ���ַ�
			if (first_token == non_terminator)//��һ���ַ�Ϊ�÷��ս��
			{
				new_production_class.erase(i);//ȥ���ò���ʽ�ڷ��ս���µļ�¼
				left_recursion_productions.emplace_back(i);//�����Ĳ���ʽ������ݹ����ʽ����
			}
		}
		if (left_recursion_productions.size())//�÷��ս������ݹ�
		{
			productions_class[non_terminator] = new_production_class;//���¸÷��ս�������в���ʽ
			token new_non_terminator=non_terminator + "\'";//�����·��ս��
			new_non_terminators.emplace_back(new_non_terminator);//������ս������
			for (int i : productions_class[non_terminator])//�����÷��ս�������в���ʽ
			{
				productions[i].right.emplace_back(new_non_terminator);//����ʽβ�������·��ս��
			}
			productions_class[non_terminator] = new_production_class;//���¸÷��ս�������в���ʽ
			productions_class[new_non_terminator].insert(productions.size());//�����·��ս����һ������ʽ
			production new_production;//�²���ʽ����
			new_production.left = new_non_terminator;//�²���ʽ���Ϊ�·��ս��
			new_production.right = deque<token>{ "0" };//�²���ʽ�Ҳ�Ϊ��
			productions.emplace_back(new_production);//������²���ʽ
			/*productions.emplace_back(new_non_terminator, deque<token>{NULL});*/
			for (int i : left_recursion_productions)//����������ݹ����ʽ
			{
				productions[i].left = new_non_terminator;//����ʽ���Ϊ�·��ս��
				productions[i].right.pop_front();//�Ҷ����ַ�����
				productions[i].right.emplace_back(new_non_terminator);//�Ҷ�ĩβ�����·��ս�� 
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

void grammar::first_derivation(const token& non_terminator)//����first��
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