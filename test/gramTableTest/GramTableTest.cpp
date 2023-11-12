#include <gtest/gtest.h>
#include <optional>
#include "GramTable.h"

TEST(gramTable, testStaticFunc) {
  EXPECT_EQ(GramTable::isTerm("+"), true);
  EXPECT_EQ(GramTable::isTerm("a"), true);
  EXPECT_EQ(GramTable::isTerm("A"), false);
  EXPECT_EQ(GramTable::firstCharToString("1233"), "1");
  EXPECT_EQ(GramTable::firstCharToString(""), "");
  auto partitionTable {GramTable::partition("123", 2, std::nullopt)};
  EXPECT_EQ(partitionTable.size(), 4);
  partitionTable = GramTable::partition("12332", 3, std::nullopt);
  EXPECT_EQ(partitionTable.size(), 21);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
