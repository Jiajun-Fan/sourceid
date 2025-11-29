#pragma once
#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace srcid {
class SourceDB;
class SourceID {
 public:
    friend SourceDB;
    SourceID() : m_Key(0) {}
    explicit SourceID(uint64_t v) : m_Key(v) {}
    SourceID(uint64_t scope, uint64_t offset) :
            m_Scope(scope), m_Offset(offset) {}

    bool operator <(SourceID a) const { return m_Key < a.m_Key; }
    uint64_t operator *() const { return m_Key; }
    uint64_t getOffset() const { return m_Offset; }
    uint64_t getScope() const { return m_Scope; }

    const std::string& getFileName() const;
    uint64_t getLineNum() const;
    uint64_t getColumnNum() const;

    static SourceID MakeSourceID();
    static void IncrOffset(uint64_t v);
    static void AddSegment(const std::string& fname, uint64_t line, uint64_t column);
    static void NextLine();
    static void PushCtx();
    static void PopCtx();

 private:
    union {
        struct {
            uint64_t                            m_Scope:8;
            uint64_t                            m_Offset:56;
        };
        uint64_t                                m_Key;
    };

    static std::unique_ptr<SourceDB>            g_DB;
};
static_assert(sizeof(SourceID)==sizeof(uint64_t), "wrong size");

}
