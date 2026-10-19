#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "introalgo/dynamic_programming/max_quad_exp.hpp"
#include "test_utils.hpp"

using introalgo::Expression;

namespace {

struct Parsed {
    std::vector<long long> nums;
    std::vector<char> ops;
};

Parsed parse(const std::string& s) {
    Parsed p;
    long long cur = 0;
    for (char c : s) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            cur = cur * 10 + (c - '0');
        } else {
            p.nums.push_back(cur);
            p.ops.push_back(c);
            cur = 0;
        }
    }
    p.nums.push_back(cur);
    return p;
}

bool apply(long long a, long long b, char op, long long& out) {
    switch (op) {
        case '+':
            out = a + b;
            return true;
        case '-':
            out = a - b;
            return true;
        case '*':
            out = a * b;
            return true;
        case '/':
            if (b == 0) return false;
            out = a / b;
            return true;
    }
    return false;
}

// All values of operands l..r over every parenthesization (exhaustive oracle).
std::set<long long> allValues(const Parsed& p, int l, int r) {
    if (l == r) return {p.nums[l]};
    std::set<long long> out;
    for (int k = l; k < r; ++k)
        for (long long a : allValues(p, l, k))
            for (long long b : allValues(p, k + 1, r)) {
                long long v;
                if (apply(a, b, p.ops[k], v)) out.insert(v);
            }
    return out;
}

// Evaluates a fully parenthesized expression such as "((5-8)+7)" with a tiny recursive-descent
// parser that is independent of the library.
struct Evaluator {
    const std::string& s;
    std::size_t pos = 0;
    bool ok = true;
    explicit Evaluator(const std::string& str) : s(str) {}
    long long expr() {
        if (s[pos] == '(') {
            ++pos;
            long long a = expr();
            char op = s[pos++];
            long long b = expr();
            if (s[pos++] != ')') ok = false;
            long long v = 0;
            if (!apply(a, b, op, v)) ok = false;
            return v;
        }
        long long n = 0;
        while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos])))
            n = n * 10 + (s[pos++] - '0');
        return n;
    }
};

std::string randomExpression(testutil::Rng& rng, int operands, const std::string& ops) {
    std::string e = std::to_string(testutil::randInt(rng, 0, 9));
    for (int i = 1; i < operands; ++i) {
        e += ops[static_cast<std::size_t>(testutil::randInt(rng, 0, int(ops.size()) - 1))];
        e += std::to_string(testutil::randInt(rng, 0, 9));
    }
    return e;
}

}  // namespace

TEST(MaxQuadExp, KnownAnswerFromOriginalDemo) {
    Expression e("5-8+7*4-8+9");
    EXPECT_EQ(e.calculateMax(), 200);  // the value printed by the original program
}

TEST(MaxQuadExp, SmallHandCheckedCases) {
    EXPECT_EQ(Expression("1+2*3-4").calculateMax(), 5);  // ((1+2)*3)-4
    EXPECT_EQ(Expression("2-1-1").calculateMax(), 2);    // 2-(1-1)
    EXPECT_EQ(Expression("7").calculateMax(), 7);
    EXPECT_EQ(Expression("12 + 30").calculateMax(), 42);  // whitespace and multi-digit numbers
    EXPECT_EQ(Expression("2x3").calculateMax(), 6);       // 'x' is multiplication
    EXPECT_EQ(Expression("8/2/2").calculateMax(), 8);     // 8/(2/2)
}

TEST(MaxQuadExp, MinimumIsTracked) {
    EXPECT_EQ(Expression("1+2*3-4").calculateMin(), -3);  // (1+2)*(3-4)
}

TEST(MaxQuadExp, MinimumOfASubtractionIsMinLeftMinusMaxRight) {
    // The original set the minimum of a '-' node to -(maximum), which is wrong whenever the left
    // operand matters: the values of 9-1-1 are 7 and 9, not -9.
    EXPECT_EQ(Expression("9-1-1").calculateMin(), 7);
    EXPECT_EQ(Expression("9-1-1").calculateMax(), 9);
}

TEST(MaxQuadExp, MaxExpressionEvaluatesToTheMaximum) {
    for (const char* text : {"5-8+7*4-8+9", "1+2*3-4", "9-1-1", "3*4-5*6+2"}) {
        Expression e(text);
        const long long best = e.calculateMax();
        const std::string shown = e.maxExpression();
        Evaluator ev(shown);
        EXPECT_EQ(ev.expr(), best) << shown;
        EXPECT_TRUE(ev.ok);
        EXPECT_EQ(ev.pos, shown.size());
    }
}

TEST(MaxQuadExp, RejectsMalformedExpressions) {
    for (const char* bad : {"", "   ", "+", "5+", "+5", "5++3", "5 $ 3", "abc", "-5+3"})
        EXPECT_THROW(Expression{bad}, std::invalid_argument) << bad;
}

TEST(MaxQuadExp, EveryParenthesizationDividesByZero) {
    EXPECT_THROW(Expression("1/0").calculateMax(), std::domain_error);
    EXPECT_EQ(Expression("1/2/3").calculateMax(), 0);  // only (1/2)/3 is defined
}

TEST(MaxQuadExp, RandomizedPlusMinusTimesMatchesExhaustiveSearch) {
    testutil::Rng rng(9);
    for (int iter = 0; iter < 400; ++iter) {
        const std::string text = randomExpression(rng, testutil::randInt(rng, 1, 8), "+-*");
        Expression e(text);
        const auto values = allValues(parse(text), 0, int(parse(text).nums.size()) - 1);
        EXPECT_EQ(e.calculateMax(), *values.rbegin()) << text;
        EXPECT_EQ(e.calculateMin(), *values.begin()) << text;

        const std::string shown = e.maxExpression();
        Evaluator ev(shown);
        EXPECT_EQ(ev.expr(), *values.rbegin()) << text << " -> " << shown;
    }
}

TEST(MaxQuadExp, DivisionNeverExceedsTheTrueMaximum) {
    // '/' is a heuristic (see the header): the answer is always achievable, so it can never
    // be larger than the exhaustive maximum.
    testutil::Rng rng(10);
    for (int iter = 0; iter < 400; ++iter) {
        const std::string text = randomExpression(rng, testutil::randInt(rng, 2, 7), "+-*/");
        const auto values = allValues(parse(text), 0, int(parse(text).nums.size()) - 1);
        Expression e(text);
        if (values.empty()) {
            EXPECT_THROW(e.calculateMax(), std::domain_error) << text;
            continue;
        }
        const long long got = e.calculateMax();
        EXPECT_LE(got, *values.rbegin()) << text;
        EXPECT_TRUE(values.count(got))
            << text << " produced " << got << ", which no parenthesization yields";
    }
}
