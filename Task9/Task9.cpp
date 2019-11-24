//Huffman

#include "Huffman.h"
#include <vector>
#include <array>

namespace custom_containers
{
	class CodeTree
	{
		/*
		 * ������������� ������
		 * ����� ��������, ��� ������ �������� ��������� ���������������, ���� ��� ���� ����� ���� ����������� � ������� ����������� ���� � � ���� ������������ ������ ���� ��������� ����� �� ����� ������. 
		 */
	public:
		CodeTree()
		{
			data_.emplace_back();
		}
		void insert(byte value)
		{
			/*
			 * ��� ������� ������������� ������� ���������� � ������� ��������� ��������� ����������:
				���� ������� ������ �������� �� ������������� � �������� � NYT ��� �������� ����: ���� ��� ���������� NYT, ������ ��� �������. ��������� ��� ������ ����� � ������� NYT � ���������� � ���� 4. ���� ������� ������ �������� �� NYT, ������� � ����� �������.
				���� ���� ���� �� ����� ���������� ��� � �����, �������� ��� � �����, ������� ���������� �����, �� �����������, ���� ���� ���� �������� ������������ ���������[3]
				���������� ���� ��� �������� ����
				���� ��� �� �������� ���� ����� � ������������ ���� ����� ��������� � ���� 2. ���� ��� ������, ���������.
			 */
			if (used_[value])
			{
				
			}
			else
			{
				data_.emplace_back();
				data_.emplace_back();

				data_[data_.size() - 1].parent = data_.size() - 3;

				data_[data_.size() - 2].value = value;
				data_[data_.size() - 2].parent = data_.size() - 3;
				data_[data_.size() - 2].weight += 1;

				data_[data_.size() - 3].leftChild = data_.size() - 2;
				data_[data_.size() - 3].rightChild = data_.size() - 1;

				auto current = data_.size() - 2;
				while (data_[current].parent != current)
				{
					if (data_[data_[current].parent].weight == data_[data_[current].parent - 1].weight)
					{
						auto i = current;
						while (!(data_[i].isLeaf() && data_[i - 1].weight > data_[current].weight))
						{
							i -= 1;
						}
						std::swap(data_[i].value, data_[current].value);
						std::swap(data_[i].weight, data_[current].weight);
						current = i;
					}
					else
					{
						data_[data_[current].parent].weight += 1;
						current = data_[current].parent;
					}
				}



				used_[value] = true;
			}
		}
	private:
		struct Node
		{
			size_t parent{0};
			size_t leftChild{0};
			size_t rightChild{0};
			size_t weight{0};
			byte value{ 0 };
			bool isLeaf() const
			{
				return leftChild == rightChild == 0;
			}
			bool isNyt() const
			{
				return isLeaf() && weight == 0;
			}
		};
		std::vector<Node> data_;
		std::array<bool, 256> used_ {false};
	};
}

namespace custom_algorithms {
	class Huffman
	{
	public:
		Huffman()
		{
			//init tree
		}
		~Huffman()
		{
			
		}
		byte pop(bool force=false)
		{
			
		}
		void encode(byte symbol)
		{
			
		}
		size_t size() const
		{
			return buffer.size() / 8;
		}
	private:

		std::vector<bool> buffer;
	};
}

void Encode(IInputStream& original, IOutputStream& compressed)
{
	custom_algorithms::Huffman coder;
	byte value;
	while (original.Read(value))
	{
		coder.encode(value);
	}

}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	copyStream(compressed, original);
}