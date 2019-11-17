#include <ctime>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cstring>
#include "sort.h"


namespace custom_algorithms
{
	void insertionSort(unsigned int *arrayElements, const size_t size)
	{
		for (int i = 1; i < size; i++)
		{
			const int key = arrayElements[i];
			int j = i - 1;
			while (j >= 0 && arrayElements[j] > key)
			{
				arrayElements[j + 1] = arrayElements[j];
				j = j - 1;
			}
			arrayElements[j + 1] = key;
		}
	}

	void quicksort(unsigned int *arrayElements, const size_t first, const size_t last)
	{
		if (last - first <= 300)
		{
			insertionSort(arrayElements + first, last - first + 1);
		}
		else
		{
			int64_t f = first, l = last;
			const unsigned int mid = arrayElements[(f + l) / 2];
			do
			{
				while (arrayElements[f] < mid) f++;
				while (arrayElements[l] > mid) l--;
				if (f <= l) //перестановка элементов
				{
					const unsigned int count = arrayElements[f];
					arrayElements[f] = arrayElements[l];
					arrayElements[l] = count;
					f++;
					l--;
				}
			} while (f < l);
			if (first < l) quicksort(arrayElements, first, l);
			if (f < last) quicksort(arrayElements, f, last);
		}
	}
}


void Sort(unsigned int* arrayElements, unsigned int size)
{
	custom_algorithms::quicksort(arrayElements, 0, size - 1);
}



/*int main()
{
	srand(time(nullptr));
	std::vector<unsigned int> numbers;
	for (auto i = 0; i < 100; ++i)
	{
		numbers.push_back(rand());
	}
	Sort(numbers.data(), numbers.size());
	for (auto number : numbers)
	{
		std::cout << number << std::endl;
	}
}

*/