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

using namespace std;
using uint = unsigned;
queue <uint> cmds;
queue <daptr> dats;
queue <inptr> inps;
queue <wrptr> wrps;

uint reg[32];
char ram[1 << 20];

void readle(){
    char s[64]; int cur = 0;
        while (scanf("%s", s) != EOF){
            if (s[0] == '@') { cur = transfer(s+1); continue;}
            ram[cur++] = transfer(s);
        }
}

void halt(){
    printf("%u", reg[10] & (255u));
    exit(0);
}

int main(){
    // freopen("sample.data", "r", stdin);
    // freopen("output.txt", "w", stdout);
    readle();
    uint pc = 0, la = -1;
    int cnt = 0;
    while (true) {
        if (pc == la) pc += 4;
        la = pc;
        // printf("#%d   PC: %u\n", ++cnt, pc);
        uint cmd = fetch(ram, pc, 4);
        if (cmd == 0x0ff00513) halt();
        cmds.push(cmd);
        if (!cmds.empty()){
            cmd = cmds.front(); cmds.pop();
            // Binum(cmd).tout();
            daptr dat = sol(cmd, reg);
            if (dat) dats.push(dat);
        }
        if (!dats.empty()){
            daptr dat = dats.front(); dats.pop();
            // dat->tout();
            inptr inp = transation(dat, pc);
            des(dat); 
            if (inp){
                switch (inp->type)
                {
                case 3: { wrps.push(wrptr(inp)); break; }
                default: { inps.push(inp); break; }
                }
            }
        }
        if (!inps.empty()){
            inptr inp = inps.front(); inps.pop();
            // inp->tout();
            wrptr wrp = instation(inp, ram);
            des(inp);
            if (wrp) wrps.push(wrp);
        }
        if (!wrps.empty()){
            wrptr wrp = wrps.front(); wrps.pop();
            // wrp->tout();
            wrstation(wrp, reg);
            delete wrp;
        }
        // cout << endl << endl;
    };
    return 0;
}