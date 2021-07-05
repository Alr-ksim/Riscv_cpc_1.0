#pragma GCC optimize(2)

#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include <queue>
#include "transfer.hpp"
#include "analysis.hpp"
#include "carried.hpp"
#include "loads.hpp"
#include "classes.hpp"

using namespace std;
using uint = unsigned;
using irptr = ins_R *;
cdptr cacdp;
daptr cadat;
inptr cainp;
wrptr cawrp;

const int Csize = 6;
uint reg[32];
char ram[1 << 20];
int cains;
cdptr code_cache[Csize];

inline void readle(){
    char s[64]; int cur = 0;
        while (scanf("%s", s) != EOF){
            if (s[0] == '@') { cur = transfer(s+1); continue;}
            ram[cur++] = transfer(s);
        }
}

inline uint cdp_pc(const cdptr &cdp){
    if (cdp) return cdp->pc;
    return -1;
}
inline cdptr code_catch(const uint &pc){
    for (int i = 0;i < Csize;i++){
        if (!code_cache[i]) break;
        if (code_cache[i]->pc == pc) return new comd(pc, code_cache[i]->cmd);
    }
    uint cmd = fetch(ram, pc, 4);
    if (code_cache[cains]) delete code_cache[cains], code_cache[cains] = nullptr;
    code_cache[cains++] = new comd(pc, cmd);
    if (cains == Csize) cains = 0;
    return new comd(pc, cmd);
}
bool tryif(const uint &pc, cdptr &cdp){
    cdp = code_catch(pc);
    if (!cdp->cmd || cdp->cmd == 0x0ff00513){
        if (cdp) delete cdp;
        cdp = nullptr;
        return 0;
    }
    return 1;
}
void reload(uint &pc, const uint &jpc){
    if (cacdp) delete cacdp, cacdp = nullptr;
    if (tryif(jpc, cacdp)) { pc = jpc; return; }
    else { pc = jpc-4; return; }
}

bool haztest(uint *haz1,const int &h1, uint *haz2, const int &h2){
    for (int i = 0;i < h1;i++)
        for (int j = 0;j < h2;j++){
            if (haz1[i] == haz2[j]) return 1;
        }
    return 0;
}

void halt(){
    printf("%u", reg[10] & (255u));
    for (int i = 0;i < Csize;i++)
        if (code_cache[i]) delete code_cache[i];
    return;
}

int main(){
    // freopen("sample.data", "r", stdin);
    // freopen("output.txt", "w", stdout);
    readle();
    uint haz1[2], haz2[2];
    for (int i = 0;i < 2;i++) haz1[i] = haz2[i] = 0;
    uint pc = 0, la = -1;
    cacdp = code_catch(pc);
    int h1, h2, cnt = 1;
    int i = 0;
    while (true) {
        if (!cnt) break;
        h1 = 0, h2 = 0, cnt = 0;
        if (cawrp) {
            wrstation(cawrp, reg), delete cawrp, cawrp = nullptr;
        }
        if (cainp) {
            cawrp = instation(cainp, ram), des(cainp), cainp = nullptr;
            if (cawrp) ++cnt;
            if (cawrp && cawrp->rd) haz1[h1++] = cawrp->rd;
        }
        if (cadat) {
            uint jpc(cadat->pc);
            cainp = transation(cadat, jpc), des(cadat), cadat = nullptr;
            if (cainp) ++cnt;
            if (cainp && (cainp->type == 1) && irptr(cainp)->rd) haz1[h1++] = irptr(cainp)->rd;
            if (cainp && (cainp->type == 3) && wrptr(cainp)->rd) haz1[h1++] = wrptr(cainp)->rd;
            if (jpc + 4 != cdp_pc(cacdp)) reload(pc, jpc);
        } 
        if (cacdp) {
            if (h1) {
                hazload(cacdp->cmd, haz2, h2);
                if (haztest(haz1, h1, haz2, h2)) continue;
            }
            cadat = sol(cacdp, reg), delete cacdp, cacdp = nullptr;
            if (cadat) ++cnt;
        }
        if (tryif(pc+4, cacdp)) { pc += 4; ++cnt; }
    }
    halt();
    return 0;
}