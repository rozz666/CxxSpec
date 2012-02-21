#include <iostream>
#include "testSpecification.hpp"
#include "testExecutor.hpp"
#include "testAssertions.hpp"

int main(int argc, char **argv) {
    testSpecification();
    testExecutor();
    testAssertions();
}
