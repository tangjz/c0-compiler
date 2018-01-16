#include "optimize.h"

int labelStartIndex[TABLE_SIZE];
int blockCount, blockStartIndex[BLOCK_MAX];
vector<int> blockNext[BLOCK_MAX];
bool blockReachable[BLOCK_MAX];
void dfsBlocks(int u) {
    if(u == blockCount || blockReachable[u])
        return;
    blockReachable[u] = true;
    for(vector<int>::iterator it = blockNext[u].begin(); it != blockNext[u].end(); ++it)
        dfsBlocks(*it);
}
void divideBlocks() { // with dead code elimination
    for(int i = 0, j = 0; i < codeCount; i = j) { // compress labels
        ++j;
        if(strcmp(codeList[i].op, "label") == 0) {
            labelStartIndex[findLabel(codeList[i].lft)] = i;
            for( ; j < codeCount && strcmp(codeList[j].op, "label") == 0; ++j)
                labelStartIndex[findLabel(codeList[j].lft)] = i;
        }
    }
    // find entrances
    set<int> entIdx;
    entIdx.insert(0); // start
    entIdx.insert(codeCount); // end
    for(int i = 0; i < codeCount; ++i) {
        QUADCODE &cur = codeList[i];
        if(strcmp(cur.op, "label") == 0) {
            if(labelStartIndex[findLabel(cur.lft)] == i)
                entIdx.insert(i);
        } else if(strcmp(cur.op, "pushPara") == 0 || strcmp(cur.op, "[]=") == 0
            || strncmp(cur.op, "j", 1) == 0 || strncmp(cur.op, "b", 1) == 0
            || strcmp(cur.op, "syscall") == 0 || strcmp(cur.op, "call") == 0 || strcmp(cur.op, "return") == 0) {
            entIdx.insert(i + 1);
        }
    }
    blockCount = -1;
    for(set<int>::iterator it = entIdx.begin(); it != entIdx.end(); ++it)
        blockStartIndex[++blockCount] = *it;
    // build transform
    for(int i = 0, blkIdx = 0, fctIdx = -1; i < codeCount; ++i) {
        QUADCODE &cur = codeList[i];
        if(blkIdx < blockCount && i == blockStartIndex[blkIdx + 1])
            ++blkIdx;
        if(strcmp(cur.op, "pushPara") == 0 || strcmp(cur.op, "[]=") == 0 || strcmp(cur.op, "syscall") == 0) {
            blockNext[blkIdx].push_back(blkIdx + 1);
        } else if(strcmp(cur.op, "j") == 0) {
            int dstIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findLabel(cur.dst)]) - blockStartIndex - 1;
            blockNext[blkIdx].push_back(dstIdx);
        } else if(strncmp(cur.op, "b", 1) == 0) {
            int dstIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findLabel(cur.dst)]) - blockStartIndex - 1;
            blockNext[blkIdx].push_back(dstIdx);
            assert(i + 1 == blockStartIndex[blkIdx + 1]);
            blockNext[blkIdx].push_back(blkIdx + 1);
        } else if(strcmp(cur.op, "call") == 0) {
            int calleeIndex = findSymbol(cur.lft);
            int staIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findFunctionLabel(calleeIndex, 1)]) - blockStartIndex - 1;
            int endIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findFunctionLabel(calleeIndex, 2)]) - blockStartIndex - 1;
            blockNext[blkIdx].push_back(staIdx);
            assert(i + 1 == blockStartIndex[blkIdx + 1]);
            blockNext[endIdx].push_back(blkIdx + 1);
        } else if(strcmp(cur.op, "return") == 0) {
            int endIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findFunctionLabel(fctIdx, 2)]) - blockStartIndex - 1;
            blockNext[blkIdx].push_back(endIdx);
        } else if(strcmp(cur.op, "function") == 0) {
            fctIdx = findSymbol(cur.lft);
            if(strcmp(cur.rht, "void") == 0) { // default return branch
                int staIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findFunctionLabel(fctIdx, 1)]) - blockStartIndex - 1;
                int endIdx = upper_bound(blockStartIndex, blockStartIndex + blockCount, labelStartIndex[findFunctionLabel(fctIdx, 2)]) - blockStartIndex - 1;
                assert(i - 1 == blockStartIndex[staIdx]);
                blockNext[staIdx].push_back(endIdx);
            }
        }
    }
    // refine unreachable blocks
    dfsBlocks(0);
#ifdef OPTIMIZE_DEBUG
    fprintf(ferr, "blocks: %d\n", blockCount);
    for(int i = 0; i < blockCount; ++i) {
        fprintf(ferr, "#%d: [%d, %d) %s:", i, blockStartIndex[i], blockStartIndex[i + 1], blockReachable[i] ? "Reachable" : "Unreachable");
        for(vector<int>::iterator it = blockNext[i].begin(); it != blockNext[i].end(); ++it)
            fprintf(ferr, " #%d", *it);
        fprintf(ferr, "\n");
    }
#endif
}

int newCodeCount;
QUADCODE newCodeList[CODE_MAX];

static bool isLetter(char ch) {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void DAG(int startIndex, int length) { // refine codeList[startIndex : startIndex + length - 1]
    int lowIndex = codeTemp[startIndex + length - 1];
    memcpy(newCodeList + newCodeCount, codeList + startIndex, length * sizeof(QUADCODE));
    map<string, int> lastIndex, curIndex;
    for(int i = 0; i < length; ++i) {
        QUADCODE &cur = newCodeList[newCodeCount + i];
        if(strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0
           || strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0
           || strcmp(cur.op, "=") == 0 || strcmp(cur.op, "=[]") == 0) {
               lastIndex[cur.dst] = i;
        }
    }
    for(int i = 0; i < length; ++i) {
        QUADCODE &cur = newCodeList[newCodeCount + i];
        if(strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0
           || strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0
           || strcmp(cur.op, "=[]") == 0) {
#ifdef CONSTANT_FOLDING_PROPAGAION
            if(!isLetter(cur.lft[0]) && !isLetter(cur.rht[0])) {
                int lft, rht, res = -1;
                sscanf(cur.lft, "%d", &lft);
                sscanf(cur.rht, "%d", &rht);
                switch(cur.op[0]) {
                    case 'a': {
                        res = lft + rht;
                        break;
                    }
                    case 's': {
                        res = lft - rht;
                        break;
                    }
                    case 'm': {
                        res = lft * rht;
                        break;
                    }
                    case 'd': {
                        if(rht)
                            res = lft / rht;
                        break;
                    }
                    default: assert(false);
                }
                strcpy(cur.op, "=");
                sprintf(cur.lft, "%d", res);
                strcpy(cur.rht, "");
            }
#endif
            if(strcmp(cur.dst, "=") != 0)
                for(int j = i - 1; j >= 0; --j) {
                    QUADCODE &pre = newCodeList[newCodeCount + j];
                    if(strcmp(pre.dst, "") == 0 || curIndex[pre.dst] != j
                        || strcmp(cur.op, pre.op) != 0 || strcmp(cur.lft, pre.lft) != 0 || strcmp(cur.rht, pre.rht) != 0)
                        continue;
                    strcpy(cur.op, "=");
                    strcpy(cur.lft, pre.dst);
                    strcpy(cur.rht, "");
                    break;
                }
        }
        if(strcmp(cur.op, "=") == 0) {
            for(int j = i + 1; j < length; ++j) {
                QUADCODE &nxt = newCodeList[newCodeCount + j];
                if(strcmp(cur.dst, nxt.lft) == 0)
                    strcpy(nxt.lft, cur.lft);
                if(strcmp(cur.dst, nxt.rht) == 0)
                    strcpy(nxt.rht, cur.lft);
                if(strcmp(cur.dst, nxt.dst) == 0)
                    break;
            }
            if(lastIndex[cur.dst] != i || getTemporaryIndex(cur.dst) > lowIndex) {
                strcpy(cur.op, "");
                strcpy(cur.lft, "");
                strcpy(cur.rht, "");
                strcpy(cur.dst, "");
            }
        }
        if(strcmp(cur.dst, "") != 0)
            curIndex[cur.dst] = i;
    }
    for(int i = 0, startIndex = newCodeCount; i < length; ++i) {
        QUADCODE &cur = newCodeList[startIndex + i];
        if(strcmp(cur.op, "") != 0)
            newCodeList[newCodeCount++] = newCodeList[startIndex + i];
    }
}
void commonSubexpressionElimination() { // with constant folding propagation
    newCodeCount = 0;
    for(int i = 0; i < blockCount; ++i)
        DAG(blockStartIndex[i], blockStartIndex[i + 1] - blockStartIndex[i]);
    codeCount = newCodeCount;
    memcpy(codeList, newCodeList, newCodeCount * sizeof(QUADCODE));
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
    void operator = (Bitset const &t) {
        memcpy(data, t.data, bsize * sizeof(*data));
    }
    void operator -= (Bitset const &t) {
        for(int i = 0; i < bsize; ++i)
            data[i] &= ~t.data[i];
    }
    void operator |= (Bitset const &t) {
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
    return true;
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
