#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>

namespace custom_containers
{
	/**
	 * \brief Класс, реализующий хеш-таблицу открытой адресации с двойным хешированием.
	 * \tparam T Тип элементов в хеш-таблице. Должен иметь метод data(), возвращающий указатель на данные, и метод size(), возвращающий количество элементов.
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
		HashMap(const std::function<size_t(const T&)>& hash1, const std::function<size_t(const T&)>& hash2) : hash1_(hash1), hash2_(hash2) {}
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
		bool insert(const T& key);

		/**
		 * \brief Удаление элемента из хеш-таблицы.
		 * \param key Вставляемый элемент.
		 * \return True, если удаление завершилось успешно, иначе - false.
		 */
		bool remove(const T& key);

		/**
		 * \brief Проверяет, содержится ли элемент в хеш-таблице.
		 * \param key Проверяемый элемент.
		 * \return True, если элемент содержится в таблице, иначе - false.
		 */
		bool contains(const T& key);

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
		 * \brief Перечисление, содержащее возможные исходы, с которыми завершается пробирование.
		 */
		enum StoppingCondition
		{
			STOP_FOR_FREE = 1,
			STOP_FOR_BUSY = 2,
			STOP_FOR_DELETED = 4,
			STOP_FOR_SEARCH = 3,
			STOP_FOR_INSERT = 5,
			STOP_FOR_ALL = 7,
		};
		/**
		 * \brief Структура, реализующая ячейку, хранящуюся в таблице и содержащую данные.
		 */
		struct Node
		{
			explicit Node(const T& value) : key(value) {}
			T key;
			bool deleted{ false };
		};
		/**
		 * \brief Тип, представляющий результат пробирования.
		 */
		using Position = std::pair<size_t, PositionType>;

		//Объявления констант.
		const size_t M = 4294967296ULL; // 2^32
		const float CRITICAL_FULLNESS = 0.75; // 3/4
		const size_t INITIAL_CAPACITY = 8;

		//Объявления закрытых методов класса.
		/**
		 * \brief Поиск позиции элемента в таблице.
		 * \param key Элемент, для которого ищем позицию.
		 * \param data Буфер хеш-таблицы, в котором подбираем позицию.
		 * \param stoppingCondition Флаги, указывающие критерии остановки. Флаги выставляются с помощью enum StoppingCondition.
		 * \return std::pair, в котором первый элемент - индекс в буфере, второй - исход пробирования типа PositionType.
		 */
		Position probePositionsInto(const T& key, std::vector<Node*>& data, uint8_t stoppingCondition = StoppingCondition::STOP_FOR_ALL);

		/**
		 * \brief Увеличивает размер хеш-таблицы вдвое.
		 */
		void rehash();

		/**
		 * \brief Определяем, заполнена ли таблица до критического уровня.
		 * \return True, если заполненность достигла критического значения, иначе - false.
		 */
		bool isFull() const { return calculateFullness() >= CRITICAL_FULLNESS; }

		/**
		 * \brief Рассчитать коэффициент заполненности хеш-таблицы.
		 * \return Коэффициент заполненности.
		 */
		float calculateFullness() const;

		/**
		 * \brief
		 * \param key Хеш-функция по методу Горнера.
		 * \param isMainHash Если true, то коэффициент a=5, иначе a=7.
		 * \return Значение хеша.
		 */
		size_t hash(const T& key, bool isMainHash = true) const;

		/**
		 * \brief Один шаг пробирования с помощью двойного хеширования.
		 * \param firstHash Значение первой хеш-функции.
		 * \param secondHash Значение второй хеш-функции.
		 * \param index Шаг пробирования.
		 * \return Результат пробирования (необходимо взять от него %size_of_table, чтобы получить позицию).
		 */
		static size_t probe(const size_t firstHash, const size_t secondHash, const size_t index);

		//Объявления полей класса.
		/**
		 * \brief Количество элементов в таблице.
		 */
		size_t size_{ 0 };
		/**
		 * \brief Массив, в котором хранятся элементы таблицы.
		 */
		std::vector<Node*> data_{ std::vector<Node*>(INITIAL_CAPACITY, nullptr) };

		std::function<size_t(const T&)> hash1_{ std::bind(&HashMap::hash, this, std::placeholders::_1, true) };
		std::function<size_t(const T&)> hash2_{ std::bind(&HashMap::hash, this, std::placeholders::_1, false) };
	};

	template <typename T>
	bool HashMap<T>::insert(const T& key)
	{
		if (contains(key))
		{
			return false;
		}
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

	template <typename T>
	bool HashMap<T>::remove(const T& key)
	{
		auto position = probePositionsInto(key, data_, StoppingCondition::STOP_FOR_SEARCH);
		if (position.second == PositionType::BUSY)
		{
			data_[position.first]->deleted = true;
			size_ -= 1;
			return true;
		}
		return false;
	}

	template <typename T>
	bool HashMap<T>::contains(const T& key)
	{
		auto position = probePositionsInto(key, data_, StoppingCondition::STOP_FOR_SEARCH);
		return static_cast<bool>(position.second == PositionType::BUSY);
	}

	template <typename T>
	typename HashMap<T>::Position HashMap<T>::probePositionsInto(const T& key, std::vector<Node*>& data, uint8_t stoppingCondition)
	{
		auto firstHash = hash1_(key);
		auto secondHash = hash2_(key);
		auto position = probe(firstHash, secondHash, 0) % data.size();
		for (size_t i = 0; i < data.size(); ++i)
		{
			if (data[position] == nullptr && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_FREE))
			{
				return { position, PositionType::FREE };
			}
			if (data[position]->key == key && !data[position]->deleted && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_BUSY))
			{
				return { position, PositionType::BUSY };
			}
			if (data[position]->deleted && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_DELETED))
			{
				return { position, PositionType::DELETED };
			}
			position = probe(firstHash, secondHash, i + 1) % data.size();
		}
		return { 0, PositionType::INVALID };
	}

	template <typename T>
	void HashMap<T>::rehash()
	{
		std::vector<Node*> newData(data_.size() * 2, nullptr);
		for (size_t i = 0; i < data_.size(); ++i)
		{
			if (data_[i] != nullptr && !static_cast<bool>(data_[i]->deleted))
			{
				auto position = probePositionsInto(data_[i]->key, newData);
				assert(position.second == PositionType::FREE);
				newData[position.first] = new Node(data_[i]->key);
			}
		}
		data_ = newData;
	}

	template <typename T>
	float HashMap<T>::calculateFullness() const
	{
		return static_cast<float>(size_) / static_cast<float>(data_.size());
	}

	template <typename T>
	size_t HashMap<T>::hash(const T& key, bool isMainHash) const
	{
		const auto a = isMainHash ? 5 : 7;
		auto bytes_string = reinterpret_cast<const char*>(key.data());
		size_t hash = 0;
		for (size_t i = 0; i < key.size(); ++i)
		{
			hash = (hash * a + *bytes_string++) % M;
		}
		if (!isMainHash && hash % 2 == 0)
		{
			hash -= 1;
		}
		return hash;
	}

	template <typename T>
	size_t HashMap<T>::probe(const size_t firstHash, const size_t secondHash, const size_t index)
	{
		return firstHash + index * secondHash;
	}
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
