/*
    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/


#ifndef CXXSPEC_ASSERT_HPP
#define CXXSPEC_ASSERT_HPP
#include <CxxSpec/AssertionFailed.hpp>
#include <sstream>

namespace CxxSpec
{

template <typename Expression>
inline std::string toString(const Expression& expr)
{
    std::ostringstream os;
    os << expr;
    return os.str();
}

template <typename Expression>
class Should
{
public:
    Should(Expression expr, std::string file, int line, std::string exprText)
        : expr(expr), file(file), line(line), exprText(exprText) { }

    void beTrue()
    {
        if (!expr)
            throwAssertionFailed("expected to be true but is false");
    }

    void operator==(Expression expected)
    {
        if (!(expr == expected))
            throwAssertionFailed("failed equality check");
    }

private:
    Expression expr;
    std::string file;
    int line;
    std::string exprText;

    void throwAssertionFailed(const std::string& expectation)
    {
        throw AssertionFailed(file, line, exprText, expectation);
    }
};

template <typename Expression>
class Expectation
{
public:
    Should<Expression> should;

    Expectation(Expression expr, std::string file, int line, std::string exprText)
        : should(expr, file, line, exprText) { }
};

template <typename Expression>
inline Expectation<Expression> makeExpectation(Expression expr, std::string file, int line, std::string exprText)
{
    return Expectation<Expression>(expr, file, line, exprText);
}

#define CXXSPEC_EXPECT(expr) CxxSpec::makeExpectation(expr, __FILE__, __LINE__, #expr)

}

#endif // CXXSPEC_ASSERT_HPP
