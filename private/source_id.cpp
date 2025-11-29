#include "source_id.h"
namespace srcid {
static_assert(sizeof(SourceID)==sizeof(uint64_t), "wrong size");
std::unique_ptr<SourceDB> SourceID::g_DB(new SourceDB());

const std::string& SourceID::getFileName() const {
    return g_DB->getFileName(*this);
}

uint64_t SourceID::getLineNum() const {
    return g_DB->getLineNum(*this);
}

uint64_t SourceID::getColumnNum() const {
    return g_DB->getColumnNum(*this);
}

void SourceID::AddSegment(const std::string& fname, uint64_t line, uint64_t column) {
    g_DB->addSegment(fname, line, column);
}

void SourceID::NextLine() {
    g_DB->nextLine();
}

void SourceID::PushCtx() {
    g_DB->pushCtx();
}

void SourceID::PopCtx() {
    g_DB->popCtx();
}

void SourceID::IncrOffset(uint64_t v) {
    g_DB->incrOffset(v);
}

SourceID SourceID::MakeSourceID() {
    return g_DB->getCurrent();
}
}
