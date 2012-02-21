#include <iostream>
#include "testSpecification.hpp"
#include "testExecutor.hpp"
#include "testAssertions.hpp"
#include "testSpecificationRegistry.hpp"
#include <CxxSpec/CxxSpec.hpp>

int main(int argc, char **argv) {
    testSpecification();
    testExecutor();
    testAssertions();
    testSpecificationRegistry();

    CxxSpec::SpecificationRegistry::getInstance().runAll();
}
