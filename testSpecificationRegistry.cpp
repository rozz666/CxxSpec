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
#include <CxxSpec/Assert.hpp>
#include "testSpecificationRegistry.hpp"
#include <iostream>
#include <CxxSpec/ISpecificationObserver.hpp>

namespace
{

class SpecificationVisitorStub : public CxxSpec::ISpecificationVisitor
{
    virtual void beginSpecification() { }
    virtual void endSpecification() { }
    virtual bool beginSection(const std::string& ) { return false; }
    virtual void endSection() { }
};

struct SpecificationObserverFake : CxxSpec::ISpecificationObserver
{
    std::vector<std::string> expressions;

    virtual void testFailed(const CxxSpec::AssertionFailed& af)
    {
        expressions.push_back(af.expression());
    }
};

bool dummySpecification1Called;
bool dummySpecification2Called;
CxxSpec::ISpecificationVisitor *dummySpecification1Visitor;

void dummySpecification1(CxxSpec::ISpecificationVisitor& sv)
{
    dummySpecification1Called = true;
    dummySpecification1Visitor = &sv;
}

void dummySpecification2(CxxSpec::ISpecificationVisitor& sv)
{
    dummySpecification2Called = true;
}

void testRegisterAndRun()
{
    SpecificationVisitorStub sv;
    CxxSpec::SpecificationRegistry registry(sv);
    registry.registerSpecification("", &dummySpecification1);
    registry.registerSpecification("", &dummySpecification2);

    dummySpecification1Called = false;
    dummySpecification1Visitor = nullptr;
    dummySpecification2Called = false;

    SpecificationObserverFake so;
    registry.runAll(so);

    ASSERT_THAT(dummySpecification1Called);
    ASSERT_THAT(dummySpecification1Visitor == &sv);
    ASSERT_THAT(dummySpecification2Called);
}

void testRunEmpty()
{
    SpecificationVisitorStub sv;
    CxxSpec::SpecificationRegistry registry(sv);
    SpecificationObserverFake so;
    registry.runAll(so);
}

void specificationWithError1(CxxSpec::ISpecificationVisitor& sv)
{
    ASSERT_THAT(1 == 2);
}

void specificationWithError2(CxxSpec::ISpecificationVisitor& sv)
{
    ASSERT_THAT(3 != 3);
}

void testRunWithErrors()
{
    SpecificationVisitorStub sv;
    CxxSpec::SpecificationRegistry registry(sv);
    registry.registerSpecification("", &specificationWithError1);
    registry.registerSpecification("", &specificationWithError2);

    SpecificationObserverFake so;

    registry.runAll(so);
    ASSERT_THAT(so.expressions.size() == 2);
    ASSERT_THAT(so.expressions[0] == "1 == 2");
    ASSERT_THAT(so.expressions[1] == "3 != 3");
}

}

void testSpecificationRegistry()
{
    try
    {
        testRegisterAndRun();
        testRunEmpty();
        testRunWithErrors();
    }
    catch (const CxxSpec::AssertionFailed& af)
    {
        std::cerr << af.file() << ":" << af.line() << ": Assertion failed: " << af.expression() << std::endl;
    }
}
