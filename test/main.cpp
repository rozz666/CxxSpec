#include <iostream>
#include <CxxSpec/CxxSpec.hpp>
#include <CxxSpec/ConsoleSpecificationObserver.hpp>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    CxxSpec::ConsoleSpecificationObserver cso(std::cerr);
    CxxSpec::SpecificationRegistry::getInstance().runAll(cso);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}