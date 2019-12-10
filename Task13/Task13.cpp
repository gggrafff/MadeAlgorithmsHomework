/*
 * Задача 13 (5 баллов)
 * «Пятнашки»
 * Написать алгоритм для решения игры в “пятнашки”. Решением задачи является приведение к виду:
 * [ 1  2  3  4 ]
 * [ 5  6  7  8 ]
 * [ 9  10 11 12]
 * [ 13 14 15 0 ]
 * где 0 задает пустую ячейку.
 * Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.
 * Формат входных данных
 * Начальная расстановка.
 * Формат выходных данных
 * Если решение существует, то в первой строке выходного файла выведите минимальное число перемещений костяшек, которое нужно сделать, 
 * чтобы достичь выигрышной конфигурации, а во второй строке выведите соответствующую последовательность ходов: 
 * L означает, что костяшка сдвинулась влево, R – вправо, U – вверх, D – вниз. Если таких последовательностей несколько, то выведите любую из них. 
 * Если же выигрышная конфигурация недостижима, то выведите в выходной файл одно число −1.
 * in
 * 1 2 3 4
 * 5 6 7 8
 * 9 10 11 0
 * 13 14 15 12
 * out
 * 1
 * U
 */
/*
 * Информация:
 * https://www.pvsm.ru/java/16174
 * https://ru.wikipedia.org/wiki/Игра_в_15
 * https://ru.wikipedia.org/wiki/%D0%98%D0%BD%D1%84%D0%BE%D1%80%D0%BC%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%BD%D1%8B%D0%B9_%D0%BC%D0%B5%D1%82%D0%BE%D0%B4_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0#IDA*
 */

#include <utility>
#include <vector>
#include <queue>
#include <functional>
#include <fstream>
#include <cassert>
#include <iostream>
#include <string>

namespace  custom_containers {

}

namespace tag {
  using Field15 = uint64_t;
  using Path15 = std::string;
  const uint8_t kDimension = 4;
  const uint8_t kMaxNumberSteps = 80;

  std::vector<uint8_t> Field15ToBeads(const Field15 field) {
    std::vector<uint8_t> beads;
    for (auto i = 0; i < kDimension; i++) {
      for (auto j = 0; j < kDimension; j++) {
        const auto coordinate = kDimension * kDimension - 1 - i * kDimension - j;
        beads.push_back((field & (0x0fULL << (coordinate * kDimension))) >> (coordinate * kDimension));
      }
    }
    assert(beads.size() == kDimension * kDimension);
    return beads;
  }

  Field15 BeadsToField15(std::vector<uint8_t> beads) {
    assert(beads.size() == kDimension * kDimension);
    Field15 field{ 0 };
    for (size_t i = 0; i < beads.size();++i) {
      field |= static_cast<uint64_t>(beads[i]) << (4 * (15 - i));
    }
    return field;
  }

  bool IsSolvable15(const std::vector<uint8_t>& beads) {
    uint16_t inversions_number = 0;
    for (size_t i = 0; i < beads.size(); ++i) {
      if (beads[i] == 0) {
        inversions_number += i / 4 + 1;
      }
      for (size_t j = i + 1; j < beads.size(); ++j) {
        if (beads[j] < beads[i] && beads[j] != 0)
          inversions_number += 1;
      }
    }
    return inversions_number % 2 == 0;
  }

  bool IsSolvable15(const Field15 field) {
    return IsSolvable15(Field15ToBeads(field));
  }

  bool IsGood(const Field15 field) {
    return field == 0x123456789ABCDEF0;
  }


  uint64_t SwapBeads(uint64_t field, const int x1, const int y1, const int x2, const int y2) {  //  в этом методе меняем два поля местами
    if (x2 > -1 && x2 < kDimension && y2 > -1 && y2 < kDimension) {
      auto coord1 = kDimension * kDimension - 1 - x1 - y1 * kDimension;
      auto coord2 = kDimension * kDimension - 1 - x2 - y2 * kDimension;
      if (coord1 > coord2) std::swap(coord1, coord2);
      field ^= (field & (0x0fULL << (coord1 * kDimension))) << ((coord2 - coord1) * kDimension);
      field ^= (field & (0x0fULL << (coord2 * kDimension))) >> ((coord2 - coord1) * kDimension);
      field ^= (field & (0x0fULL << (coord1 * kDimension))) << ((coord2 - coord1) * kDimension);
      return field;
    }
    return 0;
  }

  std::tuple<uint16_t, uint8_t, uint8_t> CalculateParametersFieldSimple(const Field15 field) {
    uint16_t inverse_quality = 0;
    uint8_t zero_x{ 0 }, zero_y{ 0 };
    for (auto i = 0; i < kDimension; i++) {  //  в этом цикле определяем координаты нуля и вычисляем h(x)
      for (auto j = 0; j < kDimension; j++) {
        const auto coord = kDimension * kDimension - 1 - i * kDimension - j;
        if ((field & (0x0fULL << (coord * kDimension))) != (uint64_t(i * kDimension + j + 1) << (coord * kDimension)) &&
          (field & (0x0fULL << (coord * kDimension))) != 0) {  // если 0 не на своем месте - не считается
          inverse_quality += 1;
        }
        if ((field & (0x0fULL << (coord * kDimension))) == 0) {
          zero_x = j;
          zero_y = i;
        }
      }
    }
    return { inverse_quality, zero_x, zero_y };
  }

  std::tuple<uint16_t, uint8_t, uint8_t> CalculateParametersFieldSmart(const Field15 field) {
	  uint16_t inverse_quality = 0;
	  uint8_t zero_x{ 0 }, zero_y{ 0 };
	  for (auto i = 0; i < kDimension; i++) {  //  в этом цикле определяем координаты нуля и вычисляем h(x)
		  for (auto j = 0; j < kDimension; j++) {
			  const auto coord = kDimension * kDimension - 1 - i * kDimension - j;
			  const auto value = (field & (0x0fULL << (coord * kDimension))) >> (coord * kDimension);
			  if ((field & (0x0fULL << (coord * kDimension))) == 0) {
				  zero_x = j;
				  zero_y = i;
				  inverse_quality += (3 - zero_x) + (3 - zero_y);
				  continue;
			  }
			  inverse_quality += std::abs(int(value-1)%4 - j) + std::abs(int(value - 1) / 4 - i);
		  }
	  }
	  return { inverse_quality, zero_x, zero_y };
  }

  struct Board {
    Path15 path;
    Field15 field;
    uint32_t priority{ 0 };
    uint8_t zero_x{ 0 };
    uint8_t zero_y{ 0 };


    explicit Board(const Field15 field) {
      this->field = field;
      auto [inverse_quality, x, y] = tag::CalculateParametersFieldSmart(field);
      zero_x = x;
      zero_y = y;
      priority = inverse_quality + path.size()-1;
    }

    Board(const Path15& oldPath, const char action, const Field15 field) {
      path = oldPath;
      path.push_back(action);
      this->field = field;
      auto [inverse_quality, x, y] = tag::CalculateParametersFieldSmart(field);
      zero_x = x;
      zero_y = y;
      priority = inverse_quality + path.size()-1;
    }

    [[nodiscard]] bool IsGood() const {
      return tag::IsGood(field);
    }


    [[nodiscard]] std::vector<Board> CreateNeighbors() const {  // все соседние позиции
        // меняем ноль с соседней клеткой, то есть всего 4 варианта
        // если соседнего нет (0 может быть с краю), chng(...) вернет null
      std::vector<Board> board_list;

      auto new_field = SwapBeads(field, zero_x, zero_y, zero_x, zero_y + 1);
      if (new_field != 0 && (path.empty() || path.back()!='D'))
        board_list.emplace_back(path, 'U', new_field);

      new_field = SwapBeads(field, zero_x, zero_y, zero_x, zero_y - 1);
      if (new_field != 0 && (path.empty() || path.back() != 'U'))
        board_list.emplace_back(path, 'D', new_field);

      new_field = SwapBeads(field, zero_x, zero_y, zero_x - 1, zero_y);
      if (new_field != 0 && (path.empty() || path.back() != 'L'))
        board_list.emplace_back(path, 'R', new_field);

      new_field = SwapBeads(field, zero_x, zero_y, zero_x + 1, zero_y);
      if (new_field != 0 && (path.empty() || path.back() != 'R'))
        board_list.emplace_back(path, 'L', new_field);

      return board_list;
    }

	  template<typename CollectionType>
	  void CreateNeighbors(CollectionType& collection) const {  // все соседние позиции
		// меняем ноль с соседней клеткой, то есть всего 4 варианта
		// если соседнего нет (0 может быть с краю), chng(...) вернет null

		  auto new_field = SwapBeads(field, zero_x, zero_y, zero_x, zero_y + 1);
		  if (new_field != 0 && (path.empty() || path.back() != 'D'))
			  collection.emplace(path, 'U', new_field);

		  new_field = SwapBeads(field, zero_x, zero_y, zero_x, zero_y - 1);
		  if (new_field != 0 && (path.empty() || path.back() != 'U'))
			  collection.emplace(path, 'D', new_field);

		  new_field = SwapBeads(field, zero_x, zero_y, zero_x - 1, zero_y);
		  if (new_field != 0 && (path.empty() || path.back() != 'L'))
			  collection.emplace(path, 'R', new_field);

		  new_field = SwapBeads(field, zero_x, zero_y, zero_x + 1, zero_y);
		  if (new_field != 0 && (path.empty() || path.back() != 'R'))
			  collection.emplace(path, 'L', new_field);
	  }
  };

  Path15 Solve15(
    const Field15 start
    ) {
    auto comp = [](const Board & lhs, const Board & rhs) {
      return lhs.priority > rhs.priority;
    };
    std::priority_queue<Board, std::vector<Board>, decltype(comp)> elements_for_processing(comp);
    elements_for_processing.push(Board(start));

    while (!elements_for_processing.empty()) {
      const auto current_element = elements_for_processing.top();
      elements_for_processing.pop();

      if (current_element.IsGood()) {
        return current_element.path;
      }
	  if (current_element.path.size() == kMaxNumberSteps)
		  continue;
      current_element.CreateNeighbors(elements_for_processing);
      /*for (auto& neighbor : neighbors) {
          elements_for_processing.push(neighbor);
      }*/
    }
    return {};
  }

}

void BeadsToField15Tests() {
  std::vector<uint8_t> beads{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
  auto field = tag::BeadsToField15(beads);
  assert(field == 0x123456789ABCDEF0ULL);

  beads = std::vector<uint8_t>{ 4, 5, 6, 1, 2, 3, 10, 11, 12, 7, 8, 9, 13, 14, 15, 0 };
  field = tag::BeadsToField15(beads);
  assert(field == 0x456123ABC789DEF0ULL);
}

void Field15ToBeadsTests() {
  auto field = 0x123456789ABCDEF0ULL;
  auto beads = tag::Field15ToBeads(field);
  std::vector<uint8_t> true_result{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0 };
  assert(beads == true_result);

  field = 0x456123ABC789DEF0ULL;
  beads = tag::Field15ToBeads(field);
  true_result = std::vector<uint8_t>{ 4, 5, 6, 1, 2, 3, 10, 11, 12, 7, 8, 9, 13, 14, 15, 0 };
  assert(beads == true_result);
}

void IsSolvable15Tests() {
  auto field = 0x123456789ABCDEF0ULL;
  auto solvable = tag::IsSolvable15(field);
  assert(solvable == true);

  field = 0x123456789ABCDFE0ULL;
  solvable = tag::IsSolvable15(field);
  assert(solvable == false);
}

void SwapBeadsTests() {
  const auto field1 = 0x123456789ABCDEF0ULL;
  const auto field2 = tag::SwapBeads(field1, 3, 3, 2, 3);
  assert(field2 == 0x123456789ABCDE0FULL);
  const auto field3 = tag::SwapBeads(field2, 2, 3, 2, 2);
  assert(field3 == 0x123456789A0CDEBFULL);
}

void CalculateParametersFieldTest() {
  const auto field1 = 0x123456789ABCDEF0ULL;
  auto [q1, x1, y1] = tag::CalculateParametersFieldSimple(field1);
  assert(q1 == 0);
  assert(x1 == 3);
  assert(y1 == 3);
  const auto field2 = 0x123456789ABCDE0FULL;
  auto [q2, x2, y2] = tag::CalculateParametersFieldSimple(field2);
  assert(q2 == 1);
  assert(x2 == 2);
  assert(y2 == 3);
  const auto field3 = 0x123456789A0CDEBFULL;
  auto [q3, x3, y3] = tag::CalculateParametersFieldSimple(field3);
  assert(q3 == 2);
  assert(x3 == 2);
  assert(y3 == 2);
}

void CreateNeighborsTests() {
  const auto field1 = 0x123456789ABCDEF0ULL;
  const auto neighbors1 = tag::Board(field1).CreateNeighbors();
  assert(neighbors1.size() == 2);
  const auto field2 = 0x123456789ABCDE0FULL;
  const auto neighbors2 = tag::Board(field2).CreateNeighbors();
  assert(neighbors2.size() == 3);
  const auto field3 = 0x123456789A0CDEBFULL;
  const auto neighbors3 = tag::Board(field3).CreateNeighbors();
  assert(neighbors3.size() == 4);
}

int main(int argc, char* argv[]) {
  const auto test = true;
  if (test) {
    BeadsToField15Tests();
    Field15ToBeadsTests();
    IsSolvable15Tests();
    SwapBeadsTests();
    CalculateParametersFieldTest();
    CreateNeighborsTests();
  }

  std::ifstream in;
  try {
    in.open("input.txt");
  }
  catch (...) {
    return 1;
  }
  std::vector<uint8_t> beads;
  for (size_t i = 0; i < 16; ++i) {
    uint16_t bead{ 0 };
    in >> bead;
    beads.push_back(bead);
  }
  if (!tag::IsSolvable15(beads)) {
    std::cout << "-1";
    return 0;
  }
  const auto solving = tag::Solve15(tag::BeadsToField15(beads));
  std::cout << solving.size() << std::endl;
  std::cout << solving;
  return 0;
}
