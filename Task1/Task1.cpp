/*
Задача № 1 (5 баллов)
Во всех задачах из следующего списка следует написать структуру данных, обрабатывающую команды push* и pop*.

Формат входных данных.
В первой строке количество команд n. n ≤ 1000000.
Каждая команда задаётся как 2 целых числа: a b.
a = 1 - push front
a = 2 - pop front
a = 3 - push back
a = 4 - pop back
Для очереди используются команды 2 и 3. Для дека используются все четыре команды.
Если дана команда pop*, то число b - ожидаемое значение.Если команда pop вызвана для пустой структуры данных, то ожидается “-1”. 
Формат выходных данных.
Требуется напечатать YES - если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.

Вариант 1. Реализовать очередь с динамическим зацикленным буфером.

Контрольные примеры:
in
3
3 44
3 50
2 44
out
YES

in
2
2 -1
3 10
out
YES

in
2
3 44
2 66
out
NO
 */

#include "pch.h"
#include <iostream>
#include <cassert>

namespace custom_containers
{
	
	class Queue
	{
	public:
		int32_t PopFront();
		void PushBack(int32_t value);
		size_t Size() const;

		Queue();
		~Queue();

		Queue(const Queue& other) = delete;
		Queue(Queue&& other) noexcept = delete;
		Queue& operator=(const Queue& other) = delete;
		Queue& operator=(Queue&& other) noexcept = delete;
	private:
		void Reallocate();
		size_t head_{ 0 };
		size_t tail_{ 1 };
		int32_t* data_{ nullptr };
		size_t capacity_{ 0 };
		bool is_empty_{ true };
	}

	Queue::Queue()
	{
		data_ = new int32_t[8];
	}

	size_t Queue::Size() const
	{
		//ToDo
	}

	Queue::~Queue()
	{
		delete[] data_;
	}

	void Queue::Reallocate()
	{
		const auto new_data = new int32_t[capacity_ * 2];
		for (auto i = head_; i < tail_; ++i)
		{
			new_data[i - head_] = data_[i];
		}
		delete[] data_;
		data_ = new_data;
		head_ = 0;
		tail_ = capacity_;
		capacity_ *= 2;
	}

}



int main()
{
    std::cout << "Hello World!\n"; 
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
