#include <gtest/gtest.h>
#include <optional>
#include <chrono>
#include "GramTable.h"
#include "RegExpr.h"
#include "BasicNode.h"
#include "BTNode.h"
static void regExprTest(const std::string& inputStr, const std::string& regExprStr)
{
    TopDownRegExprRecognition tmp(inputStr, regExprStr);
    auto ret = tmp.recognition();
    EXPECT_NE(ret, false);

}
TEST(regExpr, test1) {
    regExprTest(
        "abbaabbecee1157",
        "((ab*)()((c*e)|ecee)*)*e()(1*(2*3)*5(7))"
    );
#if 1
    regExprTest(
        "abbde",
        "ab*"
    );
    regExprTest(
        "abcdeef",
        "abc|abcde||"
    );
    regExprTest(
        "abcdeef",
        "(abc|)(deef|abc)"
    );
    regExprTest(
        "abcefg",
        "((a|b|c|e|f)*)g"
    );
    regExprTest(
    "abcef",
    "(abc|(ab)*)(c|)ef"
    );
    regExprTest(
        "abbdeef",
        "ab*de*e*"
    );
    regExprTest(
        "abbdeef",
        "e*b*a"
    );
    regExprTest(
        "abbdeef",
        "(a)bb(d)()((e))((()))e"
    );
    regExprTest(
        "abcefg",
        "((a|b|c|e|f)*)(t|)*g"
    );
    regExprTest(
    "abbabdeef",
    "((ab*)())*de*"
    );

    regExprTest(
        "abbaabbecee1157",
        "((ab*)()((c*e))*)*e()(1*(2*3)*5(7))"
    );
    regExprTest(
        "abbaabbeceee115732357",
        "((ab*)()((c*e))*)*e*()(1*(2*3)*5(7))e*()(1*(2*3)*5(7))"
    );
#endif
    
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}