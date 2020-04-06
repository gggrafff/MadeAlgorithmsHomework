/*
 * 12. Мосты
 * Ограничение времени	2 секунды
 * Ограничение памяти	256Mb
 * Ввод	bridges.in
 * Вывод bridges.out
 * Ребро неориентированного графа называется мостом, если удаление этого ребра из графа увеличивает число компонент связности.
 *
 * Дан неориентированный граф, требуется найти в нем все мосты.
 *
 * Формат ввода
 * Первая строка входного файла содержит два целых числа n и m — количество вершин и ребер графа соответственно (1 ≤ n ≤ 20000, 1 ≤ m ≤ 200000).
 *
 * Следующие m строк содержат описание ребер по одному на строке. 
 * Ребро номер i описывается двумя натуральными числами bi, ei — номерами концов ребра (1 ≤ bi, ei ≤ n).
 *
 * Формат вывода
 * Первая строка выходного файла должна содержать одно натуральное число b — количество мостов в заданном графе. 
 * На следующей строке выведите b целых чисел — номера ребер, которые являются мостами, в возрастающем порядке. 
 * Ребра нумеруются с единицы в том порядке, в котором они заданы во входном файле.
 *
 * Пример
 * Ввод
 * 6 7
 * 1 2
 * 2 3
 * 3 4
 * 1 3
 * 4 5
 * 4 6
 * 5 6
 * Вывод
 * 1
 * 3
 */

/*
 * Пытался сделать универсальный dfs. 
 * Но задача поиска мостов оказалась довольно специфичной. 
 * Получилось сделать универсальный dfs, хотя он не самый красивый и эффективный.
 * Если важна производительность, можно написать лучше. Но зато этот dfs reusable. 
 */

#include <vector>
#include <functional>
#include <stack>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <optional>

namespace custom_containers {
	/**
	 * \brief Описывает узел невзвешенного графа с пронумерованными рёбрами. Массив смежных с узлом вершин. Пары: номер смежной вершины, номер соединяющего ребра.
	 */
	using UnweighedEnumeratedGraphNode = std::vector<std::pair<size_t, size_t>>;
	/**
	 * \brief Описывает невзвешенный граф  с пронумерованными рёбрами. Массив узлов.
	 */
	using UnweighedEnumeratedGraph = std::vector<UnweighedEnumeratedGraphNode>;
	/**
	 * \brief Ребро графа, имеющее номер. Кортеж: из какой вершины, в какую вершину, номер ребра
	 */
	using LineOfEnumeratedGraph = std::tuple<size_t, size_t, size_t>; 
}

namespace custom_algorithms {
	/**
	 * \brief Нерекурсивный обход графа с пронумерованными рёбрами в глубину. 
	 * \param graph Граф с пронумерованными рёбрами. 
	 * \param start Начальная вершина для обхода. 
	 * \param beforeActionForNode Действие, выполняемое с узлом до его развёртывания.
	 * \param afterActionForNode Действие, выполняемое с узлом после его развёртывания.
	 * \param beforeActionForLine Действие, выполняемое с ребром до обработки следующей вершины (при прямом проходе вглубь графа).
	 * \param afterActionForLine Действие, выполняемое с ребром после обработки следующей вершины (при обратном возврате из глубины графа).
	 * \param visited Массив, хранящий отметки о посещённых вершинах. Передаётся снаружи, т.к. обход в глубину может обойти не весь граф, если граф не связный, и снаружи об этом нужно знать. 
	 * \return 
	 */
	std::vector<size_t> dfs(
		const custom_containers::UnweighedEnumeratedGraph& graph,
		const size_t start,
		const std::optional<std::function<void(const size_t currentElement)>>& beforeActionForNode,
		const std::optional<std::function<void(const size_t currentElement)>>& afterActionForNode,
		const std::optional<std::function<void(const custom_containers::LineOfEnumeratedGraph line)>>& beforeActionForLine,
		const std::optional<std::function<void(const custom_containers::LineOfEnumeratedGraph line)>>& afterActionForLine,
		std::vector<bool>& visited) {

		size_t dfs = 0;
		std::stack<std::pair<size_t, size_t>> elementsForProcessing;  // Номер вершины и номер соседа, на котором остановились обрабатывать вершину.
		std::vector<std::pair<size_t, size_t>> parents(graph.size(), { graph.size(), 0 });  // Хранит родителей в дереве обхода: id узла, id ребра от родителя к потомку.
		std::vector<size_t> dfsnumber(graph.size(), 0);  // Хранит порядок обхода.
		std::vector<bool> first_input(graph.size(), true);  // Хранит информацию, первый ли раз мы зашли в вершину.

		visited[start] = true;
		dfsnumber[start] = ++dfs;

		elementsForProcessing.emplace(start, 0);  // начальная вершина, начальный сосед

		while (!elementsForProcessing.empty()) {  // обход в глубину
			const auto currentElement = elementsForProcessing.top().first;
			if (first_input[currentElement]) {
				first_input[currentElement] = false;
				if (beforeActionForNode)
					beforeActionForNode.value()(currentElement);
			}
			auto currentNeighbor = elementsForProcessing.top().second;
			for (; currentNeighbor < graph[currentElement].size(); ++currentNeighbor) {
				auto nodeNumber = graph[currentElement][currentNeighbor];
				if (beforeActionForLine && nodeNumber.second != parents[currentElement].second)
					beforeActionForLine.value()({ currentElement, nodeNumber.first, nodeNumber.second });
				if (!visited[nodeNumber.first]) {
					//найдена новая вершина
					visited[nodeNumber.first] = true;
					dfsnumber[nodeNumber.first] = ++dfs;
					parents[nodeNumber.first] = { currentElement,  nodeNumber.second };

					elementsForProcessing.top().second = currentNeighbor + 1;  // запомним, что когда вернёмся в эту вершину, нужно продолжить со следующего соседа
					elementsForProcessing.emplace(nodeNumber.first, 0);  // новая вершина, её начальный сосед
					break;  // Прерываем обработку текущей вершины и немедленно переходим к обработке новой вершины
				}
			}
			if (currentNeighbor == graph[currentElement].size()) {
				elementsForProcessing.pop();  // все соседи текущей вершины просмотрены, убираем вершину из стека
				if (afterActionForNode)
					afterActionForNode.value()(currentElement);
				if (afterActionForLine && !elementsForProcessing.empty())
					afterActionForLine.value()({ elementsForProcessing.top().first, currentElement, parents[currentElement].second });
			}
		}
		return dfsnumber;
	}


	/**
	 * \brief Поиск мостов в графе
	 * \param graph Граф с пронумерованными рёбрами
	 * \return Массив рёбер. 
	 */
	std::vector<custom_containers::LineOfEnumeratedGraph> findBridgesInUnweighedGraph(
		const custom_containers::UnweighedEnumeratedGraph& graph) {
		std::vector<size_t> time_in(graph.size(), 0);  // время входа в вершину
		std::vector<size_t> fup(graph.size(), 0);  // корректируемая при повторном попадании в вершину величина, по которой найдём мосты
		std::vector<bool> visited(graph.size(), false);  // обработана ли вершина
		size_t timer = 0;
		std::vector<custom_containers::LineOfEnumeratedGraph> bridges;

		for (size_t i = 0; i < graph.size(); ++i) {  // пока не обойдём все компоненты смежности
			if (!visited[i])
				dfs(
					graph,
					i,
					[&time_in, &fup, &timer](const size_t currentElement) {
				time_in[currentElement] = fup[currentElement] = timer++;  // фиксируем время входа
			},
			{},
				[&time_in, &fup, &visited](const custom_containers::LineOfEnumeratedGraph line) {
				auto[from, to, line_id] = line;
				if (visited[to]) {
					fup[from] = std::min(fup[from], time_in[to]);  // при повторном попадании в вершину по обратному ребру fup должен быть пересчитан
				}
			},
				[&bridges, &time_in, &fup](const custom_containers::LineOfEnumeratedGraph line) {
				auto[from, to, line_id] = line;
				fup[from] = std::min(fup[from], fup[to]);  // распространяем значение fup, полученное при обнаружении обратного ребра
				if (fup[to] > time_in[from]) {  // мост ли это?
					bridges.push_back(line);
				}
			},
				visited);
		}

		return bridges;
	}
}

int main(int argc, char* argv[]) {
	std::ifstream in;
	try {
		in.open("bridges.in");
	}
	catch (...) {
		return 1;
	}
	size_t n{ 0 };
	size_t m{ 0 };
	in >> n >> m;
	custom_containers::UnweighedEnumeratedGraph graph(n);
	for (size_t i = 0; i < m; ++i) {
		size_t from{ 0 }, to{ 0 };
		in >> from >> to;
		graph[from - 1].emplace_back(to - 1 , i + 1);
		graph[to - 1].emplace_back(from - 1, i + 1);
	}
	in.close();
	auto bridges = custom_algorithms::findBridgesInUnweighedGraph(graph);
	std::ofstream out;
	try {
		out.open("bridges.out");
	}
	catch (...) {
		return 1;
	}
	out << bridges.size() << std::endl;
	std::vector<size_t> bridgesNo;
	for (auto& bridge : bridges) {
		auto[from, to, line_id] = bridge;
		bridgesNo.push_back(line_id);
	}
	std::sort(bridgesNo.begin(), bridgesNo.end());
	for (auto bridgeNo: bridgesNo)
	{
		out << bridgeNo << " ";
	}
	out.close();
	return 0;
}
