#include <stdio.h>
#include "symtable.h"
extern FILE* yyin;

void OutputInterCode (FILE * out) {
	InterCodes root = CodeHead;
	//printf("hello\n");
	while (root) {
		//printf("in root\n");
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        switch (root->data->kind)
        {
            case 0:  
            {
                char s[MAXN];
                HelperOperand(root->data->u.label.x, s);
                fprintf(out, "LABEL %s :\n", s);
            }
                break;
            case 1:
            {
                char s[MAXN];
                HelperOperand(root->data->u.func.f, s);
                fprintf(out, "FUNCTION %s :\n", s);
            }
                break;
            case 2:
            {
                char left[MAXN];
                HelperOperand(root->data->u.assign_1.left, left);
                char right[MAXN];
                HelperOperand(root->data->u.assign_1.right, right);
                
                switch (root->data->Operator) {
                    case 0:
                        fprintf(out, "%s := %s\n", left, right);
                        break;
                    case 5:
                        fprintf(out, "%s := &%s\n", left, right);
                        break;
                    case 6:
                        fprintf(out, "*%s := %s\n", left, right);
                        break;
                    case 7:
                        fprintf(out, "%s := *%s\n", left, right);
                        break;
                    default:
                        break;
                }
            }
                break;
            case 3:
            {
                char left[MAXN], right1[MAXN], right2[MAXN];
                HelperOperand(root->data->u.assign_2.left, left);
                HelperOperand(root->data->u.assign_2.right1, right1);
                HelperOperand(root->data->u.assign_2.right2, right2);
                switch (root->data->Operator)
                {
                    case 1:
                        fprintf(out, "%s := %s + %s\n", left, right1, right2);
                        break;
                    case 2:
                        fprintf(out, "%s := %s - %s\n", left, right1, right2);
                        break;
                    case 3:
                        fprintf(out, "%s := %s * %s\n", left, right1, right2);
                        break;    
                    case 4:
                        fprintf(out, "%s := %s / %s\n", left, right1, right2);
                        break;
                    default:
                        break;
                }
            }
                break;
            case 4:
            {
                char s[MAXN];
                HelperOperand(root->data->u.jmp_goto.x, s);
                fprintf(out, "GOTO %s\n",s);
            }
                break;
            case 5:
            {
                char x[MAXN], y[MAXN], z[MAXN];
                HelperOperand(root->data->u.jmp_if.op1, x);
                HelperOperand(root->data->u.jmp_if.op2, y);
                HelperOperand(root->data->u.jmp_if.z, z);
                switch(root->data->relop){
                    case 0:
                        fprintf(out, "IF %s >= %s GOTO %s\n",x,y,z);
                        break;
                    case 1:
                        fprintf(out, "IF %s <= %s GOTO %s\n",x,y,z);
                        break;
                    case 2:
                        fprintf(out, "IF %s > %s GOTO %s\n",x,y,z);
                        break;
                    case 3:
                        fprintf(out, "IF %s < %s GOTO %s\n",x,y,z);
                        break;
                    case 4:
                        fprintf(out, "IF %s == %s GOTO %s\n",x,y,z);
                        break;
                    case 5:
                        fprintf(out, "IF %s != %s GOTO %s\n",x,y,z);
                        break;
                }
            }
                break;
            case 6:
            {
                char s[MAXN];
                HelperOperand(root->data->u.stmt_return.x, s);
                fprintf(out, "RETURN %s\n", s);
            }
                break;
            case 7:
            {
                char x[MAXN];
                HelperOperand(root->data->u.dec.x,x);
                fprintf(out, "DEC %s %d\n", x, root->data->u.dec.size);
            }
                break;
            case 8:
            {
                char x[MAXN];
                HelperOperand(root->data->u.arg.x, x);
                fprintf(out, "ARG %s\n", x);
            }
                break;
            case 9:
            {
                char x[MAXN], f[MAXN];
                HelperOperand(root->data->u.call.x, x);
                HelperOperand(root->data->u.call.f, f);
                fprintf(out, "%s := CALL %s\n", x, f);
            }
                break;
            case 10:
            {
                char x[MAXN];
                HelperOperand(root->data->u.param.x, x);
                fprintf(out, "PARAM %s\n", x);
            }
                break;
            case 11:
            {
                char x[MAXN];
                HelperOperand(root->data->u.read.x, x);
                fprintf(out, "READ %s\n", x);
            }
                break;
            case 12:
            {
                char x[MAXN];
                HelperOperand(root->data->u.write.x, x);
                fprintf(out, "WRITE %s\n", x);
            }
                break;
            default:
                break;
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
	OutputInterCode(out);
	fclose(out);
	return 0;
}
