#include <stdio.h>
#include <string.h> // handle the def warning of memcpy
#include <stdlib.h> // handle the def warning of malloc
#include "symtable.h"


void HelperOperand(Operand op, char s[MAXN]){
    //    enum{ TEMP(t), VARIABLE(v), CONSTANT, ADDRESS, LABEL, FUNC} kind;
    switch (op->kind)
    {
        case TEMP:
            sprintf(s, "t%d", op->u.var_no);
            break;
        case VARIABLE:
            sprintf(s, "v%d", op->u.var_no);
            break;
        case CONSTANT_INT:
            sprintf(s, "#%d", op->u.value);
            break;
        case CONSTANT_FLOAT:
            sprintf(s, "#%f", op->u.fval);
            break;
        case ADDRESS:
            sprintf(s, "%d", op->u.var_no);
            break;
        case LABEL_OP:
            sprintf(s, "label%d", op->u.var_no);
            break;
        case FUNCTION:
            sprintf(s, "%s", op->u.id);
            break;
        default:
            break;
    }
    //printf("Helper_Operand Check, This is result in function %s\n", s);
    return ; 
}

void printInterCode(InterCodes root){
    //printf("Enter printInterCode\n");
    printf("_________________\n");
    while (root) {
        // LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
        switch (root->data->kind)
        {
            case 0:  
            {
                char s[MAXN];
                HelperOperand(root->data->u.label.x, s);
                printf("LABEL %s :\n", s);
            }
                break;
            case 1:
            {
                char s[MAXN];
                HelperOperand(root->data->u.func.f, s);
                printf("FUNCTION %s :\n", s);
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
                        printf("%s := %s\n", left, right);
                        break;
                    case 5:
                        printf("%s := &%s\n", left, right);
                        break;
                    case 6:
                        printf("*%s := %s\n", left, right);
                        break;
                    case 7:
                        printf("%s := *%s\n", left, right);
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
                        printf("%s := %s + %s\n", left, right1, right2);
                        break;
                    case 2:
                        printf("%s := %s - %s\n", left, right1, right2);
                        break;
                    case 3:
                        printf("%s := %s * %s\n", left, right1, right2);
                        break;    
                    case 4:
                        printf("%s := %s / %s\n", left, right1, right2);
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
                printf("GOTO %s\n",s);
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
                        printf("IF %s >= %s GOTO %s\n",x,y,z);
                        break;
                    case 1:
                        printf("IF %s <= %s GOTO %s\n",x,y,z);
                        break;
                    case 2:
                        printf("IF %s > %s GOTO %s\n",x,y,z);
                        break;
                    case 3:
                        printf("IF %s < %s GOTO %s\n",x,y,z);
                        break;
                    case 4:
                        printf("IF %s == %s GOTO %s\n",x,y,z);
                        break;
                    case 5:
                        printf("IF %s != %s GOTO %s\n",x,y,z);
                        break;
                }
            }
                break;
            case 6:
            {
                char s[MAXN];
                HelperOperand(root->data->u.stmt_return.x, s);
                printf("RETURN %s\n", s);
            }
                break;
            case 7:
            {
                char x[MAXN];
                HelperOperand(root->data->u.dec.x,x);
                printf("DEC %s %d\n", x, root->data->u.dec.size);
            }
                break;
            case 8:
            {
                char x[MAXN];
                HelperOperand(root->data->u.arg.x, x);
                printf("ARG %s\n", x);
            }
                break;
            case 9:
            {
                char x[MAXN], f[MAXN];
                HelperOperand(root->data->u.call.x, x);
                HelperOperand(root->data->u.call.f, f);
                printf("%s := CALL %s\n", x, f);
            }
                break;
            case 10:
            {
                char x[MAXN];
                HelperOperand(root->data->u.param.x, x);
                printf("PARAM %s\n", x);
            }
                break;
            case 11:
            {
                char x[MAXN];
                HelperOperand(root->data->u.read.x, x);
                printf("READ %s\n", x);
            }
                break;
            case 12:
            {
                char x[MAXN];
                HelperOperand(root->data->u.write.x, x);
                printf("WRITE %s\n", x);
            }
                break;
            default:
                break;
        }
        root = root->next;
    }
    return ;
}

void liltest(){
    InterCode data = malloc(sizeof(InterCodeNode));
    data->kind = FUNC;
    data->u.func.f = malloc(sizeof(OperandNode));
    data->u.func.f->kind = FUNCTION;
    char * id = "main";
    data->u.func.f->u.id = malloc(strlen(id)+1);
    strcpy(data->u.func.f->u.id, id);
    InterCodes i1 = malloc(sizeof(InterCodesNode));
    i1->data = data;

    InterCodes i2 = malloc(sizeof(InterCodesNode));
    i2->data = malloc(sizeof(InterCodeNode)); // easily get wrong
    i2->data->kind = READ;
    i2->data->u.read.x = malloc(sizeof(OperandNode));
    i2->data->u.read.x->kind = TEMP;
    i2->data->u.read.x->u.var_no = 1;
    i1->next = i2; i2->prev = i1;

    InterCodes i3 = malloc(sizeof(InterCodesNode));
    i3->data = malloc(sizeof(InterCodeNode));
    i3->data->kind = ASSIGN1;
    i3->data->Operator = ASSIGN;
    i3->data->u.assign_1.left = malloc(sizeof(OperandNode));
    i3->data->u.assign_1.left->kind = VARIABLE;
    i3->data->u.assign_1.left->u.var_no = 1;
    i3->data->u.assign_1.right = malloc(sizeof(OperandNode));
    memcpy(i3->data->u.assign_1.right, i2->data->u.read.x, sizeof(OperandNode)); // easily get wrong
    i2->next = i3; i3->prev = i2; 

    InterCodes i4 = malloc(sizeof(InterCodesNode));
    i4->data = malloc(sizeof(InterCodeNode));
    i4->data->kind = LABEL;
    i4->data->u.label.x = malloc(sizeof(OperandNode));
    i4->data->u.label.x->kind = LABEL_OP;
    i4->data->u.label.x->u.var_no = 1;
    i4->prev = i3; i3->next = i4;

    InterCodes i5 = malloc(sizeof(InterCodesNode));
    i5->data = malloc(sizeof(InterCodeNode));
    i5->data->kind = ASSIGN1;
    i5->data->Operator = ASSIGN;
    i5->data->u.assign_1.left = malloc(sizeof(OperandNode));
    i5->data->u.assign_1.left->kind = VARIABLE;
    i5->data->u.assign_1.left->u.var_no = 1;
    i5->data->u.assign_1.right = malloc(sizeof(OperandNode));
    i5->data->u.assign_1.right->kind = CONSTANT_INT;
    i5->data->u.assign_1.right->u.value = 2;
    i4->next = i5; i5->prev = i4; 

    i5->next = NULL;

    printInterCode(i1);
    return ;
}

/*int main(){
    liltest();
    return 0;
}
*/

Operand new_temp(){
    Operand temp = malloc(sizeof(OperandNode));
    temp->kind = TEMP;
    TempNo = TempNo + 1;
    temp->u.var_no = TempNo;
    return temp;
}

Operand new_label(){
    Operand label = malloc(sizeof(OperandNode));
    label->kind = LABEL_OP;
    LabelNo = LabelNo + 1;
    label->u.var_no = LabelNo;
    return label;
}

InterCodes make_label(Operand label){
    InterCodes labelCode = malloc(sizeof(InterCodesNode));
    labelCode->next = NULL; labelCode->prev = NULL;
    labelCode->data = malloc(sizeof(InterCodeNode));
    labelCode->data->kind = LABEL;
    labelCode->data->u.label.x = label;
    return labelCode;
}

InterCodes goto_label(Operand label){
    InterCodes gotocode = malloc(sizeof(InterCodesNode));
    gotocode->next = NULL; gotocode->prev = NULL;
    gotocode->data = malloc(sizeof(InterCodeNode));
    gotocode->data->kind = JMP_GOTO;
    gotocode->data->u.jmp_goto.x = label;
    return gotocode;
}

void addInterCodes(InterCodes *ListHead, InterCodes target){
    //需要修改ListHead的值，所以必须传递引用
    /*
    printf("ListHead\n");
    printInterCode(*ListHead);
    printf("target\n");
    printInterCode(target);
    printf("--------------- \n");
    */
    int layer = 0;
    if (*ListHead) {
        InterCodes temp = *ListHead;
        layer = layer + 1;
        while (temp->next) {
            temp = temp->next;
            layer = layer + 1;
        }
        if (target) {
            temp->next = target;
            temp->next->prev = temp;
            //temp->next->next = NULL;
        }
    }
    else {
        if (target) {
            *ListHead = target;
            //(*ListHead)->next=NULL;
        }
    }
    //printf("Add InterCode in depth %d of InterCode List.\n", layer);
    
    return ;
}

Operand lookup(char * id){
    SymTable temp = Head;
    Operand result = NULL;
    while (temp){
        if (strcmp(temp->name, id)==0) {
            if (temp->variable) {
                //printf("found\n");
                return temp->variable;
            }
            temp->variable = malloc(sizeof(OperandNode));
            temp->variable->kind = VARIABLE;
            VariaNo = VariaNo + 1;
            temp->variable->u.var_no = VariaNo;
            //printf("%svariable number %d %d\n",id, temp->variable->u.var_no,VariaNo);
            //printf("VariaNo %d %d\n", VariaNo, result->u.var_no);
            temp->variable->u.id = malloc(strlen(id)+1);
            strcpy(temp->variable->u.id, id);
            //printf("%svariable number %d %d\n",id, temp->variable->u.var_no,VariaNo);
            return temp->variable;
            break;
        }
        temp = temp->next;
    }
    
    FuncTable cache = FuncHead;
    while (cache) {
        if (strcmp(cache->name, id) == 0) {
            result = malloc(sizeof(OperandNode));
            result->kind = FUNCTION;
            result->u.id = malloc(strlen(id)+1);
            strcpy(result->u.id, id);
            break;
        }
        cache = cache->next;
    }
    return result;
}

void addArg(ArgList *ListHead, Operand target) {
    if (*ListHead) {
        ArgList temp = *ListHead;
        while (temp->next) {
            temp = temp->next;
        }
        if (target) {
            temp->next = malloc(sizeof(ArgNode));
            temp->next->next = NULL;
            temp->next->arg = target;
        }
    }
    else {
        //printf("ListHead is empty\n");
        (*ListHead) = malloc(sizeof(ArgNode));
        (*ListHead)->next = NULL;
        (*ListHead)->arg = target;
        //printf("exit addArg\n");
    }
    return ;
}

InterCodes translate_Args(TreeNode Args, ArgList *ArgList){
    printf("Into translate_Args\n");
    TreeNode child = Args->child;
    int numofChild = 0;
    while (child){
        numofChild = numofChild + 1;
        child = child->sibling;
    }
    if (numofChild == 1) {
        // Exp
        //printf("Arg = Exp\n");
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Args->child, t1);
        if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
            TempNo = TempNo - 1;
            memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
            code1 = NULL;
        }
        //printf("We have code1!\n");
        addArg(ArgList, t1);
        return code1;
    }
    else {
        // Exp COMMA Args
        //printf("Arg = Exp COMMA Args\n");
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Args->child, t1);
        if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
            TempNo = TempNo - 1;
            memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
            code1 = NULL;
        }
        addArg(ArgList, t1);
        InterCodes code2 = translate_Args(Args->child->sibling->sibling, ArgList);
        addInterCodes(&code1, code2);
        //printInterCode(code1);
        return code1;
    }
}

InterCodes translate_FunDec(TreeNode root){
    //printf("Enter Translate_FunDec\n");
    char * id = root->child->name;
    InterCodes func = malloc(sizeof(InterCodesNode));
    func->data = malloc(sizeof(InterCodesNode));
    func->data->kind = FUNC;
    func->data->u.func.f = malloc(sizeof(OperandNode));
    func->data->u.func.f->kind = FUNCTION;
    func->data->u.func.f->u.id = malloc(strlen(id)+1);
    strcpy(func->data->u.func.f->u.id, id);
    func->next = NULL;
    func->prev = NULL;
    return func;
}

InterCodes translate_Exp(TreeNode Exp, Operand place){
    TreeNode child = Exp->child;
    int numofChild = 0;
    TreeNode temp = child;
    while (temp){
        numofChild = numofChild + 1;
        temp = temp->sibling;
    }
    if (numofChild == 1) {
        Type type;
        switch(child->type) {
            case 1:
                { //id
                Operand variable = lookup(Exp->child->name);
                InterCodes code = malloc(sizeof(InterCodesNode));
                code->next = NULL; code->prev = NULL;
                code->data = malloc(sizeof(InterCodesNode));
                code->data->kind = ASSIGN1;
                code->data->Operator = ASSIGN;
                code->data->u.assign_1.left = place;
                code->data->u.assign_1.right = variable;
                return code;
                }
                break;
            case 2:
                { //int
                int value = Exp->child->type_int;
                InterCodes code = NULL;
                if (place)
                {

                code = malloc(sizeof(InterCodesNode));
                code->next = NULL; code->prev = NULL;
                code->data = malloc(sizeof(InterCodesNode));
                code->data->kind = ASSIGN1;
                code->data->Operator = ASSIGN;
                code->data->u.assign_1.left = place;
                code->data->u.assign_1.right = malloc(sizeof(OperandNode));
                code->data->u.assign_1.right->kind = CONSTANT_INT;
                code->data->u.assign_1.right->u.value = value;}
                return code;
                }
                break;
            case 3:
                { //float
                float value = Exp->child->type_float;
                InterCodes code = NULL;
                if (place)
               { code =  malloc(sizeof(InterCodesNode));
                code->next = NULL; code->prev = NULL;
                code->data = malloc(sizeof(InterCodesNode));
                code->data->kind = ASSIGN1;
                code->data->Operator = ASSIGN;
                code->data->u.assign_1.left = place;
                code->data->u.assign_1.right = malloc(sizeof(OperandNode));
                code->data->u.assign_1.right->kind = CONSTANT_FLOAT;
                code->data->u.assign_1.right->u.fval = value;}
                return code;
                }
                break;
        }
    }
    else if (numofChild == 2) {
        TreeNode temp1, temp2;
        temp1 = Exp->child; temp2 = temp1->sibling;
        if (strcmp(temp1->name, "MINUS") == 0) {
            // MINUS Exp
            Operand t1 = new_temp();
            InterCodes code1 = translate_Exp(temp2, t1);
            if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
            }
            if (place) 
            {InterCodes code2 = malloc(sizeof(InterCodesNode));
            code2->next = NULL; code2->prev = NULL;
            code2->data = malloc(sizeof(InterCodeNode));
            code2->data->kind = ASSIGN2;
            code2->data->Operator = SUB;
            code2->data->u.assign_2.left = place;
            code2->data->u.assign_2.right1 = malloc(sizeof(OperandNode));
            code2->data->u.assign_2.right1->kind = CONSTANT_INT;
            code2->data->u.assign_2.right1->u.value = 0;
            code2->data->u.assign_2.right2 = t1;
            addInterCodes(&code1, code2);}
            return code1;
        } else {
            // NOT Exp
            Operand label1 = new_label();
            Operand label2 = new_label();
            InterCodes code0 = malloc(sizeof(InterCodesNode));
            code0->next = NULL; code0->prev = NULL;
            code0->data = malloc(sizeof(InterCodeNode)); // [place := 0]
            code0->data->kind = ASSIGN1;
            code0->data->Operator = ASSIGN;
            code0->data->u.assign_1.left = place;
            code0->data->u.assign_1.right = malloc(sizeof(OperandNode));
            code0->data->u.assign_1.right->kind = CONSTANT_INT;
            code0->data->u.assign_1.right->u.value = 0;
            InterCodes code1 = translate_Cond(Exp, label1, label2);
            addInterCodes(&code0, code1); // if place = NULL;
            InterCodes code2 = make_label(label1);
            addInterCodes(&code0, code2);
            InterCodes code3 = malloc(sizeof(InterCodesNode));
            code3->next = NULL; code3->prev = NULL;
            code3->data = malloc(sizeof(InterCodeNode)); // [place := 1]
            code3->data->kind = ASSIGN1;
            code3->data->Operator = ASSIGN;
            code3->data->u.assign_1.left = place;
            code3->data->u.assign_1.right = malloc(sizeof(OperandNode));
            code3->data->u.assign_1.right->kind = CONSTANT_INT;
            code3->data->u.assign_1.right->u.value = 1;
            addInterCodes(&code0, code3);
            InterCodes code4 = make_label(label2);
            addInterCodes(&code0, code4);
            return code0;
        }
    }
    else if (numofChild == 3) {
        TreeNode temp1, temp2, temp3;
        temp1 = Exp->child; temp2 = temp1->sibling; temp3 = temp2->sibling;
        if ((strcmp(temp1->name, "Exp") == 0) && (strcmp(temp3->name, "Exp") == 0)) {
            // Exp Operator Exp
            if (strcmp(temp2->name,"ASSIGNOP")==0) {
                Operand variable = lookup(temp1->child->name);
                //printf("%s\n",temp1->child->name);
                //printf("variable addr %d\n",variable);
                Operand t1 = new_temp();
                InterCodes code1 = translate_Exp(temp3, t1);
                if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
                }
                InterCodes code2 = malloc(sizeof(InterCodesNode));
                code2->next = NULL; code2->prev = NULL;
                code2->data = malloc(sizeof(InterCodeNode));
                code2->data->kind = ASSIGN1;
                code2->data->Operator = ASSIGN;
                code2->data->u.assign_1.left = variable;
                code2->data->u.assign_1.right = t1;
                addInterCodes(&code1, code2);
                if (place) 
                {InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode));
                code3->data->kind = ASSIGN1;
                code3->data->Operator = ASSIGN;
                code3->data->u.assign_1.left = place;
                code3->data->u.assign_1.right = variable;
                addInterCodes(&code1, code3);}
                //printf("hello\n");
                return code1;
            }          
            else if ((strcmp(temp2->name,"AND")==0)||(strcmp(temp2->name,"OR")==0)||(strcmp(temp2->name, "RELOP")==0 )) {
                Operand label1 = new_label();
                Operand label2 = new_label();
                InterCodes code0 = malloc(sizeof(InterCodesNode));
                code0->next = NULL; code0->prev = NULL;
                code0->data = malloc(sizeof(InterCodeNode)); // [place := t1 + t2]
                code0->data->kind = ASSIGN1;
                code0->data->Operator = ASSIGN;
                code0->data->u.assign_1.left = place;
                code0->data->u.assign_1.right = malloc(sizeof(OperandNode));
                code0->data->u.assign_1.right->kind = CONSTANT_INT;
                code0->data->u.assign_1.right->u.value = 0;
                InterCodes code1 = translate_Cond(Exp, label1, label2);
                InterCodes code2 = make_label(label1);
                InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode)); // [place := t1 + t2]
                code3->data->kind = ASSIGN1;
                code3->data->Operator = ASSIGN;
                code3->data->u.assign_1.left = place;
                code3->data->u.assign_1.right = malloc(sizeof(OperandNode));
                code3->data->u.assign_1.right->kind = CONSTANT_INT;
                code3->data->u.assign_1.right->u.value = 1;
                InterCodes code4 = make_label(label2);
                addInterCodes(&code0, code1);
                addInterCodes(&code0, code2);
                addInterCodes(&code0, code3);
                addInterCodes(&code0, code4);
                return code0;
            }
            else if (strcmp(temp2->name, "PLUS") == 0) {
                Operand t1 = new_temp();
                InterCodes code1 = translate_Exp(temp1, t1);
                if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
                }
                Operand t2 = new_temp();
                InterCodes code2 = translate_Exp(temp3, t2);
                if ((code2->data->kind == ASSIGN1)&&(code2->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t2, code2->data->u.assign_1.right, sizeof(OperandNode));
                    code2 = NULL;
                }
                addInterCodes(&code1, code2);
                InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode));
                code3->data->kind = ASSIGN2;
                code3->data->Operator = ADD;
                code3->data->u.assign_2.left = place;
                code3->data->u.assign_2.right1 = t1;
                code3->data->u.assign_2.right2 = t2;                
                addInterCodes(&code1, code3);
                return code1;
            }
            else if (strcmp(temp2->name, "MINUS") == 0) {
                Operand t1 = new_temp();
                
                InterCodes code1 = translate_Exp(temp1, t1);
                if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
                }
                Operand t2 = new_temp();
                InterCodes code2 = translate_Exp(temp3, t2);
                if ((code2->data->kind == ASSIGN1)&&(code2->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t2, code2->data->u.assign_1.right, sizeof(OperandNode));
                    code2 = NULL;
                }
                InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode));
                code3->data->kind = ASSIGN2;
                code3->data->Operator = SUB;
                code3->data->u.assign_2.left = place;
                code3->data->u.assign_2.right1 = t1;
                code3->data->u.assign_2.right2 = t2;
                addInterCodes(&code1, code2);
                addInterCodes(&code1, code3);
                return code1;
            }
            else if (strcmp(temp2->name, "STAR") == 0) {
                Operand t1 = new_temp();
            
                InterCodes code1 = translate_Exp(temp1, t1);
                if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
                }
                Operand t2 = new_temp();
                //printf("we have star code1\n");
                InterCodes code2 = translate_Exp(temp3, t2);
                if ((code2->data->kind == ASSIGN1)&&(code2->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t2, code2->data->u.assign_1.right, sizeof(OperandNode));
                    code2 = NULL;
                }
                //printf("we have star code2\n");
                InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode));
                code3->data->kind = ASSIGN2;
                code3->data->Operator = MUL;
                code3->data->u.assign_2.left = place;
                code3->data->u.assign_2.right1 = t1;
                code3->data->u.assign_2.right2 = t2;
                addInterCodes(&code1, code2);
                addInterCodes(&code1, code3);
                return code1;
            }
            else if (strcmp(temp2->name, "DIV") == 0) {
                Operand t1 = new_temp();
                
                InterCodes code1 = translate_Exp(temp1, t1);
                if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
                }
                Operand t2 = new_temp();
                InterCodes code2 = translate_Exp(temp3, t2);
                if ((code2->data->kind == ASSIGN1)&&(code2->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t2, code2->data->u.assign_1.right, sizeof(OperandNode));
                    code2 = NULL;
                }
                InterCodes code3 = malloc(sizeof(InterCodesNode));
                code3->next = NULL; code3->prev = NULL;
                code3->data = malloc(sizeof(InterCodeNode));
                code3->data->kind = ASSIGN2;
                code3->data->Operator = DIV;
                code3->data->u.assign_2.left = place;
                code3->data->u.assign_2.right1 = t1;
                code3->data->u.assign_2.right2 = t2;
                addInterCodes(&code1, code2);
                addInterCodes(&code1, code3);
                return code1;
            }
        }
        else if (temp1->type == 4) { //LP Exp RP
            return translate_Exp(temp2, place);
        }
        else if (temp1->type == 1) {// ID LP RP
            Operand function = lookup(temp1->name);
            InterCodes code = malloc(sizeof(InterCodesNode));
            code->next = NULL; code->prev = NULL;
            code->data = malloc(sizeof(InterCodeNode));
            if (strcmp(function->u.id, "read")==0){
                code->data->kind = READ;
                code->data->u.read.x = place; 
                return code;
            }
            code->data->kind = CALL;
            code->data->u.call.f = function;
            code->data->u.call.x = place;
            return code; 
        }
    }
    else if (numofChild == 4) {
        TreeNode temp1, temp2, temp3, temp4;
        temp1 = Exp->child; temp2 = temp1->sibling; temp3 = temp2->sibling; temp4 = temp3->sibling;
        if (temp1->type == 1) {// ID LP Args RP
            Operand function = lookup(temp1->name);
            //printf("function name %s\n", function->u.id);
            ArgList arglist = NULL;
            InterCodes code1 = translate_Args(temp3, &arglist);
            InterCodes code2 = NULL;
            if (strcmp(function->u.id, "write")==0) {
                //printf("in write\n");
                code2 = malloc(sizeof(InterCodesNode));
                code2->prev = NULL; code2->next = NULL;
                code2->data = malloc(sizeof(InterCodeNode));
                code2->data->kind = WRITE;
                code2->data->u.write.x = arglist->arg;
                addInterCodes(&code1, code2);
                //printInterCode(code1);
                return code1;
            }
            InterCodes code3 = NULL;
            ArgList temp = arglist;
            while (temp) {
                //printf("looping over arglist\n");
                InterCodes code = malloc(sizeof(InterCodesNode));
                code->next = NULL; code->prev = NULL;
                code->data = malloc(sizeof(InterCodesNode));
                code->data->kind = ARG;
                code->data->u.arg.x = temp->arg;
                addInterCodes(&code3, code);
                temp = temp->next;
            }
            addInterCodes(&code1, code3);
            InterCodes code4 = malloc(sizeof(InterCodesNode));
            code4->next = NULL; code4->prev = NULL;
            code4->data = malloc(sizeof(InterCodesNode));
            code4->data->kind = CALL;
            code4->data->u.call.f = function;
            code4->data->u.call.x = place;
            return code1;
        }
    }
    return NULL;
}

InterCodes translate_Cond(TreeNode Exp, Operand label_true, Operand label_false) {
    // 
    int numOfChildren = 0;
    TreeNode temp = Exp->child;
    while (temp) {
        numOfChildren = numOfChildren + 1;
        temp = temp->sibling;
    }
    if (numOfChildren == 2){
        // NOT EXP
        TreeNode Exp1 = Exp->child->sibling;
        return translate_Cond(Exp1, label_true, label_false);
    }
    else if (numOfChildren == 3){
        TreeNode Operator = Exp->child->sibling;
        if (strcmp(Operator->name, "AND")==0) {
            // Exp1 AND Exp2
            Operand label1 = new_label();
            InterCodes code1 = translate_Cond(Exp->child, label1, label_false);
            InterCodes code2 = translate_Cond(Operator->sibling, label_true, label_false);
            InterCodes labelcode = make_label(label1);
            addInterCodes(&code1, labelcode);
            addInterCodes(&code1, code2);
            return code1;
        }
        if (strcmp(Operator->name, "OR")==0){
            // Exp1 OR Exp2
            Operand label1 = new_label();
            InterCodes code1 = translate_Cond(Exp->child, label_true, label1);
            InterCodes code2 = translate_Cond(Operator->sibling, label_true, label_false);
            InterCodes labelcode = make_label(label1);
            addInterCodes(&code1, labelcode);
            addInterCodes(&code1, code2);
            return code1;
        }
        if (strcmp(Operator->name, "RELOP")==0){
            Operand t1 = new_temp();
            
            InterCodes code1 = translate_Exp(Exp->child, t1);
            if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                TempNo = TempNo - 1;
                memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                code1 = NULL;
            }
            Operand t2 = new_temp();
            InterCodes code2 = translate_Exp(Operator->sibling, t2);
            if ((code2->data->kind == ASSIGN1)&&(code2->data->u.assign_1.right->kind == VARIABLE)) {
                TempNo = TempNo - 1;
                memcpy(t2, code2->data->u.assign_1.right, sizeof(OperandNode));
                code2 = NULL;
            }
            InterCodes code3 = malloc(sizeof(InterCodesNode));
            code3->data = malloc(sizeof(InterCodeNode));
            code3->data->kind = JMP_IF;
            code3->data->u.jmp_if.op1 = t1;
            code3->data->u.jmp_if.op2 = t2;
            code3->data->u.jmp_if.z = label_true;
            code3->data->relop = Operator->relop;
            code3->next = NULL; code3->prev = NULL;
            InterCodes gotolabel = goto_label(label_false);
            addInterCodes(&code1, code2);
            addInterCodes(&code1, code3);
            addInterCodes(&code1, gotolabel);
            return code1;
        }  
    }
    else {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(Exp, t1);
        if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
                    TempNo = TempNo - 1;
                    memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
                    code1 = NULL;
        }
        InterCodes code2 = malloc(sizeof(InterCodesNode));
        code2->data = malloc(sizeof(InterCodeNode));
        code2->data->kind = JMP_IF;
        code2->data->u.jmp_if.op1 = t1;
        code2->data->u.jmp_if.op2 = malloc(sizeof(OperandNode));
        code2->data->u.jmp_if.op2->kind = CONSTANT_INT;
        code2->data->u.jmp_if.op2->u.value = 0;
        code2->data->u.jmp_if.z = label_true;
        code2->data->relop = NEQ;
        code2->next = NULL; code2->prev = NULL;
        InterCodes gotolabel = goto_label(label_false);
        addInterCodes(&code1, code2);
        addInterCodes(&code1, gotolabel);
        return code1;
    }
    return NULL;
}


InterCodes translate_Stmt(TreeNode root) {
    // TODO: Implement Stmt translation
    printf("translate_stmt\n");
    int numOfChildren = 0;
    TreeNode temp = root->child;
    while (temp) {
        numOfChildren = numOfChildren + 1;
        temp = temp->sibling;
    }

    if (strcmp(root->child->name, "Exp")==0) return translate_Exp(root->child, NULL);
    if (strcmp(root->child->name, "CompSt")==0) return translate_CompSt(root->child);
    if (strcmp(root->child->name, "RETURN")==0) {
        Operand t1 = new_temp();
        InterCodes code1 = translate_Exp(root->child->sibling, t1);
        if ((code1->data->kind == ASSIGN1)&&(code1->data->u.assign_1.right->kind == VARIABLE)) {
            TempNo = TempNo - 1;
            memcpy(t1, code1->data->u.assign_1.right, sizeof(OperandNode));
            code1 = NULL;
        }
        //printInterCode(code1);
        //code2 = [RETURN t1]
        printf("We have code1\n");
        InterCodes code2 = malloc(sizeof(InterCodesNode));
        code2->data = malloc(sizeof(InterCodeNode));
        code2->prev = NULL; code2->next = NULL;
        code2->data->kind = STMT_RETURN;
        code2->data->u.stmt_return.x = t1; // might be bug
        printf("do we have code2?\n");
        addInterCodes(&code1, code2);
        return code1;
    }
    if (strcmp(root->child->name, "IF")==0) {
        if (numOfChildren==5) {
            Operand label1 = new_label();
            Operand label2 = new_label();
            TreeNode Exp = root->child->sibling->sibling;
            TreeNode Stmt = Exp->sibling->sibling;
            InterCodes code1 = translate_Cond(Exp, label1, label2);
            InterCodes code2 = translate_Stmt(Stmt);

            InterCodes labelcode1 = make_label(label1);
            InterCodes labelcode2 = make_label(label2);
            addInterCodes(&code1, labelcode1);
            addInterCodes(&code1, code2);
            addInterCodes(&code1, labelcode2);
            return code1;
        }
        else {
            //printf("ifelse\n");
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand label3 = new_label();
            TreeNode Exp = root->child->sibling->sibling;
            TreeNode Stmt1 = Exp->sibling->sibling;
            TreeNode Stmt2 = Stmt1->sibling->sibling;
            InterCodes code1 = translate_Cond(Exp, label1,label2);
            InterCodes code2 = translate_Stmt(Stmt1);
            //printf("hellooo\n");
            InterCodes code3 = translate_Stmt(Stmt2);
            InterCodes labelcode1 = make_label(label1);
            InterCodes labelcode2 = make_label(label2);
            InterCodes labelcode3 = make_label(label3);
            InterCodes gotolabel3 = goto_label(label3);
            addInterCodes(&code1, labelcode1);
            //printf("code1:\n");
            //printInterCode(code1); 
            //printf("code2:\n");
            //printInterCode(code2);
            addInterCodes(&code1, code2);
            
            addInterCodes(&code1, gotolabel3);        
            addInterCodes(&code1, labelcode2);         
            addInterCodes(&code1, code3);      
            //printf("code1:\n");
            //printInterCode(code1); 
            //printf("code3:\n");
            //printInterCode(code3);
            addInterCodes(&code1, labelcode3);
            //printf("code1:\n");
            //printInterCode(code1);
            //printf("____________________\n");
            return code1;
        }
    }
    if (strcmp(root->child->name, "WHILE")==0) {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        TreeNode Exp = root->child->sibling->sibling;
        TreeNode Stmt = Exp->sibling->sibling;
        InterCodes code1 = translate_Cond(Exp, label2, label3);
        InterCodes code2 = translate_Stmt(Stmt);
        InterCodes labelcode1 = make_label(label1);
        InterCodes labelcode2 = make_label(label2);
        InterCodes labelcode3 = make_label(label3);
        InterCodes gotolabel1 = goto_label(label1);
        addInterCodes(&labelcode1, code1);
        addInterCodes(&labelcode1, labelcode2);
        addInterCodes(&labelcode1, code2);
        addInterCodes(&labelcode1, gotolabel1);
        addInterCodes(&labelcode1, labelcode3);
        return labelcode1;
    }
    return NULL;
}

InterCodes translate_CompSt(TreeNode root){
    // translate all stmts under this node
    // CompSt = LC DefList StmtList RC
    InterCodes result = NULL;
    TreeNode StmtList = root->child->sibling->sibling;
    while (StmtList) {
        if (StmtList->child) {
            InterCodes stmt = translate_Stmt(StmtList->child);
            //printf("\n");
            //printInterCode(stmt);
            addInterCodes(&result, stmt);
            //printf("hellp\n");
            //printInterCode(result);
            StmtList = StmtList->child->sibling;
        }
        else break;
    }
    return result;
}

void translate(TreeNode root){
    //printf("Enter Translate\n");
    if (root) {
        TreeNode temp = root->child;
        if (strcmp(root->name, "FunDec")==0) {
            printf("translate_FuncDec\n");
            InterCodes func = translate_FunDec(root);
            addInterCodes(&CodeHead, func);
            return ; // Move to CompSt
        }
        
        if (strcmp(root->name, "CompSt")==0) {
            printf("translate_CompSt\n");
            InterCodes compst = translate_CompSt(root);
            addInterCodes(&CodeHead, compst);
            return ;
        }

        if (strcmp(root->name, "Stmt")==0) {
            printf("translate_Stmt\n");
            InterCodes stmt = translate_Stmt(root);
            addInterCodes(&CodeHead, stmt);
        }
        while (temp) {
            translate(temp);
            temp = temp->sibling;
        }
    }
    return ;
}