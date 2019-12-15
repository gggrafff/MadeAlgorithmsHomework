#pragma once
#include <vector>
#include <functional>
#include <optional>
#include <cassert>

namespace custom_containers
{
	/**
	 * \brief Класс, реализующий хеш-таблицу открытой адресации с двойным хешированием.
	 * \tparam T Тип элементов в хеш-таблице. Должен иметь метод data(), возвращающий указатель на данные, и метод size(), возвращающий количество элементов. T должен быть moveable для быстрой работы хеш-таблицы.
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
		~HashMap();

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
		[[nodiscard]] bool contains(const T& key) const;

		/**
		 * \brief Проверяет, есть ли элементы в хеш-таблице.
		 * \return True, если таблица пустая, иначе - false.
		 */
		[[nodiscard]] bool isEmpty() const { return size_ == 0; }

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
			explicit Node(T&& value) : key(std::forward(value)) {}
			

			Node(const Node& other)
				: key(other.key),
				  deleted(other.deleted)
			{
			}

			Node(Node&& other) noexcept
				: key(std::move(other.key)),
				  deleted(other.deleted)
			{
			}

			Node& operator=(const Node& other)
			{
				if (this == &other)
					return *this;
				key = other.key;
				deleted = other.deleted;
				return *this;
			}

			Node& operator=(Node&& other) noexcept
			{
				if (this == &other)
					return *this;
				key = std::move(other.key);
				deleted = other.deleted;
				return *this;
			}

			T key;
			bool deleted{ false };
		};
		/**
		 * \brief Тип, представляющий результат пробирования.
		 */
		using Position = std::pair<size_t, PositionType>;

		//Объявления констант.
		const size_t M = 4294967296ULL; // 2^32
		const float CRITICAL_FULLNESS_CONTAINS = 0.75; // 3/4
		const float CRITICAL_FULLNESS_DELETED = 0.5; // 1/2
		const size_t INITIAL_CAPACITY = 8;

		//Объявления закрытых методов класса.
		/**
		 * \brief Поиск позиции элемента в таблице.
		 * \param key Элемент, для которого ищем позицию.
		 * \param data Буфер хеш-таблицы, в котором подбираем позицию.
		 * \param stoppingCondition Флаги, указывающие критерии остановки. Флаги выставляются с помощью enum StoppingCondition.
		 * \return std::pair, в котором первый элемент - индекс в буфере, второй - исход пробирования типа PositionType.
		 */
		Position probePositionsInto(const T& key, std::vector<std::optional<Node>>& data, uint8_t stoppingCondition = StoppingCondition::STOP_FOR_ALL) const;

		/**
		 * \brief Увеличивает размер хеш-таблицы в указанное количество раз.
		 * \param coefficient Во сколько раз увеличить размер хеш-таблицы.
		 */
		void rehash(float coefficient);

		/**
		 * \brief Определяем, заполнена ли таблица до критического уровня.
		 * \return True, если заполненность достигла критического значения, иначе - false.
		 */
		[[nodiscard]] bool isFull() const;

		/**
		 * \brief Определяем, достигло ли количество удалённых из таблицы элементов критического уровня.
		 * \return True, если количество удалённых из таблицы элементов достигло критического значения, иначе - false.
		 */
		[[nodiscard]] bool isDegraded() const;

		/**
		 * \brief Рассчитать коэффициент заполненности хеш-таблицы.
		 * \return Пара коэффициентов заполненности: существующими элементами, удалёнными элементами.
		 */
		std::pair<float, float> calculateFullness() const;

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
		size_t deleted_number_{ 0 };
		/**
		 * \brief Массив, в котором хранятся элементы таблицы.
		 */
		std::vector<std::optional<Node>> data_{ std::vector<std::optional<Node>>(INITIAL_CAPACITY) };

		std::function<size_t(const T&)> hash1_{ std::bind(&HashMap::hash, this, std::placeholders::_1, true) };
		std::function<size_t(const T&)> hash2_{ std::bind(&HashMap::hash, this, std::placeholders::_1, false) };
	};

	template <typename T>
	HashMap<T>::~HashMap()
	{
		for (auto nodePtr : data_)
		{
			delete nodePtr;
		}
	}

	template <typename T>
	bool HashMap<T>::insert(const T& key)
	{
		if (contains(key))
		{
			return false;
		}
		if (isFull())
		{
			rehash(2);
		}
		else if (isDegraded())
		{
			rehash(1);
		}
		auto position = probePositionsInto(key, data_);
		assert(position.second != PositionType::INVALID);
		if (position.second == PositionType::FREE)
		{
			data_[position.first].emplace(key);
			size_ += 1;
			return true;
		}
		if (position.second == PositionType::DELETED)
		{
			data_[position.first]->key = key;
			data_[position.first]->deleted = false;
			size_ += 1;
			deleted_number_ -= 1;
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
			deleted_number_ += 1;
			return true;
		}
		return false;
	}

	template <typename T>
	bool HashMap<T>::contains(const T& key) const
	{
		auto position = probePositionsInto(key, data_, StoppingCondition::STOP_FOR_SEARCH);
		return static_cast<bool>(position.second == PositionType::BUSY);
	}

	template <typename T>
	typename HashMap<T>::Position HashMap<T>::probePositionsInto(const T& key, std::vector<std::optional<Node>>& data, uint8_t stoppingCondition) const
	{
		auto firstHash = hash1_(key);
		auto secondHash = hash2_(key);
		auto position = probe(firstHash, secondHash, 0) % data.size();
		for (size_t i = 0; i < data.size(); ++i)
		{
			if (!data[position] && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_FREE))
			{
				return { position, PositionType::FREE };
			}
			if (data[position].value().key == key && !data[position].value().deleted && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_BUSY))
			{
				return { position, PositionType::BUSY };
			}
			if (data[position].value().deleted && static_cast<bool>(stoppingCondition & StoppingCondition::STOP_FOR_DELETED))
			{
				return { position, PositionType::DELETED };
			}
			position = probe(firstHash, secondHash, i + 1) % data.size();
		}
		return { 0, PositionType::INVALID };
	}

	template <typename T>
	void HashMap<T>::rehash(float coefficient)
	{
		std::vector<std::optional<Node>> newData(data_.size() * coefficient);
		for (size_t i = 0; i < data_.size(); ++i)
		{
			if (data_[i] && !(data_[i].value().deleted))
			{
				auto position = probePositionsInto(data_[i].value().key, newData);
				assert(position.second == PositionType::FREE);
				newData[position.first].emplace(std::move(data_[i].value().key));
			}
		}
		data_ = newData;
	}

	template <typename T>
	bool HashMap<T>::isFull() const
	{
		const auto fullness = calculateFullness();
		return fullness.first >= CRITICAL_FULLNESS_CONTAINS;
	}

	template <typename T>
	bool HashMap<T>::isDegraded() const
	{
		const auto fullness = calculateFullness();
		return fullness.second >= CRITICAL_FULLNESS_DELETED;
	}

	template <typename T>
	std::pair<float, float> HashMap<T>::calculateFullness() const
	{
		const auto contains_number = static_cast<float>(size_) / static_cast<float>(data_.size());
		const auto deleted_number = static_cast<float>(deleted_number) / static_cast<float>(data_.size());
		return {contains_number, deleted_number};
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