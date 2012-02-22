#include <iostream>
#include "testSpecification.hpp"
#include "testExecutor.hpp"
#include "testAssertions.hpp"
#include "testSpecificationRegistry.hpp"
#include <CxxSpec/CxxSpec.hpp>
#include <CxxSpec/ConsoleSpecificationObserver.hpp>

int main(int argc, char **argv) {
    testSpecification();
    testExecutor();
    testAssertions();
    testSpecificationRegistry();

    CxxSpec::ConsoleSpecificationObserver cso;
    CxxSpec::SpecificationRegistry::getInstance().runAll(cso);
}
