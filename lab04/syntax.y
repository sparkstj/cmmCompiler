%{
	#define YYERROR_VERBOSE
	#include <stdio.h>
	#include "symtable.h"
	void add_child(struct TreeNode_ * a, struct TreeNode_ * b){
		if (a->child == NULL){
			a->child = b;
		}
		else {
			struct TreeNode_ * child = a->child;
			while (child != NULL) {
				child = child->sibling;
			}
			child = b;
		}
		return ;
}
	int flag = 0;
	struct TreeNode_ * create_node(char * name, int lineno, char * text){
		struct TreeNode_ * node = (struct TreeNode_ *) malloc (sizeof(Node));
		node->lineno = lineno;
		node->type = 0;
		node->name = malloc(strlen(name)+1);
		strcpy(node->name, name);
		node->child = NULL;
		node->sibling = NULL;
		return node;
	}

	void add_sibling(struct TreeNode_ * a, struct TreeNode_ * b){
		a->sibling = b;
		return ;
	}	
	extern int lexical_flag;
	void printTree(struct TreeNode_ *root, int layer) {
		if ((flag == 1)|| (lexical_flag == 1)) return ;
		if (root) {
			if (strcmp(root->name, "empty") != 0) {
				for (int i = 0; i < layer; i++) 
					printf("  ");
				printf("[%d]",layer);
				switch (root->type){
					case 0:
						printf("%s (%d)\n", root->name, root->lineno);
						break;
					case 1:
						printf("ID: %s\n", root->name);
						break;
					case 4: 
						printf("%s\n", root->name);
						break;
					case 2:
						printf("INT: %d\n", root->type_int);
						break;
					case 3:
						printf("%s: %f\n",root->name, root->type_float);
						break;
					case 5:
						printf("TYPE: %s\n", root->name);
						break;
				}
			}
		struct TreeNode_ *temp = root->child;
		while (temp) {
			//printf("temp name %s\n", temp->name);
			printTree(temp,layer+1);
			temp = temp->sibling;
		}
	}
	return ;
}

%}

/* declared types */
%union{
	struct TreeNode_ * node;
}


/* declared tokens */
%token <node> STRUCT RETURN IF ELSE WHILE TYPE ID FLOAT INT error
%token <node> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR AND OR DOT NOT LP RP LB RB LC RC DIV

/* declared nonterminals*/
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left PLUS MINUS 
%left STAR DIV RELOP AND OR
%right NOT 
%left LP RP LB RB DOT  

%%
/* rules */
/* High-level Definitions */

Program : ExtDefList { $$ = create_node("Program", @$.first_line, ""); add_child($$,$1); 
printTree($$, 0); 
initialIO();
//SemanticAnalysis($$); 
//Debugger(); 
//check($$);
//translate($$);
//printInterCode(CodeHead);
test();
}
	;

ExtDefList : ExtDef ExtDefList {$$ = create_node("ExtDefList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| /* empty */ {$$ = create_node("empty", @$.first_line, "");}
	;

ExtDef : Specifier ExtDecList SEMI {$$ = create_node("ExtDef", @$.first_line, "");  add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Specifier SEMI {$$ = create_node("ExtDef", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| Specifier FunDec CompSt {$$ = create_node("ExtDef", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	;


ExtDecList : VarDec {$$ = create_node("ExtDecList", @$.first_line, ""); add_child($$,$1);}
	| VarDec COMMA ExtDecList { $$ = create_node("ExtDecList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	;

/* Specifiers */
Specifier : TYPE {$$ = create_node("Specifier", @$.first_line, ""); add_child($$,$1);}
	| StructSpecifier {$$ = create_node("Specifier", @$.first_line, "");  add_child($$,$1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = create_node("StructSpecifier", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); add_sibling($4,$5);}
	| STRUCT Tag {$$ = create_node("StructSpecifier", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	;
OptTag : ID {$$ = create_node("OptTag", @$.first_line, ""); add_child($$,$1);}
	| /* empty */  {$$ = create_node("empty", @$.first_line, "");}
	;
Tag : ID {$$ = create_node("Tag", @$.first_line, ""); add_child($$,$1);}
	; 

/* Declarators */
VarDec : ID {$$ = create_node("VarDec", @$.first_line, ""); add_child($$,$1);}
	| VarDec LB INT RB {$$ = create_node("VarDec", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); }
	;
FunDec : ID LP VarList RP {$$ = create_node("FunDec", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); }
	| ID LP RP {$$ = create_node("FunDec", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); }
	;
VarList : ParamDec COMMA VarList {$$ = create_node("VarList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); }
	| ParamDec {$$ = create_node("VarList", @$.first_line, ""); add_child($$,$1);}
	;
ParamDec : Specifier VarDec {$$ = create_node("ParamDec", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	;

/* Statements */
CompSt : LC DefList StmtList RC {$$ = create_node("CompSt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); }
	| error RC
	;
StmtList : Stmt StmtList {$$ = create_node("StmtList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	|	/* empty */  {$$ = create_node("empty", @$.first_line, "");}
	;
Stmt : Exp SEMI {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| CompSt {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); }
	| RETURN Exp SEMI {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); add_sibling($4,$5);}
	| IF LP Exp RP Stmt ELSE Stmt {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); add_sibling($4,$5); add_sibling($5,$6); add_sibling($6, $7);}
	| WHILE LP Exp RP Stmt {$$ = create_node("Stmt", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); add_sibling($4,$5);}
	| error SEMI
	| Exp error ELSE Stmt
	;

/* Local Definitions */
DefList : Def DefList {$$ = create_node("DefList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| /* empty */  {$$ = create_node("empty", @$.first_line, "");}
	;
Def : Specifier DecList SEMI {$$ = create_node("Def", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	;
DecList : Dec  {$$ = create_node("DecList", @$.first_line, ""); add_child($$,$1); }
	| Dec COMMA DecList {$$ = create_node("DecList", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	;
Dec : VarDec {$$ = create_node("Dec", @$.first_line, ""); add_child($$,$1); }
	| VarDec ASSIGNOP Exp  {$$ = create_node("Dec", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	;

/* Expressions */
Exp : Exp ASSIGNOP Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp AND Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp OR Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp RELOP Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp PLUS Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp MINUS Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp STAR Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp DIV Exp  {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| LP Exp RP  {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| MINUS Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| NOT Exp {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2);}
	| ID LP Args RP {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); }
	| ID LP RP {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp LB Exp RB {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3); add_sibling($3,$4); }
	| Exp DOT ID {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| ID {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); }
	| INT {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); }
	| FLOAT {$$ = create_node("Exp", @$.first_line, ""); add_child($$,$1); }
	| Exp LB error RB 
	;
Args : Exp COMMA Args {$$ = create_node("Args", @$.first_line, ""); add_child($$,$1); add_sibling($1,$2); add_sibling($2,$3);}
	| Exp {$$ = create_node("Args", @$.first_line, ""); add_child($$,$1); }
	;
%%

#include "lex.yy.c"
	
yyerror(char* msg) {
	flag = 1;
	fprintf(stderr, "Error type B at line %d: %s %s\n", yylineno, msg, yytext);
}
