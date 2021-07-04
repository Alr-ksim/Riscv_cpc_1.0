#include <cstdio>
#include <iostream>
#include <cstring>
#include <cmath>
#include "classes.hpp"

using namespace std;
using uint = unsigned;
using daptr = data *;
using drptr = data_R *;
using diptr = data_I *;
using dsptr = data_S *;
using dbptr = data_B *;
using duptr = data_U *;
using djptr = data_J *;

const uint table[6][4] = 
    {{1, 51, 0, 0}, {3, 19, 3, 103}, {1, 35, 0, 0}, {1, 99, 0, 0}, {2, 55, 23, 0}, {1, 111, 0, 0}};
commands anatype(const uint &cmd){
    uint op = Binum(cmd).slice(0, 7);
    for (int i = 0;i < 6;i++){
        int m = table[i][0];
        for (int j = 1;j <= m;j++)
            if (op == table[i][j]) return commands(i);
    }
    return commands(6);
}
daptr sol_R(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint rd = bins.slice(7, 12);
    uint fun1 = bins.slice(12, 15);
    uint rs1 = rg[bins.slice(15, 20)];
    uint rs2 = rg[bins.slice(20, 25)];
    uint fun2 = bins.slice(25, 32);
    return new data_R(R, op, fun1, fun2, rs1, rs2, rd);
}
daptr sol_I(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint rd = bins.slice(7, 12);
    uint fun = bins.slice(12, 15);
    uint rs = rg[bins.slice(15, 20)];
    uint imm = Binum(bins.slice(20, 32), 12).extended();
    return new data_I(I, op, fun, rs, rd, imm);
}
daptr sol_S(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint imm1 = bins.slice(7, 12);
    uint fun = bins.slice(12, 15);
    uint rs1 = rg[bins.slice(15, 20)];
    uint rs2 = rg[bins.slice(20, 25)];
    uint imm2 = bins.slice(25, 32);
    uint imm = imm1 + (imm2<<5);
    imm = Binum(imm, 12).extended();
    return new data_S(S, op, fun, rs1, rs2, imm);
}
daptr sol_B(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint imm1 = bins.slice(7, 8);
    uint imm2 = bins.slice(8, 12);
    uint fun = bins.slice(12, 15);
    uint rs1 = rg[bins.slice(15, 20)];
    uint rs2 = rg[bins.slice(20, 25)];
    uint imm3 = bins.slice(25, 31);
    uint imm4 = bins.slice(31, 32);
    uint imm = (imm1<<11) + (imm2<<1) + (imm3<<5) + (imm4<<12);
    imm = Binum(imm, 13).extended();
    return new data_B(B, op, fun, rs1, rs2, imm);
}
daptr sol_U(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint rd = bins.slice(7, 12);
    uint imm = bins.slice(12, 32);
    imm <<= 12;
    return new data_U(U, op, rd, imm);
}
daptr sol_J(const uint &cmd, const uint *rg){
    Binum bins(cmd);
    uint op = bins.slice(0, 7);
    uint rd = bins.slice(7, 12);
    uint imm1 = bins.slice(12, 20);
    uint imm2 = bins.slice(20, 21);
    uint imm3 = bins.slice(21, 31);
    uint imm4 = bins.slice(31, 32);
    uint imm = (imm3<<1) + (imm2<<11) + (imm1<<12) + (imm4<<20);
    imm = Binum(imm, 21).extended();
    return new data_J(J, op, rd, imm);
}

daptr sol(const uint &cmd, const uint *rg){
    commands t(anatype(cmd));
    switch (t)
    {
    case R: return sol_R(cmd, rg);
    case I: return sol_I(cmd, rg);
    case S: return sol_S(cmd, rg);
    case B: return sol_B(cmd, rg);
    case U: return sol_U(cmd, rg);
    case J: return sol_J(cmd, rg);
    default:{
        cerr << "wrong type" << endl;
        return nullptr;
        break;
    }  
    }
}

void des(daptr dat){
    switch (dat->cd)
    {
    case R: { delete drptr(dat); return; }
    case I: { delete diptr(dat); return; }
    case S: { delete dsptr(dat); return; }
    case B: { delete dbptr(dat); return; }
    case U: { delete duptr(dat); return; }
    case J: { delete djptr(dat); return; }
    default:{ cerr << "wrong type" << endl; return; }  
    }
}
