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


#ifndef CXXSPEC_SPECIFICATIONEXECUTOR_HPP
#define CXXSPEC_SPECIFICATIONEXECUTOR_HPP
#include <CxxSpec/ISpecificationVisitor.hpp>
#include <CxxSpec/ISpecificationObserver.hpp>
#include <vector>

namespace CxxSpec {

class SpecificationExecutor : public ISpecificationVisitor
{
public:
    SpecificationExecutor()
        : assumeMoreSectionsToVisit(false)
    {
        markEnterFirstSection();
    }

    virtual void beginSpecification()
    {
        state = State::following();

        followNextPath();
        markEnterFirstSection();
    }

    virtual void endSpecification()
    {
    }

    virtual bool beginSection(const std::string& desc)
    {
        return state.beginSection(*this, desc);
    }

    virtual void endSection()
    {
        state.endSection(*this);
    }

    virtual bool done() const
    {
        return !assumeMoreSectionsToVisit;
    }

    virtual void caughtException()
    {
        if (state.moreSectionsPossible())
            assumeMoreSectionsToVisit = true;
    }

    void setObserver(std::shared_ptr<ISpecificationObserver> observer)
    {
        this->observer = observer;
    }

private:

    struct State
    {
        typedef bool (SpecificationExecutor:: *BeginSection)(const std::string& desc);
        typedef void (SpecificationExecutor:: *EndSection)();
        BeginSection beginSection_;
        EndSection endSection_;
        bool moreSectionsPossible_;

        static State following()
        {
            return State(
                &SpecificationExecutor::following_beginSection,
                &SpecificationExecutor::following_endSection,
                false);
        }

        static State running()
        {
            return State(
                &SpecificationExecutor::running_beginSection,
                &SpecificationExecutor::running_endSection,
                true);
        }

        static State finishing()
        {
            return State(
                &SpecificationExecutor::finishing_beginSection,
                &SpecificationExecutor::finishing_endSection,
                true);
        }

        static State expectingSection()
        {
            return State(
                &SpecificationExecutor::expectSection_beginSection,
                &SpecificationExecutor::expectSection_endSection,
                true);
        }

        State(BeginSection begin, EndSection end, bool moreSectionsPossible)
            : beginSection_(begin), endSection_(end), moreSectionsPossible_(moreSectionsPossible) { }

        State() { *this = finishing(); }

        bool beginSection(SpecificationExecutor& executor, const std::string& desc) const
        {
            return (executor.*beginSection_)(desc);
        }

        void endSection(SpecificationExecutor& executor) const
        {
            (executor.*endSection_)();
        }

        bool moreSectionsPossible() const { return moreSectionsPossible_; }
    };

    State state;
    std::vector<int> currentPath, nextPath;
    bool assumeMoreSectionsToVisit;
    std::shared_ptr<ISpecificationObserver> observer;

    void followNextPath()
    {
        currentPath.assign(nextPath.rbegin(), nextPath.rend());
        nextPath.clear();
        assumeMoreSectionsToVisit = false;
    }

    bool shouldEnterSection()
    {
        if (currentPath.back() == 0)
        {
            currentPath.pop_back();
            if (currentPath.empty())
            {
                state = State::running();
            }
            return true;
        }

        currentPath.back()--;
        return false;
    }

    void markEnterFirstSection()
    {
        nextPath.push_back(0);
    }

    void markLeaveSectionAndEnterNext()
    {
        nextPath.pop_back();
        nextPath.back()++;
    }

    bool following_beginSection(const std::string& desc)
    {
        markEnterFirstSection();
        if (!shouldEnterSection()) return false;
        if (observer) observer->enteredContext(desc);
        return true;
    }

    void following_endSection()
    {
        markLeaveSectionAndEnterNext();
    }

    bool running_beginSection(const std::string& desc)
    {
        if (observer) observer->enteredContext(desc);
        markEnterFirstSection();
        return true;
    }

    void running_endSection()
    {
        if (observer) observer->leftContext();
        state = State::expectingSection();

        markLeaveSectionAndEnterNext();
    }

    bool expectSection_beginSection(const std::string& desc)
    {
        assumeMoreSectionsToVisit = true;

        state = State::finishing();
        return false;
    }

    void expectSection_endSection()
    {
        if (observer) observer->leftContext();
        markLeaveSectionAndEnterNext();
    }

    bool finishing_beginSection(const std::string&)
    {
        return false;
    }

    void finishing_endSection()
    {
    }
};

}

#endif // CXXSPEC_SPECIFICATIONEXECUTOR_HPP
