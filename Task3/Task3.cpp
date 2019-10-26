/*
 * ������ � 3 (4 ����a)
 * 3_3. ���������� ��������.
 * ���� ������������������ ����� ����� �� ��������� (-10^9 .. 10^9). ����� ������������������ �� ������ 10^6. ����� �������� �� ������ � ������. ���������� ����� �� �������.  
 * ����� ���������� ��������� n, � ����� �������� � ������� a = a[i]: i �� [0..n-1].
 * ��������� ���������� ���������� ����� ��� �������� (i,j) �� [0..n-1], ��� (i < j � a[i] > a[j]).
 * ��������: ���������� �������� ����� ���� ������ 4*10^9 - ����������� int64_t.
 * #include <stdint.h>
 * int64_t cnt = 0;
 * printf(�%ld�, cnt);
 * ����������� �������:
 * In
 * 1
 * 2
 * 3
 * 4
 * Out
 * 0
 *
 * In
 * 4
 * 3
 * 2
 * 1
 * Out
 * 6
 *
 * In
 * 3
 * 2
 * 2
 * Out
 * 2
 */

/*
* ������ ��������� ���������.
* �������������� ������: O(n).
* ������� ����� ������: O(n*log(n)).
*/

#include <iostream>
#include <vector>

namespace custom_algorithms
{
    /**
	 * \brief �������, ����������� ���������� �������� � �������� ������� � ��������� ��������.
	 * \tparam T ��� ��������� � �������.
	 * \param arrayNumbers ������ ���������.
	 * \return ���������� ��������.
	 */
	template<typename T>
	size_t mergeSort(std::vector<T>& arrayNumbers)
	{
		size_t inversionsNumber = 0;
		for (size_t blockSize = 1; blockSize < arrayNumbers.size(); blockSize *= 2)
		{
			for (size_t blockIterator = 0; blockIterator < arrayNumbers.size() - blockSize; blockIterator += 2 * blockSize)
			{
				//���������� ������� � ����������� ���� ������, ������������ � �������� BlockIterator
				//����� �������� BlockSize, ������ �������� BlockSize ��� ������

                //���������� ������� ������.
                //leftBorder - ������ ������ �����
                //midBorder - ������ ������� �����
                //rightBorder - �����(�� ��������� ���������) ������� �����
				const auto leftBorder = blockIterator;
				const auto midBorder = blockIterator + blockSize;
				auto rightBorder = blockIterator + 2 * blockSize;
				if (rightBorder > arrayNumbers.size())
				{
					rightBorder = arrayNumbers.size();
				}

                //���������� ������� � ��������� ������� sortedBlock
				std::vector<T> sortedBlock(rightBorder - leftBorder);
				size_t leftBlockIterator = 0;
				size_t rightBlockIterator = 0;
				//���� � ����� �������� ���� ��������, �������� ������� �� ��� � ������� � ��������������� ����
                //���� ������� � ����� ����� ������ �������� � ������ �����, �� ���������� ��������.
                //��� ����������� �������� ����������� ������� �������� �� �������� = <����� ����� �����> - <������ �������� �������� � ����� �����>
				while (leftBorder + leftBlockIterator < midBorder && midBorder + rightBlockIterator < rightBorder)
				{
					if (arrayNumbers[leftBorder + leftBlockIterator] <= arrayNumbers[midBorder + rightBlockIterator])
					{
						sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[leftBorder + leftBlockIterator];
						leftBlockIterator += 1;
					}
					else
					{
						sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[midBorder + rightBlockIterator];
						inversionsNumber += (midBorder - leftBorder) - leftBlockIterator;
						rightBlockIterator += 1;
					}
				}
				//����� ����� ������� ���������� �������� �� ������ ��� ������� �����
				while (leftBorder + leftBlockIterator < midBorder)
				{
					sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[leftBorder + leftBlockIterator];
					leftBlockIterator += 1;
				}
				while (midBorder + rightBlockIterator < rightBorder)
				{
					sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[midBorder + rightBlockIterator];
					rightBlockIterator += 1;
				}

                //�������� ��������������� �������� � �������� ������
				for (size_t mergeIterator = 0; mergeIterator < leftBlockIterator + rightBlockIterator; ++mergeIterator)
				{
					arrayNumbers[leftBorder + mergeIterator] = sortedBlock[mergeIterator];
				}
			}
		}
        return inversionsNumber;
	}
}


int main()
{
	std::vector<int32_t> numbers;
	int32_t number{ 0 };
	while (std::cin >> number)
	{
		numbers.push_back(number);
	}
	std::cout << custom_algorithms::mergeSort(numbers);
}
