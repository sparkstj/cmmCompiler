#ifndef __SymTable_H__
#define __SymTable_H__

#include <stdlib.h>
#include <string.h>

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymTable_ * SymTable;
typedef struct TreeNode_ * TreeNode;
typedef struct FuncTable_ * FuncTable;

struct TreeNode_{
		int lineno;
		int type; // 0 for nonterminals, 1 for id, 2 for decint, 3 for float, 4 for Specifiers, 5 for type
		char * name;
		int type_int;
		float type_float;
		struct TreeNode_ * child;
		struct TreeNode_ * sibling;
		Type ExpType;
}Node;

struct SymTable_
{
	char * name;
	Type type;
	SymTable next;
}SymTableNode;

SymTable Head; // the head of symbol table;

struct FuncTable_ 
{
	char * name;
	Type returnType;
	FieldList VarList;
	FuncTable next;
	int isDefined;
}FuncNode;

FuncTable FuncHead;

struct Type_
{
	enum {BASIC, ARRAY, STRUCTURE, UNDEFINED} kind;
	union {
		int basic;
		struct {Type elem; int size; int dimension;} array;
		FieldList structure;
	}u;
	char * StructName;
}TypeNode;

struct FieldList_ {
	char* name;
	Type type;
	FieldList tail;
}Field;



void Debugger();
void SemanticAnalysis(struct TreeNode_ *);
void error(int, int, char *);
void check(TreeNode);


#endif