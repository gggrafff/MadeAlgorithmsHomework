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
		 * \brief ������� �������� � ������. 
		 * \param key ����������� �������. 
		 */
		void insert(const T & key) {
			//���� ������ ������, �� ��������� ������� � ������. 
			if (root_==nullptr)
			{
				root_ = new Node(key);
				return;
			}

			//���� ����� �������.
			Node* parent = nullptr;
			Node* insertPlace = root_;
			while (insertPlace != nullptr) {
				parent = insertPlace;
				if (parent->data < key)
					insertPlace = parent->rightChild;
				else
					insertPlace = parent->leftChild;
			}

			//������ ������� � ��������� ������� �������� ������ � ���������.
			Node* insertElement = new Node(key);
			insertElement->parent = parent;
			if (insertElement->data < parent->data)
				parent->left = insertElement;
			else
				parent->right = insertElement;

			//���������� ������� � ������.
			splay(insertElement);
		}

		/**
		 * \brief �������� �������� �� ������.
		 * \param key ��������� ��������.
		 */
		void remove(const T & key) {
			Node * removeElement = searchNode(key); //������� ������� � ������

			if (removeElement != nullptr) {
				//���� ���� �� �������� ���������� ���� �����������, �� ������� ����� - ������ ��������� �������� � ������� ���������� ����.
				if (removeElement->right == nullptr)
					transplant(removeElement, removeElement->left);
				else if (removeElement->left == nullptr)
					transplant(removeElement, removeElement->right);
				else 
				{
					//���� ���������� ��� ������� � ���������� ��������, 
					//�� ������� ��������� �� �������� ������� � ������ ��������� � �������� �� ����� ����������.
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
		 * \brief ���������, ���������� �� ������� � ������. 
		 * \param key ����������� �������.
		 * \return True, ���� ������� ���������� � ������, ����� - false.
		 */
		bool contains(const T &key) { return searchNode(key) != nullptr; }

		/**
		 * \brief ���������, ���� �� �������� � ������.
		 * \return True, ���� ������ ������, ����� - false. 
		 */
		bool isEmpty() const { return root_ == nullptr; }

		/**
		 * \brief ����� ���������� (�� �������� � ������� �����������) �������� � ������.
		 * \param key ��������, ��� �������� ���� ���������. 
		 * \return ��������� ��������.
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
		 * \brief ����� ����������� (�� �������� � ������� �����������) �������� � ������.
		 * \param key ��������, ��� �������� ���� ����������.
		 * \return ���������� ��������.
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
		//���������� ����� ������.

		/**
		 * \brief ���������, ����������� ���� ������.
		 */
		struct Node
		{
			/**
			 * \brief ��������� �� ������ �������.
			 */
			Node* left;
			/**
			 * \brief ��������� �� ������� �������. 
			 */
			Node* right;
			/**
			 * \brief ��������� �� ��������.
			 */
			Node* parent;
			/**
			 * \brief ������, ���������� � ����.
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

		//���������� ����� ������.
	
		/**
		 * \brief ��������� �� ������ ������.
		 */
		Node* root_{ nullptr };

		//���������� �������� ������� ������.
	
		/**
		 * \brief �������� ����, ��������� (�� �������� � ������� �����������) �� ���������.
		 * \param currentNode ��������� �� ����, ��� �������� ���� ���������.
		 * \return ��������� �� ��������� ����.
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
		 * \brief �������� ����, �������������� (�� �������� � ������� �����������) ����������.
		 * \param currentNode ��������� �� ����, ��� �������� ���� ��������������.
		 * \return ��������� �� �������������� ����.
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
		 * \brief ����� � ��������� ���� � ����������� ���������. 
		 * \param localRoot ��������� �� ������ ���������.
		 * \return ��������� �� ���� � ����������� ���������.
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
		 * \brief ����� � ��������� ���� � ������������ ���������.
		 * \param localRoot ��������� �� ������ ���������.
		 * \return ��������� �� ���� � ������������ ���������.
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
		 * \brief ����� � ������ ���� � ��������� ���������.
		 * \param value ������� ��������.
		 * \return ��������� �� ���� � ������� ���������.
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
		 * \brief ������������ ����� �������.
		 * \param localRoot ������ ���������, ������� ������������.
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
		 * \brief ������������ ������ �������.
		 * \param localRoot ������ ���������, ������� ������������.
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
		 * \brief �������� ���� ���� ������.
		 * \param oldNode ��������� �� ����, ������� ����� ��������. 
		 * \param newNode ��������� �� ����� ����. ����� ���� nullptr (������ ���� ����� ����� �� ������).
		 */
		void transplant(Node* oldNode, Node* newNode)
		{
			if (oldNode->parent == nullptr)
			{
				//������ ���� - ������. 
				root_ = newNode;
			}
			else if (oldNode == oldNode->parent->left)
			{
				//������ ���� - ����� �������.
				oldNode->parent->left = newNode;
			}
			else if (oldNode == oldNode->parent->right)
			{
				//������ ���� - ������ �������.
				oldNode->parent->right = newNode;
			}
			if (newNode != nullptr)
			{
				newNode->parent = oldNode->parent;
			}
		}

		/**
		 * \brief �������� ���� � ������ ������.
		 * \param pivotElement ��������� �� ����, ������� ����� ��������� � ������.
		 */
		void splay(Node* pivotElement)
		{
			while (pivotElement != root_)
			{
				if (pivotElement->parent == root_)
				{
					//Zig: �����������, ����� pivotElement->parent �������� ������. ������ �������������� �� ����� ����� pivotElement � pivotElement->parent. 
					//���������� ���� ��� ������� �������� ������ � ����������� ������ ���� ��� � �����, ����� ����������� ������� pivotElement ���� �������.
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
					//Zig-Zig: �����������, ����� � pivotElement, � pivotElement->parent �������� ������ (��� �������) ���������. 
					//������ �������������� �� ����� ����� pivotElement->parent->parent � pivotElement->parent, � ����� � �� ����� ����� pivotElement->parent � pivotElement.
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
					//Zig-Zag: �����������, ����� pivotElement �������� ������ �����, � pivotElement->parent � ����� (��� ��������). 
					//������ �������������� �� ����� ����� pivotElement->parent � pivotElement, � ����� � �� ����� ����� pivotElement � pivotElement->parent->parent.
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

