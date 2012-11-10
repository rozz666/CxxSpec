#ifndef SPECIFICATIONOBSERVERMOCK_HPP
#define SPECIFICATIONOBSERVERMOCK_HPP
#include <CxxSpec/ISpecificationObserver.hpp>

struct SpecificationObserverMock: CxxSpec::ISpecificationObserver
{
    MOCK_METHOD1(testFailed, void(const CxxSpec::AssertionFailed& ));
};

#endif // SPECIFICATIONOBSERVERMOCK_HPP
