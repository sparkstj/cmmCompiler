#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__


struct TreeNode{
		int lineno;
		int type; // 0 for nonterminals, 1 for id, 2 for decint, 3 for float, 4 for Specifiers, 5 for type
		char * name;
		int type_int;
		float type_float;
		struct TreeNode * child;
		struct TreeNode * sibling;
	}TreeNode;

void SemanticAnalysis(struct TreeNode *);

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Symtable_ * Symtable;


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
};

struct FieldList_ {
	char* name;
	Type type;
	FieldList tail;
};

#endif