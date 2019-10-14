/*
 * Задача № 1 (5 баллов)
 * Во всех задачах из следующего списка следует написать структуру данных, обрабатывающую команды push* и pop*.
 *
 * Формат входных данных.
 * В первой строке количество команд n. n ≤ 1000000.
 * Каждая команда задаётся как 2 целых числа: a b.
 * a = 1 - push front
 * a = 2 - pop front
 * a = 3 - push back
 * a = 4 - pop back
 * Для очереди используются команды 2 и 3. Для дека используются все четыре команды.
 * Если дана команда pop*, то число b - ожидаемое значение.Если команда pop вызвана для пустой структуры данных, то ожидается “-1”.
 * Формат выходных данных.
 * Требуется напечатать YES - если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.
 *
 * Вариант 1. Реализовать очередь с динамическим зацикленным буфером.
 *
 * Контрольные примеры:
 * in
 * 3
 * 3 44
 * 3 50
 * 2 44
 * out
 * YES
 *
 * in
 * 2
 * 2 -1
 * 3 10
 * out
 * YES
 *
 * in
 * 2
 * 3 44
 * 2 66
 * out
 * NO
 */

/*
 * Оценки сложности.
 * По времени
 * Добавление элементов в очередь: 
 *	Амортизированная оценка O(1)
 *	В лучшем случае O(1)
 *	В худшем случае O(n)
 * Взятие элемента из очереди O(1)
 * Всё решение задачи:
 *  В худшем случае O(n)
 * 
 * По памяти:
 *  В худшем случае O(n)
 */

#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>


namespace custom_containers
{

    /**
	 * \brief Очередь с динамическим зацикленным буфером
	 */
	class Queue
	{
	public:
		int32_t popFront();
		void pushBack(int32_t value);
		size_t size() const;

		Queue();
		~Queue();

		Queue(const Queue& other) = delete;
		Queue(Queue&& other) noexcept = delete;
		Queue& operator=(const Queue& other) = delete;
		Queue& operator=(Queue&& other) noexcept = delete;
	private:
		void reallocate();

        /**
		 * \brief Индекс первого элемента в очереди.
		 */
		size_t head_{ 0 };
        /**
		 * \brief Индекс последнего элемента в очереди. 
		 */
		size_t tail_{ 1 };
        /**
		 * \brief Динамический массив для хранения элементов очереди.
		 */
		int32_t* data_{ nullptr };
        /**
		 * \brief Размер выделенной для очереди памяти.
		 */
		size_t capacity_{ 0 };
        /**
		 * \brief Флаг, имеет значение true, если очередь пуста, false - не пуста.
		 */
		bool isEmpty_{ true };
	};

	Queue::Queue()
	{
		data_ = new int32_t[8];
		capacity_ = 8;
	}

    /**
	 * \brief Забирает элемент из очереди.
	 * \return Элемент, стоявший в очереди первым.
	 */
	int32_t Queue::popFront()
	{
		if (isEmpty_)
		{
			return -1; //по условию задачи
		}
        const auto value = data_[head_];
		head_++;
		head_ %= capacity_;
		if (head_ == tail_) //очередь стала пустой
		{
			isEmpty_ = true;
			head_+=capacity_-1;
			head_ %= capacity_;
		}
		return value;
	}

    /**
	 * \brief Добавляет элемент в очередь
	 * \param value Добавляемый элемент.
	 */
	void Queue::pushBack(const int32_t value)
	{
		if (isEmpty_)
		{
			data_[head_] = value;
			isEmpty_ = false;
		}
		else
		{
			data_[tail_] = value;
			++tail_;
			tail_ %= capacity_;
			if (tail_ == head_) //закончился буфер для хранения элементов
			{
				reallocate();
			}
		}
	}

	Queue::~Queue()
	{
		delete[] data_;
	}

    /**
	 * \brief Запрашивает новую память для объектов очереди и копирует старые данные данные.
	 */
	void Queue::reallocate()
	{
		const auto newData = new int32_t[capacity_ * 2];
        /*
         * Так как в c++ остаток от целочисленного деления может быть отрицательным, необходимо обработать ситуацию, когда tail_ < head_.
         */
		auto tmpTail = tail_;
		if (tail_ <= head_)
		{
			tmpTail = capacity_ + tail_;
		}
		for (auto i = head_; i < tmpTail; ++i)
		{
			newData[i - head_] = data_[i % capacity_];
		}
		delete[] data_;
		data_ = newData;
		head_ = 0;
		tail_ = capacity_;
		capacity_ *= 2;
	}

}


/**
 * \brief Функция проверки последовательности команд на корректность.
 * \param commands Последовательность команд, записанная в std::vector<std::pair<uint16_t, int32_t>>, порядок и значение чисел в std::pair соответствуют условию задачи.
 * \return Возвращает true, если команды верные, false - неверные.
 */
bool checkCommands(const std::vector<std::pair<uint16_t, int32_t>>& commands)
{
	custom_containers::Queue queue;
	for (auto& command : commands)
	{
		if (command.first == 2) //pop
		{
			try
			{
				const auto value = queue.popFront();
				if (value != command.second)
				{
					return false;
				}
			}
			catch (...)
			{
				return false;
			}
		}
		else if (command.first == 3) //push
		{
			try
			{
				queue.pushBack(command.second);
			}
			catch (...)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
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
	std::vector<std::pair<uint16_t, int32_t>> commands;
	for (uint32_t i = 0; i < n; ++i)
	{
		uint16_t command{ 0 };
		in >> command;
		auto value{ 0 };
		in >> value;
		commands.emplace_back(command, value);
	}
	in.close();
	if (checkCommands(commands))
	{
		std::cout << "YES";
	}
	else
	{
		std::cout << "NO";
	}
	return 0;
}
