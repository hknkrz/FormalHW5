#pragma once
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Gram {
public:
  Gram() = default;
  void Build(const std::vector<std::pair<std::string, std::string>> &data,
             const std::vector<char> &non_term, const char &start) noexcept;
  ~Gram() = default;

  bool CheckGram() noexcept;
  bool CYK(std::string word) noexcept;

private:
  std::map<std::string, std::set<std::string>> rul_;
  std::map<std::string, std::set<std::string>> rev_rul_;
  std::set<char> non_term_;
  std::unordered_map<char, int> indexes_;
  char start_{};
};
bool Gram::CheckGram() noexcept{
  for (const auto &root : rul_) {
    if (root.first.size() != 1) {
      return false;
    }
    for (auto right_part : root.second) {
      if (right_part.size() > 2) {
        return false;
      }
      if (right_part.size() == 1) {
        if (non_term_.find(right_part[0]) != non_term_.end()) {
          return false;
        }
        if (right_part[0] == '1' && root.first[0] != start_) {
          return false;
        }
      }
      if (right_part.size() == 2) {
        if ((non_term_.find(right_part[0]) == non_term_.end()) ||
            (non_term_.find(right_part[1]) == non_term_.end())) {
          return false;
        }
      }
    }
  }
  return true;
}
void Gram::Build(const std::vector<std::pair<std::string, std::string>> &data,
                 const std::vector<char> &non_term, const char &start) noexcept{
  for (const auto &pair : data) {
    rul_.insert(std::make_pair(pair.first, std::set<std::string>()));
    rev_rul_.insert(std::make_pair(pair.second, std::set<std::string>()));
  }
  for (const auto &pair : data) {
    rul_[pair.first].insert(pair.second);
    rev_rul_[pair.second].insert(pair.first);
  }
  int i = 0;

  for (auto element : non_term) {
    non_term_.insert(element);
    indexes_.insert(std::make_pair(element, i));
    ++i;
  }

  start_ = start;
}
bool Gram::CYK(std::string word) noexcept{
  std::vector<std::vector<std::vector<bool>>> table;
  table.reserve(word.size() * sizeof(std::vector<std::vector<bool>>));
  for (int i = 0; i < non_term_.size(); ++i) {
    auto v1 = new std::vector<std::vector<bool>>;
    v1->reserve(word.size() * sizeof(std::vector<bool>));
    for (int j = 0; j < word.size(); ++j) {
      auto v2 = new std::vector<bool>(word.size(), false);
      v1->push_back(*v2);
    }
    table.push_back(*v1);
  }
  for (int k = 0; k < word.size(); ++k) {
    std::string cur_letter;
    cur_letter += word[k];
    if (rev_rul_.find(cur_letter) != rev_rul_.end()) {
      for (auto non_term : rev_rul_[cur_letter]) {
        table[indexes_[non_term[0]]][k][k] = true;
      }
    }
  }
  for (int k = 1; k < word.size(); ++k) {
    for (int i = 0; i < word.size() - k; ++i) {
      int j = i + k;
      for (const auto &rule : rul_) {
        if (!table[indexes_[rule.first[0]]][i][j]) {
          bool result = false;
          for (auto right_part : rule.second) {
            if (right_part.size() == 2) {
              for (int x = i; x < j; ++x) {
                result = result || (table[indexes_[right_part[0]]][i][x] &&
                         table[indexes_[right_part[1]]][x + 1][j]);
              }
            }
          }
          table[indexes_[rule.first[0]]][i][j] = result;
        }
      }
    }
  }
  return table[indexes_[start_]][0][word.size() - 1];
}
