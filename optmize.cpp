#include "optmize.h"

int blockCount, blockStartIndex[BLOCK_MAX];
vector<int> blockNext[BLOCK_MAX];
void divideBlocks() { // with dead code elimination

}

int newCodeCount;
QUADCODE newCodeList[CODE_MAX];
void commonSubexpressionElimination() { // with constant folding propagation

}

const int maxs = (((TABLE_SIZE - 1) >> 6) + 1) | 1;
int bsize, cur, pre = 1, lbt[1 << 16 | 1];
int getLowbit(unsigned long long mask) {
    int ret = 0;
    unsigned mask2 = mask;
    if(!mask2) {
        mask2 = mask >> 32;
        ret |= 32;
    }
    unsigned short mask3 = mask2;
    if(!mask3) {
        mask3 = mask2 >> 16;
        ret |= 16;
    }
    assert(mask3);
    return ret | lbt[mask3];
}
struct Bitset {
    unsigned long long data[maxs];
    int lowbit(int lowest = 0) {
        int index = lowest >> 6;
        unsigned long long mask;
        if(index >= bsize)
            return bsize << 6;
        mask = data[index] >> (lowest & 63);
        if(mask)
            return lowest + getLowbit(mask);
        while((++index) < bsize) {
            mask = data[index];
            if(mask)
                return index << 6 | getLowbit(mask);
        }
        return bsize << 6;
    }
    void reset() {
        memset(data, 0, bsize * sizeof(*data));
    }
    void set(int pos) {
        data[pos >> 6] |= 1ULL << (pos & 63);
    }
    bool operator == (Bitset const &t) const {
        for(int i = 0; i < bsize; ++i)
            if(data[i] != t.data[i])
                return false;
        return true;
    }
    void operator = (Bitset const &t) const {
        memcpy(data, t.data, bsize * sizeof(*data));
    }
    void operator - (Bitset const &t) const {
        for(int i = 0; i < bsize; ++i)
            data[i] &= ~t.data[i];
    }
    void operator | (Bitset const &t) const {
        for(int i = 0; i < bsize; ++i)
            data[i] |= t.data[i];
    }
} def[BLOCK_MAX], use[BLOCK_MAX], ent[BLOCK_MAX];
int dsu[TABLE_SIZE];
int dsuFind(int u) {
    return dsu[u] < 0 ? u : (dsu[u] = dsuFind(dsu[u]));
}
bool dsuMerge(int u, int v) {
    u = dsuFind(u);
    v = dsuFind(v);
    if(u == v)
        return false;
    if(dsu[u] < dsu[v])
        swap(u, v);
    if(dsu[u] == dsu[v])
        --dsu[v];
    dsu[u] = v;
}
map<int, int> fstDef, lstDef, fstUse, lstUse;
void activeVariableAnalysis() { // just initialization
    bsize = ((symbolCount - 1) >> 6) + 1;
    for(int i = 1; i < 1 << 16; ++i)
        lbt[i] = i & 1 ? 0 : lbt[i >> 1] + 1;

    for(int i = 0; i < blockCount; ++i) {
        ent[i].reset();
        use[i].reset();
        def[i].reset();
        map<int, int>().swap(fstDef);
        map<int, int>().swap(lstDef);
        map<int, int>().swap(fstUse);
        map<int, int>().swap(lstUse);
        int lowIdx = blockStartIndex[i], uppIdx = blockStartIndex[i + 1];
        for(int j = lowIdx; j < uppIdx; ++j) {
            // TODO: active variables analysis
        }
    }
    while(true) {
        bool iterEnd = true;
        static Bitset snap;
        for(int i = blockCount - 1; i >= 0; --i) {
            snap = ent[i];
            ent[i].reset();
            for(vector<int>::iterator it = blockNext[i].begin(); it != blockNext[i].end(); ++it)
                ent[i] |= ent[*it];
            ent[i] -= def[i];
            ent[i] |= use[i];
            iterEnd &= snap == ent[i];
        }
        if(iterEnd)
            break;
    }
    memset(dsu, -1, symbolCount * sizeof(int));
    for(int i = 0; i < blockCount; ++i) {
        int first = ent[i].lowbit();
        for(int other = ent[i].lowbit(first + 1); other < symbolCount; other = ent[i].lowbit(other + 1))
            dsuMerge(first, other);
    }
}
bool hasConfliction(int index1, int index2) { // check for global/local variables (but not temporary)
    assert(index1 >= 0 && index1 <= symbolCount);
    return dsuFind(index1) == dsuFind(index2);
}
