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

#include <CxxSpec/SpecificationRegistry.hpp>
#include <iostream>
#include <CxxSpec/ISpecificationObserver.hpp>
#include <gmock/gmock.h>
#include <SpecificationVisitorMock.hpp>
#include "SpecificationObserverMock.hpp"

using namespace testing;

struct SpecificationRegistryTest : testing::Test
{
    std::shared_ptr<SpecificationVisitorMock> visitor1;
    std::shared_ptr<SpecificationVisitorMock> visitor2;
    CxxSpec::SpecificationRegistry registry;
    NiceMock<SpecificationObserverMock> observer;

    MOCK_METHOD0(visitorFactory, std::shared_ptr<CxxSpec::ISpecificationVisitor>());

    SpecificationRegistryTest()
        : visitor1(std::make_shared<NiceMock<SpecificationVisitorMock>>()),
        visitor2(std::make_shared<NiceMock<SpecificationVisitorMock>>()),
        registry([&]{ return visitorFactory(); })
    {
        ON_CALL(*visitor1, beginSection(_)).WillByDefault(Return(false));
        ON_CALL(*visitor2, beginSection(_)).WillByDefault(Return(false));
        ON_CALL(*visitor1, done()).WillByDefault(Return(true));
        ON_CALL(*visitor2, done()).WillByDefault(Return(true));
    }

    static bool dummySpecification1Called;
    static bool dummySpecification2Called;
    static CxxSpec::ISpecificationVisitor *dummySpecification1Visitor;

    static void dummySpecification1(CxxSpec::ISpecificationVisitor& visitor)
    {
        dummySpecification1Called = true;
        dummySpecification1Visitor = &visitor;
    }

    static void dummySpecification2(CxxSpec::ISpecificationVisitor& )
    {
        dummySpecification2Called = true;
    }

    static void specificationWithError1(CxxSpec::ISpecificationVisitor& )
    {
        throw CxxSpec::AssertionFailed("", 1, "", "");
    }

    static void specificationWithError2(CxxSpec::ISpecificationVisitor& )
    {
        throw CxxSpec::AssertionFailed("", 2, "", "");
    }
};

bool SpecificationRegistryTest::dummySpecification1Called = false;
bool SpecificationRegistryTest::dummySpecification2Called = false;
CxxSpec::ISpecificationVisitor *SpecificationRegistryTest::dummySpecification1Visitor = nullptr;


TEST_F(SpecificationRegistryTest, shouldRunSpecificationsInOrderAndPassNewVisitorForEachOne)
{
    registry.registerSpecification("", &dummySpecification1);
    registry.registerSpecification("", &dummySpecification2);

    EXPECT_CALL(*this, visitorFactory())
        .WillOnce(Return(visitor1))
        .WillOnce(Return(visitor2));

    dummySpecification1Called = false;
    dummySpecification1Visitor = nullptr;
    dummySpecification2Called = false;

    registry.runAll(observer);

    ASSERT_TRUE(dummySpecification1Called);
    ASSERT_TRUE(dummySpecification1Visitor == &*visitor1);
    ASSERT_TRUE(dummySpecification2Called);
}

TEST_F(SpecificationRegistryTest, shouldDoNothingWhenEmpty)
{
    registry.runAll(observer);
}

TEST_F(SpecificationRegistryTest, shouldCatchFailedAssertions)
{
    registry.registerSpecification("", &specificationWithError1);
    registry.registerSpecification("", &specificationWithError2);

    EXPECT_CALL(*this, visitorFactory())
        .WillOnce(Return(visitor1))
        .WillOnce(Return(visitor2));
    EXPECT_CALL(observer, testFailed(Property(&CxxSpec::AssertionFailed::line, 1)));
    EXPECT_CALL(*visitor1, caughtException());
    EXPECT_CALL(observer, testFailed(Property(&CxxSpec::AssertionFailed::line, 2)));
    EXPECT_CALL(*visitor2, caughtException());

    registry.runAll(observer);
}

TEST_F(SpecificationRegistryTest, shouldVisitSpecificationUntilDoneAndCatchAssertionsEachTime)
{
    registry.registerSpecification("", &specificationWithError1);

    EXPECT_CALL(*this, visitorFactory())
        .WillOnce(Return(visitor1));
    {
        InSequence seq;
        EXPECT_CALL(observer, testFailed(_));
        EXPECT_CALL(*visitor1, done()).WillOnce(Return(false));
        EXPECT_CALL(observer, testFailed(_));
        EXPECT_CALL(*visitor1, done()).WillOnce(Return(false));
        EXPECT_CALL(observer, testFailed(_));
        EXPECT_CALL(*visitor1, done()).WillOnce(Return(true));
    }

    registry.runAll(observer);
}

TEST_F(SpecificationRegistryTest, shouldNotifyObserverAboutSpecificationName)
{
    registry.registerSpecification("spec1", &dummySpecification1);

    EXPECT_CALL(*this, visitorFactory())
        .WillOnce(Return(visitor1));
    EXPECT_CALL(*visitor1, done())
        .WillOnce(Return(false))
        .WillOnce(Return(true));

    EXPECT_CALL(observer, testingSpecification("spec1"));

    registry.runAll(observer);
}
