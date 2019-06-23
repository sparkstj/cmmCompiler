#include <stdio.h>
#include "symtable.h"
extern FILE* yyin;

void starter (FILE * out) {
    fprintf(out, ".data\n");
    fprintf(out, "_prompt: .asciiz \"Enter an Integer\"\n");
    fprintf(out, "_ret: .asciiz \"\\n\"\n");
    fprintf(out, ".globl main\n");
    fprintf(out, ".text\n");
    fprintf(out, "read:\n");
    fprintf(out, "li $v0, 4\nla $a0, _prompt\n");
    fprintf(out, "syscall\n");
    fprintf(out, "li $v0, 5\nsyscall\njr $ra\n");
    fprintf(out, "write:\n");
    fprintf(out, "li $v0, 1\nsyscall\nli $v0, 4\nla $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n");
    return ;
}

void OutputasmCode(FILE * out){
    asmCode root = textTop;
    while (root) {
        switch(root->type){
            case asmLabel: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                fprintf(out, "%s:\n", opstr);
                break;
            }
            case asmLi: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                fprintf(out, "li %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmMove:{
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                fprintf(out, "move %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmAddi:
            {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "addi %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmAdd: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "add %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmSub: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "sub %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmMul: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "mul %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmDiv: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "div %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmLw: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                fprintf(out, "lw %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmSw: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                fprintf(out, "sw %s, %s\n", opstr1, opstr2);
                break;
            }
            case asmJ: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                fprintf(out, "j %s\n", opstr);
                break;
            }
            case asmJal: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                fprintf(out, "jal %s\n", opstr);
                break;
            }
            case asmJr: {
                char opstr[64];
                HelperAsm(root->u.one.op1, opstr);
                fprintf(out, "jr %s\n", opstr);
                break;
            }
            case asmBeq: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "beq %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBne: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "bne %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBgt: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "bgt %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBlt: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "blt %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBge: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "bge %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmBle: {
                char opstr1[64], opstr2[64], opstr3[64];
                HelperAsm(root->u.three.op1, opstr1);
                HelperAsm(root->u.three.op2, opstr2);
                HelperAsm(root->u.three.op3, opstr3);
                fprintf(out, "ble %s, %s, %s\n", opstr1, opstr2, opstr3);
                break;
            }
            case asmLa: {
                char opstr1[64], opstr2[64];
                HelperAsm(root->u.two.op1, opstr1);
                HelperAsm(root->u.two.op2, opstr2);
                fprintf(out, "la %s, %s\n", opstr1, opstr2);
                break;
            }
        }
        root = root->next;
    }
    return ;
}
int main(int argc, char** argv) {
	if (argc <= 1) return 1;
	FILE * f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	FILE * out = fopen(argv[2], "w");
	if (out == NULL){
		perror(argv[2]);
		return 1;
	}
    starter(out);
	OutputasmCode(out);
	fclose(out);
	return 0;
}
