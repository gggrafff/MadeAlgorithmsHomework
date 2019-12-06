#include <vector>
#include <functional>
#include <stack>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <optional>

namespace  custom_containers {
	using UnweighedEnumeratedGraphNode = std::vector<std::pair<size_t, size_t>>; //to, id of line
	using UnweighedEnumeratedGraph = std::vector<UnweighedEnumeratedGraphNode>;
	using LineOfEnumeratedGraph = std::tuple<size_t, size_t, size_t>; //from, to, id of line
}

namespace custom_algorithms {
	std::vector<size_t> dfs(
		const custom_containers::UnweighedEnumeratedGraph& graph,
		const size_t start,
		const std::optional<std::function<void(const size_t currentElement)>>& beforeActionForNode,
		const std::optional<std::function<void(const size_t currentElement)>>& afterActionForNode,
		const std::optional<std::function<void(const custom_containers::LineOfEnumeratedGraph line)>>& beforeActionForLine,
		const std::optional<std::function<void(const custom_containers::LineOfEnumeratedGraph line)>>& afterActionForLine,
		std::vector<bool>& visited) {

		size_t dfs = 0;
		std::stack<std::pair<size_t, size_t>> elementsForProcessing; //номер вершины, номер соседа, на котором остановились
		std::vector<std::pair<size_t, size_t>> parents(graph.size(), { graph.size(), 0 }); //id node, id line
		std::vector<size_t> dfsnumber(graph.size(), 0);
		std::vector<bool> first_input(graph.size(), true);

		visited[start] = true;
		dfsnumber[start] = ++dfs;

		elementsForProcessing.emplace(start, 0); //вершина, начальный сосед
		while (!elementsForProcessing.empty()) {
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

					elementsForProcessing.top().second = currentNeighbor + 1; //запомним, что когда вернёмся в эту вершину, нужно продолжить со следующего соседа
					elementsForProcessing.emplace(nodeNumber.first, 0); //новая вершина, её начальный сосед
					break; // Прерываем обработку текущей вершины и немедленно переходим к обработке новой вершины
				}
			}
			if (currentNeighbor == graph[currentElement].size()) {
				elementsForProcessing.pop(); //все соседи текущей вершины просмотрены, убираем вершину из стека
				if (afterActionForNode)
					afterActionForNode.value()(currentElement);
				if (afterActionForLine && !elementsForProcessing.empty())
					afterActionForLine.value()({ elementsForProcessing.top().first, currentElement, parents[currentElement].second });
			}
		}
		return dfsnumber;
	}


	std::vector<custom_containers::LineOfEnumeratedGraph> findBridgesInUnweighedGraph(
		const custom_containers::UnweighedEnumeratedGraph& graph) {
		std::vector<size_t> time_in(graph.size(), 0);
		std::vector<size_t> fup(graph.size(), 0);
		std::vector<bool> processed(graph.size(), false);
		size_t timer = 0;
		std::vector<custom_containers::LineOfEnumeratedGraph> bridges;
		for (size_t i = 0; i < graph.size(); ++i) {
			if (!processed[i])
				dfs(
					graph,
					i,
					[&time_in, &fup, &timer](const size_t currentElement) {
				time_in[currentElement] = fup[currentElement] = timer++;
			},
			{},
				[&time_in, &fup, &processed](const custom_containers::LineOfEnumeratedGraph line) {
				auto[from, to, line_id] = line;
				if (processed[to]) {
					fup[from] = std::min(fup[from], time_in[to]);
				}
			},
				[&bridges, &time_in, &fup](const custom_containers::LineOfEnumeratedGraph line) {
				auto[from, to, line_id] = line;
				fup[from] = std::min(fup[from], fup[to]);
				if (fup[to] > time_in[from]) {
					bridges.push_back(line);
				}
			},
				processed);
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
