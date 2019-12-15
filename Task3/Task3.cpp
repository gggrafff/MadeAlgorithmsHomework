/*
 * Задача № 3 (4 баллa)
 * 3_3. Количество инверсий.
 * Дана последовательность целых чисел из диапазона (-10^9 .. 10^9). Длина последовательности не больше 10^6. Числа записаны по одному в строке. Количество чисел не указано.  
 * Пусть количество элементов n, и числа записаны в массиве a = a[i]: i из [0..n-1].
 * Требуется напечатать количество таких пар индексов (i,j) из [0..n-1], что (i < j и a[i] > a[j]).
 * Указание: количество инверсий может быть больше 4*10^9 - используйте int64_t.
 * #include <stdint.h>
 * int64_t cnt = 0;
 * printf(“%ld”, cnt);
 * Контрольные примеры:
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
* Оценка сложности алгоритма.
* Дополнительная память: O(n).
* Среднее время работы: O(n*log(n)).
*/

#include <iostream>
#include <vector>
#include <functional>

namespace custom_algorithms
{
    /**
	 * \brief Функция, выполняющая сортировку слиянием в исходном массиве с подсчётом инверсий.
	 * \tparam T Тип элементов в массиве.
	 * \param arrayNumbers Массив элементов.
	 * \param less Функция сравнения элементов массива. Должна возвращать true, если первый её аргумент меньше второго.
	 * \return Количество инверсий.
	 */
	template<typename T>
	size_t mergeSort(std::vector<T>& arrayNumbers, const std::function<bool(const T& lhs, const T& rhs)> less = std::less<T>())
	{
		size_t inversionsNumber = 0;
		for (size_t blockSize = 1; blockSize < arrayNumbers.size(); blockSize *= 2)
		{
			for (size_t blockIterator = 0; blockIterator < arrayNumbers.size() - blockSize; blockIterator += 2 * blockSize)
			{
				//Производим слияние с сортировкой пары блоков, начинающуюся с элемента BlockIterator
				//Левый размером BlockSize, правый размером BlockSize или меньше

                //Определяем границы блоков.
                //leftBorder - начало левого блока
                //midBorder - начало правого блока
                //rightBorder - конец(за последним элементом) правого блока
				const auto leftBorder = blockIterator;
				const auto midBorder = blockIterator + blockSize;
				auto rightBorder = blockIterator + 2 * blockSize;
				if (rightBorder > arrayNumbers.size())
				{
					rightBorder = arrayNumbers.size();
				}

                //Производим слияние в отдельном массиве sortedBlock
				std::vector<T> sortedBlock(rightBorder - leftBorder);
				size_t leftBlockIterator = 0;
				size_t rightBlockIterator = 0;
				//Пока в обоих массивах есть элементы, выбираем меньший из них и заносим в отсортированный блок
                //Если элемент в левой части больше элемента в правой части, то обнаружены инверсии.
                //При обнаружении инверсий увеличиваем счётчик инверсий на величину = <длина левой части> - <индекс текущего элемента в левой части>
				while (leftBorder + leftBlockIterator < midBorder && midBorder + rightBlockIterator < rightBorder)
				{
					if (less(arrayNumbers[midBorder + rightBlockIterator], arrayNumbers[leftBorder + leftBlockIterator]))
					{
						//Элемент в правой части меньше элемента в левой. Инверсия.
						sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[midBorder + rightBlockIterator];
						inversionsNumber += (midBorder - leftBorder) - leftBlockIterator;
						rightBlockIterator += 1;
					}
					else
					{
						//Элемент в левой части не больше элемента в правой. Инверсии нет.
						sortedBlock[leftBlockIterator + rightBlockIterator] = arrayNumbers[leftBorder + leftBlockIterator];
						leftBlockIterator += 1;
					}
				}
				//После этого заносим оставшиеся элементы из левого или правого блока
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

                //Копируем отсортированные элементы в исходный массив
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
