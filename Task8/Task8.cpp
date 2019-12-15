/*
 * 8. Хеш-таблица
 * Ограничение времени	0.2 секунды
 * Ограничение памяти	15Mb
 * Ввод стандартный ввод или input.txt
 * Вывод стандартный вывод или output.txt
 * 
 * Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией. 
 * Хранимые строки непустые и состоят из строчных латинских букв. 
 * Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера. 
 * Начальный размер таблицы должен быть равным 8-ми. 
 * Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4. 
 * Структура данных должна поддерживать операции добавления строки в множество, удаления строки из множества и проверки принадлежности данной строки множеству. 
 * Для разрешения коллизий используйте двойное хеширование.
 * 
 * Формат ввода
 * Каждая строка входных данных задает одну операцию над множеством. 
 * Запись операции состоит из типа операции и следующей за ним через пробел строки, над которой проводится операция. 
 * Тип операции – один из трех символов: 
 * + означает добавление данной строки в множество; 
 * - означает удаление строки из множества; 
 * ? означает проверку принадлежности данной строки множеству. 
 * При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве. 
 * При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
 *
 * Формат вывода
 * Программа должна вывести для каждой операции одну из двух строк OK или FAIL.
 * Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.
 * Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве и потому не может быть добавлен. OK иначе.
 * Для операции '-': OK, если элемент присутствовал во множестве и успешно удален. FAIL иначе.
 *
 * Пример
 * Ввод	
 * + hello
 * + bye
 * ? bye
 * + bye
 * - bye
 * ? bye
 * ? hello
 * Вывод
 * OK
 * OK
 * OK
 * FAIL
 * OK
 * FAIL
 * OK
 */

#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>
#include "HashMap.hpp"

void processCommands(const std::vector<std::pair<std::string, std::string>>& commands)
{
	custom_containers::HashMap<std::string> hashMap;
	for (auto& command : commands)
	{
		auto result = false;
		if (command.first == "+")
		{
			result = hashMap.insert(command.second);
		}
		else if (command.first == "?")
		{
			result = hashMap.contains(command.second);
		}
		else if (command.first == "-")
		{
			result = hashMap.remove(command.second);
		}
		if (result)
		{
			std::cout << "OK" << std::endl;
		}
		else
		{
			std::cout << "FAIL" << std::endl;
		}
	}
}

std::vector<std::pair<std::string, std::string>> generateRandomCommands()
{
	srand(time(nullptr));
	std::vector<std::pair<std::string, std::string>> commands;
	for (auto i = 0; i < 600; ++i)
	{
		const auto operation_number = rand() % 3;
		std::string operation = "+";
		if (operation_number == 1)
		{
			operation = "-";
		}
		else if (operation_number == 2)
		{
			operation = "?";
		}
		std::string str;
		for (auto j = 0; j < 2; ++j)
		{
			str += 'a' + rand() % ('d' - 'a');
		}
		commands.emplace_back(operation, str);
	}
	return commands;
}


int main()
{
	std::ifstream in;
	try
	{
		in.open("input.txt");
	}
	catch (...)
	{
		return 1;
	}
	std::vector<std::pair<std::string, std::string>> commands;
	while (!in.eof())
	{
		std::string operation;
		std::string key;
		in >> operation;
		if (operation.empty())
		{
			break;
		}
		in >> key;
		commands.emplace_back(operation, key);
	}
	//commands = generateRandomCommands();
	processCommands(commands);
	return 0;
}
