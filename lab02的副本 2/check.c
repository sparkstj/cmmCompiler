#include <stdio.h>
#include "symtable.h"
//#define debug_2

void error(int num, int line, char * msg) {
    printf("Error type [%d] at Line [%d]: %s.\n", num, line, msg);
    return ;
}

int isEqual(Type a, Type b) {
    int flag = 0;
    if ((a->kind == UNDEFINED) || (b->kind == UNDEFINED)) {
        if (a->kind == UNDEFINED) memcpy(a, b, sizeof(TypeNode));
        if (b->kind == UNDEFINED) memcpy(b, a ,sizeof(TypeNode));
        return 0;
    }
    if (a->kind == b->kind) {
        if ((a->kind == BASIC) && (a->u.basic == b->u.basic)) {
            flag = 0;
        }
        else if ((a->kind == ARRAY) && (a->u.array.dimension == b->u.array.dimension) && (isEqual(a->u.array.elem, b->u.array.elem)==0))
            flag = 0;
        else if (a->kind == STRUCTURE) {
            
        }
        else flag = 1;
    }
    else flag = 1;
    //printf("flag = %d\n", flag);
    return flag;
}

static Type checkSymTable(char * id, int line) {
    // check Symbol Table for id, and if not found print error.
    int numofitems = 0;
    SymTable temp = Head;
    SymTable store1 = malloc(sizeof(SymTableNode));
    store1->type = malloc(sizeof(TypeNode)); 
    while (temp) {
        // search for item has the same id, if more than one, triggers an error.
        if (strcmp(temp->name, id) == 0) {
            numofitems = numofitems + 1;
            store1 = temp;
        }
        temp = temp->next;
    }
    #ifdef debug_2
    printf("For id [%s], we found [%d] matching item in SymTable.\n", id, numofitems);
    #endif
    if (numofitems == 0) {
        char msg[128];
        sprintf(msg, "Undefined Variable [%s]", id);
        error(1, line, msg);
        store1->type->kind = UNDEFINED;
    } 
    if (numofitems > 1) {
        char msg[128];
        sprintf(msg, "Redefined Variable [%s]", id);
        error(3, line, msg);
    }  
    return store1->type;
}

static Type checkStructMember(char * id, Type target, int line) {
    int numofitems = 0;
    Type target_type = malloc(sizeof(TypeNode));
    // got the correct struct;
    int numofmember = 0;
    FieldList member = target->u.structure;
    while (member) {
        if (strcmp(id, member->name) == 0) {
            numofmember = numofmember + 1;
            target_type = member->type;
        }
        member = member->tail;
    }
    if (numofmember == 0) {
        char msg[128];
        sprintf(msg, "Non-existent Struct [%s] Member [%s]", id, target->StructName);
        error(14, line, msg);
    }else if (numofmember > 1) {
        char msg[128];
        sprintf(msg, "Redefined Struct [%s] Member [%s]", id, target->StructName);
        error(15, line, msg);
    }else return target_type;
return target_type;
}

static FuncTable checkFuncTable(char * id, int line) {
    FuncTable cache = FuncHead;
    int numoffuncs = 0;
    FuncTable store2 = malloc(sizeof(FuncNode));
    store2->returnType = malloc(sizeof(TypeNode));
    while (cache) {
        if (strcmp(cache->name, id) == 0) {
            numoffuncs = numoffuncs + 1;
            store2 = cache;
        }
        cache = cache->next;
    }
    if (numoffuncs == 0) {
        store2->isDefined = 0;
        /*
        char msg[128];
        sprintf(msg, "Undefined Function [%s]", id);
        error(2, line, msg);
        */
    }  
    if (numoffuncs > 1) {
        char msg[128];
        sprintf(msg, "Redefined Function [%s]", id);
        error(4, line, msg);
    }
    return store2;
}

void checkBigExp(TreeNode Exp) {
    if (Exp->ExpType) return ;
    TreeNode child = Exp->child;
    int numofChild = 0;
    // get child number
    TreeNode temp = child;
    Exp->ExpType = malloc(sizeof(Type));
    Exp->isLeft = 0;
    while (temp){
        numofChild = numofChild + 1;
        temp = temp->sibling;
    }
    #ifdef debug_2
        printf("This Exp has [%d] children\n", numofChild);
    #endif // 
    if (numofChild == 1) {
        Type type;
        switch(child->type) {
            case 1:
                //printf("checkid\n");
                type = malloc(sizeof(TypeNode));
                type = checkSymTable(child->name, Exp->lineno);
                Exp->isLeft = 1;
                break;
            case 2:
                //printf("checkint\n");
                type = malloc(sizeof(TypeNode));
                type->kind = BASIC;
                type->u.basic = 0;
                break;
            case 3:
                //printf("checkfloat\n");
                type = malloc(sizeof(TypeNode));
                type->kind = BASIC;
                type->u.basic = 1;
                break;
        }
        Exp->ExpType = type;
    }
    else if (numofChild == 2) {
        TreeNode temp1, temp2;
        temp1 = Exp->child; temp2 = temp1->sibling;
        checkBigExp(temp2);
        temp1 = child; temp2 = child->sibling;
        if (strcmp(temp1->name, "MINUS") == 0) {
            if (temp2->ExpType->kind == BASIC)
                Exp->ExpType = temp2->ExpType;
            else error(7, Exp->lineno, "Operation Number not compatable with Operator MINUS");
        } else {
            if (temp2->ExpType->kind == BASIC) {
                Exp->ExpType = malloc(sizeof(TypeNode));
                Exp->ExpType->kind = BASIC;
                Exp->ExpType->u.basic = 0;
            }
            else error(7, Exp->lineno, "Operation Number not compatable with Operator NOT");
        }
    }
    else if (numofChild == 3) {
        TreeNode temp1, temp2, temp3;
        temp1 = Exp->child; temp2 = temp1->sibling; temp3 = temp2->sibling;
        if ((strcmp(temp1->name, "Exp") == 0) && (strcmp(temp3->name, "Exp") == 0)) {
            // Exp Operator Exp
            checkBigExp(temp1); checkBigExp(temp3);
            if (strcmp(temp2->name,"ASSIGNOP")==0) {
                //printf("%d, %d\n",temp1->ExpType->u.basic, temp3->ExpType->u.basic);
                if (isEqual(temp1->ExpType, temp3->ExpType) == 0) {
                    if (temp1->isLeft == 0) {
                        error(6, Exp->lineno, "The left-hand side of an assignment must be an variable");
                    }
                    Exp->ExpType = temp1->ExpType;
                } 
                else error(5, Exp->lineno, "Type mismatched for assignment");
            }          
            else if (strcmp(temp2->name,"AND")==0) {
                Type type_int = malloc(sizeof(TypeNode));
                type_int->kind = BASIC; type_int->u.basic = 0;
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (isEqual(temp1->ExpType, type_int) == 0)) {
                    Exp->ExpType = type_int;
                }
                else error(7, Exp->lineno, "Type mismatched for operand AND");
            } else if (strcmp(temp2->name,"OR")==0) {
                Type type_int = malloc(sizeof(TypeNode));
                type_int->kind = BASIC; type_int->u.basic = 0;
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (isEqual(temp1->ExpType, type_int) == 0)) {
                    Exp->ExpType = type_int;
                }
                else error(7, Exp->lineno, "Type mismatched for operand OR");
            }
            else if (strcmp(temp2->name, "RELOP")==0 ) {
                //printf("enter relop\n");
                Type type_int = malloc(sizeof(TypeNode));
                type_int->kind = BASIC; type_int->u.basic = 0;
                if (isEqual(temp1->ExpType, temp3->ExpType) == 0) {
                    Exp->ExpType = type_int;
                }
                else error(7, Exp->lineno, "Type mismatched for operand RELOP");
            }
            else if (strcmp(temp2->name, "PLUS") == 0) {
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (temp1->ExpType->kind == BASIC)) {
                    Exp->ExpType = temp1->ExpType;
                }
                else error(7, Exp->lineno, "Type mismatched for operand PLUS");
            }
            else if (strcmp(temp2->name, "MINUS") == 0) {
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (temp1->ExpType->kind == BASIC)) {
                    Exp->ExpType = temp1->ExpType;
                }
                else error(7, Exp->lineno, "Type mismatched for operand MINUS");
            }
            else if (strcmp(temp2->name, "STAR") == 0) {
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (temp1->ExpType->kind == BASIC)) {
                    Exp->ExpType = temp1->ExpType;
                }
                else error(7, Exp->lineno, "Type mismatched for operand MULTIFLY");
            }
            else if (strcmp(temp2->name, "DIV") == 0) {
                if ((isEqual(temp1->ExpType, temp3->ExpType) == 0) && (temp1->ExpType->kind == BASIC)) {
                    Exp->ExpType = temp1->ExpType;
                }
                else error(7, Exp->lineno, "Type mismatched for operand DIV");
            }
        }
        else if (strcmp(temp1->name, "Exp") == 0) {
            // Exp Dot ID
            Exp->isLeft = 1;
            checkBigExp(temp1);
            if (temp1->ExpType->kind == STRUCTURE) {                
                Exp->ExpType = checkStructMember(temp3->name, temp1->ExpType, Exp->lineno);
            }
            else error(13, Exp->lineno, "Illegal use of [.]");
        }
        else if (temp1->type == 1) {
            //ID LP RP
            FuncTable target_func;
            target_func = checkFuncTable(temp1->name, temp1->lineno);
            if (target_func->isDefined == 0) {
                // func undefined, check if temp1 is a variable mistaken for function.
                int numofitems = 0;
                SymTable temp = Head;
                while (temp) {
                 // search for item has the same id, if more than one, triggers an error.
                    if (strcmp(temp->name, temp1->name) == 0) {
                        numofitems = numofitems + 1;
                    }
                    temp = temp->next;
                }
                if (numofitems == 0) {
                    char msg[128];
                    sprintf(msg, "Undefined Function [%s]", temp1->name);
                    error(2, Exp->lineno, msg);
                }
                else {
                    char msg[128];
                    sprintf(msg, "[%s] is not a function", temp1->name);
                    error(11, Exp->lineno, msg);
                }
            } else{
                // func definition detected! check if varlist matches
                if (target_func->VarList) {
                    char msg[128];
                    sprintf(msg, "Function [%s] is not applicable for argument ()", target_func->name);
                    error(9, Exp->lineno, msg);
                }
                Exp->ExpType = target_func->returnType;
            }
        }
        else if (temp1->type == 4) {
            //LP Exp RP
            checkBigExp(temp2);
            Exp->ExpType = temp2->ExpType;
        }
    }
    else if (numofChild == 4) {
        //TODO: 
        TreeNode temp1, temp2, temp3, temp4;
        temp1 = Exp->child; temp2 = temp1->sibling; temp3 = temp2->sibling; temp4 = temp3->sibling;
        if (temp1->type == 1) {
            // ID LP Args RP
            FuncTable target_func;
            target_func = checkFuncTable(temp1->name, temp1->lineno);
            if (target_func->isDefined == 0) {
                // func undefined, check if temp1 is a variable mistaken for function.
                int numofitems = 0;
                SymTable temp = Head;
                while (temp) {
                 // search for item has the same id, if more than one, triggers an error.
                    if (strcmp(temp->name, temp1->name) == 0) {
                        numofitems = numofitems + 1;
                    }
                    temp = temp->next;
                }
                if (numofitems == 0) {
                    char msg[128];
                    sprintf(msg, "Undefined Function [%s]", temp1->name);
                    error(2, Exp->lineno, msg);
                }
                else {
                    char msg[128];
                    sprintf(msg, "[%s] is not a function", temp1->name);
                    error(11, Exp->lineno, msg);
                }
            } else{
                // func definition detected! check if varlist matches
                int varflag = 0; // a flag to record whether received varlist is the same with defined varlist.
                // temp2 is Args
                TreeNode Args = temp2;
                // get received varlist of target function;
                FieldList ReceivedVarList;
                FieldList ReceivedVar_Head;
                int isFirst = 0;
                while (Args) {
                    ReceivedVarList = malloc(sizeof(Field));
                    if (isFirst == 0) {
                        ReceivedVar_Head = ReceivedVarList;
                        isFirst = 1;
                    }
                    checkBigExp(Args->child);
                    ReceivedVarList->type = Args->child->ExpType;
                    if (Args->child->sibling) {
                        Args = Args->child->sibling->sibling;
                        ReceivedVarList = ReceivedVarList->tail;
                    }
                    else break;
                }
                
                FieldList cache1 = ReceivedVar_Head;
                FieldList cache2 = target_func->VarList; // defined varlist
                while ((cache1) && (cache2)) {
                    if (isEqual(cache1->type, cache2->type) != 0) varflag = 1;
                    cache1 = cache1->tail;
                    cache2 = cache2->tail;
                }
                if ((cache1==NULL && cache2!=NULL)||(cache1!=NULL && cache2==NULL)) varflag = 1;
                if (varflag != 0) {
                    char msg[128];
                    cache1 = ReceivedVar_Head;
                    char typemsg[128];
                    while (cache1) {
                        char * unitType;
                        switch (cache1->type->kind) {
                            case BASIC:
                            unitType = (cache1->type->u.basic == 0)? "int" : "float";
                            break;
                            case ARRAY:
                            unitType = "array";
                            break;
                            case STRUCTURE:
                            unitType = "struct";
                            break;
                            case UNDEFINED:
                            break;
                        }
                        sprintf(typemsg, "%s, %s", typemsg, unitType);
                    }
                    sprintf(msg, "Function [%s] is not applicable for argument (%s)", target_func->name, typemsg);
                    error(9, Exp->lineno, msg);
                }
                Exp->ExpType = target_func->returnType;
            }
        }
        else {
            // Exp LB Exp RB
            // Is Exp1 AN ARRAY?
            // Is Exp2 AN INT?
            checkBigExp(temp1); checkBigExp(temp3);
            if (temp1->ExpType->kind == ARRAY) {
                Type type_int = malloc(sizeof(TypeNode));
                type_int->kind = BASIC; type_int->u.basic = 0;
                if (isEqual(temp3->ExpType, type_int)==0) {
                    Exp->ExpType = temp1->ExpType->u.array.elem;
                }
                else {
                    char msg[128];
                    if (temp3->child->type == 3)
                        sprintf(msg, "[%f] is not an integer", temp3->child->type_float);
                    else sprintf(msg, "[%s] is not an integer", temp3->child->name);
                    error(12, Exp->lineno, msg);
                }
            }
            else {
                char msg[128];
                sprintf(msg, "[%s] is not an array", temp1->child->name);
                error(10, Exp->lineno, msg);
                Exp->ExpType->kind = UNDEFINED;
            }
            Exp->isLeft = 1;
        }
    } 
    #ifdef debug_2
    printf("At end of checkBigExp, We have Exp Type: %d\n", Exp->ExpType->kind);
    #endif
    return ;
}

void check(TreeNode root) {
    if (root) {
        #ifdef debug_2
        //printf("Enter Check\n");
        #endif
        TreeNode temp;
        if (strcmp(root->name,"Exp") == 0){
            //printf("Enter CheckExp\n");
            checkBigExp(root);
            temp = root->sibling;
        }
        else temp = root->child;
        while (temp) {
            check(temp);
            temp = temp->sibling;
        }
    }
    return ;
}