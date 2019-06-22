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
            FieldList cachea = a->u.structure;
            FieldList cacheb = b->u.structure;
            while ((cachea)&&(cacheb)) {
                if (isEqual(cachea->type, cacheb->type) != 0) flag = 1;
                cachea = cachea->tail;
                cacheb = cacheb->tail; 
            }
            if ((cachea == NULL) && (cacheb == NULL)) {}
            else flag = 1;
        }
        else flag = 1;
    }
    else flag = 1;
    return flag;
}

Type checkSymTable(char * id, int line) {
    // check Symbol Table for id, and if not found print error;
    int numofitems = 0;
    //printf("checksymtable for id %s\n", id);
    SymTable temp = Head;
    SymTable store1 = malloc(sizeof(SymTableNode));
    store1->type = malloc(sizeof(TypeNode)); 
    while (temp) {
        // search for item has the same id, if more than one, triggers an error.
        if (strcmp(temp->name, id) == 0) {
            numofitems = numofitems + 1;
            memcpy(store1, temp, sizeof(SymTableNode));
        }
        temp = temp->next;
    }
    //printf("numofitems found %d",numofitems);
    //printf("type:%d\n",store1->type->kind);
    #ifdef debug_2
    printf("For id [%s], we found [%d] matching item in SymTable.\n", id, numofitems);
    #endif
    if (numofitems == 0) { 
        char msg[128];
        sprintf(msg, "Undefined Variable [%s]", id);
        error(1, line, msg);
        store1->type->kind = UNDEFINED; 
    } 
    if (numofitems > 1) {/*
        char msg[128];
        sprintf(msg, "Redefined Variable [%s]", id);
        error(3, line, msg); */
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
            memcpy(target_type,member->type,sizeof(TypeNode));
        }
        member = member->tail;
    }
    if (numofmember == 0) {
        char msg[128];
        sprintf(msg, "Non-existent Struct [%s] Member in [%s]", id, target->StructName);
        error(14, line, msg);
        target_type->kind = UNDEFINED;
    }else if (numofmember > 1) {
        //char msg[128];
        //sprintf(msg, "Redefined Struct [%s] Member [%s]", id, target->StructName);
        //error(15, line, msg);
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
            memcpy(store2,cache,sizeof(FuncNode));
        }
        cache = cache->next;
    }
    if (numoffuncs == 0) {
        store2->isDefined = 0;
        /*
        char msg[128];
        sprintf(msg, "Undefined Function [%s]", id);
        error(2, line, msg); */
        
    }  
    if (numoffuncs > 1) {
        //char msg[128];
        //sprintf(msg, "Redefined Function [%s]", id);
        //error(4, line, msg);
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
                //printf("type %d\n",type->kind);
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
        memcpy(Exp->ExpType, type, sizeof(TypeNode));
        //printf("Exp->type %d\n", Exp->ExpType->kind);
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
                if (isEqual(temp1->ExpType, temp3->ExpType) == 0) {
                    if (temp1->isLeft == 0) {
                        error(6, Exp->lineno, "The left-hand side of an assignment must be an variable");
                    }
                } 
                else error(5, Exp->lineno, "Type mismatched for assignment");
                Exp->ExpType = temp1->ExpType;
                Exp->isLeft = 1;
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
        else if (strcmp(temp1->name, "Exp") == 0) {// Exp Dot ID
            checkBigExp(temp1);
            if (temp1->ExpType->kind == STRUCTURE) {   
                //printf("hello\n");             
                Exp->ExpType = checkStructMember(temp3->name, temp1->ExpType, Exp->lineno);
                //printf("memberid%s, type%d\n", temp3->name, Exp->ExpType->kind);
                //if (Exp->ExpType->kind == ARRAY) printf("%s, dimension%d\n", temp3->name,Exp->ExpType->u.array.dimension);
            }
            else error(13, Exp->lineno, "Illegal use of [.]");
            Exp->isLeft = temp1->isLeft;
        }
        else if (temp1->type == 1) { //ID LP RP
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
        else if (temp1->type == 4) { //LP Exp RP
            checkBigExp(temp2);
            Exp->ExpType = temp2->ExpType;
            Exp->isLeft = temp2->isLeft;
        }
    }
    else if (numofChild == 4) {
        //TODO: 
        TreeNode temp1, temp2, temp3, temp4;
        temp1 = Exp->child; temp2 = temp1->sibling; temp3 = temp2->sibling; temp4 = temp3->sibling;
        if (temp1->type == 1) {// ID LP Args RP
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
            } else{ // func definition detected! check if varlist matches
                int varflag = 0; // a flag to record whether received varlist is the same with defined varlist.
                // temp2 is Args
                TreeNode Args = temp3;
                // get received varlist of target function;
                FieldList ReceivedVarList,last;
                last = malloc(sizeof(Field));
                FieldList ReceivedVar_Head = NULL;
                while (Args) {
                    ReceivedVarList = malloc(sizeof(Field));
                    ReceivedVarList->type = malloc(sizeof(TypeNode));
                    checkBigExp(Args->child);
                    ReceivedVarList->type = Args->child->ExpType;
                    insertField(&ReceivedVar_Head, &ReceivedVarList);
                    //ReceivedVarList->tail = ReceivedVar_Head;
                    //ReceivedVar_Head = ReceivedVarList;
                    if (Args->child->sibling) {
                        Args = Args->child->sibling->sibling;  
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
                if ((cache1==NULL) && (cache2==NULL)) {}
                else varflag = 1;
                if (varflag != 0) {
                    char msg[128];
                    cache1 = ReceivedVar_Head;
                    char typemsg[128];
                    int isFirst = 0;
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
                        if (isFirst == 0) {
                            strcpy(typemsg, unitType);
                            isFirst = 1;
                        }
                        else strcat(typemsg,unitType);
                        if (cache1->tail) strcat(typemsg,", ");
                        cache1 = cache1->tail;
                    }
                    sprintf(msg, "Function [%s] is not applicable for argument (%s)", target_func->name, typemsg);
                    error(9, Exp->lineno, msg);
                }
                Exp->ExpType = target_func->returnType;
            }
        }
        else { // Exp LB Exp RB
            // Is Exp1 AN ARRAY?
            // Is Exp2 AN INT?
            checkBigExp(temp1); checkBigExp(temp3);
            //printf("temp 1%s, 2%s, 3%s, 4%s\n",temp1->name, temp2->name, temp3->name, temp4->name);
            if (temp1->ExpType->kind == ARRAY) {
                Type type_int = malloc(sizeof(TypeNode));
                type_int->kind = BASIC; type_int->u.basic = 0;
                if (isEqual(temp3->ExpType, type_int)==0) {
                    if (temp1->ExpType->u.array.dimension == 1) {
                        Exp->ExpType = temp1->ExpType->u.array.elem;
                        //printf("return type has dimension 0 and type %d\n",Exp->ExpType->kind);
                    }
                    else {
                        Exp->ExpType = temp1->ExpType;
                        Exp->ExpType->u.array.dimension = Exp->ExpType->u.array.dimension - 1;
                        //printf("return type has dimension %d and type %d\n",Exp->ExpType->u.array.dimension, Exp->ExpType->kind);
                    }
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
                char * notarray;
                TreeNode temp1_temp = temp1;
                //printf("temp1 typekind %d\n",temp1->ExpType->kind);
                while (temp1_temp){
                    if (temp1_temp->type == 1) notarray = temp1_temp->name;
                    temp1_temp = temp1_temp->child;
                }
                char msg[128];
                sprintf(msg, "[%s] is not an array", notarray);
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

TreeNode searchReturn(TreeNode root){
    TreeNode target;
    if (root) {
        if (strcmp(root->name, "RETURN") == 0) return root;
        TreeNode temp = root->child;
        while (temp) {
            target = searchReturn(temp);
            if (target) return target;
            temp = temp->sibling;
        }
        target = NULL;
    }
    return target;
}

void checkReturnType(FuncTable Func, TreeNode CompSt) {
    //printf("checkReturnType\n");
    TreeNode temp = CompSt->child;
    TreeNode target;
    int isReturn = 0;
    while (temp) {
        if (strcmp(temp->name, "StmtList") == 0) {
            target = searchReturn(temp);
            if (target) target = target->sibling;
            else return ;
            isReturn = 1;
            break;
        }
        temp = temp->sibling;
    }
    if (isReturn == 0) return ;
    FieldList VarList = Func->VarList;
    int isVarList = 0;
    if (target->child->type == 1) { //return id, needs to analyze id type 
            while (VarList) {// if function has variable list
                if (strcmp(VarList->name, target->child->name) == 0) {
                    isVarList = 1; // return id is in varlist;
                    target->ExpType = malloc(sizeof(TypeNode));
                    target->ExpType = VarList->type;
                }
                VarList = VarList->tail;
            }
            if (isVarList != 1) { // no varlist for this func, search at symtable
                checkBigExp(target);
            }
    } else checkBigExp(target);
    //printf("type kind %d, basic %d.\n", target->ExpType->kind, target->ExpType->u.basic);
    if (isEqual(target->ExpType, Func->returnType)!=0) {
        error(8, target->lineno, "Type mismatched for return");
    }
    return ;
}

void checkExtDef(TreeNode root) {
    TreeNode specifier = root->child;
    TreeNode Sibling = specifier->sibling; 
    if (strcmp(Sibling->name, "FunDec")==0) { // ExtDef -> Specifier FuncDec CompSt
        FuncTable func = checkFuncTable(Sibling->child->name, root->lineno);
        checkReturnType(func, Sibling->sibling); // check CompSt for RETURN
    }
    return ;
}

void checkIniAssignment(TreeNode temp, Type defType) {
    if (temp) {
        if (strcmp(temp->name, "Exp") == 0) {
            //Debugger();
            checkBigExp(temp);
            if (isEqual(temp->ExpType, defType) == 0) {}
            else error(5, temp->lineno, "Type mismatched for assignment at initialization");
            temp = temp->sibling;
        }
        else temp = temp->child;
        while (temp) {
            checkIniAssignment(temp, defType);
            temp = temp->sibling;
        }
    }
}
void check(TreeNode root) {
    if (root) {
        #ifdef debug_2
        //printf("Enter Check\n");
        #endif
        //printf("check %s\n", root->name);
        TreeNode temp;
        if (strcmp(root->name,"Exp") == 0){
            checkBigExp(root);
            temp = root->sibling;
        }
        else temp = root->child;
        if (strcmp(root->name, "ExtDef") == 0) {
            checkExtDef(root);
        }
        while (temp) {
            check(temp);
            temp = temp->sibling;
        }
    }
    return ;
}