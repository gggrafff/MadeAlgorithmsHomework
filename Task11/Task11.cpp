#include <vector>
#include <functional>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>

namespace  custom_containers {
  using UnweighedGraphNode = std::vector<size_t>;
  using UnweighedGraph = std::vector<UnweighedGraphNode>;
}

namespace custom_algorithms {
	/**
   * \brief Обход невзвешенного графа в ширину. 
   * \param graph Список смежности невзвешенного графа.
   * \param start Индекс узла, с которого начинаем обход.
   * \param action Действие, которое необходимо произвести с текущим узлом.
   * \param stop_condition Условие остановки обхода.
   */
  void Bfs(
    const custom_containers::UnweighedGraph& graph,
    const size_t start, 
    const std::function<void(const size_t current_element, const std::vector<bool>& visited)>& action,
    const std::function<bool(const size_t current_element)>& stop_condition) {
    std::queue<size_t> elements_for_processing;
    std::vector<bool> visited(graph.size(), false);
    elements_for_processing.push(start);
    visited[start] = true;

    while (!elements_for_processing.empty()) {
      const auto current_element = elements_for_processing.front();
      elements_for_processing.pop();

      action(current_element, visited);
      if (stop_condition(current_element)) {
        break;
      }

      for(auto& node: graph[current_element]) {
        if (!visited[node]) {
          elements_for_processing.push(node);
          visited[node] = true;
        }
      }
    }
  }

	/**
   * \brief Поиск кратчайшего пути между вершинами в невзвешенном графе. 
   * \param graph Список смежности невзвешенного графа.
   * \param start Вершина, из которой ищем путь.
   * \param finish Вершина, в которую ищем путь.
   * \return Массив индексов узлов, через которые прошёл путь.
   */
  std::vector<size_t> FindPathInUnweighedGraph(
    const custom_containers::UnweighedGraph& graph,
    const size_t start,
    const size_t finish) {
    std::vector<size_t> parents(graph.size(), 0);

    Bfs(
      graph, 
      start,
      [&graph, &parents, start](const size_t current_element, const std::vector<bool> & visited) {
        for (auto& node : graph[current_element]) {
          if (!visited[node] && node != start && node!=current_element) {
            parents[node] = current_element;
          }
        }
      },
      [finish](const size_t current_element) {
        return current_element == finish;
      });

    std::vector<size_t> path;
    auto current_element = finish;
    while(parents[current_element]!=current_element) {
      path.push_back(parents[current_element]);
      current_element = parents[current_element];
    }
    std::reverse(path.begin(), path.end());
    return path;
  }

	/**
   * \brief Поиск количества кратчайших путей между двумя вершинами в невзвешенном графе.
   * \param graph Список смежности невзвешенного графа.
   * \param start  Вершина, из которой ищем пути.
   * \param finish Вершина, в которую ищем пути.
   * \return Количество кратчайших путей.
   */
  size_t FindNumberPathsInUnweighedGraph(
    const custom_containers::UnweighedGraph& graph,
    const size_t start,
    const size_t finish) {
    std::vector<size_t> paths_counts(graph.size(), 0);
    std::vector<size_t> distances(graph.size(), 0);
	paths_counts[start] = 1;
    Bfs(
      graph, 
      start, 
      [&graph, &distances, &paths_counts](const size_t current_element, const std::vector<bool> & visited) {
        for (auto& node : graph[current_element]) {
          if (!visited[node]) {
            distances[node] = distances[current_element] + 1;
            paths_counts[node] += paths_counts[current_element];
          }
          else if(distances[node] == distances[current_element] + 1) {
            paths_counts[node] += paths_counts[current_element];
          }
        }
      },
      [](size_t) {
        return false;
    });

    return paths_counts[finish];
  }
}

int main(int argc, char* argv[]) {
  std::ifstream in;
  try {
    in.open("input.txt");
  }
  catch (...) {
    return 1;
  }
  size_t v{ 0 };
  size_t n{ 0 };
  in >> v >> n;
  custom_containers::UnweighedGraph graph(v);
  for (size_t i = 0; i < n; ++i) {
    size_t from{ 0 }, to{ 0 };
    in >> from >> to;
    graph[from].push_back(to);
    graph[to].push_back(from);
  }
  size_t start{ 0 };
  size_t finish{ 0 };
  in >> start >> finish;
  const auto number_paths = custom_algorithms::FindNumberPathsInUnweighedGraph(graph, start, finish);
  std::cout << number_paths;
  return 0;
}
