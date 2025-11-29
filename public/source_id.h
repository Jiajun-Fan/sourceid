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
    // If m_Key == 0, it return <Internal>
    SourceID() : m_Key(0) {}

    // Internal use only, use MakeSourceID instead
    explicit SourceID(uint64_t v) : m_Key(v) {}
    SourceID(uint64_t scope, uint64_t offset) :
            m_Scope(scope), m_Offset(offset) {}

    bool operator <(SourceID a) const { return m_Key < a.m_Key; }
    const std::string& getFileName() const;
    uint64_t getLineNum() const;
    uint64_t getColumnNum() const;

    // Internal functions, for testing
    uint64_t operator *() const { return m_Key; }
    uint64_t getOffset() const { return m_Offset; }
    uint64_t getScope() const { return m_Scope; }

    // Create a SourceID
    static SourceID MakeSourceID();

    // The basic idea of SourceID is that using unique Offset to represent a 
    // source location. The offset here means the n-th charactor processed by
    // lexer, so it's always unique. And we can use this offset to determin
    // order of source locations.
    // Also we have to create a map from offset to filename, line number and
    // column number. The mapping is created only for offsets which are:
    //  1. beginning of a line
    //  2. switching to other file (include, marco...)
    // In this way, for any offset:
    //  1. Find the nearest node in the map whose offset is smaller or equal to
    //     current one.
    //  2. Using the mapping to get file name, line number and start column
    //     number. And calculate the diff of the two offsets, the use start
    //     column number to get actual column number.

    // Move offset forwards
    static void IncrOffset(uint64_t v);

    // Add a new segment, for cases like:
    // 1. New file
    // 2. New line
    // 3. Include
    // 4. Marco
    static void AddSegment(const std::string& fname, uint64_t line, uint64_t column);

    // Wrapper for new line, equals to:
    //   AddSegment(current_file, current_line+1, 1);
    static void NextLine();

    // Save the current context, for example:
    //   1. before include call PushCtx
    //   2. after include is done, call PopCtx.
    //   3. then current file, line & column are restore automatically
    static void PushCtx();
    // Restore the last context
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
