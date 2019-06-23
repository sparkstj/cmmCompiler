#include <stdio.h>
#include <string.h> // handle the def warning of memcpy
#include <stdlib.h> // handle the def warning of malloc
#include "symtable.h"

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
            else if (op->no == 30) sprintf(result, "$fp");
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
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                printf("div %s, %s, %s\n", opstr1, opstr2, opstr3);
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
        printf("%d: %d; ", i, regs[i].var);
    printf("\n");
    for (int i = 0; i < VariaNo+TempNo; i++){
        printf("Kind %d, NO.%d, inreg %d, reg_no %d, instack %d, stackoff %d ;",VarList[i].var->kind, VarList[i].var->u.var_no, VarList[i].inreg, VarList[i].regNo, VarList[i].instack, VarList[i].offset);
    }
    printf("\n-----------------------**\n");
    return ;
}
void spill(Operand x, int regNum){
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
    //printf("offset in spill %d\n", VarList[x->kind*TempNo+x->u.var_no-1].offset);
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
    printRegsVars();
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
        if (used == 0) freeReg(regNum);
        else {
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
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmLabel;
                code1->u.one.op1 = malloc(sizeof(asmCodeNode));
                code1->u.one.op1->type = str;
                code1->u.one.op1->name = root->data->u.func.f->u.id;
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->type = asmMove;
                code2->next = code2->prev = NULL;
                code2->u.two.op1 = malloc(sizeof(asmCodeNode));
                code2->u.two.op1->type = reg;
                code2->u.two.op1->no = 30;
                code2->u.two.op2 = malloc(sizeof(asmCodeNode));
                code2->u.two.op2->type = reg;
                code2->u.two.op2->no = 29;
                addAsmCode(code2);

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
                        code1->next = code1->prev = NULL;}
                        break;    
                    case DIV_OP:
                        {asmCode code1 = malloc(sizeof(asmCodeNode));
                        code1->type = asmDiv;
                        code1->u.three.op1 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op1->type = reg;
                        code1->u.three.op1->no = getReg(root->data->u.assign_2.left,1,root);
                        code1->u.three.op2 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op2->type = reg;
                        code1->u.three.op2->no = getReg(root->data->u.assign_2.right1,0,root);
                        code1->u.three.op3 = malloc(sizeof(asmOperandNode));
                        code1->u.three.op3->type = reg;
                        code1->u.three.op3->no = getReg(root->data->u.assign_2.right2,0,root);
                        code1->next = code1->prev = NULL;}
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
                break;
            case CALL:
            {
                if (1) {
                    for (int i = 8; i < 26; i++){
                        if (regs[i].var != NULL) {
                            spill(regs[i].var, i);
                            regs[i].var = NULL;
                            regs[i].next = -1;
                        }
                    }
                }
                asmCode code2 = malloc(sizeof(asmCodeNode));
                code2->type = asmJal;
                code2->u.one.op1 = malloc(sizeof(asmOperandNode));
                code2->u.one.op1->type = str;
                code2->u.one.op1->name = root->data->u.call.f->u.id;
                code2->next = code2->prev = NULL;
                addAsmCode(code2);
                asmCode code1 = malloc(sizeof(asmCodeNode));
                code1->type = asmMove;
                code1->u.two.op1 = malloc(sizeof(asmOperandNode));
                code1->u.two.op1->type = reg; 
                code1->u.two.op1->no = getReg(root->data->u.call.x,1,root);
                code1->u.two.op2 = malloc(sizeof(asmOperandNode));
                code1->u.two.op2->type = reg;
                code1->u.two.op2->no = 2;
                code1->next = code1->prev = NULL;
                addAsmCode(code1);
            }
                break;
            case PARAM:
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
    printasm(textTop);
    for (int i = 0; i < 64; i++)
        printf("%d ",basicBlock[i]);
}
