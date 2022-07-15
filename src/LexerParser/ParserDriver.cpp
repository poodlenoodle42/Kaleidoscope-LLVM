#include "ParserDriver.hpp"
#include "Scanner.hpp"
#include "parser.tab.hpp"
ParserDriver::ParserDriver() {}

int ParserDriver::parse(const std::string& f) {
    file = f;
    yy::Scanner scanner(*this);
    yy::Parser parser(*this,scanner);
    parser.set_debug_level(false);
    int res = parser();
    return res;
}
