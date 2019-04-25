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
        return 0;
    }
    if (a->kind == b->kind) {
        if ((a->kind == BASIC) && (a->u.basic == b->u.basic)) {
            flag = 0;
        }
        else if ((a->kind == ARRAY) && (a->u.array.dimension == b->u.array.dimension) && (isEqual(a->u.array.elem, b->u.array.elem)==0))
            flag = 0;
        else if ((a->kind == STRUCTURE) && (strcmp(a->StructName, b->StructName) == 0))
            flag = 0;
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
        char msg[128];
        sprintf(msg, "Undefined Function [%s]", id);
        error(2, line, msg);
    }  
    if (numoffuncs > 1) {
        char msg[128];
        sprintf(msg, "Redefined Function [%s]", id);
        error(4, line, msg);
    }
    return store2;
}

void checkBigExp(TreeNode Exp) {
    TreeNode child = Exp->child;
    int numofChild = 0;
    // get child number
    TreeNode temp = child;
    Exp->ExpType = malloc(sizeof(Type));
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
                break;
            case 2:
                printf("checkint\n");
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
            //else if ( ) {}
        }
        else if (strcmp(temp1->name, "Exp") == 0) {
            // Exp Dot ID
        }
        else if (temp->type == 1) {
            // ID LP RP
        }
        else if (temp->type == 4) {
            // LP Exp RP
        }
    }
    else if (numofChild == 4) {

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
            printf("Enter CheckExp\n");
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