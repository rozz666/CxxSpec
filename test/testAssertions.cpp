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
#include "AssertionTestingClasses.hpp"

struct AssertionTest : testing::Test
{
    template <typename F>
    void expectAssertionFailedWithExpectation(F call, const std::string& expectation)
    {
        try
        {
            call();
            FAIL() << "expected AssertionFailed";
        }
        catch (const CxxSpec::AssertionFailed& af)
        {
            EXPECT_EQ(expectation, af.expectation());
        }
    }
};

TEST_F(AssertionTest, toStringShouldUseLeftShiftOperatorToConvertItToStringAndNotCopyTheExpression)
{
    ASSERT_EQ("Printable", CxxSpec::toString(Printable()));
}

TEST_F(AssertionTest, toStringShouldPrintBoolCorrectly)
{
    ASSERT_EQ("true", CxxSpec::toString(true));
    ASSERT_EQ("false", CxxSpec::toString(false));
}

TEST_F(AssertionTest, CXXSPEC_EXPECT_shouldPassExpressionLineFileAndExpressionTextToExpectation)
{
    int line;
    try
    {
        line = __LINE__; CXXSPEC_EXPECT(true && false).should.beTrue();
        FAIL() << "expected AssertionFailed";
    }
    catch (const CxxSpec::AssertionFailed& af)
    {
        EXPECT_EQ(__FILE__, af.file());
        EXPECT_EQ(line, af.line());
        EXPECT_EQ("true && false", af.expression());
    }
}

TEST_F(AssertionTest, shouldShouldBeOfDifferentTypeThanExpectation)
{
    ASSERT_TRUE(typeid(CXXSPEC_EXPECT(0)) != typeid(CXXSPEC_EXPECT(0).should));
}

TEST_F(AssertionTest, beTrueShouldNotThrowWhenExpressionIsTrue)
{
    ASSERT_NO_THROW(CXXSPEC_EXPECT(true).should.beTrue());
}

TEST_F(AssertionTest, beTrueShouldThrowWhenExpressionIsFalse)
{
    expectAssertionFailedWithExpectation(
        []{ CXXSPEC_EXPECT(3 == 5).should.beTrue(); },
        "expected to be true but is false");
}

TEST_F(AssertionTest, operatorEqShouldNotThrowWhenExpressionsAreEqual)
{
    CXXSPEC_EXPECT(OperatorEqOnly(7)).should == OperatorEqOnly(7);
}

TEST_F(AssertionTest, operatorEqShouldThrowWhenExpressionsAreNotEqual)
{
    expectAssertionFailedWithExpectation(
        []{ CXXSPEC_EXPECT(OperatorEqOnly(7)).should == OperatorEqOnly(8); },
        "failed equality check");
}

TEST_F(AssertionTest, operatorEqShouldPrintValuesWhenExpressionsArePrintable)
{
    expectAssertionFailedWithExpectation(
        []{ CXXSPEC_EXPECT(4).should == 8; },
        "expected to equal 8 but equals 4");
}