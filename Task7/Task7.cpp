#include <iostream>
#include <vector>
#include <fstream>

namespace custom_containers
{
	template<typename T>
	class SplayTree
	{
	public:
		SplayTree() { root_ = nullptr; }
		~SplayTree() { delete root_; }

		SplayTree(const SplayTree& other) = delete;
		SplayTree(SplayTree&& other) noexcept = delete;
		SplayTree& operator=(const SplayTree& other) = delete;
		SplayTree& operator=(SplayTree&& other) noexcept = delete;

		/**
		 * \brief Вставка элемента в дерево. 
		 * \param key Вставляемый элемент. 
		 */
		void insert(const T & key) {
			//Если дерево пустое, то вставляем элемент в корень. 
			if (root_==nullptr)
			{
				root_ = new Node(key);
				return;
			}

			//Ищем место вставки.
			Node* parent = nullptr;
			Node* insertPlace = root_;
			while (insertPlace != nullptr) {
				parent = insertPlace;
				if (parent->data < key)
					insertPlace = parent->rightChild;
				else
					insertPlace = parent->leftChild;
			}

			//Создаём элемент и связываем создаем взаимные ссылки с родителем.
			Node* insertElement = new Node(key);
			insertElement->parent = parent;
			if (insertElement->data < parent->data)
				parent->left = insertElement;
			else
				parent->right = insertElement;

			//Перемещаем вершину в корень.
			splay(insertElement);
		}

		/**
		 * \brief Удаление значения из дерева.
		 * \param key Удаляемое значение.
		 */
		void remove(const T & key) {
			Node * removeElement = searchNode(key); //Находим элемент в дереве

			if (removeElement != nullptr) {
				//Если один из потомков удаляемого узла отсутствует, то удалить легко - просто соединяем родителя и потомка удаляемого узла.
				if (removeElement->right == nullptr)
					transplant(removeElement, removeElement->left);
				else if (removeElement->left == nullptr)
					transplant(removeElement, removeElement->right);
				else 
				{
					//Если существуют оба потомка у удаляемого элемента, 
					//то находим следующий по величине элемент в правом поддереве и помещаем на место удаляемого.
					Node * newLocalRoot = minimumNode(removeElement->right);

					if (newLocalRoot->parent != removeElement) {

						transplant(newLocalRoot, newLocalRoot->right);

						newLocalRoot->right = removeElement->right;
						newLocalRoot->right->parent = newLocalRoot;
					}

					transplant(removeElement, newLocalRoot);

					newLocalRoot->left = removeElement->left;
					newLocalRoot->left->parent = newLocalRoot;

					splay(newLocalRoot);
				}

				delete removeElement;
			}
		}

		/**
		 * \brief Проверяет, содержится ли элемент в дереве. 
		 * \param key Проверяемый элемент.
		 * \return True, если элемент содержится в дереве, иначе - false.
		 */
		bool contains(const T &key) { return searchNode(key) != nullptr; }

		/**
		 * \brief Проверяет, есть ли элементы в дереве.
		 * \return True, если дерево пустое, иначе - false. 
		 */
		bool isEmpty() const { return root_ == nullptr; }

		/**
		 * \brief Поиск следующего (по величине в порядке возрастания) значения в дереве.
		 * \param key Значение, для которого ищем следующее. 
		 * \return Следующее значение.
		 */
		T successor(const T & key) {
			if (successorNode(searchNode(key)) != nullptr) {
				return successorNode(searchNode(key))->data;
			}
			else {
				return -1;
			}
		}

		/**
		 * \brief Поиск предыдущего (по величине в порядке возрастания) значения в дереве.
		 * \param key Значение, для которого ищем предыдущее.
		 * \return Предыдущее значение.
		 */
		T predecessor(const T & key) {
			if (predecessorNode(searchNode(key)) != nullptr) {
				return predecessorNode(searchNode(key))->getValue();
			}
			else {
				return -1;
			}
		}

	private:
		//Объявления типов данных.

		/**
		 * \brief Структура, описывающая узел дерева.
		 */
		struct Node
		{
			/**
			 * \brief Указатель на левого потомка.
			 */
			Node* left;
			/**
			 * \brief Указатель на правого потомка. 
			 */
			Node* right;
			/**
			 * \brief Указатель на родителя.
			 */
			Node* parent;
			/**
			 * \brief Данные, хранящиеся в узле.
			 */
			T data;

			explicit Node(const T& data = T()) :left(nullptr), right(nullptr), parent(nullptr), data(data) {}
			~Node()
			{
				delete left;
				delete right;
			}

			Node(const Node& other) = delete;
			Node(Node&& other) noexcept = delete;
			Node& operator=(const Node& other) = delete;
			Node& operator=(Node&& other) noexcept = delete;
		};

		//Объявления полей класса.
	
		/**
		 * \brief Указатель на корень дерева.
		 */
		Node* root_{ nullptr };

		//Объявления закрытых методов класса.
	
		/**
		 * \brief Получить узел, следующий (по величине в порядке возрастания) за указанным.
		 * \param currentNode Указатель на узел, для которого ищем следующий.
		 * \return Указатель на следующий узел.
		 */
		Node* successorNode(Node* currentNode) const
		{
			Node* successor = currentNode;
			if (successor->right != nullptr)
			{
				successor = minimumNode(successor->right);
			}
			else
			{
				while (successor != root_ || successor != successor->parent->left)
				{
					successor = successor->parent;
				}
			}
			return successor;
		}

		/**
		 * \brief Получить узел, предшествующий (по величине в порядке возрастания) указанному.
		 * \param currentNode Указатель на узел, для которого ищем предшествующий.
		 * \return Указатель на предшествующий узел.
		 */
		Node* predecessorNode(Node* currentNode) const
		{
			Node* predecessor = currentNode;
			if (predecessor->left != nullptr)
			{
				predecessor = maximumNode(predecessor->left);
			}
			else
			{
				while (predecessor != root_ || predecessor != predecessor->parent->right)
				{
					predecessor = predecessor->parent;
				}
			}
			return predecessor;
		}

		/**
		 * \brief Поиск в поддереве узла с минимальным значением. 
		 * \param localRoot Указатель на корень поддерева.
		 * \return Указатель на узел с минимальным значением.
		 */
		Node* minimumNode(Node* localRoot) const
		{
			Node* minimum = localRoot;
			while (minimum->left != nullptr)
			{
				minimum = minimum->left;
			}
			return minimum;
		}

		/**
		 * \brief Поиск в поддереве узла с максимальным значением.
		 * \param localRoot Указатель на корень поддерева.
		 * \return Указатель на узел с максимальным значением.
		 */
		Node* maximumNode(Node* localRoot) const
		{
			Node* maximum = localRoot;
			while (maximum->right != nullptr)
			{
				maximum = maximum->right;
			}
			return maximum;
		}

		/**
		 * \brief Поиск в дереве узла с указанным значением.
		 * \param value Искомое значение.
		 * \return Указатель на узел с искомым значением.
		 */
		Node* searchNode(const T& value) const
		{
			Node* searchedElement = root_;
			while (searchedElement != nullptr)
			{
				if (searchedElement->data < value)
				{
					searchedElement = searchedElement->right;
				}
				else if (value < searchedElement->data)
				{
					searchedElement = searchedElement->left;
				}
				else
				{
					splay(searchedElement);
					return searchedElement;
				}
			}
			return nullptr;
		}

		/**
		 * \brief Осуществляет левый поворот.
		 * \param localRoot Корень поддерева, которое поворачиваем.
		 */
		void leftRotate(Node* localRoot)
		{
			Node* rightChild = localRoot->right;
			localRoot->right = rightChild->left;
			if (rightChild->left != nullptr)
			{
				rightChild->left->parent = localRoot;
			}
			transplant(localRoot, rightChild);
			rightChild->left = localRoot;
			rightChild->left->parent = rightChild;
		}

		/**
		 * \brief Осуществляет правый поворот.
		 * \param localRoot Корень поддерева, которое поворачиваем.
		 */
		void rightRotate(Node* localRoot)
		{
			Node* leftChild = localRoot->left;
			localRoot->left = leftChild->right;
			if (leftChild->right != nullptr)
			{
				leftChild->right->parent = localRoot;
			}
			transplant(localRoot, leftChild);
			leftChild->right = localRoot;
			leftChild->right->parent = leftChild;
		}

		/**
		 * \brief Заменить один узел другим.
		 * \param oldNode Указатель на узел, который нужно заменить. 
		 * \param newNode Указатель на новый узел. Может быть nullptr (старый узел будет убран из дерева).
		 */
		void transplant(Node* oldNode, Node* newNode)
		{
			if (oldNode->parent == nullptr)
			{
				//Старый узел - корень. 
				root_ = newNode;
			}
			else if (oldNode == oldNode->parent->left)
			{
				//Старый узел - левый потомок.
				oldNode->parent->left = newNode;
			}
			else if (oldNode == oldNode->parent->right)
			{
				//Старый узел - правый потомок.
				oldNode->parent->right = newNode;
			}
			if (newNode != nullptr)
			{
				newNode->parent = oldNode->parent;
			}
		}

		/**
		 * \brief Помещает узел в корень дерева.
		 * \param pivotElement Указатель на узел, который нужно поместить в корень.
		 */
		void splay(Node* pivotElement)
		{
			while (pivotElement != root_)
			{
				if (pivotElement->parent == root_)
				{
					//Zig: выполняется, когда pivotElement->parent является корнем. Дерево поворачивается по ребру между pivotElement и pivotElement->parent. 
					//Существует лишь для разбора крайнего случая и выполняется только один раз в конце, когда изначальная глубина pivotElement была нечётна.
					if (pivotElement == pivotElement->parent->left)
					{
						rightRotate(pivotElement);
					}
					else if (pivotElement == pivotElement->parent->right)
					{
						leftRotate(pivotElement);
					}
				}
				else
				{
					//Zig-Zig: выполняется, когда и pivotElement, и pivotElement->parent являются левыми (или правыми) сыновьями. 
					//Дерево поворачивается по ребру между pivotElement->parent->parent и pivotElement->parent, а потом — по ребру между pivotElement->parent и pivotElement.
					if (pivotElement == pivotElement->parent->left &&
						pivotElement->parent == pivotElement->parent->parent->left) {

						rightRotate(pivotElement->parent->parent);
						rightRotate(pivotElement->parent);

					}
					else if (pivotElement == pivotElement->parent->right &&
						pivotElement->parent == pivotElement->parent->parent->right) {

						leftRotate(pivotElement->parent->parent);
						leftRotate(pivotElement->parent);
					}
					//Zig-Zag: выполняется, когда pivotElement является правым сыном, а pivotElement->parent — левым (или наоборот). 
					//Дерево поворачивается по ребру между pivotElement->parent и pivotElement, а затем — по ребру между pivotElement и pivotElement->parent->parent.
					else if (pivotElement == pivotElement->parent->right &&
						pivotElement->parent == pivotElement->parent->parent->left) {

						leftRotate(pivotElement->parent);
						rightRotate(pivotElement->parent);

					}
					else if (pivotElement == pivotElement->parent->left &&
						pivotElement->parent == pivotElement->parent->parent->right) {

						rightRotate(pivotElement->parent);
						leftRotate(pivotElement->parent);
					}
				}
			}
		}
	};
}

void processNumbers(const std::vector<std::pair<int32_t, size_t>>& commands)
{
	custom_containers::SplayTree<uint32_t> tree;
	for(auto& command: commands)
	{
		
	}
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
	size_t n{ 0 };
	in >> n;
	std::vector<std::pair<int32_t, size_t>> commands;
	for (size_t i = 0; i < n; ++i)
	{
		uint32_t number{ 0 }, k{ 0 };
		in >> number >> k;
		commands.emplace_back(number, k);
	}
	in.close();
	processNumbers(commands);
	return 0;
}

