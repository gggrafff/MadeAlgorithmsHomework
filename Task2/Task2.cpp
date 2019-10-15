/*
 * Задача № 2 (5 баллов)
 * Решение всех задач данного раздела предполагает использование кучи.
 * 
 * 2_1. Жадина.
 * Вовочка ест фрукты из бабушкиной корзины. В корзине лежат фрукты разной массы. Вовочка может поднять не более K грамм. Каждый фрукт весит не более K грамм. За раз он выбирает несколько самых тяжелых фруктов, которые может поднять одновременно, откусывает от каждого половину и кладет огрызки обратно в корзину. Если фрукт весит нечетное число грамм, он откусывает большую половину. Фрукт массы 1гр он съедает полностью.
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

#include <cstdint>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>


namespace custom_containers
{
	class Heap
	{
	public:
		void Push(int32_t value);
		int32_t Pop();

		Heap();
		~Heap();

		bool IsEmpty() const { return size_ == 0; }
	private:
		void SiftUp(size_t index);
		void SiftDown(size_t index);
		void Reallocate();

		int32_t* data_{ nullptr };
		size_t capacity_{ 0 };
		size_t size_{ 0 };
	};



	void Heap::Push(const int32_t value)
	{
		data_[size_++] = value;
		SiftUp(size_ - 1);
		if (size_ == capacity_)
		{
			Reallocate();
		}
	}



	void Heap::SiftUp(const size_t index)
	{
		if (index == 0) return;
        const size_t parentIndex = (index + 1) / 2 - 1;
		if (data_[parentIndex] > data_[index])
		{
			std::swap(data_[index], data_[parentIndex]);
			SiftUp(parentIndex);
		}
	}



    void Heap::SiftDown(const size_t index)
    {
        auto childIndex = (index + 1) * 2 - 1;
		if (childIndex >= size_) return;
		if (childIndex + 1 < size_ && data_[childIndex + 1] < data_[childIndex]) ++childIndex;
        if (data_[childIndex] < data_[index])
        {
			std::swap(data_[childIndex], data_[index]);
			SiftDown(childIndex);
        }
    }



    int32_t Heap::Pop()
	{
		if (size_ == 0)
		{
			throw std::exception("Heap is empty.");
		}
        const auto value = data_[0];
		data_[0] = data_[size_ - 1];
		--size_;
		SiftDown(0);
		return value;
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



    void Heap::Reallocate()
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


uint32_t calculateIterations(const std::vector<uint32_t>& fruits, uint32_t limit)
{
	uint32_t count = 0;
	custom_containers::Heap heap;
	for(auto& fruit: fruits)
	{
		heap.Push(fruit);
	}
	while (!heap.IsEmpty())
	{
		uint32_t current_weight = 0;
		std::vector<uint32_t> current_fruits;
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

