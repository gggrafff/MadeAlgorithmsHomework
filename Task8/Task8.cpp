#include <iostream>
#include <vector>
#include <functional>
#include <fstream>

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
		 * \param data Буфер хеш-таблицы, в котором подбираем позицию.
		 * \return std::pair, в котором первый элемент - индекс в буфере, второй - исход пробирования типа PositionType.
		 */
		Position probePositionsInto(const T& key, std::vector<Node*>& data);

		/**
		 * \brief Увеличивает размер хеш-таблицы вдвое.
		 */
		void rehash();

		/**
		 * \brief Определяем, заполнена ли таблица до критического уровня.
		 * \return True, если заполненность достигла критического значения, иначе - false.
		 */
		bool isFull() const { return calculateFullness >= CRITICAL_FULLNESS; }

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
		 * \brief Емкость таблицы.
		 */
		size_t capacity_{ 0 };
		/**
		 * \brief Количество элементов в таблице.
		 */
		size_t size_{ 0 };

		std::vector<Node*> data_{ INITIAL_CAPACITY, nullptr };
		std::function<size_t(const T&)> hash1_{ std::bind(HashMap::hash(), this, std::placeholders::_1, true) };
		std::function<size_t(const T&)> hash2_{ std::bind(HashMap::hash(), this, std::placeholders::_1, false) };
	};

	template <typename T>
	bool HashMap<T>::insert(const T& key)
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

	template <typename T>
	bool HashMap<T>::remove(const T& key)
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

	template <typename T>
	bool HashMap<T>::contains(const T& key)
	{
		auto position = probePositionsInto(key, data_);
		assert(position.second != PositionType::INVALID);
		if (position.second == PositionType::BUSY)
		{
			return true;
		}
		return false;
	}

	template <typename T>
	typename HashMap<T>::Position HashMap<T>::probePositionsInto(const T& key, std::vector<Node*>& data)
	{
		auto firstHash = hash(key);
		auto secondHash = hash(key, false);
		auto position = probe(firstHash, secondHash, 0) % data.size();
		for (auto i = 0; i < data.size(); ++i)
		{
			if (data[position] == nullptr)
			{
				return {position, PositionType::FREE};
			}
			if (data[position]->key == key && data[position]->deleted == false)
			{
				return {position, PositionType::BUSY};
			}
			if (data[position]->deleted == true)
			{
				return {position, PositionType::DELETED};
			}
			position = probe(firstHash, secondHash, i + 1) % data.size();
		}
		return {0, PositionType::INVALID};
	}

	template <typename T>
	void HashMap<T>::rehash()
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

	template <typename T>
	float HashMap<T>::calculateFullness() const
	{
		return static_cast<float>(size_) / static_cast<float>(capacity_);
	}

	template <typename T>
	size_t HashMap<T>::hash(const T& key, bool isMainHash) const
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

	template <typename T>
	size_t HashMap<T>::probe(const size_t firstHash, const size_t secondHash, const size_t index)
	{
		return firstHash + index * secondHash;
	}
}

void processCommands(const std::vector<std::pair<char, std::string>>& commands)
{
	
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
	std::vector<std::pair<char, std::string>> commands;
	/*while(!in.eof())
	{
		char operation{ '+' };
		std::string key;
		in >> operation;
		in.;
	}*/
	
}
