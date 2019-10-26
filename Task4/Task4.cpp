/*
 * Задача № 4 (5 баллов)
 * Даны неотрицательные целые числа n,k и массив целых чисел из [0..10^9] размера n.
 * Требуется найти k-ю порядковую статистику. т.е. напечатать число,
 * которое бы стояло на позиции с индексом k (0..n-1) в отсортированном массиве. Напишите нерекурсивный алгоритм.
 * 
 * Требования к дополнительной памяти: O(n). Требуемое среднее время работы: O(n).
 * 
 * Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении. 
 * Описание для случая прохода от начала массива к концу:
 * Выбирается опорный элемент. Опорный элемент меняется с последним элементом массива.
 * Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. 
 * Затем располагаются элементы, строго бОльшие опорного. 
 * В конце массива лежат нерассмотренные элементы. 
 * Последним элементом лежит опорный.
 * Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
 * Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
 * Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j.
 * Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
 * В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.
 * 
 * 4_1. Реализуйте стратегию выбора опорного элемента “медиана трёх”. Функцию Partition реализуйте методом прохода двумя итераторами от начала массива к концу.
 * 
 * Контрольные примеры:
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
 * Оценка сложности алгоритма.
 * Дополнительная память: O(1). 
 * Среднее время работы: O(n).
 */


#include <iostream>
#include <fstream>
#include <vector>

namespace custom_algorithms
{
    /**
     * \brief Класс, объединяющий функции, участвующие в алгоритме быстрой сортировки и подсчёте статистик.
     */
	class QuickSort
	{
	public:
		/**
		 * \brief Функция подсчёта статистики k-го порядка для массива.
		 * \tparam T Тип элементов в массиве.
		 * \param arrayElements Массив элементов.
		 * \param k Порядок подсчитываемой статистики.
		 * \return k-я порядковая статистика.
		 */
		template<typename T>
		static size_t calculateStatistic(std::vector<T>& arrayElements, const size_t k);
	private:
		/**
		 * \brief Функция, разделяющая элементы в подмассиве на большие и меньшие опорного на каждом шаге быстрой сортировки.
		 * \tparam T Тип элементов в массиве.
		 * \param arrayElements Массив элементов.
		 * \param low Индекс первого элемента подмассива в массиве.
		 * \param high Индекс последнего элемента подмассива в массиве.
		 * \return Индекс опорного элемента после упорядочивания.
		 */
		template<typename T>
		static size_t partition(std::vector<T>& arrayElements, const size_t low, const size_t high);

		/**
		 * \brief Функция, выбирающая опорный элемент в подмассиве.
		 * \brief Опорный элемент является медианой трёх элементов: первого, последнего, среднего.
		 * \brief После завершения работы функции опорный элемент находится на позиции high.
		 * \brief После завершения работы функции в позиции low находится элемент не больше опорного.
		 * \tparam T Тип элементов в массиве.
		 * \param arrayElements Массив элементов.
		 * \param low Индекс первого элемента подмассива в массиве.
		 * \param high Индекс последнего элемента подмассива в массиве.
		 */
		template<typename T>
		static void selectMainElement(std::vector<T>& arrayElements, size_t low, size_t high);
	};



    template <typename T>
    size_t custom_algorithms::QuickSort::calculateStatistic(std::vector<T>& arrayElements, const size_t k)
    {
        size_t currentMainElementPosition = arrayElements.size();
        size_t leftBorder = 0;
        size_t rightBorder = arrayElements.size() - 1;
        /*
         * Если после разделения индекс опорного элемента < k, 
         * то продолжаем сортировку для подмассива слева от опорного, иначе - справа.
         * Продолжаем, пока не будет найдена k-я порядковая статистика.
         */
        while (currentMainElementPosition != k)
        {
            if (currentMainElementPosition > k)
            {
                rightBorder = currentMainElementPosition - 1;
                selectMainElement(arrayElements, leftBorder, rightBorder);
                currentMainElementPosition = partition(arrayElements, leftBorder, rightBorder);
            }
            else
            {
                leftBorder = currentMainElementPosition + 1;
                selectMainElement(arrayElements, leftBorder, rightBorder);
                currentMainElementPosition = partition(arrayElements, leftBorder, rightBorder);
            }
        }
        return arrayElements[currentMainElementPosition];
    }



    template <typename T>
    size_t QuickSort::partition(std::vector<T>& arrayElements, const size_t low, const size_t high)
    {
        /*
         * Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. 
         * Затем располагаются элементы, строго бОльшие опорного. 
         * В конце массива лежат нерассмотренные элементы. 
         * Последним элементом лежит опорный.
         * Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
         * Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
         * Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j.
         * Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
         * В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.
         */

        auto leftIterator = low;
        while (arrayElements[leftIterator] <= arrayElements[high] && leftIterator < high)
        {
            leftIterator += 1;
        }
        if (leftIterator == high)
        {
            return high;
        }

        for (auto rightIterator = leftIterator + 1; rightIterator < high; ++rightIterator)
        {
            if (arrayElements[rightIterator] > arrayElements[high])
            {
                continue;
            }
            else
            {
                std::swap(arrayElements[leftIterator], arrayElements[rightIterator]);
                leftIterator += 1;
            }
        }
        std::swap(arrayElements[leftIterator], arrayElements[high]);
        return leftIterator;
    }



    template <typename T>
    void QuickSort::selectMainElement(std::vector<T>& arrayElements, size_t low, size_t high)
    {
        auto mid = (low + high) / 2;
        if (arrayElements[mid] < arrayElements[low])
        {
            std::swap(arrayElements[mid], arrayElements[low]);
        }
        if (arrayElements[high] < arrayElements[low])
        {
            std::swap(arrayElements[high], arrayElements[low]);
        }
        if (arrayElements[mid] < arrayElements[high])
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

