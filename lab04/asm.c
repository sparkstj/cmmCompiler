#include <stdio.h>
#include <string.h> // handle the def warning of memcpy
#include <stdlib.h> // handle the def warning of malloc
#include "symtable.h"


void append(asmOperand new){
    if (asmstackTop){
        stack newTop = malloc(sizeof(stackNode));
        newTop->data = malloc(sizeof(asmOperandNode));
        memcpy(newTop->data, new, sizeof(asmOperandNode));
        newTop->next = asmstackTop;
        newTop->prev = NULL;
        asmstackTop = newTop;
    }
    else {
        asmstackTop = malloc(sizeof(stackNode));
        asmstackTop->data = malloc(sizeof(asmOperandNode));
        memcpy(asmstackTop->data, new, sizeof(asmOperandNode));
        asmstackTop->prev = NULL;
        asmstackTop->next = NULL;
    }
}

asmOperand pop(){
    asmOperand result;
    if (asmstackTop) {
        result = asmstackTop->data;
        asmstackTop = asmstackTop->next;
        if (asmstackTop) asmstackTop->prev = NULL;
        return result;
    }
    else {
        printf("StackPop is empty!\n");
    }
    return NULL;
}

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


void test() {
    asmCode one = malloc(sizeof(asmCodeNode));
    one->type = asmLabel;
    one->u.one.op1 = malloc(sizeof(asmOperandNode));
    one->u.one.op1->type = str;
    one->u.one.op1->name = "main";
    one->prev = NULL;
    one->next = NULL;
    asmCode two = malloc(sizeof(asmCodeNode));
    two->type = asmLi;
    two->u.two.op1 = malloc(sizeof(asmOperandNode));
    two->u.two.op1->type = reg;
    two->u.two.op1->no = 2;
    two->u.two.op2 = malloc(sizeof(asmOperandNode));
    two->u.two.op2->type = immediate;
    two->u.two.op2->value = 4;
    two->next = NULL;
    two->prev = one;
    one->next = two;
    asmCode three = malloc(sizeof(asmCodeNode));
    three->type = asmAddi;
    three->u.three.op1 = malloc(sizeof(asmOperandNode));
    three->u.three.op1->type = reg;
    three->u.three.op1->no = 29;
    three->u.three.op2 = malloc(sizeof(asmOperandNode));
    three->u.three.op2->type = reg;
    three->u.three.op2->no = 29;
    three->u.three.op3 = malloc(sizeof(asmOperandNode));
    three->u.three.op3->type = immediate;
    three->u.three.op3->value = -4;
    three->prev = two;
    three->next = NULL;
    two->next = three;
    asmCode four = malloc(sizeof(asmCodeNode));
    four->type = asmLw;
    four->u.two.op1 = malloc(sizeof(asmOperandNode));
    four->u.two.op1->type = reg;
    four->u.two.op1->no = 31;
    four->u.two.op2 = malloc(sizeof(asmOperandNode));
    four->u.two.op2->type = addr;
    four->u.two.op2->offset = 0;
    four->u.two.op2->address = malloc(sizeof(asmOperandNode));
    four->u.two.op2->address->type = reg;
    four->u.two.op2->address->no = 29;
    four->prev = three;
    three->next = four;
    four->next = NULL;
    printasm(one);
    append(one->u.one.op1);
    append(two->u.two.op1);
    append(two->u.two.op2);
    char s1[64],s2[64],s3[64];
    asmOperand pop1 = pop();
    HelperAsm(pop1, s1);
    asmOperand pop2 = pop();
    HelperAsm(pop2, s2);
    asmOperand pop3 = pop();
    HelperAsm(pop3, s3);
    printf("%s %s %s\n", s1, s2, s3);
}
