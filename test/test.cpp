#include <sstream>
#include <string>
#include "source_id.h"
using srcid::SourceID;
extern FILE* yyin;
extern int yylex (void);
std::vector<SourceID> g_IDs;
std::vector<char> g_Chars = {' ',};
int main() {
    std::string fname(BASE_DIR "/test/f1");
    SourceID::AddSegment(fname, 1, 1);
    yyin = fopen(fname.c_str(), "r");
    assert(yyin);
    while (yylex() != 0) {}
    fclose(yyin);
    g_Chars.emplace_back('\0');

    for (auto id : g_IDs) {
        std::stringstream ss;
        char c = g_Chars[id.getOffset()];
        if (c == '\n') {
            ss << "\\n";
        } else if (c == '\0') {
            ss << "EOF";
        } else {
            ss << c;
        }
        printf("%5lu(%2lu|%2lu) %-3s: %20s:%lu, col %lu\n",
            *id, id.getOffset(), id.getScope(), ss.str().c_str(),
            id.getFileName().c_str(),
            id.getLineNum(), id.getColumnNum());
    }

    return 0;
}
