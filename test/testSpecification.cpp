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

#include <CxxSpec/Specification.hpp>
#include <type_traits>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <SpecificationVisitorMock.hpp>
#include <stdexcept>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;


namespace CxxSpec
{
namespace
{

std::map<std::string, SpecificationFunction> registeredSpec;

void registerSpecification(const std::string& desc, SpecificationFunction func)
{
    registeredSpec.insert(std::make_pair(desc, func));
}

}
}







static_assert(__LINE__ == 62, "must be line 62");
CXXSPEC_DESCRIBE("An empty specification") // line 63
{
}

static_assert(__LINE__ == 67, "must be line 67");
CXXSPEC_DESCRIBE("another empty specification") // line 68
{
}

TEST(SpecificationTest, RegisterSpecification)
{
    ASSERT_EQ(&CxxSpec__Specification_at_line_63, CxxSpec::registeredSpec["An empty specification"]);
    ASSERT_EQ(&CxxSpec__Specification_at_line_68, CxxSpec::registeredSpec["another empty specification"]);
}

CXXSPEC_DESCRIBE("one section")
{
    CXXSPEC_CONTEXT("one")
    {
    }
}

TEST(SpecificationTest, OneSection)
{
    StrictMock<SpecificationVisitorMock> sv;
    {
        InSequence seq;
        EXPECT_CALL(sv, beginSpecification());
        EXPECT_CALL(sv, beginSection(_))
            .WillOnce(Return(false));
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, endSpecification());
    }
    CxxSpec::registeredSpec["one section"](sv);
}

CXXSPEC_DESCRIBE("nested sections")
{
    CXXSPEC_CONTEXT("outer")
    {
        CXXSPEC_CONTEXT("inner")
        {
        }
    }
}

TEST(SpecificationTest, NestedSections)
{
    StrictMock<SpecificationVisitorMock> sv;
    {
        InSequence seq;
        EXPECT_CALL(sv, beginSpecification());
        EXPECT_CALL(sv, beginSection("outer"))
            .WillOnce(Return(true));
        EXPECT_CALL(sv, beginSection("inner"))
            .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, endSpecification());
    }
    CxxSpec::registeredSpec["nested sections"](sv);
}

CXXSPEC_DESCRIBE("section sequence")
{
    CXXSPEC_CONTEXT("1") { }
    CXXSPEC_CONTEXT("2") { }
    CXXSPEC_CONTEXT("3") { }
}

TEST(SpecificationTest, SectionSequence)
{
    NiceMock<SpecificationVisitorMock> sv;
    {
        InSequence seq;
        EXPECT_CALL(sv, beginSection("1"))
             .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, beginSection("2"))
            .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, beginSection("3"))
            .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
    }
    CxxSpec::registeredSpec["section sequence"](sv);
}

CXXSPEC_DESCRIBE("nested with exception")
{
    CXXSPEC_CONTEXT("1")
    {
        CXXSPEC_CONTEXT("2")
        {
            throw std::runtime_error("error");
        }
    }
}

TEST(SpecificationTest, nextedWithException)
{
    StrictMock<SpecificationVisitorMock> sv;
    {
        InSequence seq;
        EXPECT_CALL(sv, beginSpecification());
        EXPECT_CALL(sv, beginSection(_)).Times(2).WillRepeatedly(Return(true));
        EXPECT_CALL(sv, endSection()).Times(2);
        EXPECT_CALL(sv, endSpecification());
    }
    ASSERT_THROW(CxxSpec::registeredSpec["nested with exception"](sv), std::runtime_error);
}

CXXSPEC_DESCRIBE("sequence with exception")
{
    CXXSPEC_CONTEXT("1") { }
    CXXSPEC_CONTEXT("2")
    {
        throw std::runtime_error("error");
    }
    CXXSPEC_CONTEXT("3") { }
}

TEST(SpecificationTest, sequnceWithException)
{
    NiceMock<SpecificationVisitorMock> sv;
    {
        InSequence seq;
        EXPECT_CALL(sv, beginSection("1"))
             .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
        EXPECT_CALL(sv, beginSection("2"))
            .WillOnce(Return(true));
        EXPECT_CALL(sv, endSection());
    }
    ASSERT_THROW(CxxSpec::registeredSpec["sequence with exception"](sv), std::runtime_error);
}

namespace
{

int checkExecuteParam;
void checkExecute(int param)
{
    checkExecuteParam = param;
}

}

CXXSPEC_DESCRIBE("execution")
{
    auto x = 7;
    checkExecute(x);
}

TEST(SpecificationTest, SpecificationExecution)
{
    NiceMock<SpecificationVisitorMock> sv;
    checkExecuteParam = 0;
    CxxSpec::registeredSpec["execution"](sv);
    ASSERT_EQ(7, checkExecuteParam);
}

namespace
{

int selectionIndex;

class SelectionVisitor : public CxxSpec::ISpecificationVisitor
{
public:

    SelectionVisitor(const std::set<std::string>& selection)
        : selection(selection) { }

    virtual void beginSpecification() { }
    virtual void endSpecification() { }

    virtual bool beginSection(const std::string& desc)
    {
        return selection.count(desc) > 0;
    }

    virtual void endSection() { }

private:
    std::set<std::string> selection;
};

}

CXXSPEC_DESCRIBE("selection")
{
    CXXSPEC_CONTEXT("first")
    {
        selectionIndex = 1;
        CXXSPEC_CONTEXT("second")
        {
            selectionIndex = 2;
        }
    }
    CXXSPEC_CONTEXT("third")
    {
        selectionIndex = 3;
    }
}

void havingRunSelection(std::set<std::string> selection)
{
    selectionIndex = 0;
    SelectionVisitor sv(selection);
    CxxSpec::registeredSpec["selection"](sv);
}

TEST(SpecificationTest, shouldExecuteFirst)
{
    havingRunSelection({ "first" });
    ASSERT_EQ(1, selectionIndex);
}

TEST(SpecificationTest, shouldExecuteFirstWithSecond)
{
    havingRunSelection({ "first", "second" });
    ASSERT_EQ(2, selectionIndex);
}

TEST(SpecificationTest, shouldExecuteThird)
{
    havingRunSelection({ "third" });
    ASSERT_EQ(3, selectionIndex);
}

TEST(SpecificationTest, shouldNotExecuteSecond)
{
    havingRunSelection({ "second" });
    ASSERT_EQ(0, selectionIndex);
}