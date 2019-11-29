//Huffman

#include "Huffman.h"
#include <vector>
#include <array>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <tuple>

namespace custom_containers {
  class CodeTree {
    /*
     * Упорядоченное Дерево
     * Будем говорить, что дерево обладает свойством упорядоченности, если его узлы могут быть перечислены в порядке возрастания веса и в этом перечислении каждый узел находится рядом со своим братом. 
     */
  public:
    CodeTree() {
      data_.emplace_back();
    }


    void insert(const byte value) {
      /*
       * Для каждого передающегося символа передатчик и приёмник выполняют процедуру обновления:
       *	Если текущий символ является не встречавшимся — добавить к NYT два дочерних узла: один для следующего NYT, другой для символа. Увеличить вес нового листа и старого NYT и переходить к шагу 4. Если текущий символ является не NYT, перейти к листу символа.
       *	Если этот узел не имеет наибольший вес в блоке, поменять его с узлом, имеющим наибольшее число, за исключением, если этот узел является родительским элементом[3]
       *	Увеличение веса для текущего узла
       *	Если это не корневой узел зайти в родительский узел затем перейдите к шагу 2. Если это корень, окончание.
       */
      size_t current = 0;
      if (usedBytes_[value].first) {
        while (data_[current].value != value) {
          current += 1;
        }
        data_[current].weight += 1;
        current = repairSortingForElement(current);
      }
      else {
        data_.emplace_back();
        data_.emplace_back();

        data_[data_.size() - 1].parent = data_.size() - 3;

        data_[data_.size() - 2].value = value;
        data_[data_.size() - 2].parent = data_.size() - 3;
        data_[data_.size() - 2].weight += 1;

        data_[data_.size() - 3].leftChild = data_.size() - 1;
        data_[data_.size() - 3].rightChild = data_.size() - 2;

        current = data_.size() - 2;

        usedBytes_[value].first = true;
        usedBytes_[value].second = current;
      }

      while (data_[current].parent != current) {
        data_[data_[current].parent].weight += 1;
        current = data_[current].parent;
        if (data_[current].parent == current) {
          break;
        }
        current = repairSortingForElement(current);
      }
      assert(isSortedTree());
    }


    bool isSortedTree() {
      if (data_.size() <= 1)
        return true;
      for (size_t i = 1; i < data_.size(); ++i) {
        if (data_[i].leftChild != 0 || data_[i].rightChild != 0) {
          //У каждого узла есть родной брат
          if (!(data_[i].leftChild != 0 && data_[i].rightChild != 0)) {
            return false;
          }
          //Вес промежуточного узла равен сумме весов детей
          if (data_[i].weight != data_[data_[i].leftChild].weight + data_[data_[
            i].rightChild].weight) {
            return false;
          }
        }
        //Узлы с высокими весами имеют высокие порядки. Все узлы одного веса хранятся последовательно.
        if (data_[i].weight > data_[i - 1].weight) {
          return false;
        }
        if (!data_[i].isNyt() && data_[i].isLeaf()) {
          //Листы содержат символ
          if (!usedBytes_[data_[i].value].first)
            return false;
          //Информация о позиции узла верная
          if (usedBytes_[data_[i].value].second != i)
            return false;
        }

      }
      return true;
    }


	static std::vector<bool> getEOFCode() {
      return std::vector<bool>(9, false);
    }


    std::vector<bool> getNytCode() {
      std::vector<bool> code;
      auto current = data_.size() - 1;
      while (!isRoot(current)) {
        if (isLeftChild(current)) {
          code.push_back(false);
        }
        else {
          code.push_back(true);
        }
        current = data_[current].parent;
      }
      std::reverse(code.begin(), code.end());
      return code;
    }


    std::vector<bool> getCode(const byte value) {
      std::vector<bool> code;
      if (usedBytes_[value].first) {
        auto current = usedBytes_[value].second;
        while (!isRoot(current)) {
          if (isLeftChild(current)) {
            code.push_back(false);
          }
          else {
            code.push_back(true);
          }
          current = data_[current].parent;
        }
        std::reverse(code.begin(), code.end());
      }
      else {
        code = getNytCode();
        std::vector<bool> mainCode;
        for (auto i = 0; i < 8; ++i) {
          if (static_cast<bool>((value >> i) & 1)) {
            mainCode.push_back(true);
          }
          else {
            mainCode.push_back(false);
          }
        }
        std::reverse(mainCode.begin(), mainCode.end());
        code.insert(code.end(), mainCode.begin(), mainCode.end());
      }

      return code;
    }


    std::vector<bool> encode(const byte value) {
      auto code = getCode(value);
      insert(value);
      assert(usedBytes_[value].first);
      return code;
    }


    std::tuple<byte, size_t> getValue(const std::vector<bool>& code) {
      size_t current = 0;
      size_t index = 0;
      while (!data_[current].isLeaf()) {
        assert(index < code.size());
        if (code[index++]) {
          current = data_[current].rightChild;
        }
        else {
          current = data_[current].leftChild;
        }
      }
      if (!data_[current].isNyt()) {
        return {data_[current].value, index};
      }
      if (code.size()<index+8) {
        return { 0, 0 };
      }
      byte value = 0;
      std::vector<bool> reversedCode(code.begin() + index, code.begin() + index + 8);
      std::reverse(reversedCode.begin(), reversedCode.end());
      for (size_t i = 0; i < reversedCode.size(); ++i) {
        value |= static_cast<byte>(reversedCode[i]) << (i);
      }
      return {value, index + 8};
    }


    std::tuple<byte, size_t> decode(const std::vector<bool>& code) {
	  byte value{ 0 };
	  size_t shift{ 0 };
      std::tie(value, shift) = getValue(code);
      insert(value);
      assert(usedBytes_[value].first);
      return {value, shift};
    }


    friend bool operator==(const CodeTree& lhs, const CodeTree& rhs) {
      return lhs.data_ == rhs.data_
        && lhs.usedBytes_ == rhs.usedBytes_;
    }


    friend bool operator!=(const CodeTree& lhs, const CodeTree& rhs) {
      return !(lhs == rhs);
    }


  private:
    struct Node {
      size_t parent{0};
      size_t leftChild{0};
      size_t rightChild{0};
      size_t weight{0};
      byte value{0};


      [[nodiscard]] bool isLeaf() const {
        return leftChild == 0 && rightChild == 0;
      }


      [[nodiscard]] bool isNyt() const {
        return isLeaf() && weight == 0;
      }


      void changeChild(size_t oldChild, size_t newChild) {
        if (leftChild == oldChild) {
          leftChild = newChild;
        }
        else if (rightChild == oldChild) {
          rightChild = newChild;
        }
      }


      friend bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.parent == rhs.parent
          && lhs.leftChild == rhs.leftChild
          && lhs.rightChild == rhs.rightChild
          && lhs.weight == rhs.weight
          && lhs.value == rhs.value;
      }


      friend bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
      }
    };


    void swapNode(const size_t lhs, const size_t rhs) {
      assert(data_[lhs].parent != rhs);
      assert(data_[rhs].parent != lhs);
      std::swap(data_[lhs], data_[rhs]);
      if (data_[lhs].parent != data_[rhs].parent) {
        data_[data_[lhs].parent].changeChild(lhs, rhs);
        data_[data_[rhs].parent].changeChild(rhs, lhs);
      }
      std::swap(data_[lhs].parent, data_[rhs].parent);
      if (!data_[lhs].isLeaf()) {
        data_[data_[lhs].leftChild].parent = lhs;
        data_[data_[lhs].rightChild].parent = lhs;
      }
      if (!data_[rhs].isLeaf()) {
        data_[data_[rhs].leftChild].parent = rhs;
        data_[data_[rhs].rightChild].parent = rhs;
      }
      if (data_[lhs].isLeaf()) {
        usedBytes_[data_[lhs].value].second = lhs;
      }
      if (data_[rhs].isLeaf()) {
        usedBytes_[data_[rhs].value].second = rhs;
      }
    }


    bool isRoot(const size_t index) {
      return data_[index].parent == index;
    }


    bool isLeftChild(const size_t index) {
      return data_[data_[index].parent].leftChild == index;
    }


    bool isRightChild(const size_t index) {
      return data_[data_[index].parent].rightChild == index;
    }


    size_t repairSortingForElement(const size_t index) {
      if (index == 0) {
        return index;
      }
      auto indexCompare = index - 1;
      if (data_[index].parent == indexCompare) {
        if (indexCompare == 0) {
          return index;
        }
        --indexCompare;
      }
      if (data_[index].weight > data_[indexCompare].weight) {
        auto i = indexCompare;
        while (!(data_[i - 1].weight > data_[i].weight) && i != 0) {
          i -= 1;
        }
        if (i == 0) {
          return index;
        }
        swapNode(i, index);
        return i;
      }
      return index;
    }


    std::vector<Node> data_;
    std::array<std::pair<bool, size_t>, 256> usedBytes_{{{false, 0}}};
  };
}

namespace custom_algorithms {
  class HuffmanEncoder {
  public:
    [[nodiscard]] bool IsEmpty() const {
      return buffer_.empty();
    }

    [[nodiscard]] size_t Size() const {
      return buffer_.size()/8;
    }

    void Push(const byte value) {
      auto code = code_tree_.encode(value);
      buffer_.insert(buffer_.end(), code.begin(), code.end());
    }

    byte Pop(const bool force=false) {
      std::vector<bool> code;
      if (buffer_.size() > 8) {
        code = std::vector<bool>(buffer_.begin(), buffer_.begin() + 8);
        buffer_.erase(buffer_.begin(), buffer_.begin() + 8);
        std::reverse(code.begin(), code.end());
      }
      else if (force){
        code = std::vector<bool>(buffer_.begin(), buffer_.end());
        buffer_.erase(buffer_.begin(), buffer_.end());
        while(code.size()<8) {
          code.push_back(false);
        }
        std::reverse(code.begin(), code.end());
      }
      byte value = 0;
      for (size_t i = 0; i < code.size(); ++i) {
        value |= static_cast<byte>(code[i]) << (i);
      }
      return value;
    }

    void Eof() {
      auto code = code_tree_.getNytCode();
      buffer_.insert(buffer_.end(), code.begin(), code.end());
      //code = code_tree_.getEOFCode();
      //buffer_.insert(buffer_.end(), code.begin(), code.end());
    }

  private:
    std::vector<bool> buffer_;
    custom_containers::CodeTree code_tree_;
  };

  class HuffmanDecoder {
  public:
    [[nodiscard]] bool IsEmpty() const {
      return buffer_.empty();
    }

    [[nodiscard]] size_t Size() const {
      return buffer_.size() / 8;
    }

    void Push(const byte value) {
      std::vector<bool> code;
      for (auto i = 0; i < 8; ++i) {
        if (static_cast<bool>((value >> i) & 1)) {
          code.push_back(true);
        }
        else {
          code.push_back(false);
        }
      }
      std::reverse(code.begin(), code.end());
      buffer_.insert(buffer_.end(), code.begin(), code.end());
    }

    byte Pop() {
		byte value{ 0 };
		size_t shift{ 0 };
		std::tie(value, shift) = code_tree_.decode(buffer_);
      if (shift==0) {
        buffer_.clear();
        return 0;
      }
      buffer_.erase(buffer_.begin(), buffer_.begin() + shift);
      return value;
    }

    [[nodiscard]] bool IsEof() const {
      return eof_;
    }

  private:
    std::vector<bool> buffer_;
    custom_containers::CodeTree code_tree_;
    bool eof_ = false;
  };
}


void CodeTreeTest_checkSortingWithRepetitions1() {
  custom_containers::CodeTree codeTree;

  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions2() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('a');
  codeTree.insert('r');
  codeTree.insert('d');
  codeTree.insert('v');
}


void CodeTreeTest_checkSortingWithRepetitions3() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions4() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('a');
  codeTree.insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions5() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('c');
  codeTree.insert('c');
  codeTree.insert('c');
  codeTree.insert('c');
}


void CodeTreeTest_checkSortingWithoutRepetitions1() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('c');
  codeTree.insert('d');
}


void CodeTreeTest_checkSortingWithoutRepetitions2() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert('c');
  codeTree.insert('d');
  codeTree.insert('e');
  codeTree.insert('f');
}


void CodeTreeTest_insertNull() {
  custom_containers::CodeTree codeTree;
  codeTree.insert('a');
  codeTree.insert('b');
  codeTree.insert(0);
}


void CodeTreeTest_randomSmallBytes() {
  custom_containers::CodeTree codeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 10; ++i) {
    const byte value = rand() % 5;
    codeTree.insert(value);
  }
}


void CodeTreeTest_randomMediumBytes() {
  custom_containers::CodeTree codeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 1000; ++i) {
    const byte value = rand() % 64;
    codeTree.insert(value);
  }
}


void CodeTreeTest_randomLargeBytes() {
  custom_containers::CodeTree codeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 10000; ++i) {
    const byte value = rand() % 256;
    codeTree.insert(value);
  }
}


void CodeTreeTest_encodeAndDecodeStringWithoutRepetitions() {
  std::string str = "abcdefgh";
  custom_containers::CodeTree encodeTree;
  custom_containers::CodeTree decodeTree;
  for (auto ch : str) {
    const byte value = static_cast<byte>(ch);
    auto code = encodeTree.encode(value);
	byte decodedValue{ 0 };
	size_t shiftBytes{ 0 };
	std::tie(decodedValue, shiftBytes) = decodeTree.decode(code);
    assert(encodeTree == decodeTree);
    assert(value == decodedValue);
    assert(shiftBytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeStringWithRepetitions() {
  std::string str = "aabbcc";
  custom_containers::CodeTree encodeTree;
  custom_containers::CodeTree decodeTree;
  for (auto ch : str) {
    const byte value = static_cast<byte>(ch);
    auto code = encodeTree.encode(value);
	byte decodedValue{ 0 };
	size_t shiftBytes{ 0 };
	std::tie(decodedValue, shiftBytes) = decodeTree.decode(code);
    assert(encodeTree == decodeTree);
    assert(value == decodedValue);
    assert(shiftBytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomSmallBytes() {
  custom_containers::CodeTree encodeTree;
  custom_containers::CodeTree decodeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 10; ++i) {
    const byte value = rand() % 5;
    auto code = encodeTree.encode(value);
	byte decodedValue{ 0 };
	size_t shiftBytes{ 0 };
	std::tie(decodedValue, shiftBytes) = decodeTree.decode(code);
    assert(encodeTree == decodeTree);
    assert(value == decodedValue);
    assert(shiftBytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomMediumBytes() {
  custom_containers::CodeTree encodeTree;
  custom_containers::CodeTree decodeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 1000; ++i) {
    const byte value = rand() % 64;
    auto code = encodeTree.encode(value);
	byte decodedValue{ 0 };
	size_t shiftBytes{ 0 };
	std::tie(decodedValue, shiftBytes) = decodeTree.decode(code);
    assert(encodeTree == decodeTree);
    assert(value == decodedValue);
    assert(shiftBytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomLargeBytes() {
  custom_containers::CodeTree encodeTree;
  custom_containers::CodeTree decodeTree;
  srand(time(nullptr));
  for (auto i = 0; i < 10000; ++i) {
    const byte value = rand() % 256;
    auto code = encodeTree.encode(value);
	byte decodedValue{ 0 };
	size_t shiftBytes{ 0 };
	std::tie(decodedValue, shiftBytes) = decodeTree.decode(code);
    assert(encodeTree == decodeTree);
    assert(value == decodedValue);
    assert(shiftBytes == code.size());
  }
}

void HuffmanTest_encodeAndDecodeStringWithRepetitions() {
  custom_algorithms::HuffmanEncoder encoder;
  std::string str("aabcdbcdefgiuaabcdbcdefgiuip;lhigkfhkjghfjfgaabcdbcdefgiu");
  for (auto ch: str) {
    encoder.Push(static_cast<byte>(ch));
  }
  encoder.Eof();
  std::vector<byte> encoded;
  while(!encoder.IsEmpty()) {
    encoded.push_back(encoder.Pop(true));
  }
  custom_algorithms::HuffmanDecoder decoder;
  for (auto ch : encoded){
    decoder.Push(ch);
  }
  std::string decoded;
  while (!decoder.IsEmpty()) {
    decoded.push_back(static_cast<char>(decoder.Pop()));
  }
  decoded.erase(decoded.size()-1);
  assert(str == decoded);
}


void HuffmanTest_encodeAndDecodeRandomBytes() {
  std::vector<byte> original;

  srand(time(nullptr));
  for (auto i = 0; i < 10000; ++i) {
    original.push_back(rand() % 256);
  }

  custom_algorithms::HuffmanEncoder encoder;
  for (auto ch : original) {
    encoder.Push(ch);
  }
  encoder.Eof();
  std::vector<byte> encoded;
  while (!encoder.IsEmpty()) {
    encoded.push_back(encoder.Pop(true));
  }
  custom_algorithms::HuffmanDecoder decoder;
  for (auto ch : encoded) {
    decoder.Push(ch);
  }
  std::vector<byte> decoded;
  while (!decoder.IsEmpty()) {
    decoded.push_back(decoder.Pop());
  }
  decoded.erase(prev(decoded.end()));
  assert(original == decoded);
}


void Encode(IInputStream& original, IOutputStream& compressed) {
  
  custom_algorithms::HuffmanEncoder encoder;
  byte value;
  while (original.Read(value))
  {
    encoder.Push(value);
  }
  encoder.Eof();
  while (!encoder.IsEmpty()) {
    compressed.Write(encoder.Pop(true));
  }
}


void Decode(IInputStream& compressed, IOutputStream& original) {
  custom_algorithms::HuffmanDecoder decoder;
  byte value;
  while (compressed.Read(value)) {
    decoder.Push(value);
  }
  while (true) {
    const auto decoded = decoder.Pop();
    if (decoder.IsEmpty()) {
	    break;
    }
    original.Write(decoded);
  }
}


void StreamsTest_encodeAndDecode() {
	IInputStream inOriginal(kStr);
	IOutputStream outCompressed;
	Encode(inOriginal, outCompressed);
	IInputStream inCompressed(outCompressed.output);
	IOutputStream outOriginal;
	Decode(inCompressed, outOriginal);
	assert(kStr == outOriginal.output);
}


int main(int argc, char* argv[]) {
  const auto is_test = true;
  if (is_test) {
    CodeTreeTest_checkSortingWithRepetitions1();
    CodeTreeTest_checkSortingWithRepetitions2();
    CodeTreeTest_checkSortingWithRepetitions3();
    CodeTreeTest_checkSortingWithRepetitions4();
    CodeTreeTest_checkSortingWithRepetitions5();
    CodeTreeTest_checkSortingWithoutRepetitions1();
    CodeTreeTest_checkSortingWithoutRepetitions2();
    CodeTreeTest_insertNull();
    CodeTreeTest_randomSmallBytes();
    CodeTreeTest_randomMediumBytes();
    CodeTreeTest_randomLargeBytes();
    CodeTreeTest_encodeAndDecodeStringWithoutRepetitions();
    CodeTreeTest_encodeAndDecodeStringWithRepetitions();
    CodeTreeTest_encodeAndDecodeRandomSmallBytes();
    CodeTreeTest_encodeAndDecodeRandomMediumBytes();
    CodeTreeTest_encodeAndDecodeRandomLargeBytes();
    HuffmanTest_encodeAndDecodeStringWithRepetitions();
    HuffmanTest_encodeAndDecodeRandomBytes();
	StreamsTest_encodeAndDecode();
  }

  return 0;
}

