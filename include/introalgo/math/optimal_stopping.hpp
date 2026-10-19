#pragma once

namespace introalgo {

/// The secretary problem / optimal stopping.  Candidates arrive in random order; we reject the
/// first x of them and then hire the first one who is better than everybody seen so far.  The
/// probability of ending up with the best candidate is
///
///     P(x) = x / n * sum_{i=x}^{n-1} 1 / i          (x >= 1),
///
/// which is maximal for x close to n / e, where it approaches 1 / e = 0.3679.
class CalFormula {
public:
    /// Throws std::invalid_argument for n < 1.
    explicit CalFormula(int n);

    /// Success probability when the first x candidates are rejected (0 <= x <= n; P(0) is
    /// reported as 0 -- hiring the first candidate has probability 1/n, outside the formula).
    double calculate(int x) const;

    /// The best number of candidates to reject (the first x in 1..n with the largest probability;
    /// 0 for n = 1, where the formula never exceeds 0).  The probability is in bestProbability().
    int getMax();
    double bestProbability() const { return best; }

private:
    int num;
    double best = 0.0;
};

}  // namespace introalgo
