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

#include "testSpecification.hpp"
#include <cassert>
#include <CxxSpec/Specification.hpp>
#include <type_traits>
#include <string>
#include <map>
#include <vector>
#include <set>






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
SPECIFICATION("An empty specification") // line 63
{
}

static_assert(__LINE__ == 67, "must be line 67");
SPECIFICATION("another empty specification") // line 68
{
}

void testRegisterSpecification()
{
    assert(CxxSpec::registeredSpec["An empty specification"] == &CxxSpec__Specification_at_line_63);
    assert(CxxSpec::registeredSpec["another empty specification"] == &CxxSpec__Specification_at_line_68);
}

SPECIFICATION("one section")
{
    SECTION("one")
    {
    }
}

namespace
{

enum SpecOp
{
    BEGIN_SPECIFICATION,
    END_SPECIFICATION,
    BEGIN_SECTION,
    END_SECTION
};

class SpecificationVisitor : public ::CxxSpec::ISpecificationVisitor
{
public:
    std::vector<SpecOp> ops;
    std::vector<std::string> descs;

    virtual void beginSpecification()
    {
        ops.push_back(BEGIN_SPECIFICATION);
    }

    virtual void endSpecification()
    {
        ops.push_back(END_SPECIFICATION);
    }

    virtual bool beginSection(const std::string& desc)
    {
        ops.push_back(BEGIN_SECTION);
        descs.push_back(desc);
        return true;
    }

    virtual void endSection()
    {
        ops.push_back(END_SECTION);
    }
};

}

void testOneSection()
{
    SpecificationVisitor sv;
    CxxSpec::registeredSpec["one section"](sv);
    assert(sv.ops.size() == 4);
    assert(sv.ops[0] == BEGIN_SPECIFICATION);
    assert(sv.ops[1] == BEGIN_SECTION);
    assert(sv.ops[2] == END_SECTION);
    assert(sv.ops[3] == END_SPECIFICATION);
}

SPECIFICATION("nested sections")
{
    SECTION("outer")
    {
        SECTION("inner")
        {
        }
    }
}

void testNestedSections()
{
    SpecificationVisitor sv;
    CxxSpec::registeredSpec["nested sections"](sv);
    assert(sv.ops.size() == 6);
    assert(sv.ops[0] == BEGIN_SPECIFICATION);
    assert(sv.ops[1] == BEGIN_SECTION);
    assert(sv.descs[0] == "outer");
    assert(sv.ops[2] == BEGIN_SECTION);
    assert(sv.descs[1] == "inner");
    assert(sv.ops[3] == END_SECTION);
    assert(sv.ops[4] == END_SECTION);
    assert(sv.ops[5] == END_SPECIFICATION);
}

SPECIFICATION("section sequence")
{
    SECTION("1") { }
    SECTION("2") { }
    SECTION("3") { }
}

void testSectionSequence()
{
    SpecificationVisitor sv;
    CxxSpec::registeredSpec["section sequence"](sv);
    assert(sv.ops.size() == 8);
    assert(sv.ops[1] == BEGIN_SECTION);
    assert(sv.ops[3] == BEGIN_SECTION);
    assert(sv.ops[5] == BEGIN_SECTION);
    assert(sv.ops[6] == END_SECTION);
}

namespace
{

int checkExecuteParam;
void checkExecute(int param)
{
    checkExecuteParam = param;
}

}

SPECIFICATION("execution")
{
    auto x = 7;
    checkExecute(x);
}

void testSpecificationExecution()
{
    SpecificationVisitor sv;
    checkExecuteParam = 0;
    CxxSpec::registeredSpec["execution"](sv);
    assert(checkExecuteParam == 7);
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

SPECIFICATION("selection")
{
    SECTION("first")
    {
        selectionIndex = 1;
        SECTION("second")
        {
            selectionIndex = 2;
        }
    }
    SECTION("third")
    {
        selectionIndex = 3;
    }
}

void runSelection(std::set<std::string> selection)
{
    selectionIndex = 0;
    SelectionVisitor sv(selection);
    CxxSpec::registeredSpec["selection"](sv);
}

void testSectionSelectionFirst()
{
    std::set<std::string> selection;
    selection.insert("first");
    runSelection(selection);
    assert(selectionIndex == 1);
}

void testSectionSelectionSecond()
{
    std::set<std::string> selection;
    selection.insert("first");
    selection.insert("second");
    runSelection(selection);
    assert(selectionIndex == 2);
}

void testSectionSelectionThird()
{
    std::set<std::string> selection;
    selection.insert("third");
    runSelection(selection);
    assert(selectionIndex == 3);
}

void testSectionSelectionNone()
{
    std::set<std::string> selection;
    selection.insert("second");
    runSelection(selection);
    assert(selectionIndex == 0);
}

void testSpecification()
{
    testRegisterSpecification();
    testOneSection();
    testNestedSections();
    testSectionSequence();
    testSpecificationExecution();
    testSectionSelectionFirst();
    testSectionSelectionSecond();
    testSectionSelectionThird();
    testSectionSelectionNone();
}
