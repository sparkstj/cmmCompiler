#include <stdio.h>
#include "symtable.h"
#define debug_1

static void checkSymTable(char * id, int line) {
    // check Symbol Table for id, and if not found print error.
    int numofitems = 0;
    SymTable temp = Head;
    SymTable store = malloc(sizeof(SymTableNode));
    while (temp) {
        // search for item has the same id, if more than one, triggers an error.
        if (strcmp(temp->name, id) == 0) {
            numofitems = numofitems + 1;
            store = temp;
        }
        temp = temp->next;
    }
    if (numofitems > 0) {
        char msg[128];
        if (store->type->kind != STRUCTURE) {
            sprintf(msg, "Redefined Variable [%s]", id);
            error(3, line, msg);
        }
        else {
            sprintf(msg, "Redefined Struct [%s]", id);
            error(16, line, msg);
        }
        
    }  
    return ;
}

static void checkStructMember(char * id, Type target, int line) {
    // got the correct struct;
    int numofmember = 0;
    FieldList member = target->u.structure;
    while (member) {
        if (strcmp(id, member->name) == 0) {
            numofmember = numofmember + 1;
        }
        if (strcmp(member->name, "empty") == 0) break;
        member = member->tail;
    }
    //printf("%d\n", numofmember);
    if (numofmember > 0) {
        char msg[128];
        sprintf(msg, "Redefined Struct [%s] Member [%s]", id, target->StructName);
        error(15, line, msg);
    }
    return ;
}

static void checkFuncTable(char * id, int line) {
    FuncTable cache = FuncHead;
    int numoffuncs = 0;
    while (cache) {
        if (strcmp(cache->name, id) == 0) {
            numoffuncs = numoffuncs + 1;
        }
        cache = cache->next;
    }
    if (numoffuncs > 0) {
        char msg[128];
        sprintf(msg, "Redefined Function [%s]", id);
        error(4, line, msg);
    }
    return ;
}

int getsize(Type type) {
    int kind = type->kind;
    int basicsize,sum;
    FieldList structure;
    switch (kind) {
        case 0:
            //printf("%d\n",type->u.basic);
            sum = type->u.basic == 0 ? 4 : 8;
            break;
        case 1:
            //printf("ElemTypeKind %d\n",type->u.array.elem->kind);
            basicsize = getsize(type->u.array.elem);
            //printf("%d\n", basicsize);
            sum = basicsize * (type->u.array.size);
            break;
        case 2:
            structure = type->u.structure;
            sum = 0;
            while (structure) {
                //printf("In GetType of Struct, there's a type kind: %d, id : %s\n",structure->type->kind, structure->name);
                //if (structure->type->kind == ARRAY) printf("ElemTypeKind %d\n",structure->type->u.array.elem->kind);
                sum += getsize(structure->type);
                //printf("Hello\n");
                structure = structure->tail;
                if (strcmp(structure->name,"empty")==0) break;
            }
            break;
    }
    return sum;
}

void AddSymTable(char * id, Type type, int line) {
    #ifdef debug_1
        printf("ID %s, KIND %d\n",id, type->kind);
    #endif // 
    //printf("check\n");
    checkSymTable(id, line);
    //printf("aftercheck\n");
    SymTable new = malloc(sizeof(SymTableNode));
    new->name = id;
    new->type = type;
    new->next = Head;
    Head = new;
    return ;
}

char * TraverseDec(TreeNode root, int *num, int *ans){
    #ifdef debug_1
        //printf("Enter TraverseDec.\n");
        //printf("%s\n",root->name);
    #endif 
    char * return_id;
    if (root) {      
        if (root->type == 1) 
            return_id = root->name;
        else return_id = NULL;
        if (root->type == 2)
            *ans = *ans * root->type_int;
        if (strcmp(root->name, "VarDec") == 0){
            *num = *num + 1;
        }
            
        TreeNode temp = root->child;
        while (temp) {
            char * rd = TraverseDec(temp, num, ans);
            if (rd) return_id = rd;
            temp = temp->sibling;
        }
    }
    return return_id;
}

Type getType(TreeNode specifier) {
    TreeNode child = specifier->child; 
    Type type = malloc(sizeof(TypeNode)); // return value
    if (child->type == 5) {
        // It's a Basic Type
        type->kind = BASIC;
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
            SymTable temp = Head;
            char * struct_id = tag->child->name;
            int numofstruct = 0;
            Type store;
            char msg[128];
            //printf("%s\n", struct_id);
            while (temp) {
                if (strcmp(struct_id, temp->name) == 0) {
                    if (temp->type->kind != STRUCTURE) {    
                        sprintf(msg, "Struct [%s] Name Duplicate with Variable", struct_id);
                        error(16, tag->lineno, msg);
                    }
                    else {
                        numofstruct = numofstruct + 1;
                        store = temp->type;
                    }
                }
                temp = temp->next;
            }
            //printf("numofstruct: %d", numofstruct);
            if (numofstruct == 0) {
                sprintf(msg, "Struct [%s] Undefined.\n", struct_id);
                error(17, tag->lineno, msg);
            } else if (numofstruct > 1) {
                sprintf(msg, "Struct [%s] Name Duplicate with other Struct", struct_id);
                error(16, tag->lineno, msg);
            } else {
                memcpy(type, store, sizeof(TypeNode));
            }
            //printf("type %s\n", store->u.structure->name);
        }
        else {
            if (tag->child) {
                //printf("OptTag %s\n", tag->child->name);
                type->StructName = tag->child->name; // get struct name if possible, this is to get global definition name.
                //printf("StoreinType %s\n", type->StructName);
            }
            type->u.structure = (FieldList) malloc(sizeof(Field)); 
            FieldList structureField = type->u.structure;
            structureField->name = "empty";
            //printf("%s\n", structureField->name);
            TreeNode DefList = tag->sibling->sibling; // get DefList
            while (DefList) {
                TreeNode Def = DefList->child; // get Def
                Type temp_type = getType(Def->child); //get type
                TreeNode DecList = Def->child->sibling; 
                //get DecList
                TreeNode Dec = DecList->child; // get Dec
                while (DecList) {
                    int num = 0; int ans = 1;
                    char * id = TraverseDec(Dec, &num, &ans);
                    checkStructMember(id, type, DefList->lineno);
                    structureField->name = id;
                    structureField->type = malloc(sizeof(TypeNode));
                    if (num > 1) {
                        structureField->type->u.array.elem = malloc(sizeof(TypeNode));
                        ans = ans * getsize(temp_type);
                        structureField->type->u.array.size = ans;
                        structureField->type->u.array.elem = temp_type;
                        structureField->type->u.array.dimension = num - 1;
                        structureField->type->kind = ARRAY;
                    }
                    else structureField->type = temp_type;
                    structureField->tail = (FieldList) malloc(sizeof(Field));
                    structureField = structureField->tail;
                    structureField->name = "empty";
                    //printf("GetType id%s, type %d\n",structureField->name,structureField->type->kind);
                    if (Dec->sibling == NULL) break;
                    DecList = Dec->sibling->sibling;
                    Dec = DecList->child;
                }
                DefList = Def->sibling;
                if(strcmp(DefList->name,"empty")==0) break;
                
            } 
            //structureField->name = "empty";
            //printf("%d\n",type->kind);
        }
    }
    return type;
}


void AnalyzeExtDecList(Type type, TreeNode ExtDecList) { // Analysis ExDef -> Specifier ExtDecList SEMI
    TreeNode VarDec = ExtDecList->child; 
    char * id;
    // get VarDec
    // if VarDec has no sibling, its either id or array
    // if VarDec has sibling, then one type has more than one parameter
    Type modified = malloc(sizeof(TypeNode));
    while (ExtDecList) {
        int num = 0;
        int ans = 1;
        id = TraverseDec(VarDec, &num, &ans);
        Type modified = malloc(sizeof(TypeNode));
        if (num > 1) {
            //printf("num %d, typekind:%d\n",num, type->kind);
            ans = ans * getsize(type);
            modified->u.array.elem = malloc(sizeof(TypeNode));
            modified->u.array.elem = type;
            modified->u.array.dimension = num - 1;
            //printf("ArrayType %d\n", modified->u.array.elem->kind);
            modified->kind = ARRAY;
            //printf("ArrayType %d\n", modified->u.array.elem->kind);
            modified->u.array.size = ans;
        }
        else modified = type;
        #ifdef debug_1
            //printf("This is in ExDef -> Specifier ExtDecList SEMI, ExtDecList\n");
            //printf("ID %s, kind: %d\n", id, type->kind);
        #endif // debug_1
        AddSymTable(id, modified, ExtDecList->lineno);
        if (VarDec->sibling == NULL) break;
        ExtDecList = VarDec->sibling->sibling;
        VarDec = ExtDecList->child;
    }
}

void AnalyzeFuncDec(Type type, TreeNode FunDec) { // Analysis ExtDef -> Specifier FuncDec CompSt
    int hasVarList = 0;
    TreeNode id = FunDec->child;
    TreeNode VarList = id->sibling->sibling;
    if (strcmp(VarList->name, "RP") == 0) hasVarList = 1;
    checkFuncTable(id->name, id->lineno);
    FuncTable new = malloc(sizeof(FuncNode));
    new->name = id->name;
    new->returnType = type;
    new->isDefined = 1; // is Defined
    new->next = FuncHead; // add to FuncTable List
    FuncHead = new; 
    // Analyze VarList
    FieldList newVar;
    if (hasVarList == 0) {
        new->VarList = malloc(sizeof(Field));
        newVar = new->VarList;
        while (VarList) { // Analyze one Paramdec
            TreeNode ParamDec = VarList->child;
            TreeNode specifier = ParamDec->child;
            Type VarType = getType(specifier);
            TreeNode VarDec = specifier->sibling;
            int num = 0;
            int ans = 1;
            char * id = TraverseDec(VarDec, &num, &ans);
            newVar->name = id;
            Type modified = malloc(sizeof(TypeNode));
            if (num > 1) {
                ans = ans * getsize(VarType);
                modified->u.array.elem = malloc(sizeof(TypeNode));
                modified->u.array.elem = VarType;
                modified->u.array.dimension = num - 1;
                modified->kind = ARRAY;
                modified->u.array.size = ans;
            }
            else modified = VarType;
            newVar->type = modified;
            newVar->tail = malloc(sizeof(Field));
            newVar = newVar->tail;
            if (ParamDec->sibling == NULL) break;
            VarList = ParamDec->sibling->sibling;
        }
        newVar->name="empty";
    }
    return ;
}

void AddExtDef(TreeNode root) {
    TreeNode specifier = root->child;
    Type type = getType(specifier);
    TreeNode Sibling = specifier->sibling; 
    if (strcmp(Sibling->name, "ExtDecList")==0) {  // ExDef -> Specifier ExtDecList SEMI
        AnalyzeExtDecList(type, Sibling);
    }
    else if (strcmp(Sibling->name, "FunDec")==0) { // ExtDef -> Specifier FuncDec CompSt
        AnalyzeFuncDec(type, Sibling);// not sure around parameters.
    }
    else {
        AddSymTable(type->StructName, type, specifier->lineno); //  ExtDef -> Specifier Semi
    }
    #ifdef debug_1
    #endif
    return ;
}

void AddDef(TreeNode root) { // Def -> Specifier DecList SEMI
    TreeNode Specifier = root->child;
    TreeNode DecList = Specifier->sibling; // DecList -> Dec | Dec COMMA DecList
    Type type = getType(Specifier);
    while (DecList) {
        TreeNode Dec = DecList->child;
        int num = 0; int ans = 1;
        char * id = TraverseDec(Dec, &num, &ans);
        Type modified = malloc(sizeof(TypeNode));
        if (num > 1) {
            ans = ans * getsize(type);
            modified->u.array.elem = malloc(sizeof(TypeNode));
            modified->u.array.elem = type;
            modified->u.array.dimension = num - 1;
            modified->kind = ARRAY;
            modified->u.array.size = ans;
        }
        else modified = type;
        AddSymTable(id, type, DecList->lineno);
        if (Dec->sibling == NULL) break;
        DecList = Dec->sibling->sibling;
    }
    return ;
}

void SemanticAnalysis(TreeNode root){
    if (root) {
        //printf("%s\n", root->name);
        struct TreeNode_ * temp = root->child;
        if (strcmp(root->name, "ExtDef") == 0) {
            AddExtDef(root);
            //printf("Hellp\n");
            temp = root->child->sibling->sibling;
            //printf("%s\n", temp->name);
        }
        if (strcmp(root->name, "Def") == 0) {
            AddDef(root);
            temp = root->child;
        }
        
        while (temp) {
			SemanticAnalysis(temp);
			temp = temp->sibling;
		}
    }
}

void Debugger() {
    printf("** SYMBOL TABLE **\n");
    SymTable temp = Head;
    while (temp) {
        printf("Name: %s, Type.Kind%d\n",temp->name, temp->type->kind);
        if (temp->type->kind == ARRAY) {
            printf("Type: %d, Size: %d, Dimension: %d\n", temp->type->u.array.elem->kind, temp->type->u.array.size, temp->type->u.array.dimension);
        }
        if (temp->type->kind == STRUCTURE) {
            FieldList temp2 = temp->type->u.structure;
            while (temp2) {
                if (strcmp(temp2->name, "empty") == 0) break;
                printf("struct :: name %s, type %d\n", temp2->name, temp2->type->kind);
                temp2 = temp2->tail;  
            }
            
        }
        temp = temp->next;
    }
    printf("** FUNCTION TABLE **\n");
    FuncTable temp3 = FuncHead;
    while (temp3){
        printf("name: %s, return type: %d\n", temp3->name, temp3->returnType->kind);
        FieldList temp4 = temp3->VarList;
        while (temp4) {
            if (strcmp(temp4->name, "empty") == 0) break;
            printf("VarList :: name %s, type %d\n", temp4->name, temp4->type->kind);
            temp4 = temp4->tail;
        }
        temp3 = temp3->next;
    }
    return ;
}