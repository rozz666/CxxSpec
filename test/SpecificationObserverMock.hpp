#ifndef SPECIFICATIONOBSERVERMOCK_HPP
#define SPECIFICATIONOBSERVERMOCK_HPP
#include <CxxSpec/ISpecificationObserver.hpp>

struct SpecificationObserverMock: CxxSpec::ISpecificationObserver
{
    MOCK_METHOD1(testFailed, void(const CxxSpec::AssertionFailed& ));
    MOCK_METHOD1(testingSpecification, void(const std::string&));
};

#endif // SPECIFICATIONOBSERVERMOCK_HPP
