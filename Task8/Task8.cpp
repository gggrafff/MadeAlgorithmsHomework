#include <iostream>
#include <vector>
#include <functional>

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
		 * \param key 
		 * \return 
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
		 * \return True, если элемент содержится в дереве, иначе - false.
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
		enum PositionType
		{
			FREE,
			BUSY,
			DELETED,
			INVALID
		};

		struct Node
		{
			explicit Node(const T& value) : key(value) {}
			T key;
			bool deleted{ true };
		};

		using Position=std::pair<size_t, PositionType>;

		//Объявления констант.
		const size_t M = 4294967296ULL; // 2^32
		const float CRITICAL_FULLNESS = 0.75; // 3/4
		const float INITIAL_CAPACITY = 8;

		//Объявления закрытых методов класса.
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


int main()
{
    std::cout << "Hello World!\n"; 
}
