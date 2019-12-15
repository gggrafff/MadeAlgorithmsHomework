#pragma once
#include <vector>
#include <functional>
#include <optional>
#include <cassert>

namespace custom_containers
{
	/**
	 * \brief �����, ����������� ���-������� �������� ��������� � ������� ������������.
	 * \tparam T ��� ��������� � ���-�������. ������ ����� ����� data(), ������������ ��������� �� ������, � ����� size(), ������������ ���������� ���������. T ������ ���� moveable ��� ������� ������ ���-�������.
	 */
	template<typename T>
	class HashMap
	{
	public:
		/**
			 * \brief ����������� �� ���������.
			 */
		HashMap() = default;
		/**
		 * \brief ����������� � ������������ ������ ���������������� ���-�������.
		 * \param hash1 ������ �������� ���-�������
		 * \param hash2 ������ ���-������� ��� ���������� ��������
		 */
		HashMap(const std::function<size_t(const T&)>& hash1, const std::function<size_t(const T&)>& hash2) : hash1_(hash1), hash2_(hash2) {}
		~HashMap();

		HashMap(const HashMap& other) = delete;
		HashMap(HashMap&& other) noexcept = delete;
		HashMap& operator=(const HashMap& other) = delete;
		HashMap& operator=(HashMap&& other) noexcept = delete;

		/**
		 * \brief ������� �������� � ���-�������.
		 * \param key ����������� �������.
		 * \return True, ���� ������� ����������� �������, ����� - false.
		 */
		bool insert(const T& key);

		/**
		 * \brief �������� �������� �� ���-�������.
		 * \param key ����������� �������.
		 * \return True, ���� �������� ����������� �������, ����� - false.
		 */
		bool remove(const T& key);

		/**
		 * \brief ���������, ���������� �� ������� � ���-�������.
		 * \param key ����������� �������.
		 * \return True, ���� ������� ���������� � �������, ����� - false.
		 */
		[[nodiscard]] bool contains(const T& key) const;

		/**
		 * \brief ���������, ���� �� �������� � ���-�������.
		 * \return True, ���� ������� ������, ����� - false.
		 */
		[[nodiscard]] bool isEmpty() const { return size_ == 0; }

	private:
		//���������� ����� ������.
		/**
		 * \brief ������������, ���������� ��������� ������, � �������� ����������� ������������.
		 */
		enum PositionType
		{
			FREE,
			BUSY,
			DELETED,
			INVALID
		};
		/**
		 * \brief ������������, ���������� ��������� ������, � �������� ����������� ������������.
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
		 * \brief ���������, ����������� ������, ���������� � ������� � ���������� ������.
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
		 * \brief ���, �������������� ��������� ������������.
		 */
		using Position = std::pair<size_t, PositionType>;

		//���������� ��������.
		const size_t M = 4294967296ULL; // 2^32
		const float CRITICAL_FULLNESS_CONTAINS = 0.75; // 3/4
		const float CRITICAL_FULLNESS_DELETED = 0.5; // 1/2
		const size_t INITIAL_CAPACITY = 8;

		//���������� �������� ������� ������.
		/**
		 * \brief ����� ������� �������� � �������.
		 * \param key �������, ��� �������� ���� �������.
		 * \param data ����� ���-�������, � ������� ��������� �������.
		 * \param stoppingCondition �����, ����������� �������� ���������. ����� ������������ � ������� enum StoppingCondition.
		 * \return std::pair, � ������� ������ ������� - ������ � ������, ������ - ����� ������������ ���� PositionType.
		 */
		Position probePositionsInto(const T& key, std::vector<std::optional<Node>>& data, uint8_t stoppingCondition = StoppingCondition::STOP_FOR_ALL) const;

		/**
		 * \brief ����������� ������ ���-������� � ��������� ���������� ���.
		 * \param coefficient �� ������� ��� ��������� ������ ���-�������.
		 */
		void rehash(float coefficient);

		/**
		 * \brief ����������, ��������� �� ������� �� ������������ ������.
		 * \return True, ���� ������������� �������� ������������ ��������, ����� - false.
		 */
		[[nodiscard]] bool isFull() const;

		/**
		 * \brief ����������, �������� �� ���������� �������� �� ������� ��������� ������������ ������.
		 * \return True, ���� ���������� �������� �� ������� ��������� �������� ������������ ��������, ����� - false.
		 */
		[[nodiscard]] bool isDegraded() const;

		/**
		 * \brief ���������� ����������� ������������� ���-�������.
		 * \return ���� ������������� �������������: ������������� ����������, ��������� ����������.
		 */
		std::pair<float, float> calculateFullness() const;

		/**
		 * \brief
		 * \param key ���-������� �� ������ �������.
		 * \param isMainHash ���� true, �� ����������� a=5, ����� a=7.
		 * \return �������� ����.
		 */
		size_t hash(const T& key, bool isMainHash = true) const;

		/**
		 * \brief ���� ��� ������������ � ������� �������� �����������.
		 * \param firstHash �������� ������ ���-�������.
		 * \param secondHash �������� ������ ���-�������.
		 * \param index ��� ������������.
		 * \return ��������� ������������ (���������� ����� �� ���� %size_of_table, ����� �������� �������).
		 */
		static size_t probe(const size_t firstHash, const size_t secondHash, const size_t index);

		//���������� ����� ������.
		/**
		 * \brief ���������� ��������� � �������.
		 */
		size_t size_{ 0 };
		size_t deleted_number_{ 0 };
		/**
		 * \brief ������, � ������� �������� �������� �������.
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