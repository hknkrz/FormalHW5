#pragma once
#ifndef FORMALHW5__SOURCE_H_
#define FORMALHW5__SOURCE_H_
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

class Authomat {
 public:
  static int max_cond_;
  static int min_cond_;

  Authomat() = default;
  explicit Authomat(char letter) noexcept;
  static Authomat* Build(const std::string& reg_str);
  typedef std::multimap<int, std::pair<char, int>>::iterator DeltaIter;
  Authomat* DelEps() noexcept;
  Authomat* Determine() noexcept;
  void FullAuth() noexcept;

  Authomat* Minimize() noexcept{
    std::map<int, int> class_dist;// Распределение вершин по классам

    int class_counter = 2;
    std::map<int, std::set<int>> classes;
    classes.insert(std::make_pair(0, std::set<int>()));
    classes.insert(std::make_pair(1, std::set<int>()));
    for (auto cond : conditions_) {// Разбиение на два стартовых класса
      auto itr1 = delta_.lower_bound(cond);
      auto itr2 = delta_.upper_bound(cond);
      while (itr1 != itr2) {
        if (exit_.find(cond) != exit_.end()) {
          classes[1].insert(itr1->first);
          class_dist.insert(std::pair(itr1->first, 1));
        } else {
          classes[0].insert(itr1->first);
          class_dist.insert(std::pair(itr1->first, 0));
        }
        ++itr1;
      }
    }
    std::queue<std::pair<int, char>> class_queue;
    for (auto letter : alphabet_) {
      class_queue.push(std::make_pair(1, letter));
    }
    while (!class_queue.empty()) {
      std::pair<int, char> cur_pair = class_queue.front();
      class_queue.pop();
      for (const auto& [key, val] : classes) {
        auto new_class_dist = class_dist;
        std::set<int> new_class1;
        std::set<int> new_class2;


        for (int itr : val) {
          auto itr1 = delta_.lower_bound(itr);
          auto itr2 = delta_.upper_bound(itr);
          while (itr1 != itr2) {
            if (itr1->second.first == cur_pair.second) {
              if (class_dist[itr1->second.second] == cur_pair.first) {
                new_class1.insert(itr);
              } else {
                new_class2.insert(itr);
                new_class_dist[itr] = class_counter;
              }
            }
            ++itr1;
          }
        }
        if (!new_class2.empty() && !new_class1.empty()) {
          classes.erase(key);
          classes.insert(std::make_pair(key, new_class1));
          classes.insert(std::make_pair(class_counter, new_class2));
          class_dist = new_class_dist;
          for (auto letter : alphabet_) {
            class_queue.push(std::make_pair(key, letter));
            class_queue.push(std::make_pair(class_counter, letter));
          }

          ++class_counter;
        }
      }
    }
    auto ans = new Authomat();
    ans->start_ = class_dist[start_];
    ans->alphabet_ = alphabet_;
    for (auto cond : conditions_) {// Строим новое множество завершающих состояний
      if (exit_.find(cond) != exit_.end()) {
        ans->exit_.insert(class_dist[cond]);
      }
    }
    std::set<int> used_classes;
    for (const auto& cls : classes) {
      ans->conditions_.push_back(cls.first);
      std::set<char> used_tr;
      for (auto cond : cls.second) {
        auto itr1 = delta_.lower_bound(cond);
        auto itr2 = delta_.upper_bound(cond);
        while (itr1 != itr2) {
          if (used_tr.find(itr1->second.first) != used_tr.end()) {
            ++itr1;
            continue;
          }
          ans->delta_.insert(std::make_pair(cls.first, std::make_pair(itr1->second.first, class_dist[itr1->second.second])));
          used_tr.insert(itr1->second.first);
          ++itr1;
        }
      }
    }
    return ans;
  }

  Authomat* Plus(Authomat* other);
  Authomat* Iteration();
  Authomat* Concat(Authomat* other);

  int SearchPath(char letter, int quantity);

  std::vector<int>* GetConds() noexcept;
  std::multimap<int, std::pair<char, int>>* GetDelta() noexcept;

 private:
  int start_{};
  std::vector<int> conditions_;
  std::multimap<int, std::pair<char, int>> delta_;
  std::set<int> exit_;
  std::set<char> alphabet_;
};
int Authomat::max_cond_ = 0;
int Authomat::min_cond_ = 0;
Authomat* Authomat::Build(const std::string& reg_str) {
  std::stack<Authomat*> auth_stack;
  for (char element : reg_str) {
    if (element == '*') {
      if (auth_stack.empty()) {
        throw std::invalid_argument("No matching operand for operation *");
      }
      auth_stack.top()->Iteration();
    } else if (element == '+') {
      if (auth_stack.empty()) {
        throw std::invalid_argument("No matching operands for operation +");
      }
      Authomat* first = auth_stack.top();
      auth_stack.pop();
      if (auth_stack.empty()) {
        throw std::invalid_argument("No matching operands for operation +");
      }
      Authomat* second = auth_stack.top();
      auth_stack.pop();
      first->Plus(second);
      auth_stack.push(first);
    } else if (element == '.') {
      if (auth_stack.empty()) {
        throw std::invalid_argument("No matching operands for operation .");
      }
      Authomat* first = auth_stack.top();
      auth_stack.pop();
      if (auth_stack.empty()) {
        throw std::invalid_argument("No matching operands for operation .");
      }
      Authomat* second = auth_stack.top();
      auth_stack.pop();
      second->Concat(first);
      auth_stack.push(second);
    } else {
      auto* cur = new Authomat(element);
      cur->alphabet_ = {element};
      auth_stack.push(cur);
    }
  }
  if (auth_stack.size() > 1) {
    throw std::invalid_argument("Expected operand");
  }
  return auth_stack.top();
}
Authomat* Authomat::DelEps() noexcept{
  for (auto cond : conditions_) {
    auto itr1 = delta_.lower_bound(cond);
    auto itr2 = delta_.upper_bound(cond);
    while (itr1 != itr2) {

      if (itr1->second.first == '1') {
        if (exit_.find(itr1->second.second) != exit_.end()) {
          exit_.insert(cond);
        }
        auto itr_next1 = delta_.lower_bound(itr1->second.second);
        auto itr_next2 = delta_.upper_bound(itr1->second.second);
        while (itr_next1 != itr_next2) {

          delta_.insert({cond, std::make_pair(itr_next1->second.first, itr_next1->second.second)});

          ++itr_next1;
        }
        auto old_itr = itr1;
        ++itr1;
        delta_.erase(old_itr);
      } else {
        ++itr1;
      }
    }
  }
  return this;
}
std::vector<int>* Authomat::GetConds() noexcept{
  return &conditions_;
}
Authomat* Authomat::Determine() noexcept{
  std::queue<std::set<int>> cur_queue;
  cur_queue.push({this->start_});
  auto ans = new Authomat();
  ans->alphabet_ = alphabet_;
  Authomat& det_auth = *ans;
  det_auth.conditions_.push_back({0});
  det_auth.start_ = 0;
  if (exit_.find(start_) != exit_.end()) {
    det_auth.exit_.insert(det_auth.start_);
  }

  std::map<std::set<int>, std::pair<int, bool>> new_conds;
  int conds_iter = 0;
  DeltaIter set_itr;
  new_conds.insert({{this->start_}, std::make_pair(conds_iter, true)});
  ++conds_iter;
  std::set<int> used_conds;

  while (!cur_queue.empty()) {
    std::map<char, std::set<int>> transits;

    for (auto letter : alphabet_) {
      transits.insert(std::make_pair(letter, std::set<int>()));
    }

    std::map<char, bool> is_exiting;
    std::set<int>::iterator itr;
    for (itr = cur_queue.front().begin(); itr != cur_queue.front().end(); ++itr) {
      auto itr1 = delta_.lower_bound(*itr);
      auto itr2 = delta_.upper_bound(*itr);
      while (itr1 != itr2) {
        for (auto letter : alphabet_) {
          if (itr1->second.first == letter) {
            transits[letter].insert(itr1->second.second);
            if (exit_.find(itr1->second.second) != exit_.end()) {
              is_exiting.insert(std::make_pair(letter, true));
            } else {
              is_exiting.insert(std::make_pair(letter, false));
            }
          }
        }
        ++itr1;
      }
    }
    int cur_cond = new_conds[cur_queue.front()].first;
    if (used_conds.find(cur_cond) == used_conds.end()) {
      for (auto letter : alphabet_) {
        if ((!transits[letter].empty())) {
          if (new_conds.find(transits[letter]) == new_conds.end()) {
            new_conds.insert({transits[letter], std::make_pair(conds_iter, false)});
            ++conds_iter;
            det_auth.conditions_.push_back(new_conds[transits[letter]].first);
          }
          det_auth.delta_.insert({cur_cond, std::make_pair(letter, new_conds[transits[letter]].first)});

          if (is_exiting[letter]) {
            det_auth.exit_.insert(new_conds[transits[letter]].first);
          }
          if ((new_conds.find(transits[letter]) == new_conds.end()) || !new_conds[transits[letter]].second) {
            cur_queue.push(transits[letter]);
          }
        }
      }
    }
    used_conds.insert(cur_cond);

    new_conds[cur_queue.front()].second = true;
    cur_queue.pop();
  }
  max_cond_ = conds_iter;
  return ans;
}
void Authomat::FullAuth() noexcept{
  conditions_.push_back(max_cond_);
  for (auto cond : conditions_) {
    auto itr1 = delta_.lower_bound(cond);
    auto itr2 = delta_.upper_bound(cond);
    std::map<char, bool> has_transit;
    for (auto letter : alphabet_) {
      has_transit.insert(std::make_pair(letter, false));
    }
    while (itr1 != itr2) {
      for (auto letter : alphabet_) {
        if (itr1->second.first == letter) {
          has_transit[letter] = true;
        }
      }
      ++itr1;
    }
    for (auto letter : alphabet_) {
      if (!has_transit[letter]) {
        delta_.insert({cond, std::make_pair(letter, max_cond_)});
      }
    }
  }
  ++max_cond_;
}
int Authomat::SearchPath(char letter, int quantity) {
  std::vector<int> check_cycle;
  for (auto element : conditions_) {
    check_cycle.push_back(-1);
  }
  std::queue<std::tuple<int, int, int>> cond_q;
  cond_q.push(std::make_tuple(start_, 0, 0));
  while (!cond_q.empty()) {

    if (std::get<2>(cond_q.front()) == quantity && exit_.find(std::get<0>(cond_q.front())) != exit_.end()) {
      return std::get<1>(cond_q.front());
    }
    auto itr1 = delta_.lower_bound(std::get<0>(cond_q.front()));
    auto itr2 = delta_.upper_bound(std::get<0>(cond_q.front()));
    while (itr1 != itr2) {
      if (itr1->second.first == letter) {
        if (std::get<2>(cond_q.front()) + 1 <= quantity) {
          cond_q.push(std::make_tuple(itr1->second.second, std::get<1>(cond_q.front()) + 1, std::get<2>(cond_q.front()) + 1));
        }
      } else {
        if (check_cycle[itr1->second.second] == -1) {
          check_cycle[itr1->second.second] = std::get<2>(cond_q.front());
          cond_q.push(std::make_tuple(itr1->second.second, std::get<1>(cond_q.front()) + 1, std::get<2>(cond_q.front())));
        } else {
          if (check_cycle[itr1->second.second] < std::get<2>(cond_q.front())) {
            check_cycle[itr1->second.second] = std::get<2>(cond_q.front());
            cond_q.push(std::make_tuple(itr1->second.second, std::get<1>(cond_q.front()) + 1, std::get<2>(cond_q.front())));
          }
        }
      }
      ++itr1;
    }
    cond_q.pop();
  }
  return -1;
}
std::multimap<int, std::pair<char, int>>* Authomat::GetDelta() noexcept{
  return &delta_;
}
Authomat::Authomat(char letter) noexcept{
  ++max_cond_;

  delta_.insert(std::make_pair(max_cond_, std::make_pair(letter, max_cond_ + 1)));

  start_ = max_cond_;
  conditions_.push_back(max_cond_);
  conditions_.push_back(max_cond_ + 1);
  exit_.insert(max_cond_ + 1);
  alphabet_ = {letter};
  ++max_cond_;
}
Authomat* Authomat::Plus(Authomat* other) {
  --min_cond_;
  delta_.insert(other->delta_.begin(), other->delta_.end());
  delta_.insert({min_cond_, std::make_pair('1', start_)});
  delta_.insert({min_cond_, std::make_pair('1', other->start_)});

  conditions_.push_back(min_cond_);
  conditions_.insert(conditions_.end(), other->conditions_.begin(), other->conditions_.end());

  exit_.insert(other->exit_.begin(), other->exit_.end());

  start_ = min_cond_;
  alphabet_.insert(other->alphabet_.begin(), other->alphabet_.end());
  return this;
}
Authomat* Authomat::Iteration() {
  --min_cond_;
  conditions_.push_back(min_cond_);

  delta_.insert({min_cond_, std::make_pair('1', start_)});
  for (int element : exit_) {
    delta_.insert({element, std::make_pair('1', min_cond_)});
  }

  exit_ = {min_cond_};
  start_ = min_cond_;

  return this;
}
Authomat* Authomat::Concat(Authomat* other) {
  for (int element : exit_) {
    delta_.insert(std::make_pair(element, std::make_pair('1', other->start_)));
  }
  delta_.insert(other->delta_.begin(), other->delta_.end());

  conditions_.insert(conditions_.end(), other->conditions_.begin(), other->conditions_.end());

  exit_ = other->exit_;
  alphabet_.insert(other->alphabet_.begin(), other->alphabet_.end());

  return this;
}

#endif//FORMALHW5__SOURCE_H_
