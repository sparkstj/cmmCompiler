#include <stdio.h>
#include "symtable.h"


void addFunction(Type type, struct TreeNode *  func){
    printf("Enter addFunction.\n");
    return ;
}

void addValue(Type type, struct TreeNode *  glob){
    // add struct, array, basic types
    printf("Enter addValue.\n");
    return ;
}

Type getType(struct TreeNode *  specifier) {
    struct TreeNode *  child = specifier->child;
    Type type;
    if (strcmp(child->name, "TYPE") == 0) {
        type->kind = BASIC;
        child = child->child;
        if (strcmp(child->name, "int") == 0) 
            type->u.basic = 0;
        else type->u.basic = 1;
    }
    else {
        child = child->child;
        type->kind = STRUCTURE;
        struct TreeNode * sibling = child->sibling;
        if (strcmp(sibling->name, "Tag")) {
            //look up this struct in symbol table.
        }
        else {
            sibling = sibling->sibling->sibling; // get DefList 
            struct TreeNode * temp2;
            while (temp2)
            type->u.structure = ;
        }
    }
}

void Add(struct TreeNode *  root) {
    struct TreeNode *  temp = root->child;
    struct TreeNode *  specifier = root->child;
    Type type = getType(specifier);
    while (temp) {
        if (strcmp(temp->name, "FunDec") == 0) {
            addFunction(type, temp);
        }
        else addValue(type, temp);
    return ;
}

void SemanticAnalysis(struct TreeNode *  root){
    printf("Semantic Analysis Begins Here.\n");
    if (root) {
        if (strcmp(root->name, "ExtDef") == 0) {
            Add(root);
        }
        if (strcmp(root->name, "Def") == 0) {
            Add(root);
        }
        struct TreeNode * temp = root->child;
        while (temp) {
			SemanticAnalysis(temp);
			temp = temp->sibling;
		}
    }

}