/*
 * 6_4. Обход дерева в порядке level-order
 * 
 * Ограничение времени	0.4 секунды
 * Ограничение памяти	64Mb
 * 
 * Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.
 * Требуется построить бинарное дерево поиска, заданное наивным порядком вставки. 
 * Т.е., при добавлении очередного числа K в дерево с корнем root, 
 * если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
 * Выведите элементы в порядке level-order (по слоям, “в ширину”).
 *
 * Формат ввода
 * В первой строке записано число N.
 * Во последующих N строках — целые числа исходной последовательности.
 *
 * Формат вывода
 * N целых чисел, разделенных пробелом.
 * 
 * Контрольные примеры:
 * In:
 * 10
 * 10
 * 5
 * 4
 * 7
 * 9
 * 8
 * 6
 * 3
 * 2
 * 1
 * Out:
 * 10 5 4 7 3 6 9 2 8 1 
 * 
 * In:
 * 10
 * 9
 * 8
 * 6
 * 10
 * 4
 * 3
 * 2
 * 5
 * 1
 * 7
 * Out:
 * 9 8 10 6 4 7 3 5 2 1 
 * 
 * In:
 * 10
 * 1
 * 7
 * 10
 * 9
 * 4
 * 3
 * 8
 * 5
 * 2
 * 6
 * Out:
 * 1 7 4 10 3 5 9 2 6 8 
 *
 */

/*
 * Сложность:
 * 					В среднем		В худшем случае (у дерева одна ветвь)
 * 	Расход памяти	O(n)			O(n)
 * 	Поиск			O(log n)		O(n)
 * 	Вставка			O(log n)		O(n)
 * 	Удаление		O(log n)		O(n)
 */

#include <iostream>
#include <functional>
#include <queue>

namespace custom_containers
{

	/**
	 * \brief Класс, реализующий двоичное дерево поиска в целом и узел дерева в частности.
	 * \tparam T Тип элементов, хранящихся в дереве. 
	 */
	template<typename T>
	class BinarySearchTree
	{
	public:
		explicit BinarySearchTree(T root);
		~BinarySearchTree();
		BinarySearchTree(const BinarySearchTree& other) = delete;
		BinarySearchTree(BinarySearchTree&& other) noexcept = delete;
		BinarySearchTree& operator=(const BinarySearchTree& other) = delete;
		BinarySearchTree& operator=(BinarySearchTree&& other) noexcept = delete;

		/**
		 * \brief Обход дерева в ширину.
		 * \param action Функция, выполняемая над элементами.
		 */
		void bfs(std::function<void(const T&)> action);
		/**
		 * \brief Вставка элемента в дерево.
		 * \param value Вставляемый элемент.
		 */
		void insert(const T& value);

	private:
		/**
		 * \brief Значение элемента, хранящееся в текущем узле. 
		 */
		T key_;
		/**
		 * \brief Указатель на левого потомка.
		 */
		BinarySearchTree* left_{ nullptr };
		/**
		 * \brief Указатель на правого потомка. 
		 */
		BinarySearchTree* right_{ nullptr };
	};

	template <typename T>
	BinarySearchTree<T>::BinarySearchTree(T root)
	{
		key_ = root;
	}

	template <typename T>
	BinarySearchTree<T>::~BinarySearchTree()
	{
		delete left_;
		delete right_;
	}

	template <typename T>
	void BinarySearchTree<T>::bfs(std::function<void(const T&)> action)
	{
		std::queue<BinarySearchTree*> elementsForProcessing;
		elementsForProcessing.push(this);
		while (!elementsForProcessing.empty())
		{
			auto currentElement = elementsForProcessing.front();
			if (currentElement->left_ != nullptr)
			{
				elementsForProcessing.push(currentElement->left_);
			}
			if (currentElement->right_ != nullptr)
			{
				elementsForProcessing.push(currentElement->right_);
			}
			action(currentElement->key_);
			elementsForProcessing.pop();
		}
	}

	template <typename T>
	void BinarySearchTree<T>::insert(const T& value)
	{
		if (value < key_)
		{
			if (left_ == nullptr)
			{
				left_ = new BinarySearchTree(value);
			}
			else
			{
				left_->insert(value);
			}
		}
		else
		{
			if (right_ == nullptr)
			{
				right_ = new BinarySearchTree(value);
			}
			else
			{
				right_->insert(value);
			}
		}
	}
}


void processNumbers(const std::vector<int32_t>& numbers)
{
	custom_containers::BinarySearchTree<int32_t> tree(numbers.front());
	for (size_t i = 1; i < numbers.size(); ++i)
	{
		tree.insert(numbers[i]);
	}
	tree.bfs([](const int32_t & number) {std::cout << number << " "; });
}


int main()
{
	size_t n{ 0 };
	std::cin >> n;
	std::vector<int32_t> numbers;
	for (size_t i = 0; i < n; ++i)
	{
		auto number{ 0 };
		std::cin >> number;
		numbers.push_back(number);
	}
	processNumbers(numbers);
}

