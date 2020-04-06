/*
 * ������ � 4 (5 ������)
 * ���� ��������������� ����� ����� n,k � ������ ����� ����� �� [0..10^9] ������� n.
 * ��������� ����� k-� ���������� ����������. �.�. ���������� �����,
 * ������� �� ������ �� ������� � �������� k (0..n-1) � ��������������� �������. �������� ������������� ��������.
 * 
 * ���������� � �������������� ������: O(n). ��������� ������� ����� ������: O(n).
 * 
 * ������� Partition ������� ������������� ������� ������� ����� ����������� � ����� �����������. 
 * �������� ��� ������ ������� �� ������ ������� � �����:
 * ���������� ������� �������. ������� ������� �������� � ��������� ��������� �������.
 * �� ����� ������ Partition � ������ ������� ���������� ��������, �� ������� ��������. 
 * ����� ������������� ��������, ������ ������� ��������. 
 * � ����� ������� ����� ��������������� ��������. 
 * ��������� ��������� ����� �������.
 * �������� (������) i ��������� �� ������ ������ ���������, ������ ������� ��������.
 * �������� j ������ i, �������� j ��������� �� ������ ��������������� �������.
 * ��� ���������. ��������������� �������, �� ������� ��������� j. ���� �� ������ ��������, �� �������� j.
 * ���� �� �� ������ ��������, �� ������ a[i] � a[j] �������, �������� i � �������� j.
 * � ����� ������ ��������� ������ ������� � �������, �� ������� ��������� �������� i.
 * 
 * 4_1. ���������� ��������� ������ �������� �������� �������� ����. ������� Partition ���������� ������� ������� ����� ����������� �� ������ ������� � �����.
 * 
 * ����������� �������:
 * In
 * 10 4
 * 1 2 3 4 5 6 7 8 9 10
 * Out
 * 5
 * 
 * In
 * 10 0
 * 3 6 5 7 2 9 8 10 4 1
 * Out
 * 1
 * 
 * In
 * 10 9
 * 0 0 0 0 0 0 0 0 0 1
 * Out
 * 1
 */

/*
 * ������ ��������� ���������.
 * �������������� ������: O(1). 
 * ������� ����� ������: O(n).
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

namespace custom_algorithms
{
    /**
     * \brief �����, ������������ �������, ����������� � ��������� ������� ���������� � �������� ���������. 
     */
	class QuickSort
	{
	public:
		/**
		 * \brief ������� �������� ���������� k-�� ������� ��� �������.
		 * \tparam T ��� ��������� � �������.
		 * \param arrayElements ������ ���������.
		 * \param k ������� �������������� ����������.
		 * \param less ������� ��������� ��������� �������. ������ ���������� true, ���� ������ � �������� ������ �������.
		 * \return k-� ���������� ����������.
		 */
		template<typename T>
		static size_t calculateStatistic(std::vector<T>& arrayElements, const size_t k, const std::function<bool(const T& lhs, const T& rhs)> less = std::less<T>());
	private:
		/**
		 * \brief �������, ����������� �������� � ���������� �� ������� � ������� �������� �� ������ ���� ������� ����������.
		 * \tparam T ��� ��������� � �������.
		 * \param arrayElements ������ ���������.
		 * \param low ������ ������� �������� ���������� � �������.
		 * \param high ������ ���������� �������� ���������� � �������.
		 * \param less ������� ��������� ��������� �������. ������ ���������� true, ���� ������ � �������� ������ �������.
		 * \return ������ �������� �������� ����� ��������������.
		 */
		template<typename T>
		static size_t partition(std::vector<T>& arrayElements, const size_t low, const size_t high, const std::function<bool(const T& lhs, const T& rhs)>& less);

		/**
		 * \brief �������, ���������� ������� ������� � ����������.
		 * \brief ������� ������� �������� �������� ��� ���������: �������, ����������, ��������.
		 * \brief ����� ���������� ������ ������� ������� ������� ��������� �� ������� high.
		 * \brief ����� ���������� ������ ������� � ������� low ��������� ������� �� ������ ��������.
		 * \tparam T ��� ��������� � �������.
		 * \param arrayElements ������ ���������.
		 * \param low ������ ������� �������� ���������� � �������.
		 * \param high ������ ���������� �������� ���������� � �������.
		 * \param less ������� ��������� ��������� �������. ������ ���������� true, ���� ������ � �������� ������ �������.
		 */
		template<typename T>
		static void selectMainElement(std::vector<T>& arrayElements, size_t low, size_t high, const std::function<bool(const T& lhs, const T& rhs)>& less);
	};



    template <typename T>
    size_t custom_algorithms::QuickSort::calculateStatistic(std::vector<T>& arrayElements, const size_t k, const std::function<bool(const T& lhs, const T& rhs)> less)
    {
        size_t currentMainElementPosition = arrayElements.size();
        size_t leftBorder = 0;
        size_t rightBorder = arrayElements.size() - 1;
        /*
         * ���� ����� ���������� ������ �������� �������� < k, 
         * �� ���������� ���������� ��� ���������� ����� �� ��������, ����� - ������.
         * ����������, ���� �� ����� ������� k-� ���������� ����������.
         */
        while (currentMainElementPosition != k)
        {
            if (currentMainElementPosition > k)
            {
                rightBorder = currentMainElementPosition - 1;
                selectMainElement(arrayElements, leftBorder, rightBorder, less);
                currentMainElementPosition = partition(arrayElements, leftBorder, rightBorder, less);
            }
            else
            {
                leftBorder = currentMainElementPosition + 1;
                selectMainElement(arrayElements, leftBorder, rightBorder, less);
                currentMainElementPosition = partition(arrayElements, leftBorder, rightBorder, less);
            }
        }
        return arrayElements[currentMainElementPosition];
    }



    template <typename T>
    size_t QuickSort::partition(std::vector<T>& arrayElements, const size_t low, const size_t high, const std::function<bool(const T& lhs, const T& rhs)>& less)
    {
        /*
         * �� ����� ������ Partition � ������ ������� ���������� ��������, �� ������� ��������. 
         * ����� ������������� ��������, ������ ������� ��������. 
         * � ����� ������� ����� ��������������� ��������. 
         * ��������� ��������� ����� �������.
         * �������� (������) i ��������� �� ������ ������ ���������, ������ ������� ��������.
         * �������� j ������ i, �������� j ��������� �� ������ ��������������� �������.
         * ��� ���������. ��������������� �������, �� ������� ��������� j. ���� �� ������ ��������, �� �������� j.
         * ���� �� �� ������ ��������, �� ������ a[i] � a[j] �������, �������� i � �������� j.
         * � ����� ������ ��������� ������ ������� � �������, �� ������� ��������� �������� i.
         */

        auto leftIterator = low;
        while (!less(arrayElements[high], arrayElements[leftIterator]) && (leftIterator < high))
        {
            leftIterator += 1;
        }
        if (leftIterator == high)
        {
            return high;
        }

        for (auto rightIterator = leftIterator + 1; rightIterator < high; ++rightIterator)
        {
            if (!less(arrayElements[high], arrayElements[rightIterator]))
            {
                std::swap(arrayElements[leftIterator], arrayElements[rightIterator]);
                leftIterator += 1;
            }
        }
        std::swap(arrayElements[leftIterator], arrayElements[high]);
        return leftIterator;
    }



    template <typename T>
    void QuickSort::selectMainElement(std::vector<T>& arrayElements, size_t low, size_t high, const std::function<bool(const T& lhs, const T& rhs)>& less)
    {
        auto mid = (low + high) / 2;
        if (less(arrayElements[mid], arrayElements[low]))
        {
            std::swap(arrayElements[mid], arrayElements[low]);
        }
        if (less(arrayElements[high], arrayElements[low]))
        {
            std::swap(arrayElements[high], arrayElements[low]);
        }
        if (less(arrayElements[mid], arrayElements[high]))
        {
            std::swap(arrayElements[mid], arrayElements[high]);
        }
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
	size_t k{ 0 };
	in >> k;
	std::vector<uint32_t> numbers;
	for (size_t i = 0; i < n; ++i)
	{
		uint32_t number{ 0 };
		in >> number;
		numbers.push_back(number);
	}
	in.close();
	std::cout << custom_algorithms::QuickSort::calculateStatistic(numbers, k);
	return 0;
}

