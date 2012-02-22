#include <iostream>
#include "testSpecification.hpp"
#include "testExecutor.hpp"
#include "testAssertions.hpp"
#include "testSpecificationRegistry.hpp"
#include <CxxSpec/CxxSpec.hpp>
#include <CxxSpec/ConsoleSpecificationObserver.hpp>
#include "testConsoleSpecificationObserver.hpp"

int main(int argc, char **argv) {
    testSpecification();
    testExecutor();
    testAssertions();
    testSpecificationRegistry();
    testConsoleSpecificationObserver();

    CxxSpec::ConsoleSpecificationObserver cso(std::cerr);
    CxxSpec::SpecificationRegistry::getInstance().runAll(cso);
}
