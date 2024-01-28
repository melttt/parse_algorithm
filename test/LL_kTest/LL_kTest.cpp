#include <gtest/gtest.h>
#include <optional>
#include "GramTable.h"
#include "LL_k.h"
#include "BasicNode.h"
#include "BTNode.h"

TEST(LL_kTest, testLL_kTest) {
  // GramTable gramTable{{
  //      "S:SB|a",
  //      "B:b"
  //   }};
  // gramTable.getFirstKMap(2);
  // gramTable.getFollowKMap(2);
  // GramTable gramTable2{{
  //       "S:S+A|A",
  //       "E:(",
  //       "F:)",
  //       "G:0|1",
  //       "A:A*D|D",
  //       "D:(S)|G"
  //   }};
  //gramTable2.getFollowKMap(2);
  //gramTable2.getFirstKMap(4);
  // GramTable gramTable{{
  //      "S:A|D",
  //      "A:B|AB",
  //      "D:AEF",
  //      "E:.A",
  //      "F:eGA|",
  //      "B:1",
  //      "G:+|-"
  //   }};
    GramTable gramTable{{
       "S:FQ|(S)S",
       "F:GF|",
       "G:!C",
       "Q:?C",
       "C:a"
    }};
  // gramTable2.getFollowKMap(2);
    // GramTable gramTable{{
    //    "S:SB|a",
    //    "B:b"
    // }};
    try
    {
    auto x {PARSE_UTIL::StrongLLk(gramTable, "(!a!a?a)?a", 2)};
    auto root {x.parse()};

    BTTree<BasicNode> printer(root.get(), &BasicNode::getChildren, &BasicNode::getData);
      printer.print();
    }
    catch(const char* e)
    {
      std::cerr << e << '\n';
    }
    

}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
