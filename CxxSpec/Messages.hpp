/*
    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/


#ifndef CXXSPEC_MESSAGES_HPP
#define CXXSPEC_MESSAGES_HPP
#include <iomanip>
#include <sstream>

namespace CxxSpec {

template <typename Expression>
inline std::string toString(const Expression& expr)
{
    std::ostringstream os;
    os << std::boolalpha << expr;
    return os.str();
}

namespace Detail
{

struct UniqType
{
    char c[17943];
};

template <typename T>
UniqType operator<<(std::ostream&, const T& );

template <typename T>
struct IsPrintable {
    static const bool value =
        sizeof(*static_cast<std::ostream *>(nullptr) << *static_cast<const T *>(nullptr)) != sizeof(UniqType);
};

template <>
struct IsPrintable<void> {
    static const bool value = false;
};

};

template <typename Expression, bool ExpressionPrintable = Detail::IsPrintable<Expression>::value>
struct Messages
{
    static std::string equalityFailed(const Expression&, const Expression&)
    {
        return "failed equality check";
    }
};

template <typename Expression>
struct Messages<Expression, true>
{
    static std::string equalityFailed(const Expression& actual, const Expression& expected)
    {
        return "expected to equal " + toString(expected) + " but equals " + toString(actual);
    }
};

}

#endif // CXXSPEC_MESSAGES_HPP
