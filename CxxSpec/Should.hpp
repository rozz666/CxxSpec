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


#ifndef CXXSPEC_SHOULD_HPP
#define CXXSPEC_SHOULD_HPP
#include <CxxSpec/AssertionFailed.hpp>
#include <CxxSpec/Messages.hpp>
#include <type_traits>

namespace CxxSpec {

template <typename Expression, typename ExpressionType>
class ShouldBase
{
public:
    ShouldBase(Expression expr, std::string file, int line, std::string exprText)
        : expr(expr), file(file), line(line), exprText(exprText) { }

    template <typename E>
    void throwException()
    {
        try
        {
            expr();
            throwAssertionFailed("expected to throw an exception");
        }
        catch (const AssertionFailed& )
        {
            throw;
        }
        catch (const E&)
        {
        }
        catch (...)
        {
            throwAssertionFailed("has thrown an unexpected exception");
        }
    }

protected:
    Expression expr;
    std::string file;
    int line;
    std::string exprText;

    typedef CxxSpec::Messages<ExpressionType> Messages;

    void throwAssertionFailed(const std::string& expectation)
    {
        throw AssertionFailed(file, line, exprText, expectation);
    }
};

template <typename Expression, typename ExpressionType = typename std::result_of<Expression()>::type>
class Should : public ShouldBase<Expression, ExpressionType>
{
    typedef ShouldBase<Expression, ExpressionType> Base;
public:

    Should(Expression expr, std::string file, int line, std::string exprText)
        : Base(expr, file, line, exprText) { }

    void beTrue()
    {
        if (!Base::expr())
            Base::throwAssertionFailed("expected to be true but is false");
    }

    void beFalse()
    {
        if (Base::expr())
            Base::throwAssertionFailed("expected to be false but is true");
    }

    void operator==(ExpressionType expected)
    {
        auto val = Base::expr();
        if (!(val == expected))
            Base::throwAssertionFailed(Base::Messages::equalityFailed(val, expected));
    }

};

template <typename Expression>
class Should<Expression, void> : public ShouldBase<Expression, void>
{
    typedef ShouldBase<Expression, void> Base;
public:
    Should(Expression expr, std::string file, int line, std::string exprText)
        : Base(expr, file, line, exprText) { }
};

}

#endif // CXXSPEC_SHOULD_HPP
