#include <gtest/gtest.h>
#include <optional>
#include <chrono>
#include "GramTable.h"
#include "chartTable.h"
#include "BasicNode.h"
#include "BTNode.h"
using Stragegy = PARSE_UTIL::ChartTable::Strategy;
static void chartTableTest(std::vector<std::string> gramVec, std::string inputStr, Stragegy strategy)
{
    GramTable gramTable{gramVec};
    PARSE_UTIL::ChartTable temp{gramTable, inputStr, strategy};
    auto t1 = std::chrono::steady_clock::now();
    auto ret = temp.parse();
    auto t2 = std::chrono::steady_clock::now();
    EXPECT_NE(ret, nullptr);
    if (ret == nullptr)
    {
        std::cout << "error parse" << std::endl;
        return ;
    }
    BTTree<BasicNode> printer(ret.get(), &BasicNode::getChildren, &BasicNode::getData);
    printer.print();
    std::cout << "cost time : " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
}
TEST(chartTableTest, test1) {

    chartTableTest({
       "S:SB|a",
       "B:b"
    }, 
    "abbbbbbbb", Stragegy::BOTTOMUP);

    chartTableTest({
       "S:SB|a",
       "B:b"
    }, 
    "abbbbbbbb", Stragegy::TOPDOWN);

    chartTableTest({
       "S:SB|a",
       "B:b"
    }, 
    "abbbbbbbb", Stragegy::LEFTCORNER);

    chartTableTest({
       "S:SB|a",
       "B:b"
    }, 
    "abbbbbbbb", Stragegy::EARLEY);
}

TEST(chartTableTest, test2) {
    chartTableTest({
        "S:0|1|2|3|4|5|6|7|8|9|AB|CD|AE",
        "C:AE",
        "A:0|1|2|3|4|5|6|7|8|9|AB",
        "E:GA",
        "G:.",
        "D:HA",
        "H:IJ",
        "I:e",
        "B:0|1|2|3|4|5|6|7|8|9",
        "J:+|-"
    }, 
    "323232.5233e+12323", Stragegy::EARLEY);

    chartTableTest({
        "S:0|1|2|3|4|5|6|7|8|9|AB|CD|AE",
        "C:AE",
        "A:0|1|2|3|4|5|6|7|8|9|AB",
        "E:GA",
        "G:.",
        "D:HA",
        "H:IJ",
        "I:e",
        "B:0|1|2|3|4|5|6|7|8|9",
        "J:+|-"
    }, 
    "323232.5233e+12323", Stragegy::TOPDOWN);
}

TEST(chartTableTest, test3) {
    chartTableTest({
        "S:SHA|A",
        "E:(",
        "F:)",
        "H:+",
        "I:*",
        "G:0|1|2|3|4|5|6|7|8|9",
        "A:AID|D",
        "D:ESF|G"
    }, 
    "((7*8))+3+8*(9+1)", Stragegy::EARLEY);

    chartTableTest({
        "S:SHA|A",
        "E:(",
        "F:)",
        "H:+",
        "I:*",
        "G:0|1|2|3|4|5|6|7|8|9",
        "A:AID|D",
        "D:ESF|G"
    }, 
    "((7*8))+3+8*(9+1)", Stragegy::LEFTCORNER);
}
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
