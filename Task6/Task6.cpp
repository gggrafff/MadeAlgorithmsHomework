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
		BinarySearchTree() = default;
		explicit BinarySearchTree(const std::function<bool(const T& lhs, const T& rhs)> less) { less_ = less; }
		~BinarySearchTree();
		BinarySearchTree(const BinarySearchTree& other) = delete;
		BinarySearchTree(BinarySearchTree&& other) noexcept = delete;
		BinarySearchTree& operator=(const BinarySearchTree& other) = delete;
		BinarySearchTree& operator=(BinarySearchTree&& other) noexcept = delete;

		/**
		 * \brief Обход дерева в ширину.
		 * \param action Функция, выполняемая над элементами.
		 */
		void bfs(std::function<void(T&)> action);
		/**
		 * \brief Вставка элемента в дерево.
		 * \param value Вставляемый элемент.
		 */
		void insert(const T& value);

	private:
		/**
		 * \brief Структура, описывающая узел дерева.
		 */
		struct Node {
			/**
			 * \brief Значение элемента, хранящееся в текущем узле.
			 */
			T key_;
			/**
			 * \brief Указатель на левого потомка.
			 */
			Node* left_{ nullptr };
			/**
			 * \brief Указатель на правого потомка.
			 */
			Node* right_{ nullptr };

			explicit Node(const T& data = T()) : key_(data), left_(nullptr), right_(nullptr) {}
			~Node() = default;

			Node(const Node& other) = delete;
			Node(Node&& other) noexcept = delete;
			Node& operator=(const Node& other) = delete;
			Node& operator=(Node&& other) noexcept = delete;
		};

		/**
		 * \brief Обход дерева в ширину.
		 * \param action Функция, выполняемая над узлами.
		 */
		void bfsNodes(std::function<void(Node*)> action);

		Node* root_{ nullptr };
		std::function<bool(const T& lhs, const T& rhs)> less_{ std::less<T>() };
	};

	template <typename T>
	BinarySearchTree<T>::~BinarySearchTree()
	{
		bfsNodes(std::default_delete<Node>());
	}

	template <typename T>
	void BinarySearchTree<T>::bfs(std::function<void(T&)> action)
	{
		bfsNodes([&action](Node* node)
		{
			action(node->key_);
		});
	}

	template <typename T>
	void BinarySearchTree<T>::insert(const T& value)
	{
		if (root_ == nullptr)
		{
			root_ = new Node(value);
			return;
		}
		Node* parent{ nullptr };
		Node* insertPlace = root_;
		while (insertPlace != nullptr)
		{
			parent = insertPlace;
			if (less_(value, insertPlace->key_))
			{
				insertPlace = insertPlace->left_;
			}
			else
			{
				insertPlace = insertPlace->right_;
			}
		}
		insertPlace = new Node(value);
		if (less_(value, parent->key_))
		{
			parent->left_ = insertPlace;
		}
		else
		{
			parent->right_ = insertPlace;
		}
	}

	template <typename T>
	void BinarySearchTree<T>::bfsNodes(std::function<void(Node*)> action)
	{
		std::queue<Node*> elementsForProcessing;
		elementsForProcessing.push(root_);
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
			action(currentElement);
			elementsForProcessing.pop();
		}
	}
}


void processNumbers(const std::vector<int32_t>& numbers)
{
	custom_containers::BinarySearchTree<int32_t> tree;
	for (int number : numbers)
	{
		tree.insert(number);
	}
	tree.bfs([](const int32_t& number) {std::cout << number << " "; });
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

