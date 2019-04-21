#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__


typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Symtable_ * Symtable;
typedef struct TreeNode_ * TreeNode;

struct TreeNode_{
		int lineno;
		int type; // 0 for nonterminals, 1 for id, 2 for decint, 3 for float, 4 for Specifiers, 5 for type
		char * name;
		int type_int;
		float type_float;
		struct TreeNode_ * child;
		struct TreeNode_ * sibling;
}Node;

struct Symtable_
{
	char * name;
	Type type;
	Symtable next;
};


struct Type_
{
	enum {BASIC, ARRAY, STRUCTURE } kind;
	union {
		int basic;
		struct {Type elem; int size;} array;
		FieldList structure;
	}u;
}TypeNode;

struct FieldList_ {
	char* name;
	Type type;
	FieldList tail;
}Field;


void SemanticAnalysis(struct TreeNode_ *);

#endif