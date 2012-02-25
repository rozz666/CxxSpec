#include <iostream>
#include "testSpecification.hpp"
#include "testExecutor.hpp"
#include "testSpecificationRegistry.hpp"
#include <CxxSpec/CxxSpec.hpp>
#include <CxxSpec/ConsoleSpecificationObserver.hpp>
#include "testConsoleSpecificationObserver.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testSpecification();
    testExecutor();
    testSpecificationRegistry();
    testConsoleSpecificationObserver();

    CxxSpec::ConsoleSpecificationObserver cso(std::cerr);
    CxxSpec::SpecificationRegistry::getInstance().runAll(cso);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
