#include <CxxSpec/CxxSpec.hpp>

#define describe CXXSPEC_DESCRIBE
#define it CXXSPEC_CONTEXT
#define context CXXSPEC_CONTEXT
#define expect CXXSPEC_EXPECT

describe("int")
{
    it("should be equal to same values")
    {
        expect(5 == 5).should.beTrue();
    }
}

describe("std::vector<int>")
{
    std::vector<int> v;

    it("should be empty when created")
    {
        expect(v.empty()).should == true;
    }

    it("should have size equal to the number of pushed elements")
    {
        v.push_back(7);
        expect(v.size()).should == 1u;
        v.push_back(8);
        expect(v.size()).should == 2u;
    }

    context("with [ 3, 7 ]")
    {
        v = { 3, 7 };
        it("should provide accessors")
        {
            expect(v[0]).should == 3;
            expect(v[1]).should == 7;
        }
        it("should shrink when poping elements")
        {
            v.pop_back();
            expect(v.size()).should == 1u;
            v.pop_back();
            expect(v.size()).should == 0u;
        }
    }
}
