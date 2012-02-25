#include <iostream>
#include "testSpecification.hpp"
#include "testSpecificationRegistry.hpp"
#include <CxxSpec/CxxSpec.hpp>
#include <CxxSpec/ConsoleSpecificationObserver.hpp>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testSpecification();
    testSpecificationRegistry();

    CxxSpec::ConsoleSpecificationObserver cso(std::cerr);
    CxxSpec::SpecificationRegistry::getInstance().runAll(cso);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
