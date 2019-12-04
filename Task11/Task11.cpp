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
  void bfs(
    const custom_containers::UnweighedGraph& graph,
    const size_t start, 
    const std::function<void(const size_t currentElement, const std::vector<bool>& visited)>& action,
    const std::function<bool(const size_t currentElement)>& stopCondition) {
    std::queue<size_t> elementsForProcessing;
    std::vector<bool> visited(graph.size(), false);
    elementsForProcessing.push(start);
    visited[start] = true;

    while (!elementsForProcessing.empty()) {
      const auto currentElement = elementsForProcessing.front();
      elementsForProcessing.pop();

      action(currentElement, visited);
      if (stopCondition(currentElement)) {
        break;
      }

      for(auto& node: graph[currentElement]) {
        if (!visited[node]) {
          elementsForProcessing.push(node);
          visited[node] = true;
        }
      }
    }
  }


  std::vector<size_t> findPathInUnweighedGraph(
    const custom_containers::UnweighedGraph& graph,
    const size_t start,
    const size_t finish) {
    std::vector<size_t> parents(graph.size(), 0);

    bfs(
      graph, 
      start,
      [&graph, &parents, start](const size_t currentElement, const std::vector<bool> & visited) {
        for (auto& node : graph[currentElement]) {
          if (!visited[node] && node != start && node!=currentElement) {
            parents[node] = currentElement;
          }
        }
      },
      [finish](const size_t currentElement) {
        return currentElement == finish;
      });

    std::vector<size_t> path;
    auto currentElement = finish;
    while(parents[currentElement]!=currentElement) {
      path.push_back(parents[currentElement]);
      currentElement = parents[currentElement];
    }
    std::reverse(path.begin(), path.end());
    return path;
  }


  size_t findNumberPathsInUnweighedGraph(
    const custom_containers::UnweighedGraph& graph,
    const size_t start,
    const size_t finish) {
    std::vector<size_t> pathsCounts(graph.size(), 0);
    std::vector<size_t> distances(graph.size(), 0);
	pathsCounts[start] = 1;
    bfs(
      graph, 
      start, 
      [&graph, &distances, &pathsCounts](const size_t currentElement, const std::vector<bool> & visited) {
        for (auto& node : graph[currentElement]) {
          if (!visited[node]) {
            distances[node] = distances[currentElement] + 1;
            pathsCounts[node] += pathsCounts[currentElement];
          }
          else if(distances[node] == distances[currentElement] + 1) {
            pathsCounts[node] += pathsCounts[currentElement];
          }
        }
      },
      [](size_t) {
        return false;
    });

    return pathsCounts[finish];
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
  const size_t numberPaths = custom_algorithms::findNumberPathsInUnweighedGraph(graph, start, finish);
  std::cout << numberPaths;
  return 0;
}
