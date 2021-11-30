#include "source.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(Build, Default) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      std::make_pair("S", "AS"), std::make_pair("A", "CD"),
      std::make_pair("D", "SE"), std::make_pair("C", "("),
      std::make_pair("E", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'D', 'E'};

  char start = 'S';
  ASSERT_NO_THROW(gram.Build(data, nonterm, start));
}

TEST(CheckGram, Default) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      std::make_pair("S", "AS"), std::make_pair("A", "CD"),
      std::make_pair("D", "SE"), std::make_pair("C", "("),
      std::make_pair("E", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'D', 'E'};

  char start = 'S';
  gram.Build(data, nonterm, start);
  ASSERT_TRUE(gram.CheckGram());
}
TEST(CheckGram, UncorrectGram) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      std::make_pair("S", "AS"), std::make_pair("A", "CDE"),
      std::make_pair("D", "SE"), std::make_pair("C", "("),
      std::make_pair("E", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'D', 'E'};

  char start = 'S';
  gram.Build(data, nonterm, start);
  ASSERT_FALSE(gram.CheckGram());
}

TEST(CYK, Default) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      // ПСП в НФ Хомского
      std::make_pair("S", "AC"),
      std::make_pair("C", "SD"),
      std::make_pair("D", "BS"),
      std::make_pair("Z", "AC"),
      std::make_pair("A", "("),
      std::make_pair("B", ")"),
      std::make_pair("C", ")"),
      std::make_pair("D", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'Z'};
  char start = 'S';
  gram.Build(data, nonterm, start);

  ASSERT_TRUE(gram.CYK("()"));
}

TEST(CYK, LongExpr) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      // ПСП в НФ Хомского
      std::make_pair("S", "AC"),
      std::make_pair("C", "SD"),
      std::make_pair("D", "BS"),
      std::make_pair("Z", "AC"),
      std::make_pair("A", "("),
      std::make_pair("B", ")"),
      std::make_pair("C", ")"),
      std::make_pair("D", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'Z'};

  char start = 'S';
  gram.Build(data, nonterm, start);

  ASSERT_TRUE(gram.CYK("(())()(()()(()))((()))((((((((()))))))))"));
}

TEST(CYK, IncorrectWord) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      // ПСП в НФ Хомского
      std::make_pair("S", "AC"),
      std::make_pair("C", "SD"),
      std::make_pair("D", "BS"),
      std::make_pair("Z", "AC"),
      std::make_pair("A", "("),
      std::make_pair("B", ")"),
      std::make_pair("C", ")"),
      std::make_pair("D", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'Z'};

  char start = 'S';
  gram.Build(data, nonterm, start);
  ASSERT_FALSE(gram.CYK("))(("));
}

TEST(CYK, longIncorrect) {
  auto gram = Gram();
  std::vector<std::pair<std::string, std::string>> data = {
      // ПСП в НФ Хомского
      std::make_pair("S", "AC"),
      std::make_pair("C", "SD"),
      std::make_pair("D", "BS"),
      std::make_pair("Z", "AC"),
      std::make_pair("A", "("),
      std::make_pair("B", ")"),
      std::make_pair("C", ")"),
      std::make_pair("D", ")")

  };
  std::vector<char> nonterm = {'S', 'A', 'B', 'C', 'Z'};

  char start = 'S';
  gram.Build(data, nonterm, start);
  ASSERT_FALSE(gram.CYK("(())()(()()((((()))"));
}
