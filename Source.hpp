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

  static Authomat *Build(const std::string &reg_str) {
    std::stack<Authomat *> auth_stack;
    for (char element : reg_str) {
      if (element == '*') {
        if(auth_stack.empty()){
          throw std::invalid_argument("No matching operand for operation *");
        }
        auth_stack.top()->Iteration();
      } else if (element == '+') {
        if(auth_stack.empty()){
          throw std::invalid_argument("No matching operands for operation +");
        }
        Authomat *first = auth_stack.top();
        auth_stack.pop();
        if(auth_stack.empty()){
          throw std::invalid_argument("No matching operands for operation +");
        }
        Authomat *second = auth_stack.top();
        auth_stack.pop();
        first->Plus(second);
        auth_stack.push(first);
      } else if (element == '.') {
        if(auth_stack.empty()){
          throw std::invalid_argument("No matching operands for operation .");
        }
        Authomat *first = auth_stack.top();
        auth_stack.pop();
        if(auth_stack.empty()){
          throw std::invalid_argument("No matching operands for operation .");
        }
        Authomat *second = auth_stack.top();
        auth_stack.pop();
        second->Concat(first);
        auth_stack.push(second);
      } else if (element == 'a' || element == 'b' || element == 'c' || element == '1') {
        auto *cur = new Authomat(element);
        auth_stack.push(cur);
      }
      else{
        throw std::invalid_argument(std::string("Wrong symbol in regular expression- ") + element );
      }
    }
    if(auth_stack.size() > 1){
      throw std::invalid_argument("Expected operand");
    }
    return auth_stack.top();
  }
  typedef std::multimap<int, std::pair<char, int>>::iterator DeltaIter;

  Authomat *DelEps() {
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

  Authomat *Determine() {
    std::queue<std::set<int>> cur_queue;
    cur_queue.push({this->start_});
    auto ans = new Authomat();
    Authomat &det_auth = *ans;
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
    while (!cur_queue.empty()) {
      std::set<int> a_tr = {};
      std::set<int> b_tr = {};
      std::set<int> c_tr = {};
      bool is_exiting[4] = {false, false, false};
      std::set<int>::iterator itr;
      for (itr = cur_queue.front().begin(); itr != cur_queue.front().end(); ++itr) {
        auto itr1 = delta_.lower_bound(*itr);
        auto itr2 = delta_.upper_bound(*itr);
        while (itr1 != itr2) {
          switch (itr1->second.first) {
            case 'a':
              a_tr.insert(itr1->second.second);
              if (exit_.find(itr1->second.second) != exit_.end()) {
                is_exiting[0] = true;
              }
              break;
            case 'b':
              b_tr.insert(itr1->second.second);
              if (exit_.find(itr1->second.second) != exit_.end()) {
                is_exiting[1] = true;
              }
              break;
            case 'c':
              c_tr.insert(itr1->second.second);
              if (exit_.find(itr1->second.second) != exit_.end()) {
                is_exiting[2] = true;
              }
              break;
          }
          ++itr1;
        }
      }
      std::set<int> aa = cur_queue.front();
      int cur_cond = new_conds[cur_queue.front()].first;
      if ((!a_tr.empty())) {
        if (new_conds.find(a_tr) == new_conds.end()) {
          new_conds.insert({a_tr, std::make_pair(conds_iter, false)});
          ++conds_iter;
          det_auth.conditions_.push_back(new_conds[a_tr].first);
        }
        det_auth.delta_.insert({cur_cond, std::make_pair('a', new_conds[a_tr].first)});
        if (is_exiting[0]) {
          det_auth.exit_.insert(new_conds[a_tr].first);
        }
        if ((new_conds.find(a_tr) == new_conds.end()) || !new_conds[a_tr].second) {
          cur_queue.push(a_tr);
        }
      }
      if ((!b_tr.empty()) && ((new_conds.find(b_tr) == new_conds.end()) || !new_conds[b_tr].second)) {
        if (new_conds.find(b_tr) == new_conds.end()) {
          new_conds.insert({b_tr, std::make_pair(conds_iter, false)});
          ++conds_iter;
          det_auth.conditions_.push_back(new_conds[b_tr].first);
        }
        det_auth.delta_.insert({cur_cond, std::make_pair('b', new_conds[b_tr].first)});
        if (is_exiting[1]) {
          det_auth.exit_.insert(new_conds[b_tr].first);
        }
        cur_queue.push(b_tr);
      }

      if ((!c_tr.empty()) && ((new_conds.find(c_tr) == new_conds.end()) || !new_conds[c_tr].second)) {
        if (new_conds.find(c_tr) == new_conds.end()) {
          new_conds.insert({c_tr, std::make_pair(conds_iter, false)});
          ++conds_iter;
          det_auth.conditions_.push_back(new_conds[c_tr].first);
        }
        det_auth.delta_.insert({cur_cond, std::make_pair('c', new_conds[c_tr].first)});
        if (is_exiting[2]) {
          det_auth.exit_.insert(new_conds[c_tr].first);
        }
        cur_queue.push(c_tr);
      }
      new_conds[cur_queue.front()].second = true;
      cur_queue.pop();
    }
    max_cond_ = conds_iter;
    return ans;
  };

  explicit Authomat(char letter) {
    ++max_cond_;

    delta_.insert(std::make_pair(max_cond_, std::make_pair(letter, max_cond_ + 1)));

    start_ = max_cond_;
    conditions_.push_back(max_cond_);
    conditions_.push_back(max_cond_ + 1);
    exit_.insert(max_cond_ + 1);
    ++max_cond_;
  }
  Authomat *Plus(Authomat *other) {
    --min_cond_;
    delta_.insert(other->delta_.begin(), other->delta_.end());
    delta_.insert({min_cond_, std::make_pair('1', start_)});
    delta_.insert({min_cond_, std::make_pair('1', other->start_)});

    conditions_.push_back(min_cond_);
    conditions_.insert(conditions_.end(), other->conditions_.begin(), other->conditions_.end());

    exit_.insert(other->exit_.begin(), other->exit_.end());

    start_ = min_cond_;

    return this;
  }
  Authomat *Iteration() {
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
  Authomat *Concat(Authomat *other) {
    for (int element : exit_) {
      delta_.insert(std::make_pair(element, std::make_pair('1', other->start_)));
    }
    delta_.insert(other->delta_.begin(), other->delta_.end());

    conditions_.insert(conditions_.end(), other->conditions_.begin(), other->conditions_.end());

    exit_ = other->exit_;

    return this;
  }
  void FullAuth() {
    conditions_.push_back(max_cond_);
    for (auto cond : conditions_) {
      auto itr1 = delta_.lower_bound(cond);
      auto itr2 = delta_.upper_bound(cond);
      std::vector<bool> has_transit = {false, false, false};
      while (itr1 != itr2) {
        switch (itr1->second.first) {
          case 'a':
            has_transit[0] = true;
            break;
          case 'b':
            has_transit[1] = true;
            break;
          case 'c':
            has_transit[2] = true;
            break;
        }
        ++itr1;
      }
      if (!has_transit[0]) {
        delta_.insert({cond, std::make_pair('a', max_cond_)});
      }
      if (!has_transit[1]) {
        delta_.insert({cond, std::make_pair('b', max_cond_)});
      }
      if (!has_transit[2]) {
        delta_.insert({cond, std::make_pair('c', max_cond_)});
      }
    }
  }

  int SearchPath(char letter, int quantity) {
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

  std::vector<int> * GetConds(){
    return &conditions_;
  }
  std::multimap<int, std::pair<char, int>> * GetDelta(){
    return &delta_;
  }
 private:
  int start_{};
  std::vector<int> conditions_;
  std::multimap<int, std::pair<char, int>> delta_;
  std::set<int> exit_;
};
int Authomat::max_cond_ = 0;
int Authomat::min_cond_ = 0;

#endif//FORMALHW5__SOURCE_H_
