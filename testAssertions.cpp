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


#include <CxxSpec/Assert.hpp>
#include <sstream>
#include <gtest/gtest.h>

TEST(AssertionTest, beTrueShouldNotThrowWhenExpressionIsTrue)
{
    ASSERT_NO_THROW(
    {
        CXXSPEC_EXPECT(true).should.beTrue();
    });
}

TEST(AssertionTest, beTrueShouldThrowWhenExpressionIsFalse)
{
    int line;
    try
    {
        line = __LINE__; CXXSPEC_EXPECT(3 == 5).should.beTrue();
        FAIL() << "must throw";
    }
    catch (const CxxSpec::AssertionFailed& af)
    {
        EXPECT_EQ(__FILE__, af.file());
        EXPECT_EQ(line, af.line());
        EXPECT_EQ("3 == 5", af.expression());
        EXPECT_EQ("expected to be true but is false", af.expectation());
    }
}
