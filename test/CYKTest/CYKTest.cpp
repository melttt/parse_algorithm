#include <gtest/gtest.h>
#include <optional>
#include <chrono>
#include "GramTable.h"
#include "CYK.h"
#include "BasicNode.h"
#include "BTNode.h"
static void CYKTest(std::vector<std::string> gramVec, std::string inputStr)
{
    GramTable gramTable{gramVec};
    CYK tmp(gramTable, inputStr);
    auto t1 = std::chrono::steady_clock::now();
    auto ret = tmp.parse();
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
TEST(CYK, test1) {

    CYKTest({
       "S:SB|a",
       "B:b"
    }, 
    "abbbbbbbb");

    CYKTest({
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
    "323232.5233e+12323");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
