#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void two_arg_func(std::vector< LN > &elements, std::string &str)
{
	auto e1 = std::move(elements[elements.size() - 1]);
	elements.pop_back();
	auto e2 = std::move(elements[elements.size() - 1]);
	elements.pop_back();
	switch (str[0])
	{
	case '+':
		elements.push_back(e2 + e1);
		break;
	case '-':
		elements.push_back(e2 - e1);
		break;
	case '*':
		elements.push_back(e2 * e1);
		break;
	case '/':
		elements.push_back(e2 / e1);
		break;
	case '%':
		elements.push_back(e2 % e1);
		break;
	case '<':
		if (str.size() > 1)
			elements.push_back(e2 <= e1);
		else
			elements.push_back(e2 < e1);
		break;
	case '>':
		if (str.size() > 1)
			elements.push_back(e2 >= e1);
		else
			elements.push_back(e2 > e1);
		break;
	case '=':
		elements.push_back(e2 == e1);
		break;
	case '!':
		elements.push_back(e2 != e1);
		break;
	}
}

void one_arg_func(std::vector< LN > &elements, std::string &str)
{
	auto e1 = std::move(elements[elements.size() - 1]);
	elements.pop_back();
	switch (str[0])
	{
	case '~':
		elements.push_back(~e1);
		break;
	case '_':
		elements.push_back(-e1);
		break;
	}
}

bool is_num(std::string &str)
{
	if ('0' <= str[0] && str[0] <= '9')
		return true;
	if ((str[0] == '+' || str[0] == '-') && str.size() > 1 && ('0' <= str[1] && str[1] <= '9'))
		return true;
	if (str[0] == 'N')
		return true;
	return false;
}

int main(int argc, const char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "wrong number of arguments\n";
		return ERROR_INVALID_DATA;
	}

	std::ifstream reader(argv[1]);
	if (!reader.is_open())
	{
		std::cerr << "file not found to read\n";
		return ERROR_NOT_FOUND;
	}

	std::vector< LN > elements;
	std::string str;
	try
	{
		while (reader >> str)
		{
			if (is_num(str))
				elements.emplace_back(str);
			else
			{
				if (str[0] == '~' || str[0] == '_')
					one_arg_func(elements, str);
				else
					two_arg_func(elements, str);
			}
		}
	} catch (const std::bad_alloc &e)
	{
		std::cerr << "no memory allocated" << std::endl;
		reader.close();
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	reader.close();

	std::ofstream writer(argv[2]);
	if (!writer.is_open())
	{
		std::cerr << "file not found to write\n";
		return ERROR_NOT_FOUND;
	}
	for (auto it = elements.rbegin(); it != elements.rend(); ++it)
		writer << *it << std::endl;
	writer.close();
}
