#include <stdio.h>
#include <string.h> // handle the def warning of memcpy
#include <stdlib.h> // handle the def warning of malloc
#include "symtable.h"
#define frameSize 40

int basicBlock[64];
int basicFlag = 1;
int layer = -1;
struct reg regs[32];
vardes VarList;

/* ------------ stack methods --------- */

int currentOff;
int offs[128];

/* ------------ infrastructure functions ----------- */

void HelperAsm(asmOperand op, char result[64]){
    switch (op->type)
    {
        case immediate:
            sprintf(result, "%d", op->value);
            break;
        case reg:{
            if (op->no == 0) sprintf(result, "$zero");
            else if (op->no == 1) sprintf(result, "$at");
            else if (op->no <= 3) sprintf(result, "$v%d", op->no-2);
            else if (op->no <= 7) sprintf(result, "$a%d", op->no-4);
            else if (op->no <= 15) sprintf(result, "$t%d", op->no-8);
            else if (op->no <= 23) sprintf(result, "$s%d", op->no-16);
            else if (op->no <= 25) sprintf(result, "$t%d", op->no-16);
            else if (op->no <= 27) sprintf(result, "$k%d", op->no-26);
            else if (op->no == 28) sprintf(result, "$gp");
            else if (op->no == 29) sprintf(result, "$sp");
            else if (op->no == 30) sprintf(result, "$s8");
            else if (op->no == 31) sprintf(result, "$ra");
            break;
            }
        case str:
            sprintf(result, "%s", op->name);
            break;
        case addr:{
            char s[64];
            HelperAsm(op->address, s);
            sprintf(result, "%d(%s)", op->offset, s);
            break;
        }
        default:
            break;
    }
    return ;
}

void printasm(asmCode root){
    printf("-----------------\n");
    while (root) {
        switch(root->type){
            case asmLabel: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                printf("%s:\n", opstr);
                break;
            }
            case asmLi: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                printf("li %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmMove:{
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                printf("move %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmAddi:
            {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("addi %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmAdd: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("add %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmSub: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("sub %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmMul: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("mul %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmDiv: {
                char opstr2[64], opstr3[64];
                HelperAsm(root->u.two.op1, opstr2);
                HelperAsm(root->u.two.op2, opstr3);
                printf("div %s, %s\n", opstr2, opstr3);
                break;
            }
            case asmLw: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                printf("lw %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmSw: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                printf("sw %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmJ: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                printf("j %s\n", opstr);
                break;
            }
            case asmJal: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                printf("jal %s\n", opstr);
                break;
            }
            case asmJr: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                printf("jr %s\n", opstr);
                break;
            }
            case asmBeq: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("beq %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBne: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("bne %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBgt: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("bgt %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBlt: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("blt %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBge: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("bge %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBle: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("ble %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmLa: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                printf("la %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmMflo: {
                char opstr1[64];
                HelperAsm(root->u.one.op1, opstr1);
                printf("mflo %s\n", opstr1);
                break;
            }
        }
        root = root->next;
    }
    return ;
}

void addAsmCode(asmCode newCode) { 
    // add one at a time
    if (textTop) {
        asmCode temp = textTop;
        while (temp->next) {
            temp = temp->next;
        }
        if (newCode) {
            temp->next = newCode;
            temp->next->prev = temp;
            temp->next->next = NULL; // add one at a time
        }
    }
    else {
        if (newCode) {
            textTop = newCode;
            textTop->prev = NULL; // add one at a time
            textTop->next = NULL; // add one at a time
        }
    }
}

int operandCmp(Operand x, Operand y){ // for temps and variables ONLY
    if ((x->kind == y->kind) &&(x->u.var_no == y->u.var_no))
        return 1;
    else return 0;
}

/* ------------- write simpler asmcode ----------------*/
void subsp4(){ // addi $sp, $sp, -4
    asmCode code1 = malloc(sizeof(asmCodeNode));
    code1->next = code1->prev = NULL;
    code1->type = asmAddi;
    code1->u.three.op1 = malloc(sizeof(asmOperandNode));
    code1->u.three.op1->type = reg;
    code1->u.three.op1->no = 29;
    code1->u.three.op2 = malloc(sizeof(asmOperandNode));
    code1->u.three.op2->type = reg;
    code1->u.three.op2->no = 29;
    code1->u.three.op3 = malloc(sizeof(asmOperandNode));
    code1->u.three.op3->type = immediate;
    code1->u.three.op3->value = -4;
    offs[currentOff] += 4;
    addAsmCode(code1);
    return ;
}
void swra(){ // sw $ra, 0($sp)
    asmCode code1 = malloc(sizeof(asmCodeNode));
    code1->next = code1->prev = NULL;
    code1->type = asmSw;
    code1->u.two.op1 = malloc(sizeof(asmOperandNode));
    code1->u.two.op1->type = reg;
    code1->u.two.op1->no = 31;
    code1->u.two.op2 = malloc(sizeof(asmOperandNode));
    code1->u.two.op2->type = addr;
    code1->u.two.op2->offset = 0;
    code1->u.two.op2->address = malloc(sizeof(asmOperandNode));
    code1->u.two.op2->address->type = reg;
    code1->u.two.op2->address->no = 29;
    addAsmCode(code1);
    return ;
}

void lwra(){ // lw $ra, 0($sp)
    asmCode code1 = malloc(sizeof(asmCodeNode));
    code1->next = code1->prev = NULL;
    code1->type = asmLw;
    code1->u.two.op1 = malloc(sizeof(asmOperandNode));
    code1->u.two.op1->type = reg;
    code1->u.two.op1->no = 31;
    code1->u.two.op2 = malloc(sizeof(asmOperandNode));
    code1->u.two.op2->type = addr;
    code1->u.two.op2->offset = 0;
    code1->u.two.op2->address = malloc(sizeof(asmOperandNode));
    code1->u.two.op2->address->type = reg;
    code1->u.two.op2->address->no = 29;
    addAsmCode(code1);
    return ;
}

void addsp4(){ // addi $sp, $sp, 4
    asmCode code1 = malloc(sizeof(asmCodeNode));
    code1->next = code1->prev = NULL;
    code1->type = asmAddi;
    code1->u.three.op1 = malloc(sizeof(asmOperandNode));
    code1->u.three.op1->type = reg;
    code1->u.three.op1->no = 29;
    code1->u.three.op2 = malloc(sizeof(asmOperandNode));
    code1->u.three.op2->type = reg;
    code1->u.three.op2->no = 29;
    code1->u.three.op3 = malloc(sizeof(asmOperandNode));
    code1->u.three.op3->type = immediate;
    code1->u.three.op3->value = 4;
    offs[currentOff] -= 4;
    addAsmCode(code1);
    return ;
}
/* ------------------ basic block operations -------------- */
void updateRegs(Operand x, int depth) {
    for (int i = 8; i < 26; i++)
        if ((operandCmp(x, regs[i].var) == 1)&&(regs[i].next == -1)) {
            regs[i].next = depth;
        }
    return ;
}

void detectOperand(InterCodes root, int nextBlock) { // detect operand usage in the basic block
    while (root && root->layer < nextBlock) {
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        switch (root->data->kind)
        {
            case ASSIGN1:
            {        
                updateRegs(root->data->u.assign_1.left, root->layer);
                updateRegs(root->data->u.assign_1.right, root->layer);
            }
                break;
            case ASSIGN2:
            {
                updateRegs(root->data->u.assign_2.left, root->layer);
                updateRegs(root->data->u.assign_2.right1, root->layer);
                updateRegs(root->data->u.assign_2.right2, root->layer);
            }
                break;
            case JMP_IF:
            {
                updateRegs(root->data->u.jmp_if.op1, root->layer);
                updateRegs(root->data->u.jmp_if.op2, root->layer);
                updateRegs(root->data->u.jmp_if.z, root->layer);
            }
                break;
            case STMT_RETURN:
                updateRegs(root->data->u.stmt_return.x, root->layer);
                break;
            case ARG:
                updateRegs(root->data->u.arg.x, root->layer);
                break;
            case CALL:
                updateRegs(root->data->u.call.x, root->layer);
                break;
            case PARAM:
                updateRegs(root->data->u.param.x, root->layer);
                break;
            case READ:
                updateRegs(root->data->u.read.x, root->layer);
                break;
            case WRITE:
                updateRegs(root->data->u.write.x, root->layer);
                break;
            default:
                break;
        }
        root = root->next;
    }
    return ;
}

int used = 0; 
void detectUsed(InterCodes root, int nextBlock, Operand x) { // detect is x is used in this basic block
    while (root) {
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        switch (root->data->kind)
        {
            case ASSIGN1:
            {        
                if (operandCmp(root->data->u.assign_1.left, x)==1) used = 1;
                if (operandCmp(root->data->u.assign_1.right, x)==1) used = 1;
            }
                break;
            case ASSIGN2:
            {
                if (operandCmp(root->data->u.assign_2.left, x)==1) used = 1;
                if (operandCmp(root->data->u.assign_2.right1, x)==1) used = 1;
                if (operandCmp(root->data->u.assign_2.right2, x)==1) used = 1;
            }
                break;
            case JMP_IF:
            {
                if (operandCmp(root->data->u.jmp_if.op1, x)==1) used=1;
                if (operandCmp(root->data->u.jmp_if.op2, x)==1) used=1;
                if (operandCmp(root->data->u.jmp_if.z, x)==1) used=1;
            }
                break;
            case STMT_RETURN:
                if (operandCmp(root->data->u.stmt_return.x, x)==1) used=1;
                break;
            case ARG:
                if (operandCmp(root->data->u.arg.x, x)==1) used=1;
                break;
            case CALL:
                if (operandCmp(root->data->u.call.x, x)==1) used=1;
                break;
            case PARAM:
                if (operandCmp(root->data->u.param.x, x)==1) used=1;
                break;
            case READ:
                if (operandCmp(root->data->u.read.x, x)==1) used=1;
                break;
            case WRITE:
                if (operandCmp(root->data->u.write.x, x)==1) used=1;
                break;
            default:
                break;
        }
        root = root->next;
    }
    return ;
}

void detectBlock(InterCodes root) {
    while (root) {
        layer = layer + 1;
        root->layer = layer;
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        switch (root->data->kind)
        {
            case LABEL:  
            case CALL:
            {
                basicBlock[basicFlag] = root->layer;
                basicFlag = basicFlag + 1;
            }
                break;
            case JMP_GOTO:
            case JMP_IF:
            {   
                basicBlock[basicFlag] = layer + 1;
                basicFlag = basicFlag + 1;
            }
                break;
            default:
                break;
        }
        root = root->next;
    }
    return ;
}

/* ------------------- allocate registers ------------------ */
void printRegsVars() { // 
    printf("------------------\n");
    for (int i = 0; i < 32; i++)
        printf("%d: %d; ", i, (int)regs[i].var);
    printf("\n");
    for (int i = 0; i < VariaNo+TempNo; i++){
        printf("Kind %d, NO.%d, inreg %d, reg_no %d, instack %d, stackoff %d ;",VarList[i].var->kind, VarList[i].var->u.var_no, VarList[i].inreg, VarList[i].regNo, VarList[i].instack, VarList[i].offset);
    }
    printf("\n-----------------------**\n");
    return ;
}
void spill(Operand x, int regNum){
    if (x->kind > 1) return ;
    if (VarList[x->kind*TempNo+x->u.var_no-1].instack == 1){
        VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
        VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
        asmCode code = malloc(sizeof(asmCodeNode));
        code->next = code->prev = NULL;
        code->type = asmSw;
        code->u.two.op1 = malloc(sizeof(asmOperandNode));
        code->u.two.op1->type = reg;
        code->u.two.op1->no = regNum;
        code->u.two.op2 = malloc(sizeof(asmOperandNode));
        code->u.two.op2->type = addr;
        code->u.two.op2->address = malloc(sizeof(asmOperandNode));
        code->u.two.op2->address->type = reg;
        code->u.two.op2->address->no = 30;
        code->u.two.op2->offset = VarList[x->kind*TempNo+x->u.var_no-1].offset;
        //printRegsVars();
        addAsmCode(code);
        return ;
    }
    VarList[x->kind*TempNo+x->u.var_no-1].instack = 1;
    VarList[x->kind*TempNo+x->u.var_no-1].offset = -offs[currentOff];
    printf("regNum%d, offset in spill %d\n", regNum, offs[currentOff]);
    VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
    VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
    subsp4();
    asmCode code = malloc(sizeof(asmCodeNode));
    code->next = code->prev = NULL;
    code->type = asmSw;
    code->u.two.op1 = malloc(sizeof(asmOperandNode));
    code->u.two.op1->type = reg;
    code->u.two.op1->no = regNum;
    code->u.two.op2 = malloc(sizeof(asmOperandNode));
    code->u.two.op2->type = addr;
    code->u.two.op2->address = malloc(sizeof(asmOperandNode));
    code->u.two.op2->address->type = reg;
    code->u.two.op2->address->no = 29;
    code->u.two.op2->offset = 0;
    addAsmCode(code);
    //printRegsVars();
    return ;
}

int allocate(Operand x, InterCodes now){
    for (int i = 8; i < 26; i++){
        if (regs[i].var == NULL) {
            return i;
        }
    }
    int nextBlock;
    for (int i = 0; i <= basicFlag; i++)
        if (basicBlock[i]>now->layer) nextBlock = basicBlock[i];
  
    for (int i = 8; i < 26; i++){
        regs[i].next = -1;
    }
    detectOperand(now, nextBlock);
    int farthest=0, farthest_reg = 8;
    for (int i = 8; i < 26; i++) {
        if (farthest < regs[i].next){
            farthest = regs[i].next;
            farthest_reg = i;
        }
    }
    spill(regs[farthest_reg].var, farthest_reg);
    regs[farthest_reg].var = NULL; 
    regs[farthest_reg].next = -1;
    return farthest_reg;
}

int ensure(Operand x, InterCodes now){
    //printf("ensure?\n");
    int result = -1;
    if (VarList[x->kind*TempNo+x->u.var_no-1].inreg == 1) {
        //printf("x-kind %d, x-no %d, regno %d.\n", x->kind, x->u.var_no, VarList[x->kind*TempNo+x->u.var_no-1].regNo);
        return VarList[x->kind*TempNo+x->u.var_no-1].regNo;
    }
    result = allocate(x, now);
    //printf("result %d\n", result);
    return result;
}

void freeReg(int regNum){
    regs[regNum].var = NULL;
    regs[regNum].next = -1;
    return ;
}

int getReg(Operand x, int isZ, InterCodes now){
    int regNum;
    if (x->kind == CONSTANT_INT) {
        regNum = allocate(x, now);
        asmCode code1 = malloc(sizeof(asmCodeNode));
        code1->type = asmLi;
        code1->u.two.op1 = malloc(sizeof(asmOperandNode));
        code1->u.two.op1->type = reg;
        code1->u.two.op1->no = regNum;
        code1->u.two.op2 = malloc(sizeof(asmOperandNode));
        code1->u.two.op2->type = immediate;
        code1->u.two.op2->value = x->u.value;
        code1->next = code1->prev = NULL;
        addAsmCode(code1);
        regs[regNum].var = x;
        return regNum;
    }
    if (isZ){
        regNum = ensure(x, now);
        //printf("regNum %d isZ %d\n", regNum, isZ);
        if (VarList[x->kind*TempNo+x->u.var_no-1].instack == 1){
            asmCode code = malloc(sizeof(asmCodeNode));
            code->next = code->prev = NULL;
            code->type = asmLw;
            code->u.two.op1 = malloc(sizeof(asmOperandNode));
            code->u.two.op1->type = reg;
            code->u.two.op1->no = regNum;
            code->u.two.op2 = malloc(sizeof(asmOperandNode));
            code->u.two.op2->type = addr;
            printf("offset in getreg %d\n", VarList[x->kind*TempNo+x->u.var_no-1].offset);
            code->u.two.op2->offset = VarList[x->kind*TempNo+x->u.var_no-1].offset;
            code->u.two.op2->address = malloc(sizeof(asmOperandNode));
            code->u.two.op2->address->type = reg;
            code->u.two.op2->address->no = 30;
            addAsmCode(code);
        }
        
        //printf("--%d %d %d %d--",x->kind,x->u.var_no, TempNo, x->kind*TempNo+x->u.var_no-1);
        VarList[(x->kind)*TempNo+x->u.var_no-1].inreg = 1;
        //printRegsVars();
        VarList[(x->kind)*TempNo+x->u.var_no-1].regNo = regNum;
        regs[regNum].var = x;
        //printRegsVars();
    }
    else {
        regNum = ensure(x, now);
        //printf("regNum %d isZ %d\n", regNum, isZ);
        used = 0;
        int nextBlock;
        for (int i = 0; i <= basicFlag; i++)
            if (basicBlock[i]>now->layer) nextBlock = basicBlock[i];
        detectUsed(now, nextBlock, x);
        if (VarList[x->kind*TempNo+x->u.var_no-1].instack == 1){
                asmCode code = malloc(sizeof(asmCodeNode));
                code->next = code->prev = NULL;
                code->type = asmLw;
                code->u.two.op1 = malloc(sizeof(asmOperandNode));
                code->u.two.op1->type = reg;
                code->u.two.op1->no = regNum;
                code->u.two.op2 = malloc(sizeof(asmOperandNode));
                code->u.two.op2->type = addr;
                //printf("offset in getreg %d\n", VarList[x->kind*TempNo+x->u.var_no-1].offset);
                code->u.two.op2->offset = VarList[x->kind*TempNo+x->u.var_no-1].offset;
                code->u.two.op2->address = malloc(sizeof(asmOperandNode));
                code->u.two.op2->address->type = reg;
                code->u.two.op2->address->no = 30;
                addAsmCode(code);
        }
        if (used == 0) freeReg(regNum);
        else {
            
            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
            VarList[x->kind*TempNo+x->u.var_no-1].regNo = regNum;
            regs[regNum].var = x;
        }
    }
    return regNum;
}

void init() {
    for (int i = 0; i < 128; i++)
        offs[i] = 4;
    currentOff = 0;
    for (int i = 0; i < 32; i++) {
       regs[i].var = NULL;
       regs[i].next = -1;
       regs[i].out = -1;
       regs[i].oldout = NULL;
    }
    VarList = (vardes) calloc(TempNo+VariaNo, sizeof(varNode));
    for (int i = 0; i < TempNo; i++) {
        VarList[i].var = malloc(sizeof(OperandNode));
        VarList[i].var->kind = TEMP;
        VarList[i].var->u.var_no = i+1;
    }
    for (int i = TempNo; i < TempNo+VariaNo; i++) {
        VarList[i].var = malloc(sizeof(OperandNode));
        VarList[i].var->kind = VARIABLE;
        VarList[i].var->u.var_no = i-TempNo+1;
    }
    //printRegsVars();
    return ;
}


/* ------------- TranslateAsm ------------------ */
void translateAsm(InterCodes root) {
    while (root) {
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        /*-------------- basic block registers rewrite-----------------*/
        
        /* ---------------- translate now ------------------ */
        switch (root->data->kind)
        {
            case LABEL:  
            {
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmLabel;
                code1->u.one.op1 = malloc(sizeof(asmCodeNode));
                code1->u.one.op1->type = str;
                if (root->data->u.label.x->kind == FUNCTION) code1->u.one.op1->name = root->data->u.label.x->u.id;
                else if (root->data->u.label.x->kind == LABEL_OP) {
                    code1->u.one.op1->name = malloc(64);
                    sprintf(code1->u.one.op1->name, "Label%d", root->data->u.label.x->u.var_no);
                }
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
                //printasm(textTop);
            }
                break;
            case FUNC:
            {
                for (int i = 0; i < TempNo+VariaNo; i++)
                    if (VarList[i].inreg == 1){
                        regs[VarList[i].regNo].var = NULL;
                        regs[VarList[i].regNo].next = -1;
                        VarList[i].inreg = 0;
                        VarList[i].regNo = -1;
                    }
                offs[currentOff] = 4;
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmLabel;
                code1->u.one.op1 = malloc(sizeof(asmCodeNode));
                code1->u.one.op1->type = str;
                code1->u.one.op1->name = root->data->u.func.f->u.id;
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
                if (strcmp(root->data->u.func.f->u.id, "main")==0)
                {asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->type = asmMove;
                code2->next = code2->prev = NULL;
                code2->u.two.op1 = malloc(sizeof(asmCodeNode));
                code2->u.two.op1->type = reg;
                code2->u.two.op1->no = 30;
                code2->u.two.op2 = malloc(sizeof(asmCodeNode));
                code2->u.two.op2->type = reg;
                code2->u.two.op2->no = 29;
                addAsmCode(code2);}
                else{ 
                    asmCode code3 = malloc(sizeof(asmCodeNode));
                    code3->next = code3->prev = NULL;
                    code3->type = asmAddi;
                    code3->u.three.op1 = malloc(sizeof(asmOperandNode));
                    code3->u.three.op1->type = reg;
                    code3->u.three.op1->no = 29;
                    code3->u.three.op2 = malloc(sizeof(asmOperandNode));
                    code3->u.three.op2->type = reg;
                    code3->u.three.op2->no = 29;
                    code3->u.three.op3 = malloc(sizeof(asmOperandNode));
                    code3->u.three.op3->type = immediate;
                    code3->u.three.op3->value = -frameSize;
                    addAsmCode(code3);
                    asmCode code4 = malloc(sizeof(asmCodeNode));
                    code4->next = code4->prev = NULL;
                    code4->type = asmSw;
                    code4->u.two.op1 = malloc(sizeof(asmOperandNode));
                    code4->u.two.op1->type = reg;
                    code4->u.two.op1->no = 31;
                    code4->u.two.op2 = malloc(sizeof(asmOperandNode));
                    code4->u.two.op2->type = addr;
                    code4->u.two.op2->offset = frameSize-4;
                    code4->u.two.op2->address = malloc(sizeof(asmOperandNode));
                    code4->u.two.op2->address->type = reg;
                    code4->u.two.op2->address->no = 29;
                    addAsmCode(code4);
                    asmCode code5 = malloc(sizeof(asmCodeNode));
                    code5->next = code5->prev = NULL;
                    code5->type = asmSw;
                    code5->u.two.op1 = malloc(sizeof(asmOperandNode));
                    code5->u.two.op1->type = reg;
                    code5->u.two.op1->no = 30;
                    code5->u.two.op2 = malloc(sizeof(asmOperandNode));
                    code5->u.two.op2->type = addr;
                    code5->u.two.op2->offset = frameSize-8;
                    code5->u.two.op2->address = malloc(sizeof(asmOperandNode));
                    code5->u.two.op2->address->type = reg;
                    code5->u.two.op2->address->no = 29;
                    addAsmCode(code5);
                    asmCode code6 = malloc(sizeof(asmCodeNode));
                    code6->next = code6->prev = NULL;
                    code6->type = asmAddi;
                    code6->u.three.op1 = malloc(sizeof(asmOperandNode));
                    code6->u.three.op1->type = reg;
                    code6->u.three.op1->no = 30;
                    code6->u.three.op2 = malloc(sizeof(asmOperandNode));
                    code6->u.three.op2->type = reg;
                    code6->u.three.op2->no = 29;
                    code6->u.three.op3 = malloc(sizeof(asmOperandNode));
                    code6->u.three.op3->type = immediate;
                    code6->u.three.op3->value = frameSize;
                    addAsmCode(code6);
                    offs[currentOff] += 8;
                    for (int i = 16; i < 24; i++)
                        if (1){
                            if (regs[i].var != NULL) {
                                regs[i].oldout = regs[i].var;
                                Operand x = regs[i].var;
                                VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                                VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                                //这个地方好像没有必要标记是否instack，反正已经记录了位移到时候直接恢复就好。
                                regs[i].out = -offs[currentOff];          
                            }
                            asmCode code7 = malloc(sizeof(asmCodeNode));
                            code7->next = code7->prev = NULL;
                            code7->type = asmSw;
                            code7->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code7->u.two.op1->type = reg;
                            code7->u.two.op1->no = i;
                            code7->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code7->u.two.op2->type = addr;
                            code7->u.two.op2->offset = -offs[currentOff];
                            offs[currentOff] += 4;
                            code7->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code7->u.two.op2->address->type = reg;
                            code7->u.two.op2->address->no = 30;
                            addAsmCode(code7);
                            if (regs[i].var) {
                                Operand x = regs[i].var;
                                VarList[x->kind*TempNo+x->u.var_no-1].instack = 1;
                                VarList[x->kind*TempNo+x->u.var_no-1].offset = -offs[currentOff];
                                VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                                VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                            }
                            regs[i].var = NULL;
                            regs[i].next = -1;
        
                        }
                    FuncTable temp = FuncHead;
                    while (strcmp(temp->name, root->data->u.func.f->u.id)!=0) temp = temp->next;
                    if (!temp) printf("Fuck No func found in FUnctable you twat\n");
                    FieldList ArgTemp = temp->VarList; int ArgNum = 0;
                    while (ArgTemp) {ArgNum++; ArgTemp = ArgTemp->tail;}
                    InterCodes ParamTemp = root->next;
                    //printf("argnum in func: %d\n", ArgNum);
                    if (ArgNum > 4)
                    {
                        for (int i = ArgNum-5; i >=0 ;i--)
                        {
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->next = code1->prev = NULL;
                            code1->type = asmLw;
                            code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op1->type = reg;
                            code1->u.two.op1->no = getReg(ParamTemp->data->u.param.x, 0, root);
                            code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op2->type = addr;
                            code1->u.two.op2->offset = frameSize+4+i*4;
                            code1->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code1->u.two.op2->address->type = reg;
                            code1->u.two.op2->address->no = 29;
                            ParamTemp = ParamTemp->next;
                            addAsmCode(code1);
                        }
                    
                        for (int i = 7; i >=4; i--)
                        {
                            Operand x = ParamTemp->data->u.param.x;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = i;
                            ParamTemp = ParamTemp->next;
                        }
                    }
                    else {
                       
                        while (ParamTemp->data->kind == PARAM){
                            Operand x = ParamTemp->data->u.param.x;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = ArgNum+3;
                            ArgNum--;
                            ParamTemp = ParamTemp->next;
                        }
                    }

                    //printRegsVars();
                    
                }

            }
                break;
            case PARAM:
            {
               
            }
            break;
            case ASSIGN1:
            {        
                switch (root->data->Operator) {
                    case ASSIGN:
                        if (root->data->u.assign_1.right->kind == CONSTANT_INT) 
                        {
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmLi;
                            code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op1->type = reg;
                            code1->u.two.op1->no = getReg(root->data->u.assign_1.left, 1, root);
                            code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op2->type = immediate;
                            code1->u.two.op2->value = root->data->u.assign_1.right->u.value;
                            code1->next = code1->prev = NULL;
                            //printf("This is immediate %d\n", code1->u.two.op2->value);
                            addAsmCode(code1);
                        }
                        else if ((root->data->u.assign_1.right->kind == TEMP) || (root->data->u.assign_1.right->kind == VARIABLE)) {
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmMove;
                            code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op1->type = reg;
                            code1->u.two.op1->no = getReg(root->data->u.assign_1.left, 1, root);
                            code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.two.op2->type = reg;
                            code1->u.two.op2->no = getReg(root->data->u.assign_1.right, 0, root);
                            code1->next = code1->prev = NULL;
                            addAsmCode(code1);
                        } 
                        break;
                    case INDl:{
                        asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmSw;
                        code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op1->type = reg;
                        code1->u.two.op1->no = getReg(root->data->u.assign_1.right, 0, root);
                        code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op2->type = addr;
                        code1->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code1->u.two.op2->address->type = reg;
                        code1->u.two.op2->address->no = getReg(root->data->u.assign_1.left, 1, root);
                        code1->u.two.op2->offset = 0;
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);
                    }
                        break;
                    case INDr:{
                        asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmLw;
                        code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op1->type = reg;
                        code1->u.two.op1->no = getReg(root->data->u.assign_1.left, 1, root);
                        code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op2->type = addr;
                        code1->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code1->u.two.op2->address->type = reg;
                        code1->u.two.op2->address->no = getReg(root->data->u.assign_1.right, 0, root);
                        code1->u.two.op2->offset = 0;
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);
                    } 
                        break;
                    default:
                        break;
                }
            }
                break;
            case ASSIGN2:
            {
                switch (root->data->Operator)
                {
                    case ADD:
                        {if (root->data->u.assign_2.right2->kind == CONSTANT_INT){
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmAddi;
                            code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op1->type = reg;
                            code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                            code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op2->type = reg;
                            code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                            code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op3->type = immediate;
                            code1->u.three.op3->value = root->data->u.assign_2.right2->u.value;
                            code1->next = code1->prev = NULL;
                            addAsmCode(code1);
                        }
                        else if ((root->data->u.assign_2.right2->kind == TEMP)||(root->data->u.assign_2.right2->kind == VARIABLE)) {
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmAdd;
                            code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op1->type = reg;
                            code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                            code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op2->type = reg;
                            code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                            code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op3->type = reg;
                            code1->u.three.op3->no = getReg(root->data->u.assign_2.right2,0,root);
                            code1->next = code1->prev = NULL;
                            addAsmCode(code1);
                        }}
                        break;
                    case SUB:
                        {if (root->data->u.assign_2.right2->kind == CONSTANT_INT){
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmAddi;
                            code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op1->type = reg;
                            code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                            code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op2->type = reg;
                            code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                            code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op3->type = immediate;
                            code1->u.three.op3->value = - root->data->u.assign_2.right2->u.value;
                            code1->next = code1->prev = NULL;
                            addAsmCode(code1);
                        }
                        else if ((root->data->u.assign_2.right2->kind == TEMP)||(root->data->u.assign_2.right2->kind == VARIABLE)) {
                            asmCode code1 = malloc(sizeof(asmCodeNode));
                            code1->type = asmSub;
                            code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op1->type = reg;
                            code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                            code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op2->type = reg;
                            code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                            code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                            code1->u.three.op3->type = reg;
                            code1->u.three.op3->no = getReg(root->data->u.assign_2.right2,0,root);
                            code1->next = code1->prev = NULL;
                            addAsmCode(code1);
                        }}
                        break;
                    case MUL:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmMul;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = reg;
                        code1->u.three.op3->no = getReg(root->data->u.assign_2.right2,0,root);
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;    
                    case DIV_OP:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmDiv;
                        code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op1->type = reg;
                        code1->u.two.op1->no = getReg(root->data->u.assign_2.right1,0,root);
                        code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.two.op2->type = reg;
                        code1->u.two.op2->no = getReg(root->data->u.assign_2.right2,0,root);
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        asmCode code2 = malloc(sizeof(asmCodeNode));
                        code2->type = asmMflo;
                        code2->u.one.op1 = malloc(sizeof(asmOperandNode));
                        code2->u.one.op1->type = reg;
                        code2->u.one.op1->no = getReg(root->data->u.assign_2.left,1,root);
                        code2->next = code2->prev = NULL;
                        addAsmCode(code2);
                        break;
                    default:
                        break;
                }

            }
                break;
            case JMP_GOTO:
            {   
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmJ;
                code1->u.one.op1 = malloc(sizeof(asmOperandNode));
                code1->u.one.op1->type = str;
                if (root->data->u.jmp_goto.x->kind == FUNCTION) code1->u.one.op1->name = root->data->u.jmp_goto.x->u.id;
                else if (root->data->u.jmp_goto.x->kind == LABEL_OP) {
                    code1->u.one.op1->name = malloc(64);
                    sprintf(code1->u.one.op1->name , "Label%d", root->data->u.jmp_goto.x->u.var_no);
                }
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
            }
                break;
            case JMP_IF:
            {
                switch(root->data->relop){
                    case GRQ:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBge;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                    case LEQ:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBle;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                    case GE:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBgt;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                    case LE:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBlt;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                    case EQ:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBeq;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                    case NEQ:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmBne;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.jmp_if.op1,0,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.jmp_if.op2,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = str;
                        if (root->data->u.jmp_if.z->kind == FUNCTION) code1->u.three.op3->name = root->data->u.jmp_if.z->u.id;
                        else if (root->data->u.jmp_if.z->kind == LABEL_OP) {
                            code1->u.three.op3->name = malloc(64);
                            sprintf(code1->u.three.op3->name, "Label%d", root->data->u.jmp_if.z->u.var_no);
                        }
                        code1->next = code1->prev = NULL;
                        addAsmCode(code1);}
                        break;
                }
            }
                break;
            case STMT_RETURN:
            {
                if (root->next)
                {
                    for (int i = 16; i < 24; i++){
                    if (regs[i].out != -1) {
                        asmCode code8 = malloc(sizeof(asmCodeNode));
                        code8->prev = code8->next = NULL;
                        code8->type = asmLw;
                        code8->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code8->u.two.op1->type = reg;
                        code8->u.two.op1->no = i;
                        code8->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code8->u.two.op2->type = addr;
                        code8->u.two.op2->offset = regs[i].out;
                        code8->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code8->u.two.op2->address->type = reg;
                        code8->u.two.op2->address->no = 30;
                        addAsmCode(code8);
                        Operand x = regs[i].oldout;
                        VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
                        VarList[x->kind*TempNo+x->u.var_no-1].regNo = i;
                        VarList[x->kind*TempNo+x->u.var_no-1].instack = 0;
                        VarList[x->kind*TempNo+x->u.var_no-1].offset = -1;
                        regs[i].var = regs[i].oldout;
                        regs[i].oldout = NULL;
                        regs[i].next = -1;
                        regs[i].out = -1;
                    }                    
                }
                asmCode code4 = malloc(sizeof(asmCodeNode));
                code4->next = code4->prev = NULL;
                code4->type = asmLw;
                code4->u.two.op1 = malloc(sizeof(asmOperandNode));
                code4->u.two.op1->type = reg;
                code4->u.two.op1->no = 31;
                code4->u.two.op2 = malloc(sizeof(asmOperandNode));
                code4->u.two.op2->type = addr;
                code4->u.two.op2->offset = -4;
                code4->u.two.op2->address = malloc(sizeof(asmOperandNode));
                code4->u.two.op2->address->type = reg;
                code4->u.two.op2->address->no = 30;
                addAsmCode(code4);
                asmCode code3 = malloc(sizeof(asmCodeNode));
                code3->next = code3->prev = NULL;
                code3->type = asmAddi;
                code3->u.three.op1 = malloc(sizeof(asmOperandNode));
                code3->u.three.op1->type = reg;
                code3->u.three.op1->no = 29;
                code3->u.three.op2 = malloc(sizeof(asmOperandNode));
                code3->u.three.op2->type = reg;
                code3->u.three.op2->no = 30;
                code3->u.three.op3 = malloc(sizeof(asmOperandNode));
                code3->u.three.op3->type = immediate;
                code3->u.three.op3->value = 0;
                addAsmCode(code3);
                asmCode code5 = malloc(sizeof(asmCodeNode));
                code5->next = code5->prev = NULL;
                code5->type = asmLw;
                code5->u.two.op1 = malloc(sizeof(asmOperandNode));
                code5->u.two.op1->type = reg;
                code5->u.two.op1->no = 30;
                code5->u.two.op2 = malloc(sizeof(asmOperandNode));
                code5->u.two.op2->type = addr;
                code5->u.two.op2->offset = -8;
                code5->u.two.op2->address = malloc(sizeof(asmOperandNode));
                code5->u.two.op2->address->type = reg;
                code5->u.two.op2->address->no = 29;
                addAsmCode(code5);}
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmMove;
                code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                code1->u.two.op1->type = reg;
                code1->u.two.op1->no = 2; 
                code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                code1->u.two.op2->type = reg;
                code1->u.two.op2->no = getReg(root->data->u.stmt_return.x,0,root);
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->type = asmJr;
                code2->u.one.op1 = malloc(sizeof(asmOperandNode));
                code2->u.one.op1->type = reg;
                code2->u.one.op1->no = 31;
                code2->next = code2->prev = NULL;
                addAsmCode(code2);
                    
            }
                break;
            case ARG:
            {
            }
                break;
            case CALL:
            {
                int sumOfSpace = 0;
                for (int i = 4; i < 8; i++) {
                    if (regs[i].var == NULL) break;
                    sumOfSpace+=4;
                    subsp4();
                    asmCode code6 = malloc(sizeof(asmCodeNode));
                    code6->prev = code6->next = NULL;
                    code6->type = asmSw;
                    code6->u.two.op1 = malloc(sizeof(asmOperandNode));
                    code6->u.two.op1->type = reg;
                    code6->u.two.op1->no = i;
                    code6->u.two.op2 = malloc(sizeof(asmOperandNode));
                    code6->u.two.op2->type = addr;
                    code6->u.two.op2->offset = 0;
                    code6->u.two.op2->address = malloc(sizeof(asmOperandNode));
                    code6->u.two.op2->address->type = reg;
                    code6->u.two.op2->address->no = 29;
                    addAsmCode(code6);
                    Operand x = regs[i].var;
                    VarList[x->kind*TempNo+x->u.var_no-1].instack = 1;
                    VarList[x->kind*TempNo+x->u.var_no-1].offset = 4-offs[currentOff];
                    VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                    VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                    VarList[x->kind*TempNo+x->u.var_no-1].isa = i;
                }
                //printRegsVars();
                InterCodes temp = root->prev;
                int argNum = 0;
                while (temp->data->kind == ARG) {
                    argNum ++;
                    temp = temp->prev;
                }
                int argspace = (0 < 4*(argNum-5))? 4*(argNum-5):0;
                sumOfSpace+=argspace;
                if (argspace != 0)
                {asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->next = code1->prev = NULL;
                code1->type = asmAddi;
                code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                code1->u.three.op1->type = reg;
                code1->u.three.op1->no = 29;
                code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                code1->u.three.op2->type = reg;
                code1->u.three.op2->no = 29;
                code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                code1->u.three.op3->type = immediate;
                code1->u.three.op3->value = -argspace;
                offs[currentOff] += argspace;
                addAsmCode(code1);}
                temp = temp->next;
                //printf("argNum %d\n",argNum);
                if (argNum <= 4) {
                    int regCount = 4;
                    while (temp != root){
                        asmCode code = malloc(sizeof(asmCodeNode));
                        code->next = code->prev = NULL;
                        code->type = asmMove;
                        code->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code->u.two.op1->type = reg;
                        code->u.two.op1->no = regCount;
                        regCount++;
                        code->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code->u.two.op2->type = reg;
                        code->u.two.op2->no = getReg(temp->data->u.arg.x, 0, root);
                        addAsmCode(code);
                        /*
                        regs[code->u.two.op2->no].var = NULL;
                        regs[code->u.two.op2->no].next = -1;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].inreg = 1;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].regNo = regCount;
                        */
                        temp = temp->next;
                    }
                    for (int i = 8; i < 15; i++){
                        if (regs[i].var != NULL) {
                            sumOfSpace+=4;
                            regs[i].oldout = regs[i].var;
                            Operand x = regs[i].var;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                            //这个地方好像没有必要标记是否instack，反正已经记录了位移到时候直接恢复就好。
                            subsp4();
                            asmCode code6 = malloc(sizeof(asmCodeNode));
                            code6->prev = code6->next = NULL;
                            code6->type = asmSw;
                            code6->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op1->type = reg;
                            code6->u.two.op1->no = i;
                            code6->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->type = addr;
                            code6->u.two.op2->offset = 0;
                            code6->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->address->type = reg;
                            code6->u.two.op2->address->no = 29;
                            addAsmCode(code6);
                            regs[i].out = 4-offs[currentOff];
                            regs[i].var = NULL;
                            regs[i].next = -1;
                            
                        }
                    }
                    for (int i = 24; i < 26; i++){
                        if (regs[i].var != NULL) {
                            sumOfSpace+=4;
                            regs[i].oldout = regs[i].var;
                            Operand x = regs[i].var;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                            subsp4();
                            asmCode code6 = malloc(sizeof(asmCodeNode));
                            code6->prev = code6->next = NULL;
                            code6->type = asmSw;
                            code6->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op1->type = reg;
                            code6->u.two.op1->no = i;
                            code6->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->type = addr;
                            code6->u.two.op2->offset = 0;
                            code6->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->address->type = reg;
                            code6->u.two.op2->address->no = 29;
                            addAsmCode(code6);
                            regs[i].out = 4-offs[currentOff];
                            regs[i].var = NULL;
                            regs[i].next = -1;
                            
                        }
                    }
                }
                else {
                    for (int i = 4; i < 8; i++) {
                        asmCode code = malloc(sizeof(asmCodeNode));
                        code->next = code->prev = NULL;
                        code->type = asmMove;
                        code->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code->u.two.op1->type = reg;
                        code->u.two.op1->no = i;
                        code->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code->u.two.op2->type = reg;
                        code->u.two.op2->no = getReg(temp->data->u.arg.x, 0, root);
                        addAsmCode(code);
                        /*
                        regs[code->u.two.op2->no].var = NULL;
                        regs[code->u.two.op2->no].next = -1;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].inreg = 1;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].regNo = i;
                        */
                        temp = temp->next;
                    }
                    
                    for (int i = 8; i < 15; i++){
                        if (regs[i].var != NULL) {
                            sumOfSpace+=4;
                            regs[i].oldout = regs[i].var;
                            Operand x = regs[i].var;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                            subsp4();
                            asmCode code6 = malloc(sizeof(asmCodeNode));
                            code6->prev = code6->next = NULL;
                            code6->type = asmSw;
                            code6->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op1->type = reg;
                            code6->u.two.op1->no = i;
                            code6->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->type = addr;
                            code6->u.two.op2->offset = 0;
                            code6->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->address->type = reg;
                            code6->u.two.op2->address->no = 29;
                            addAsmCode(code6);
                            regs[i].out = 4-offs[currentOff];
                            regs[i].var = NULL;
                            regs[i].next = -1;
                            
                        }
                    }
                    for (int i = 24; i < 26; i++){
                        if (regs[i].var != NULL) {
                            sumOfSpace+=4;
                            regs[i].oldout = regs[i].var;
                            Operand x = regs[i].var;
                            VarList[x->kind*TempNo+x->u.var_no-1].inreg = 0;
                            VarList[x->kind*TempNo+x->u.var_no-1].regNo = -1;
                            subsp4();
                            asmCode code6 = malloc(sizeof(asmCodeNode));
                            code6->prev = code6->next = NULL;
                            code6->type = asmSw;
                            code6->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op1->type = reg;
                            code6->u.two.op1->no = i;
                            code6->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->type = addr;
                            code6->u.two.op2->offset = 0;
                            code6->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code6->u.two.op2->address->type = reg;
                            code6->u.two.op2->address->no = 29;
                            addAsmCode(code6);
                            regs[i].out = 4-offs[currentOff];
                            regs[i].var = NULL;
                            regs[i].next = -1;
                            
                        }
                    }
                    
                    int argoffset = 0;
                    while (temp != root){
                        asmCode code3 = malloc(sizeof(asmCodeNode));
                        code3->next = code3->prev = NULL;
                        code3->type = asmSw;
                        code3->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code3->u.two.op1->type = reg;
                        code3->u.two.op1->no = getReg(temp->data->u.arg.x, 0, root);
                        code3->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code3->u.two.op2->type = addr;
                        code3->u.two.op2->offset = argoffset;
                        argoffset += 4;
                        code3->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code3->u.two.op2->address->type = reg;
                        code3->u.two.op2->address->no = 29;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].instack = 1;
                        VarList[temp->data->u.arg.x->kind*TempNo+temp->data->u.arg.x->u.var_no-1].offset = argoffset;
                        addAsmCode(code3);
                        temp = temp->next;
                    }
                }
                
                subsp4();
                swra();

                asmCode code4 = malloc(sizeof(asmCodeNode));
                code4->type = asmJal;
                code4->u.one.op1 = malloc(sizeof(asmOperandNode));
                code4->u.one.op1->type = str;
                code4->u.one.op1->name = root->data->u.call.f->u.id;
                code4->next = code4->prev = NULL;
                addAsmCode(code4);

                lwra();
                addsp4();

                for (int i = 0; i < TempNo+VariaNo; i++)
                    {
                        if (VarList[i].isa != 0){
                            asmCode code7 = malloc(sizeof(asmCodeNode));
                            code7->prev = code7->next = NULL;
                            code7->type = asmLw;
                            code7->u.two.op1 = malloc(sizeof(asmOperandNode));
                            code7->u.two.op1->type = reg;
                            code7->u.two.op1->no = VarList[i].isa;
                            code7->u.two.op2 = malloc(sizeof(asmOperandNode));
                            code7->u.two.op2->type = addr;
                            code7->u.two.op2->offset = VarList[i].offset;
                            code7->u.two.op2->address = malloc(sizeof(asmOperandNode));
                            code7->u.two.op2->address->type = reg;
                            code7->u.two.op2->address->no = 30;
                            addAsmCode(code7);
                            VarList[i].inreg = 1;
                            VarList[i].regNo = VarList[i].isa;
                            VarList[i].isa = 0;
                        }
                    }

                // didnt load back, dunno what will happen
                // load back
                for (int i = 8; i < 15; i++){
                    if (regs[i].out != -1) {
                        asmCode code8 = malloc(sizeof(asmCodeNode));
                        code8->prev = code8->next = NULL;
                        code8->type = asmLw;
                        code8->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code8->u.two.op1->type = reg;
                        code8->u.two.op1->no = i;
                        code8->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code8->u.two.op2->type = addr;
                        code8->u.two.op2->offset = regs[i].out;
                        code8->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code8->u.two.op2->address->type = reg;
                        code8->u.two.op2->address->no = 30;
                        addAsmCode(code8);
                        Operand x = regs[i].oldout;
                        VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
                        VarList[x->kind*TempNo+x->u.var_no-1].regNo = i;
                        VarList[x->kind*TempNo+x->u.var_no-1].instack = 0;
                        VarList[x->kind*TempNo+x->u.var_no-1].offset = -1;
                        regs[i].var = regs[i].oldout;
                        regs[i].oldout = NULL;
                        regs[i].next = -1;
                        regs[i].out = -1;
                    }                    
                }
                for (int i = 24; i < 26; i++){
                    if (regs[i].out != -1) {
                        asmCode code9 = malloc(sizeof(asmCodeNode));
                        code9->prev = code9->next = NULL;
                        code9->type = asmLw;
                        code9->u.two.op1 = malloc(sizeof(asmOperandNode));
                        code9->u.two.op1->type = reg;
                        code9->u.two.op1->no = i;
                        code9->u.two.op2 = malloc(sizeof(asmOperandNode));
                        code9->u.two.op2->type = addr;
                        code9->u.two.op2->offset = -regs[i].out;
                        code9->u.two.op2->address = malloc(sizeof(asmOperandNode));
                        code9->u.two.op2->address->type = reg;
                        code9->u.two.op2->address->no = 30;
                        addAsmCode(code9);
                        Operand x = regs[i].oldout;
                        VarList[x->kind*TempNo+x->u.var_no-1].inreg = 1;
                        VarList[x->kind*TempNo+x->u.var_no-1].regNo = i;
                        VarList[x->kind*TempNo+x->u.var_no-1].instack = 0;
                        VarList[x->kind*TempNo+x->u.var_no-1].offset = -1;
                        regs[i].var = regs[i].oldout;
                        regs[i].oldout = NULL;
                        regs[i].next = -1;
                        regs[i].out = -1;
                    }   
                }
                    

                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->type = asmMove;
                code2->u.two.op1 = malloc(sizeof(asmOperandNode));
                code2->u.two.op1->type = reg; 
                code2->u.two.op1->no = getReg(root->data->u.call.x,1,root);
                code2->u.two.op2 = malloc(sizeof(asmOperandNode));
                code2->u.two.op2->type = reg;
                code2->u.two.op2->no = 2;
                code2->next = code2->prev = NULL;
                addAsmCode(code2);

                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->next = code1->prev = NULL;
                code1->type = asmAddi;
                code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                code1->u.three.op1->type = reg;
                code1->u.three.op1->no = 29;
                code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                code1->u.three.op2->type = reg;
                code1->u.three.op2->no = 29;
                code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                code1->u.three.op3->type = immediate;
                code1->u.three.op3->value = sumOfSpace;
                offs[currentOff] -= sumOfSpace;
                addAsmCode(code1);
            }
                break;
            
            case READ:
            {
                subsp4();
                swra();
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->prev = code1->next = NULL;
                code1->type = asmJal;
                code1->u.one.op1 = malloc(sizeof(asmOperandNode));
                code1->u.one.op1->type = str;
                code1->u.one.op1->name = "read";
                addAsmCode(code1);
                lwra();
                addsp4();
                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->prev = code2->next = NULL;
                code2->type = asmMove;
                code2->u.two.op1 = malloc(sizeof(asmOperandNode));
                code2->u.two.op1->type = reg;
                code2->u.two.op1->no = getReg(root->data->u.read.x, 1, root);
                code2->u.two.op2 = malloc(sizeof(asmOperandNode));
                code2->u.two.op2->type = reg;
                code2->u.two.op2->no = 2;
                addAsmCode(code2);
            }
            break;
            case WRITE:
            {
                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->prev = code2->next = NULL;
                code2->type = asmMove;
                code2->u.two.op1 = malloc(sizeof(asmOperandNode));
                code2->u.two.op1->type = reg;
                code2->u.two.op1->no = 4;
                code2->u.two.op2 = malloc(sizeof(asmOperandNode));
                code2->u.two.op2->type = reg;
                code2->u.two.op2->no = getReg(root->data->u.write.x, 0, root);
                addAsmCode(code2);
                subsp4();
                swra();
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->prev = code1->next = NULL;
                code1->type = asmJal;
                code1->u.one.op1 = malloc(sizeof(asmOperandNode));
                code1->u.one.op1->type = str;
                code1->u.one.op1->name = "write";
                addAsmCode(code1);
                lwra();
                addsp4();
            }
            break;
            default:
                break;
        }
        root = root->next;
    }
    return ;
}

void ASM(InterCodes root) {
    init();
    detectBlock(root);
    translateAsm(root);
    //printasm(textTop);
    //for (int i = 0; i < 64; i++)
    //    printf("%d ",basicBlock[i]);
}
