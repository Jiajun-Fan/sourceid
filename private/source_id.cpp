#include "source_id.h"
namespace srcid {
class SourceDB {
 public:
    static constexpr const char* k_Internal = "<Internal>";
    SourceDB() : m_Current{0, 1},
                 m_CurFile{0},
                 m_FileVec{ k_Internal },
                 m_FileIndex{{k_Internal, 0}},
                 m_SegVec{{0, 0, 1}},
                 m_SegIndex{{0, 0}} {}

    void incrOffset(uint64_t v) {
        m_Current.m_Offset += v;
        m_Current.m_Scope = 0;
    }

    void addSegment(const std::string& fname, uint64_t line, uint64_t column) {
        addSegment(getFileID(fname), line, column);
    }

    void addSegment(size_t fid, uint64_t line, uint64_t column) {
        size_t id = m_SegVec.size();
        m_SegVec.emplace_back(SegInfo{fid, line, column});
        auto sit = m_SegIndex.find(m_Current.m_Key);
        if (sit != m_SegIndex.end()) {
            m_Current.m_Scope++;
        }
        m_SegIndex.emplace(m_Current.m_Key, id);
    }
    
    void nextLine() {
        size_t id = m_SegVec.size();
        m_SegVec.emplace_back(SegInfo{
            m_SegVec.back().m_FileID,
            m_SegVec.back().m_Line+1,
            1
        });
        m_SegIndex.emplace(m_Current.m_Key, id);
    }
    
    const std::string& getFileName(SourceID id) const {
        size_t sid = getSegIndex(id)->second;
        assert(sid < m_SegVec.size());
        size_t fid = m_SegVec[sid].m_FileID;
        assert(fid < m_FileVec.size());
        return m_FileVec[fid];
    }
    
    uint64_t getLineNum(SourceID id) const {
        size_t sid = getSegIndex(id)->second;
        assert(sid < m_SegVec.size());
        return m_SegVec[sid].m_Line;
    }
    
    uint64_t getColumnNum(SourceID id) const {
        auto it = getSegIndex(id);
        size_t sid = it->second;
        assert(sid < m_SegVec.size());
        return m_SegVec[sid].m_Column + id.m_Offset - SourceID(it->first).m_Offset;
    }

    SourceID incrLine(SourceID id) const {
        auto it = m_SegIndex.upper_bound(id.m_Key);
        if (it == m_SegIndex.end()) {
            return id;
        }
        return SourceID(it->first);
    }

    SourceID getCurrent() const { return m_Current; }

    void pushCtx() {
        auto it = getSegIndex(m_Current);
        size_t sid = it->second;
        assert(sid < m_SegVec.size());
        SegInfo info = m_SegVec[sid];
        info.m_Column += m_Current.m_Offset - SourceID(it->first).m_Offset;
        m_Contexts.emplace_back(info);
    }

    void popCtx() {
        assert(!m_Contexts.empty());
        const auto& ctx = m_Contexts.back();
        addSegment(ctx.m_FileID, ctx.m_Line, ctx.m_Column);
        m_Contexts.pop_back();
    }

 private:
    std::map<uint64_t, size_t>::const_iterator getSegIndex(SourceID id) const {
        auto it = m_SegIndex.upper_bound(id.m_Key);
        if (it != m_SegIndex.begin()) {
            --it;
        }
        return it;
    }

    size_t getFileID(const std::string& fname) {
        auto it = m_FileIndex.find(fname);
        if (it == m_FileIndex.end()) {
            size_t id = m_FileVec.size();
            m_FileVec.emplace_back(fname);
            it = m_FileIndex.emplace(fname, id).first;
        }
        return it->second;
    }

    struct SegInfo {
        size_t                              m_FileID;
        uint64_t                            m_Line;
        uint64_t                            m_Column;
    };

 private:
    SourceID                                m_Current;
    size_t                                  m_CurFile;
    std::vector<std::string>                m_FileVec;
    std::unordered_map<std::string, size_t> m_FileIndex;
    std::vector<SegInfo>                    m_SegVec;
    std::map<uint64_t, size_t>              m_SegIndex;
    std::vector<SegInfo>                    m_Contexts;
};

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
