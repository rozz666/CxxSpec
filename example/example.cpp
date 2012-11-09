#include <CxxSpec/CxxSpec.hpp>

#define describe CXXSPEC_DESCRIBE
#define it CXXSPEC_CONTEXT
#define expect CXXSPEC_EXPECT

describe("int")
{
    it("should be equal to same values")
    {
        expect(5 == 5).should.beTrue();
    }
}