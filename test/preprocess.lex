%{
#include <limits.h>
#include <cstdlib>
#include <sstream>
#include <vector>
#include "source_id.h"
using srcid::SourceID;
struct yy_buffer_state;
struct FileInfo {
    FILE*                   m_File;
    yy_buffer_state*        m_Buff;
};
std::vector<FileInfo> g_Files;
extern std::vector<SourceID> g_IDs;
extern std::vector<char> g_Chars;
%}

%%
"#include "\"[^\"]+\" {
    char fname[PATH_MAX];
    size_t len = strlen(yytext);
    for (size_t i = 0; i < len; ++i) {
        g_Chars.emplace_back(yytext[i]);
    }
    sscanf(yytext, "#include \"%[^\"]\"", fname);
    printf("include file %s\n", fname);

    std::stringstream ss;
    ss << BASE_DIR << "/test/" << fname;

    g_IDs.emplace_back(SourceID::MakeSourceID());
    g_Files.emplace_back(FileInfo{yyin, YY_CURRENT_BUFFER});

    FILE* fp = fopen(ss.str().c_str(), "r");
    if (!fp) {
        fprintf(stderr, "Fatal: Failed to open file %s\n", ss.str().c_str());
        exit(-1);
    }

    yyin = fp;
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

    SourceID::IncrOffset(len);
    SourceID::PushCtx();
    SourceID::AddSegment(ss.str(), 1, 1);
    continue;
}

\n { 
    g_Chars.emplace_back('\n');
    g_IDs.emplace_back(SourceID::MakeSourceID());
    SourceID::IncrOffset(1);
    SourceID::NextLine();
    g_IDs.emplace_back(SourceID::MakeSourceID());
}
.  {
    g_Chars.emplace_back(yytext[0]);
    g_IDs.emplace_back(SourceID::MakeSourceID());
    SourceID::IncrOffset(1);
}
%%

int yywrap() {
    if (g_Files.empty()) return 1;
    fclose(yyin);
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yyin = g_Files.back().m_File;
    yy_switch_to_buffer(g_Files.back().m_Buff);
    SourceID::PopCtx();
    g_Files.pop_back();
    return 0;
}
