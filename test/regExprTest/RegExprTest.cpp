#include <gtest/gtest.h>
#include <optional>
#include <chrono>
#include "GramTable.h"
#include "RegExpr.h"
#include "BasicNode.h"
#include "BTNode.h"
#include "DFA.h"
static void regExprTest(const std::string& inputStr, const std::string& regExprStr)
{
    
    TopDownRegExprRecognition tmp(inputStr, regExprStr);
    auto ret {tmp.recognition()};
    
    
    
    TopDownRegExprParse tmp2(regExprStr);
    auto tree {tmp2.parse()};
    auto ret2 {!ret};
    if (tree.has_value())
    {
        PARSE_UTIL::NFA nfa{};
        if (nfa.buildNFA(tree.value()))
        {
            PARSE_UTIL::DFA dfa(nfa);
            ret2 = dfa.isMatch(inputStr).has_value();
        }
    }

    EXPECT_EQ(ret, ret2);
    EXPECT_NE(ret, false);

}
TEST(regExpr, test1) {

    regExprTest(
        "abbde",
        "ab*"
    );
#if 1
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
        "abbaabbecee1157",
        "((ab*)()((c*e)|ecee)*)*e()(1*(2*3)*5(7))"
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