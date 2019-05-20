#ifndef __SymTable_H__
#define __SymTable_H__

#include <stdlib.h>
#include <string.h>

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymTable_ * SymTable;
typedef struct TreeNode_ * TreeNode;
typedef struct FuncTable_ * FuncTable;
typedef struct InterCode_* InterCode;
typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct ArgList_* ArgList;

struct TreeNode_{
		int lineno;
		int type; // 0 for nonterminals, 1 for id, 2 for decint, 3 for float, 4 for Specifiers, 5 for type 
		char * name;
		int relop;
		// GRQ >= 0, LEQ <= 1, GE > 2, LE < 3, EQ == 4, NEQ != 5
		int type_int;
		float type_float;
		struct TreeNode_ * child;
		struct TreeNode_ * sibling;
		Type ExpType;
		int isLeft;
}Node;

struct SymTable_
{
	char * name;
	Type type;
	Operand variable;
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
void checkBigExp(TreeNode);
int isEqual(Type, Type);
Type checkSymTable(char *, int);
void insertField(FieldList*, FieldList*);
Type getType(TreeNode );
void checkIniAssignment(TreeNode , Type );
void initialIO();


/*--------------- Translate -----------------*/
#define MAXN 64

struct Operand_ {
    enum{ TEMP, VARIABLE, CONSTANT_INT, ADDRESS, LABEL_OP, FUNCTION, CONSTANT_FLOAT} kind;
    struct{
        int var_no; // temp (t), label, variable(v)
        int value; // constant, (temp, address, variable)
        float fval;
        char * id; // func // malloc id
        //...
    }u;
}OperandNode;

struct InterCode_{
    enum{ LABEL, FUNC, ASSIGN1, ASSIGN2, JMP_GOTO, JMP_IF, STMT_RETURN, DEC, ARG, CALL, PARAM, READ, WRITE } kind;
    enum {ASSIGN, ADD, SUB, MUL, DIV, ADDR, INDl, INDr, RELOP} Operator;
    // ADDR is &, INDl is (*x) = y, INDr is x = *y, ASSIGN is purely assign(x=y),
    enum {GRQ, LEQ, GE, LE, EQ, NEQ} relop;
    // GRQ >=, LEQ <=, GE >, LE <, EQ ==, NEQ !=
    union{ // malloc Operand
        struct {Operand x;} label; // LABEL X:
        struct {Operand f;} func; // FUNCTION f:
        struct {Operand right, left;} assign_1; // x := (operator) y
        struct {Operand right1, right2, left;} assign_2; // x := y operator z
        //struct {Operand result, op1, op2;} binop; // x [relop] y
        struct {Operand x;} jmp_goto; //GOTO x
        struct {Operand result, op1, op2, z;} jmp_if; // IF x [relop] y GOTO z
        struct {Operand x;} stmt_return; // RETURN x
        struct {Operand x; int size;} dec; // DEC x [size]
        struct {Operand x;} arg; // ARG x
        struct {Operand x, f;} call; // CALL f
        struct {Operand x;} param; // PARAM x
        struct {Operand x;} read; // READ x
        struct {Operand x;} write; // WRITE x
    }u;
}InterCodeNode;

struct InterCodes_ {
    InterCode data; // malloc
    InterCodes prev;
    InterCodes next;
}InterCodesNode;

struct ArgList_ {
    Operand arg;
    ArgList next;
}ArgNode;

void translate(TreeNode );
void printInterCode(InterCodes );
InterCodes translate_Exp(TreeNode , Operand );
InterCodes translate_CompSt(TreeNode );
InterCodes translate_Args(TreeNode, ArgList *);
InterCodes translate_FunDec(TreeNode );
InterCodes translate_Cond(TreeNode , Operand , Operand );


InterCodes CodeHead;
int TempNo;
int VariaNo;
int LabelNo;
 

#endif