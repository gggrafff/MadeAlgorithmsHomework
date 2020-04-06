/*
 * ������ 9. �������� ������ ������ �������� (15 ������)
 * ���������� �������� ���������� ������������ ����������� ���� ��������. 
 * ��� ������ ��������� ���������� ��� ������� ��� �������� ������ �� ������ ����� � ���������� ����� �� ������.
 *
 *
 * // ����� ���������� ������ �� ������ original
 * void Encode(IInputStream& original, IOutputStream& compressed);
 * // ����� ��������������� ������������ ������
 * void Decode(IInputStream& compressed, IOutputStream& original);
 * ���:
 * typedef unsigned char byte;
 *
 * struct IInputStream {
 * 	// ���������� false, ���� ����� ����������
 * 	bool Read(byte& value) = 0;
 * };
 *
 * struct IOutputStream {
 * 	void Write(byte value) = 0;
 * };
 *
 * � ������� ���������� ��������� .cpp ���� ���������� ������� Encode, Decode, � ����� ���������� ���� Huffman.h. ����������� ��������� ������� ������� ��������� �� ����������� ������� ������� ����� � ���������.
 *
 * ������ ������������ �������:
 * #include "Huffman.h"
 *
 * static void copyStream(IInputStream& input, IOutputStream& output)
 * {
 * 	byte value;
 * 	while (input.Read(value))
 * 	{
 * 		output.Write(value);
 * 	}
 * }
 *
 * void Encode(IInputStream& original, IOutputStream& compressed)
 * {
 * 	copyStream(original, compressed);
 * }
 *
 * void Decode(IInputStream& compressed, IOutputStream& original)
 * {
 * 	copyStream(compressed, original);
 * }
 * 
 */

/*
 * ��������� ������� ������� �� ������� �������� ����������� ��������, � ����������.
 * 
 * ��� ��������?
 * ����� ��������: 
 * https://ru.wikipedia.org/wiki/%D0%9A%D0%BE%D0%B4_%D0%A5%D0%B0%D1%84%D1%84%D0%BC%D0%B0%D0%BD%D0%B0
 * https://ru.wikipedia.org/wiki/%D0%90%D0%B4%D0%B0%D0%BF%D1%82%D0%B8%D0%B2%D0%BD%D1%8B%D0%B9_%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A5%D0%B0%D1%84%D1%84%D0%BC%D0%B0%D0%BD%D0%B0
 * ���� ���������:
 * https://mf.grsu.by/UchProc/livak/po/comprsite/theory_huffman.html
 * https://mf.grsu.by/UchProc/livak/po/comprsite/theory_huffman_adapt.html
 * ���������� ��������, ����� ����������� ��������:
 * https://www2.cs.duke.edu/csed/curious/compression/adaptivehuff.html
 * 
 * ���� �� ��������� ����:
 * ����� ������� ����������� ����������� ������ � ���������� ��� ����������� ������, �.�. 
 * ������� ����������� ������������ ������������ ��������, �� ��� ����� ����� �������� ���� ������ ������ � ����������.
 * ��� ����� ���� ����� ���������� �� ��������� �������.
 * ������ � ��������������� ��������� �������� ���������� ��� �����������.
 * 
 */

#include "Huffman.h"
#include <vector>
#include <array>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <tuple>

namespace custom_containers {
	/**
   * \brief ������� ������.
   * \details ==========
   * \details �����������. ������������� ������.
   * \details ����� ��������, ��� ������ �������� ��������� ���������������, ���� ��� ���� ����� ���� ����������� � ������� ����������� ���� � 
   * \details � ���� ������������ ������ ���� ��������� ����� �� ����� ������. 
   * \details �������. �������� ������ �������� ������� ����������� �������� ����� � ������ �����, ����� ��� ������������� �������� ���������������. 
   * \details ���������� �������� ��������������� � �������� ���������� ������ ��������� ��� ���� ��������� � ���, ��� �������� ������, 
   * \details � ������� �� ��������, � ��� ������ ����������� �������� � ��, � ����� ���������� ���� � ������� ������. 
   */
  class CodeTree {
  public:
    CodeTree() {
      data_.emplace_back();
    }

	  /**
     * \brief ������� ������� � ������ ��� ���������� ���� ���������� �������.
     * \details ==========
     * \details ����� ����� � ����������� ������ ������ ������.
     * \details � ���� ��������� ������������ ����������� ���� (���� ��� ��������), NYT (�� ����. not yet transmitted � ��� �� ���������� ������), 
     * \details �� �������� ������� �����, ����� �� �������������, �������.
     * \details ==========
     * \details ��� ������� ������������� ������� ���������� � ������� ��������� ��������� ����������:
     * \details 1. ���� ������� ������ �������� �� ������������� � �������� � NYT ��� �������� ����: ���� ��� ���������� NYT, ������ ��� �������. ��������� ��� ������ ����� � ������� NYT � ���������� � ���� 4. ���� ������� ������ �������� �� NYT, ������� � ����� �������.
     * \details 2. ���� ���� ���� �� ����� ���������� ��� � �����, �������� ��� � �����, ������� ���������� �����, �� �����������, ���� ���� ���� �������� ������������ ���������.
     * \details 3. ���������� ���� ��� �������� ����.
     * \details 4. ���� ��� �� �������� ���� ����� � ������������ ���� ����� ��������� � ���� 2. ���� ��� ������, ���������.
     * \param value ����������� ������.
     */
    void Insert(const byte value) {
      size_t current = 0;
      if (used_bytes_[value].first) {
        while (data_[current].value != value) {
          current += 1;
        }
        data_[current].weight += 1;
        current = RepairSortingForElement(current);
      }
      else {
        data_.emplace_back();
        data_.emplace_back();

        data_[data_.size() - 1].parent = data_.size() - 3;

        data_[data_.size() - 2].value = value;
        data_[data_.size() - 2].parent = data_.size() - 3;
        data_[data_.size() - 2].weight += 1;

        data_[data_.size() - 3].left_child = data_.size() - 1;
        data_[data_.size() - 3].right_child = data_.size() - 2;

        current = data_.size() - 2;

        used_bytes_[value].first = true;
        used_bytes_[value].second = current;
      }

      while (data_[current].parent != current) {
        data_[data_[current].parent].weight += 1;
        current = data_[current].parent;
        if (data_[current].parent == current) {
          break;
        }
        current = RepairSortingForElement(current);
      }
      assert(IsSortedTree());
    }

	  /**
     * \brief ������� �������� ����, ��� ������ ������������� �������� ���������������. ������������ ��� ������� ������ ������. 
     * \return true - ���� ������ �����������, ����� - false.
     */
    bool IsSortedTree() {
      if (data_.size() <= 1)
        return true;
      for (size_t i = 1; i < data_.size(); ++i) {
        if (data_[i].left_child != 0 || data_[i].right_child != 0) {
          //� ������� ���� ���� ������ ����
          if (!(data_[i].left_child != 0 && data_[i].right_child != 0)) {
            return false;
          }
          //��� �������������� ���� ����� ����� ����� �����
          if (data_[i].weight != data_[data_[i].left_child].weight + data_[data_[
            i].right_child].weight) {
            return false;
          }
        }
        //���� � �������� ������ ����� ������� �������. ��� ���� ������ ���� �������� ���������������.
        if (data_[i].weight > data_[i - 1].weight) {
          return false;
        }
        if (!data_[i].IsNyt() && data_[i].IsLeaf()) {
          //����� �������� ������
          if (!used_bytes_[data_[i].value].first)
            return false;
          //���������� � ������� ���� ������
          if (used_bytes_[data_[i].value].second != i)
            return false;
        }

      }
      return true;
    }

	  /**
	 * \brief ��������� ������ ����� ����� (�� ����������� ������������ - ������� �� ������).
	 * \return ������ ���, ��������������� ������� ����� �����.
	 */
	static std::vector<bool> GetEofCode() {
      return std::vector<bool>(9, false);
    }

	  /**
     * \brief ��������� ��� NYT-������� (��������� ����� ��������, ������� ���������� �������).
     * \return ������ ���, ��������������� ������� NYT.
     */
    std::vector<bool> GetNytCode() {
      std::vector<bool> code;
      auto current = data_.size() - 1;
      while (!IsRoot(current)) {
        if (IsLeftChild(current)) {
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

	  /**
     * \brief ��������� ��� �������.
     * \param value ������, ��� �������� ����� ������������ ���.
     * \return ������ ���, ��������������� ���� ����������� �������.
     */
    std::vector<bool> GetCode(const byte value) {
      std::vector<bool> code;
      if (used_bytes_[value].first) {
		//���� ������ ����������, �� �������� �� ������, �������� ���. 
        auto current = used_bytes_[value].second;
        while (!IsRoot(current)) {
          if (IsLeftChild(current)) {
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
		//���� ������ ����������, �� �������� �� ������, �������� ���. 
        code = GetNytCode();
        for (auto i = 0; i < 8; ++i) {
          if (static_cast<bool>((value >> i) & 1)) {
			  code.push_back(true);
          }
          else {
			  code.push_back(false);
          }
        }
      }

      return code;
    }

	  /**
     * \brief ������������ ������ � ��������� ��� ��� � ������.
     * \param value ������, ��� �������� ����� ������������ ���.
     * \return ������ ���, ��������������� ���� ����������� �������.
     */
    std::vector<bool> Encode(const byte value) {
      auto code = GetCode(value);
      Insert(value);
      assert(used_bytes_[value].first);
      return code;
    }

	  /**
     * \brief ��������� ������ �� ��� ����.
     * \param code ������ ���, ��������������� ���� �������.
     * \return ������, ��� �������� ��� �������.
     */
    std::tuple<byte, size_t> GetValue(const std::vector<bool>& code) {
      size_t current = 0;
      size_t index = 0;
      while (!data_[current].IsLeaf()) {
        assert(index < code.size());
        if (code[index++]) {
          current = data_[current].right_child;
        }
        else {
          current = data_[current].left_child;
        }
      }
      if (!data_[current].IsNyt()) {
        return {data_[current].value, index};
      }
      if (code.size()<index+8) {
        return { 0, 0 };
      }
      byte value = 0;
      for (size_t i = 0; i < 8; ++i) {
        value |= static_cast<byte>(code[index + i]) << (i);
      }
      return {value, index + 8};
    }

	  /**
	 * \brief ��������� ������ �� ��� ����.
	 * \param begin_code ��������, ����������� �� ������ ���� �������. 
	 * \param end_code ��������, ����������� �� ����� ���� �������. 
	 * \return ������, ��� �������� ��� �������.
	 */
	std::tuple<byte, size_t> GetValue(std::vector<bool>::const_iterator begin_code, const std::vector<bool>::const_iterator end_code) {
		size_t current = 0;
		size_t index = 0;
		while (!data_[current].IsLeaf()) {
			assert(begin_code != end_code);
			if (*begin_code) {
				current = data_[current].right_child;
			}
			else {
				current = data_[current].left_child;
			}
			std::advance(begin_code, 1);
			index++;
		}
		if (!data_[current].IsNyt()) {
			return { data_[current].value, index };
		}
		if (std::distance(begin_code, end_code) < 8) {
			return { 0, 0 };
		}
		byte value = 0;
		for (size_t i = 0; i < 8; ++i) {
			value |= static_cast<byte>(*next(begin_code, i)) << (i);
		}
		return { value, index + 8 };
	}

	  /**
     * \brief ������������� ������ � ��������� ��� � ������.
     * \param code ������ ��� � ����� �������.
     * \return ������, ��� �������� ��� �������.
     */
    std::tuple<byte, size_t> Decode(const std::vector<bool>& code) {
	  byte value{ 0 };
	  size_t shift{ 0 };
      std::tie(value, shift) = GetValue(code);
      Insert(value);
      assert(used_bytes_[value].first);
      return {value, shift};
    }

	  /**
	 * \brief ������������� ������ � ��������� ��� � ������.
	 * \param begin_code ��������, ����������� �� ������ ���� �������. 
	 * \param end_code ��������, ����������� �� ����� ���� �������. 
	 * \return ������, ��� �������� ��� �������.
	 */
	std::tuple<byte, size_t> Decode(const std::vector<bool>::const_iterator begin_code, const std::vector<bool>::const_iterator end_code) {
		byte value{ 0 };
		size_t shift{ 0 };
		std::tie(value, shift) = GetValue(begin_code, end_code);
		Insert(value);
		assert(used_bytes_[value].first);
		return { value, shift };
	}


    friend bool operator==(const CodeTree& lhs, const CodeTree& rhs) {
      return lhs.data_ == rhs.data_
        && lhs.used_bytes_ == rhs.used_bytes_;
    }


    friend bool operator!=(const CodeTree& lhs, const CodeTree& rhs) {
      return !(lhs == rhs);
    }


  private:
	  /**
     * \brief ���� �������� ������.
     */
    struct Node {
      size_t parent{0};
      size_t left_child{0};
      size_t right_child{0};
      size_t weight{0};
      byte value{0};


      [[nodiscard]] bool IsLeaf() const {
        return left_child == 0 && right_child == 0;
      }

      [[nodiscard]] bool IsNyt() const {
        return IsLeaf() && weight == 0;
      }

	    /**
       * \brief �������� ������� ����.
       * \param old_child ������ ������� �������.
       * \param new_child ������ ������ �������.
       */
      void ChangeChild(const size_t old_child, const size_t new_child) {
        if (left_child == old_child) {
          left_child = new_child;
        }
        else if (right_child == old_child) {
          right_child = new_child;
        }
      }


      friend bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.parent == rhs.parent
          && lhs.left_child == rhs.left_child
          && lhs.right_child == rhs.right_child
          && lhs.weight == rhs.weight
          && lhs.value == rhs.value;
      }


      friend bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
      }
    };


	  /**
     * \brief �������� ���� �������.
     * \param node1 ������ ������� ����.
     * \param node2 ������ ������� ����.
     */
    void SwapNode(const size_t node1, const size_t node2) {
      assert(data_[node1].parent != node2);
      assert(data_[node2].parent != node1);
      std::swap(data_[node1], data_[node2]);
      if (data_[node1].parent != data_[node2].parent) {
        data_[data_[node1].parent].ChangeChild(node1, node2);
        data_[data_[node2].parent].ChangeChild(node2, node1);
      }
      std::swap(data_[node1].parent, data_[node2].parent);
      if (!data_[node1].IsLeaf()) {
        data_[data_[node1].left_child].parent = node1;
        data_[data_[node1].right_child].parent = node1;
      }
      if (!data_[node2].IsLeaf()) {
        data_[data_[node2].left_child].parent = node2;
        data_[data_[node2].right_child].parent = node2;
      }
      if (data_[node1].IsLeaf()) {
        used_bytes_[data_[node1].value].second = node1;
      }
      if (data_[node2].IsLeaf()) {
        used_bytes_[data_[node2].value].second = node2;
      }
    }


	[[nodiscard]] bool IsRoot(const size_t index) {
      return data_[index].parent == index;
    }

	[[nodiscard]] bool IsLeftChild(const size_t index) {
      return data_[data_[index].parent].left_child == index;
    }

	[[nodiscard]] bool IsRightChild(const size_t index) {
      return data_[data_[index].parent].right_child == index;
    }

	  /**
     * \brief ������������ ��������������� ������ ����� ���������� ���� ����. 
     * \param index ������ ���� � ����������� �����. 
     * \return ����� ������ ���� � ����������� �����.
     */
    size_t RepairSortingForElement(const size_t index) {
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
        SwapNode(i, index);
        return i;
      }
      return index;
    }

    std::vector<Node> data_;
    std::array<std::pair<bool, size_t>, 256> used_bytes_{{{false, 0}}};
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

	  /**
     * \brief �������� ������ �� �����������. 
     * \param value ���������� ������. 
     */
    void Push(const byte value) {
      auto code = code_tree_.Encode(value);
      buffer_.insert(buffer_.end(), code.begin(), code.end());
    }

	  /**
     * \brief �������� 1 ���� �� ��������������� ������.
     * \param force ���� true, �� ���� ����� �����������, ���� ���� � �������������� ������ ������ 8 ���.  
     * \return �������������� ����.
     */
    byte Pop(const bool force=false) {
      std::vector<bool> code;
      if (buffer_.size() - buffer_cursor_ > 8) {
        code = std::vector<bool>(std::next(buffer_.begin(), buffer_cursor_), std::next(buffer_.begin(), buffer_cursor_+8));
		buffer_cursor_ += 8;
        std::reverse(code.begin(), code.end());
      }
      else if (force){
        code = std::vector<bool>(std::next(buffer_.begin(), buffer_cursor_), buffer_.end());
		buffer_.clear();
		buffer_cursor_ = 0;
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

	  /**
     * \brief ���������� ����� �����������.
     */
    void Eof() {
      auto code = code_tree_.GetNytCode();
      buffer_.insert(buffer_.end(), code.begin(), code.end());
    }

  private:
    std::vector<bool> buffer_;
	size_t buffer_cursor_{0};
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

	  /**
     * \brief �������� ���� ��������������� ������ ��� ��������������.
     * \param value ���� ��������������� ������.
     */
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

	  /**
     * \brief �������� ��������������� ������ �� ������.
     * \return ��������������� ������.
     */
    byte Pop() {
		byte value{ 0 };
		size_t shift{ 0 };
		std::tie(value, shift) = code_tree_.Decode(std::next(buffer_.begin(), buffer_cursor_), buffer_.end());
      if (shift==0) {
        buffer_.clear();
		buffer_cursor_ = 0;
        return 0;
      }
      buffer_cursor_ += shift;
      return value;
    }

	  /**
     * \brief ����������� �� ��������������.
     * \return true - ���� �������������� �����������.
     */
    [[nodiscard]] bool IsEof() const {
      return eof_;
    }

  private:
    std::vector<bool> buffer_;
	size_t buffer_cursor_{ 0 };
    custom_containers::CodeTree code_tree_;
    bool eof_ = false;
  };
}


void CodeTreeTest_checkSortingWithRepetitions1() {
  custom_containers::CodeTree code_tree;

  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions2() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('a');
  code_tree.Insert('r');
  code_tree.Insert('d');
  code_tree.Insert('v');
}


void CodeTreeTest_checkSortingWithRepetitions3() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions4() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('a');
  code_tree.Insert('b');
}


void CodeTreeTest_checkSortingWithRepetitions5() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('c');
  code_tree.Insert('c');
  code_tree.Insert('c');
  code_tree.Insert('c');
}


void CodeTreeTest_checkSortingWithoutRepetitions1() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('c');
  code_tree.Insert('d');
}


void CodeTreeTest_checkSortingWithoutRepetitions2() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert('c');
  code_tree.Insert('d');
  code_tree.Insert('e');
  code_tree.Insert('f');
}


void CodeTreeTest_insertNull() {
  custom_containers::CodeTree code_tree;
  code_tree.Insert('a');
  code_tree.Insert('b');
  code_tree.Insert(0);
}


void CodeTreeTest_randomSmallBytes() {
  custom_containers::CodeTree code_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 10; ++i) {
    const byte value = rand() % 5;
    code_tree.Insert(value);
  }
}


void CodeTreeTest_randomMediumBytes() {
  custom_containers::CodeTree code_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 1000; ++i) {
    const byte value = rand() % 64;
    code_tree.Insert(value);
  }
}


void CodeTreeTest_randomLargeBytes() {
  custom_containers::CodeTree code_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 10000; ++i) {
    const byte value = rand() % 256;
    code_tree.Insert(value);
  }
}


void CodeTreeTest_encodeAndDecodeStringWithoutRepetitions() {
  std::string str = "abcdefgh";
  custom_containers::CodeTree encode_tree;
  custom_containers::CodeTree decode_tree;
  for (auto ch : str) {
    const byte value = static_cast<byte>(ch);
    auto code = encode_tree.Encode(value);
	byte decoded_value{ 0 };
	size_t shift_bytes{ 0 };
	std::tie(decoded_value, shift_bytes) = decode_tree.Decode(code);
    assert(encode_tree == decode_tree);
    assert(value == decoded_value);
    assert(shift_bytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeStringWithRepetitions() {
  std::string str = "aabbcc";
  custom_containers::CodeTree encode_tree;
  custom_containers::CodeTree decode_tree;
  for (auto ch : str) {
    const byte value = static_cast<byte>(ch);
    auto code = encode_tree.Encode(value);
	byte decoded_value{ 0 };
	size_t shift_bytes{ 0 };
	std::tie(decoded_value, shift_bytes) = decode_tree.Decode(code);
    assert(encode_tree == decode_tree);
    assert(value == decoded_value);
    assert(shift_bytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomSmallBytes() {
  custom_containers::CodeTree encode_tree;
  custom_containers::CodeTree decode_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 10; ++i) {
    const byte value = rand() % 5;
    auto code = encode_tree.Encode(value);
	byte decoded_value{ 0 };
	size_t shift_bytes{ 0 };
	std::tie(decoded_value, shift_bytes) = decode_tree.Decode(code);
    assert(encode_tree == decode_tree);
    assert(value == decoded_value);
    assert(shift_bytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomMediumBytes() {
  custom_containers::CodeTree encode_tree;
  custom_containers::CodeTree decode_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 1000; ++i) {
    const byte value = rand() % 64;
    auto code = encode_tree.Encode(value);
	byte decoded_value{ 0 };
	size_t shift_bytes{ 0 };
	std::tie(decoded_value, shift_bytes) = decode_tree.Decode(code);
    assert(encode_tree == decode_tree);
    assert(value == decoded_value);
    assert(shift_bytes == code.size());
  }
}


void CodeTreeTest_encodeAndDecodeRandomLargeBytes() {
  custom_containers::CodeTree encode_tree;
  custom_containers::CodeTree decode_tree;
  srand(time(nullptr));
  for (auto i = 0; i < 10000; ++i) {
    const byte value = rand() % 256;
    auto code = encode_tree.Encode(value);
	byte decoded_value{ 0 };
	size_t shift_bytes{ 0 };
	std::tie(decoded_value, shift_bytes) = decode_tree.Decode(code);
    assert(encode_tree == decode_tree);
    assert(value == decoded_value);
    assert(shift_bytes == code.size());
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
	IInputStream in_original(kStr);
	IOutputStream out_compressed;
	Encode(in_original, out_compressed);
	IInputStream in_compressed(out_compressed.output);
	IOutputStream out_original;
	Decode(in_compressed, out_original);
	assert(kStr == out_original.output);
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

