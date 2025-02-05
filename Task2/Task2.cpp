/*
 * Задача № 2 (5 баллов)
 * Решение всех задач данного раздела предполагает использование кучи.
 * 
 * 2_1. Жадина.
 * Вовочка ест фрукты из бабушкиной корзины. В корзине лежат фрукты разной массы. 
 * Вовочка может поднять не более K грамм. Каждый фрукт весит не более K грамм. 
 * За раз он выбирает несколько самых тяжелых фруктов, которые может поднять одновременно, 
 * откусывает от каждого половину и кладет огрызки обратно в корзину. 
 * Если фрукт весит нечетное число грамм, он откусывает большую половину. Фрукт массы 1гр он съедает полностью.
 * Определить за сколько подходов Вовочка съест все фрукты в корзине.
 * 
 * Формат входных данных. Вначале вводится n - количество фруктов и n строк с массами фруктов.
 * n ≤ 50000.
 * Затем K - "грузоподъемность". K ≤ 1000.
 * 
 * Формат выходных данных. Неотрицательное число - количество подходов к корзине.
 * 
 * Контрольные примеры:
 * in
 * 3
 * 1 2 2
 * 2
 * out
 * 4
 * 
 * in
 * 3
 * 4 3 5
 * 6
 * out
 * 5
 * 
 * in
 * 7
 * 1 1 1 1 1 1 1
 * 3
 * out
 * 3
 */

 /*
  * Оценки сложности.
  * По времени
  * Добавление элементов в кучу O(log n)
  * Исключение максимального элемента из кучи O(log n)
  * Всё решение задачи:
  *  Каждый элемент оказывается в куче не более log k раз. Тогда общая сложность O(n*log(k)*log(n))
  *
  * По памяти O(n+k)
  */

#include <cstdint>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>


namespace custom_containers
{
	/**
	 * \brief Двоичная куча
	 */
	class Heap
	{
	public:
		void push(int32_t value);
		int32_t pop();
		int32_t top() const;

		Heap();
		~Heap();
		Heap(const Heap& other) = delete;
		Heap(Heap&& other) noexcept = delete;
		Heap& operator=(const Heap& other) = delete;
		Heap& operator=(Heap&& other) noexcept = delete;

		/**
		 * \brief Пустая ли куча?
		 * \return true - куча пуста; false - не пуста.
		 */
		bool isEmpty() const { return size_ == 0; }
	private:
		void siftUp(size_t index);
		void siftDown(size_t index);
		void reallocate();

		/**
		 * \brief Динамический массив для хранения элементов кучи.
		 */
		int32_t* data_{ nullptr };
		/**
		 * \brief Объём выделенной под кучу памяти.
		 */
		size_t capacity_{ 0 };
		/**
		 * \brief Количество элементов в куче.
		 */
		size_t size_{ 0 };
	};



	/**
	 * \brief Добавляет элемент в кучу
	 * \param value Новый элемент
	 */
	void Heap::push(const int32_t value)
	{
		data_[size_++] = value;
		siftUp(size_ - 1);
		if (size_ == capacity_)
		{
			reallocate();
		}
	}



	/**
	 * \brief Проверяет соответствие положения элемента в куче его величине и при необходимости поднимает элемент.
	 * \param index Индекс проверяемого элемента.
	 */
	void Heap::siftUp(const size_t index)
	{
		if (index == 0) return;
        const auto parentIndex = (index + 1) / 2 - 1;
		if (data_[parentIndex] < data_[index])
		{
			std::swap(data_[index], data_[parentIndex]);
			siftUp(parentIndex);
		}
	}


	/**
	 * \brief Проверяет соответствие положения элемента в куче его величине и при необходимости опускает элемент.
	 * \param index Индекс проверяемого элемента.
	 */
    void Heap::siftDown(const size_t index)
    {
        auto childIndex = (index + 1) * 2 - 1;
		if (childIndex >= size_) return;
		if (childIndex + 1 < size_ && data_[childIndex + 1] > data_[childIndex]) ++childIndex;
        if (data_[childIndex] > data_[index])
        {
			std::swap(data_[childIndex], data_[index]);
			siftDown(childIndex);
        }
    }



	/**
     * \brief Извлекает с удалением максимальный элемент из кучи.
     * \return Максимальный элемент кучи.
     */
    int32_t Heap::pop()
	{
		if (size_ == 0)
		{
			throw std::runtime_error("Heap is empty.");
		}
        const auto value = data_[0];
		data_[0] = data_[size_ - 1];
		--size_;
		siftDown(0);
		return value;
	}

	/**
	 * \brief Возвращает без удаления максимальный элемент кучи.
	 * \return Максимальный элемент кучи.
	 */
	int32_t Heap::top() const
	{
		if (size_ == 0)
		{
			throw std::runtime_error("Heap is empty.");
		}
		return data_[0];
	}


	Heap::Heap()
    {
		data_ = new int32_t[8];
		capacity_ = 8;
    }



    Heap::~Heap()
    {
		delete[] data_;
    }


	/**
	 * \brief Запрашивает новую память для объектов кучи и копирует старые данные данные.
	 */
    void Heap::reallocate()
    {
		const auto newData = new int32_t[capacity_ * 2];
		for (auto i = 0; i < capacity_; ++i)
		{
			newData[i] = data_[i];
		}
		delete[] data_;
		data_ = newData;
		capacity_ *= 2;
    }
}


/**
 * \brief Подсчёт количества подходов Вовочки к корзине с фруктами.
 * \param fruits Массив фруктов в корзине.
 * \param limit Ограничение на подъём тяжестей для Вовочки.
 * \return Искомое в задаче количество подходов.
 */
uint32_t calculateIterations(const std::vector<uint32_t>& fruits, const uint32_t limit)
{
	uint32_t count = 0;
	custom_containers::Heap heap;
	for(auto& fruit: fruits)
	{
		heap.push(fruit);
	}
	while (!heap.isEmpty())
	{
		++count;
		uint32_t current_weight = 0;
		std::vector<uint32_t> current_fruits;
		while (!heap.isEmpty() && current_weight + heap.top() <= limit)
		{
			current_fruits.push_back(heap.pop());
			current_weight += current_fruits.back();
		}
		for (auto& fruit : current_fruits)
		{
			fruit /= 2;
			if (fruit != 0) heap.push(fruit);
		}
	}
	return count;
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
	uint32_t n{ 0 };
	in >> n;
	std::vector<uint32_t> fruits;
	for (uint32_t i = 0; i < n; ++i)
	{
		uint32_t mass {0};
		in >> mass;
		fruits.push_back(mass);
	}
	uint32_t k{ 0 };
	in >> k;
	in.close();
	std::cout << calculateIterations(fruits, k);

	return 0;
}

