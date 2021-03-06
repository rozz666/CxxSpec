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
#include <CxxSpec/SpecificationExecutor.hpp>
#include <map>
#include <vector>
#include <stdexcept>
#include <gmock/gmock.h>
#include "SpecificationObserverMock.hpp"

using namespace testing;

namespace CxxSpec
{

struct SpecificationExecutorTest : testing::Test
{
    static std::map<std::string, SpecificationFunction> registeredSpec;
    static std::vector<int> steps;

    std::shared_ptr<SpecificationObserverMock> observer;
    std::shared_ptr<SpecificationExecutor> executor;

    SpecificationExecutorTest()
        : observer(std::make_shared<NiceMock<SpecificationObserverMock>>()),
        executor(std::make_shared<SpecificationExecutor>(nullptr))
    {
    }

    static void step(int n)
    {
        steps.push_back(n);
    }

    void havingExecuted(const std::string& specName)
    {
        steps.clear();
        registeredSpec[specName](*executor);
    }
};

std::map<std::string, SpecificationFunction> SpecificationExecutorTest::registeredSpec;
std::vector<int> SpecificationExecutorTest::steps;

namespace
{

void registerSpecification(const std::string& desc, SpecificationFunction func)
{
    SpecificationExecutorTest::registeredSpec.insert({ desc, func });
}

}


CXXSPEC_DESCRIBE("no sections")
{
    SpecificationExecutorTest::step(1);
}

TEST_F(SpecificationExecutorTest, shouldExecuteDescriptionWithNoSections)
{
    havingExecuted("no sections");
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1));
}

CXXSPEC_DESCRIBE("one section")
{
    SpecificationExecutorTest::step(1);
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(2);
    }
}

TEST_F(SpecificationExecutorTest, shouldExecuteDescriptionWithOneSection)
{
    havingExecuted("one section");
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1, 2));
}

CXXSPEC_DESCRIBE("parallel")
{
    SpecificationExecutorTest::step(1);
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(11);
    }
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(12);
    }
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(13);
    }
    SpecificationExecutorTest::step(2);
}

TEST_F(SpecificationExecutorTest, shouldExecuteSectionsInOrder)
{
    havingExecuted("parallel");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1, 11, 2));

    havingExecuted("parallel");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1, 12, 2));

    havingExecuted("parallel");
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1, 13, 2));
}

CXXSPEC_DESCRIBE("nested")
{
    SpecificationExecutorTest::step(1);
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(11);
        CXXSPEC_CONTEXT("")
        {
            SpecificationExecutorTest::step(111);
            CXXSPEC_CONTEXT("")
            {
                SpecificationExecutorTest::step(1111);
            }
        }
    }
}

TEST_F(SpecificationExecutorTest, shouldExecuteAllNestedSectionOnce)
{
    havingExecuted("nested");
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1, 11, 111, 1111));
}

CXXSPEC_DESCRIBE("parallel and nested")
{
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(1);
    }
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(2);
        CXXSPEC_CONTEXT("")
        {
            SpecificationExecutorTest::step(21);
            CXXSPEC_CONTEXT("")
            {
                SpecificationExecutorTest::step(211);
            }
        }
        CXXSPEC_CONTEXT("")
        {
            SpecificationExecutorTest::step(22);
        }
        CXXSPEC_CONTEXT("")
        {
            SpecificationExecutorTest::step(23);
        }
    }
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(3);
    }
}

TEST_F(SpecificationExecutorTest, shouldExecuteSectionsDepthFirst)
{
    havingExecuted("parallel and nested");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1));

    havingExecuted("parallel and nested");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(2, 21, 211));

    havingExecuted("parallel and nested");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(2, 22));

    havingExecuted("parallel and nested");
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(2, 23));

    havingExecuted("parallel and nested");
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre(3));
}

CXXSPEC_DESCRIBE("no section with exception")
{
    throw std::runtime_error("error");
}

TEST_F(SpecificationExecutorTest, shouldFinishDescriptionWithNoSectionsAndPropagateTheException)
{
    ASSERT_ANY_THROW(havingExecuted("no section with exception"));
    executor->caughtException();
    ASSERT_TRUE(executor->done());
}

CXXSPEC_DESCRIBE("nested sections with exception")
{
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(1);
        CXXSPEC_CONTEXT("")
        {
            CXXSPEC_CONTEXT("")
            {
                throw std::runtime_error("");
            }
        }
    }
}

TEST_F(SpecificationExecutorTest, shouldFinishDescriptionWithNestedSectionsAndPropagateTheException)
{
    ASSERT_ANY_THROW(havingExecuted("nested sections with exception"));
    executor->caughtException();
    ASSERT_FALSE(executor->done());

    ASSERT_NO_THROW(havingExecuted("nested sections with exception"));
    ASSERT_TRUE(executor->done());
    ASSERT_THAT(steps, ElementsAre());
}

CXXSPEC_DESCRIBE("parallel with exceptions")
{
    CXXSPEC_CONTEXT("")
    {
        throw std::invalid_argument("");
    }
    CXXSPEC_CONTEXT("")
    {
        throw std::length_error("");
    }
    CXXSPEC_CONTEXT("")
    {
        throw std::out_of_range("");
    }
}

TEST_F(SpecificationExecutorTest, shouldExecuteSectionsInOrderAndPropagateExceptions)
{
    ASSERT_THROW(havingExecuted("parallel with exceptions"), std::invalid_argument);
    executor->caughtException();
    ASSERT_FALSE(executor->done());

    ASSERT_THROW(havingExecuted("parallel with exceptions"), std::length_error);
    executor->caughtException();
    ASSERT_FALSE(executor->done());

    ASSERT_THROW(havingExecuted("parallel with exceptions"), std::out_of_range);
    executor->caughtException();
    ASSERT_FALSE(executor->done());

    ASSERT_NO_THROW(havingExecuted("parallel with exceptions"));
    ASSERT_TRUE(executor->done());

    ASSERT_NO_THROW(havingExecuted("parallel with exceptions"));
    ASSERT_TRUE(executor->done());
}

CXXSPEC_DESCRIBE("exception after sections")
{
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(1);
    }
    CXXSPEC_CONTEXT("")
    {
        SpecificationExecutorTest::step(2);
    }
    throw std::runtime_error("");
}

TEST_F(SpecificationExecutorTest, shouldExecuteAllSectionsWhenExceptionIsThrownAfterThem)
{
    ASSERT_ANY_THROW(havingExecuted("exception after sections"));
    executor->caughtException();
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(1));

    ASSERT_ANY_THROW(havingExecuted("exception after sections"));
    executor->caughtException();
    ASSERT_FALSE(executor->done());
    ASSERT_THAT(steps, ElementsAre(2));

    ASSERT_ANY_THROW(havingExecuted("exception after sections"));
    ASSERT_TRUE(executor->done());
}

CXXSPEC_DESCRIBE("contexts")
{
    CXXSPEC_CONTEXT("a")
    {
        CXXSPEC_CONTEXT("b")
        {
        }
    }

    CXXSPEC_CONTEXT("c")
    {
    }
}

TEST_F(SpecificationExecutorTest, shouldNotifyAboutEnteringAndLeavingSections)
{
    executor = std::make_shared<SpecificationExecutor>(observer);
    InSequence seq;
    EXPECT_CALL(*observer, enteredContext("a"));
    EXPECT_CALL(*observer, enteredContext("b"));
    EXPECT_CALL(*observer, leftContext()).Times(2);
    havingExecuted("contexts");

    EXPECT_CALL(*observer, enteredContext("c"));
    EXPECT_CALL(*observer, leftContext());
    havingExecuted("contexts");

    // no contexts
    havingExecuted("contexts");
}

CXXSPEC_DESCRIBE("nested consecutive contexts")
{
    CXXSPEC_CONTEXT("a")
    {
        CXXSPEC_CONTEXT("b");
        CXXSPEC_CONTEXT("c");
    }
}

TEST_F(SpecificationExecutorTest, shouldNotifyAboutEnteringAndLeavingNestedConsecutiveSections)
{
    executor = std::make_shared<SpecificationExecutor>(observer);
    InSequence seq;

    EXPECT_CALL(*observer, enteredContext("a"));
    EXPECT_CALL(*observer, enteredContext("b"));
    EXPECT_CALL(*observer, leftContext()).Times(2);
    havingExecuted("nested consecutive contexts");

    EXPECT_CALL(*observer, enteredContext("a"));
    EXPECT_CALL(*observer, enteredContext("c"));
    EXPECT_CALL(*observer, leftContext()).Times(2);
    havingExecuted("nested consecutive contexts");

    havingExecuted("nested consecutive contexts");
}

}