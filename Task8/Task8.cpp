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

namespace custom_containers
{
    /**
	 * \brief Класс, реализующий хеш-таблицу открытой адресации с двойным хешированием.
	 * \tparam T Тип элементов в хеш-таблице. Должен иметь метод data(), возвращающий указатель на данные. 
	 */
	template<typename T>
	class HashMap
	{
	public:
	/**
         * \brief Конструктор по умолчанию.
         */
		HashMap() = default;
        /**
         * \brief Конструктор с возможностью задать пользовательские хеш-функции.
         * \param hash1 первая основная хеш-функция
         * \param hash2 вторая хеш-функция для разрешения коллизий
         */
        HashMap(const std::function<size_t(const T&)>& hash1, const std::function<size_t(const T&)>& hash2): hash1_(hash1), hash2_(hash2) {}
		~HashMap()
		{
			for (auto nodePtr : data_)
			{
				delete nodePtr;
			}
		}

		HashMap(const HashMap& other) = delete;
		HashMap(HashMap&& other) noexcept = delete;
		HashMap& operator=(const HashMap& other) = delete;
		HashMap& operator=(HashMap&& other) noexcept = delete;

        /**
		 * \brief Вставка элемента в хеш-таблицу.
		 * \param key Вставляемый элемент.
		 * \return True, если вставка завершилась успешно, иначе - false.
		 */
		bool insert(const T& key)
		{
			if (isFull())
			{
				rehash();
			}
			auto position = probePositionsInto(key, data_);
			assert(position.second != PositionType::INVALID);
			if (position.second == PositionType::FREE)
			{
				data_[position.first] = new Node(key);
				size_ += 1;
				return true;
			}
			if (position.second == PositionType::DELETED)
			{
				data_[position.first]->key = key;
				data_[position.first]->deleted = false;
				size_ += 1;
				return true;
			}
			return false;
		}

		/**
		 * \brief Удаление элемента из хеш-таблицы.
		 * \param key Вставляемый элемент.
		 * \return True, если удаление завершилось успешно, иначе - false.
		 */
		bool remove(const T& key)
		{
			auto position = probePositionsInto(key, data_);
			assert(position.second != PositionType::INVALID);
			if (position.second == PositionType::BUSY)
			{
				data_[position]->deleted = true;
				size_ -= 1;
				return true;
			}
			return false;
		}

		/**
		 * \brief Проверяет, содержится ли элемент в хеш-таблице.
		 * \param key Проверяемый элемент.
		 * \return True, если элемент содержится в таблице, иначе - false.
		 */
		bool contains(const T& key)
		{
			auto position = probePositionsInto(key, data_);
			assert(position.second != PositionType::INVALID);
			if (position.second == PositionType::BUSY)
			{
				return true;
			}
			return false;
		}

		/**
		 * \brief Проверяет, есть ли элементы в хеш-таблице.
		 * \return True, если таблица пустая, иначе - false.
		 */
		bool isEmpty() const { return size_ == 0; }

	private:
		//Объявления типов данных.
		/**
		 * \brief Перечисление, содержащее возможные исходы, с которыми завершается пробирование.
		 */
		enum PositionType
		{
			FREE,
			BUSY,
			DELETED,
			INVALID
		};
		/**
		 * \brief Структура, реализующая ячейку, хранящуюся в таблице и содержащую данные.
		 */
		struct Node
		{
			explicit Node(const T& value) : key(value) {}
			T key;
			bool deleted{ true };
		};
		/**
		 * \brief Тип, представляющий результат пробирования.
		 */
		using Position=std::pair<size_t, PositionType>;

		//Объявления констант.
		const size_t M = 4294967296ULL; // 2^32
		const float CRITICAL_FULLNESS = 0.75; // 3/4
		const float INITIAL_CAPACITY = 8;

		//Объявления закрытых методов класса.
		/**
		 * \brief Поиск позиции элемента в таблице.
		 * \param key Элемент, для которого ищем позицию.
		 * \param data Буффер хеш-таблицы, в котором подбираем позицию.
		 * \return std::pair, в котором первый элемент - индекс в буффере, второй - исход пробирования типа PositionType.
		 */
		Position probePositionsInto(const T& key, std::vector<Node*>& data)
		{
			auto firstHash = hash(key);
			auto secondHash = hash(key, false);
			auto position = probe(firstHash, secondHash, 0) % data.size();
			for (auto i = 0; i < data.size(); ++i)
			{
				if (data[position] == nullptr)
				{
					return { position, PositionType::FREE };
				}
				if (data[position]->key == key && data[position]->deleted == false)
				{
					return { position, PositionType::BUSY };
				}
				if (data[position]->deleted == true)
				{
					return { position, PositionType::DELETED };
				}
				position = probe(firstHash, secondHash, i + 1) % data.size();
			}
			return { 0, PositionType::INVALID };
		}

		/**
		 * \brief Увеличивает размер хеш-таблицы вдвое.
		 */
		void rehash()
		{
			std::vector<Node*> newData(data_.size() * 2);
			for (auto i = 0; i < data_.size(); ++i)
			{
				if (data_[i] != nullptr && data_[i]->deleted == false)
				{
					auto position = probePositionsInto(data_[i]->key, newData);
					assert(position.second == PositionType::FREE);
					newData[position.first] = data_[i]->key;
				}
			}
			data_ = newData;
		}

		/**
		 * \brief Определяем, заполнена ли таблица до критического уровня.
		 * \return True, если заполненность достигла критического значения, иначе - false.
		 */
		bool isFull() const
		{
			return calculateFullness >= CRITICAL_FULLNESS;
		}

		
		float calculateFullness() const
		{
			return static_cast<float>(size_) / static_cast<float>(capacity_);
		}

		size_t hash(const T & key, bool isMainHash = true) const
		{
			static const size_t a = isMainHash ? 5 : 7;
			auto bytes_string = reinterpret_cast<char*>(key.data());
			size_t hash = 0;
			for (size_t i = 0; i < sizeof(key); ++i)
			{
				hash = (hash * a + *bytes_string++) % M;
			}
			return hash;
		}

		static size_t probe(const size_t firstHash, const size_t secondHash, const size_t index)
		{
			return firstHash + index * secondHash;
		}

		//Объявления полей класса.
		size_t capacity_{ 0 };
		size_t size_{ 0 };
		std::vector<Node*> data_{ INITIAL_CAPACITY, nullptr };
		std::function<size_t(const T&)> hash1_{ std::bind(HashMap::hash(), this, std::placeholders::_1, true) };
		std::function<size_t(const T&)> hash2_{ std::bind(HashMap::hash(), this, std::placeholders::_1, false) };
	};
}

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
