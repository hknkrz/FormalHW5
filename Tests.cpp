#include "Source.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(Build, Default) {
  ASSERT_THROW(Authomat::Build("*"),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("+"),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("."),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("a."),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("b+"),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("q"),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("aa"),std::invalid_argument);
  ASSERT_THROW(Authomat::Build("aa.ba*...."),std::invalid_argument);

}

TEST(DeleteEps, Default) {
  Authomat *y = Authomat::Build("ab.bc.+acb..*.");
  y->DelEps();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    while(itr1!=itr2){
      ASSERT_NE(itr1->second.first,'1');
      ++itr1;
    }
  }
}

TEST(DeleteEps, LongReg) {
  Authomat *y = Authomat::Build("ac.bc.+acbc...aa.+*.");
  y->DelEps();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    while(itr1!=itr2){
      ASSERT_NE(itr1->second.first,'1');
      ++itr1;
    }
  }
}

TEST(FullAuth,Default){
  Authomat *y = Authomat::Build("ab.bc.+acb..*.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    std::map<char,bool> is_visited ;
    is_visited.insert({'a',false});
    is_visited.insert({'b',false});
    is_visited.insert({'c',false});
    while(itr1!=itr2){
      is_visited[itr1->second.first] = true;
      ++itr1;
    }
    ASSERT_TRUE(is_visited['a']);
    ASSERT_TRUE(is_visited['b']);
    ASSERT_TRUE(is_visited['c']);
  }
}

TEST(FullAuth,LongReg){
  Authomat *y = Authomat::Build("ac.bc.+acbc...aa.+*.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    std::map<char,bool> is_visited ;
    is_visited.insert({'a',false});
    is_visited.insert({'b',false});
    is_visited.insert({'c',false});
    while(itr1!=itr2){
      is_visited[itr1->second.first] = true;
      ++itr1;
    }
    ASSERT_TRUE(is_visited['a']);
    ASSERT_TRUE(is_visited['b']);
    ASSERT_TRUE(is_visited['c']);
  }
}

TEST(FullAuth,ShortReg){
  Authomat *y = Authomat::Build("cbc.+*a.aa.cc.+.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    std::map<char,bool> is_visited ;
    is_visited.insert({'a',false});
    is_visited.insert({'b',false});
    is_visited.insert({'c',false});
    while(itr1!=itr2){
      is_visited[itr1->second.first] = true;
      ++itr1;
    }
    ASSERT_TRUE(is_visited['a']);
    ASSERT_TRUE(is_visited['b']);
    ASSERT_TRUE(is_visited['c']);
  }
}

TEST(FullAuth,LongConcat){
  Authomat *y = Authomat::Build("cabcbc.....bcbacba......+*bababa.....cabbba.....+*.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  for (auto element : *y->GetConds()) {
    auto itr1 = y->GetDelta()->lower_bound(element);
    auto itr2 = y->GetDelta()->upper_bound(element);
    std::map<char,bool> is_visited ;
    is_visited.insert({'a',false});
    is_visited.insert({'b',false});
    is_visited.insert({'c',false});
    while(itr1!=itr2){
      is_visited[itr1->second.first] = true;
      ++itr1;
    }
    ASSERT_TRUE(is_visited['a']);
    ASSERT_TRUE(is_visited['b']);
    ASSERT_TRUE(is_visited['c']);
  }
}

TEST(FindShortest, Default){
  Authomat *y = Authomat::Build("ab.bc.+acb..*.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  ASSERT_EQ(y->SearchPath('a', 3), 8);

}
TEST(FindShortest, LongExp){
  Authomat *y = Authomat::Build("ac.bc.+acbc...aa.+*.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  ASSERT_EQ(y->SearchPath('c', 13), 26);

}
TEST(FindShortest, NoPath){
  Authomat *y = Authomat::Build("ab+*c.");
  y->DelEps();
  y = y->Determine();
  y->FullAuth();
  ASSERT_EQ(y->SearchPath('c', 2), -1);

}