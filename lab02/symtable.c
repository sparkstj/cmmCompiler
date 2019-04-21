#include <stdio.h>
#include "symtable.h"
#define debug_1

void addFunction(Type type, TreeNode func){
    printf("Enter addFunction.\n");
    return ;
}

void addValue(Type type, TreeNode glob){
    // add struct, array, basic types
    printf("Enter addValue.\n");
    return ;
}
char * TraverseDec(TreeNode root, int *num, int *sum){
    #ifdef debug_1
        printf("Enter TraverseDec.\n");
        printf("%s\n",root->name);
    #endif 
    char * return_id;
    if (root) {      
        if (root->type == 1) 
            return_id = root->name;
        else return_id = NULL;
        if (root->type == 2)
            *sum = *sum * root->type_int;
        if (strcmp(root->name, "VarDec") == 0){
            *num = *num + 1;
        }
            
        TreeNode temp = root->child;
        while (temp) {
            char * rd = TraverseDec(temp, num, sum);
            if (rd) return_id = rd;
            temp = temp->sibling;
        }
    }
    return return_id;
}

Type getType(TreeNode specifier) {
    #ifdef debug_1
     printf("Enter get Type\n");
     printf("%s\n", specifier->name);
    #endif // 
    TreeNode child = specifier->child; 
    Type type = malloc(sizeof(TypeNode)); // return value
    if (child->type == 5) {
        // It's a Basic Type
        type->kind = BASIC;
        #ifdef debug_1
            printf("kind:%d\n",type->kind);
        #endif
        if (strcmp(child->name, "int") == 0) 
            type->u.basic = 0;
        else type->u.basic = 1;
    }
    else { // It's a Struct
        TreeNode tag = child->child->sibling;
        type->kind = STRUCTURE;
        if (strcmp(tag->name, "Tag") == 0) {
            #ifdef debug_1
                printf("look up this struct in symbol table.\n");
            #endif
            //look up this struct in symbol table.
        }
        else {
            type->u.structure = (FieldList) malloc(sizeof(Field)); 
            FieldList structureField = type->u.structure;
            TreeNode DefList = tag->sibling->sibling; // get DefList
            while (DefList) {
                TreeNode Def = DefList->child; // get Def
                structureField->type = getType(Def->child); //get type
                TreeNode DecList = Def->child->sibling; 
                //get DecList
                TreeNode Dec = DecList->child; // get Dec
                while (DecList) {
                    TreeNode temp = Dec;  
                    int num = 0;
                    int sum = 1;
                    char * id = TraverseDec(Dec, &num, &sum);
                    structureField->name = id;
                    if (num > 1) {
                        sum = (structureField->type->u.basic == 0) ? sum * 4 : sum * 8;
                        structureField->type->u.array.elem = structureField->type;
                        structureField->type->kind = ARRAY;
                        structureField->type->u.array.size = sum;
                    }
                    if (Dec->sibling == NULL) break;
                    DecList = Dec->sibling->sibling;
                    Dec = DecList->child;
                }
                #ifdef debug_1
                    printf("%d\n",structureField->type->kind);
                #endif
                structureField->tail = (FieldList) malloc(sizeof(Field));
                structureField = structureField->tail;
                DefList = Def->sibling;
                if(strcmp(DefList->name,"empty")==0) break;
                }
            }
        }
    return type;
}

void Add(TreeNode root) {
    #ifdef debug_1
        printf("Enter Add\n");
    #endif
    TreeNode temp = root->child;
    TreeNode specifier = root->child;
    Type type = getType(specifier);
    #ifdef debug_1
        printf("type: kind: %d\n",type->kind);
    #endif 
    /*
    while (temp) {
        if (strcmp(temp->name, "FunDec") == 0) {
            addFunction(type, temp);
        }
        else addValue(type, temp);
    */
    return ;
}

void SemanticAnalysis(TreeNode root){
    #ifdef debug_1
        printf("Semantic Analysis Begins Here.\n");
    #endif

    if (root) {
        if (strcmp(root->name, "ExtDef") == 0) {
            Add(root);
        }
        if (strcmp(root->name, "Def") == 0) {
            Add(root);
        }
        struct TreeNode_ * temp = root->child;
        while (temp) {
			SemanticAnalysis(temp);
			temp = temp->sibling;
		}
    }

}