
// Test library configuration for googletest.cfg
//
// Usage:
// $ cppcheck --check-library --library=googletest --enable=information --error-exitcode=1 --inline-suppr --suppress=missingIncludeSystem test/cfg/googletest.cpp
// =>
// No warnings about bad library configuration, unmatched suppressions, etc. exitcode=0
//

#include <gmock/gmock-generated-matchers.h>

// #9397 syntaxError when MATCHER_P is not known
namespace ExampleNamespace {
    constexpr long long TOLERANCE = 10;

    MATCHER_P(ExampleMatcherTest, expected, "")
    {
        return ((arg <= (expected + TOLERANCE)) && (arg >= (expected - TOLERANCE)));
    }
}
