#include <gtest/gtest.h>
#include <optional>
#include <chrono>
#include "GramTable.h"
#include "unger.h"
#include "BasicNode.h"
#include "BTNode.h"
static void ungerTest(std::vector<std::string> gramVec, std::string inputStr)
{
    GramTable gramTable{gramVec};
    unger tmp(gramTable, inputStr);
    auto t1 = std::chrono::steady_clock::now();
    auto ret = tmp.parse().get();
    auto t2 = std::chrono::steady_clock::now();
    EXPECT_NE(ret, nullptr);
    BTTree<BasicNode> printer(ret, &BasicNode::getChildren, &BasicNode::getData);
    printer.print();
    std::cout << "cost time : " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
}
TEST(unger, test1) {
    ungerTest({
       "S:Sb|a"
    }, 
    "abbb");
    
    ungerTest({
        "S:S+A|A",
        "E:(",
        "F:)",
        "G:0|1|2|3|4|5|6|7|8|9",
        "A:A*D|D",
        "D:(S)|G"
    }, 
    "(1+2)*3+3+((7*8))+3+8*(9+1)+2+8*(9+1)+3+8*(9+1)+2+(8*(9+1)+4)+3+2+4+3+2+2+(8*(9+1)+4)+3+2+4+3+2+2");

    ungerTest({
       "S:A|D",
       "A:B|AB",
       "D:AEF",
       "E:.A",
       "F:eGA|",
       "B:0|1|2|3|4|5|6|7|8|9",
       "G:+|-"
    }, 
    "32.5e+1");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
