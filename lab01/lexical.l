%option yylineno
%{
	#include "syntax.tab.h"
	//#define debug
	int yycolumn = 1;
  	#define YY_USER_ACTION \
   	yylloc.first_line = yylloc.last_line = yylineno; \
  	yylloc.first_column = yycolumn; \
   	yylloc.last_column = yycolumn + yyleng - 1; \
   	yycolumn += yyleng;
	int lexical_flag = 0;
%}
%option
digit [0-9]
frontdecint "-"?([1-9]|[1-9][0-9]{1}|[1-9][0-9]{2}|[1-9][0-9]{3}|[1-9][0-9]{4}|[1-9][0-9]{5}|[1-9][0-9]{6}|[1-9][0-9]{7}|[1-9][0-9]{8}|0+)
latterdecint "-"?("1"[0-9]{9}|"20"[0-9]{8}|"21"[0-3][0-9]{7}|"214"[0-6][0-9]{6}|"2147"[0-3][0-9]{5}|"21474"[0-7][0-9]{4}|"214748"[0-2][0-9]{3}|"2147483"[0-5][0-9]{2}|"21474836"[0-3][0-9]|"214748364"[0-7])
specialnegint "-2147483648"
notint "-"?[1-9][0-9]+
octint "-"?"0"[0-7]+
notoctint "-"?"0"[0-9]+
hexint "-"?"0"[xX][0-9a-fA-F]+
nothexint "-"?"0"[xX][0-9a-zA-Z]+
id [a-zA-Z_][0-9a-zA-Z_]*
pfloat "-"?[0-9]+"."[0-9]+
efloat "-"?([0-9]+"."|[0-9]*"."[0-9]+)("e"|"E")("+"|"-")?[0-9]+
notfloat "-"?[0-9]*"."?[0-9]*("e"|"E")+("+"|"-")?[0-9]*"."?[0-9]*
relop ">"|"<"|">="|"<="|"=="|"!="
type "int"|"float"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
Blankspace [\t\f\v\r" "]
nestedcomments "/*"[^"*/"]*"/*"(.|\n)*"*/"
%%
"//" {char c = input();  while (c != '\n') c = input();}
"/*" {
	char now = input(); 
	char last = now; 
	while ((now != '/')||(last !='*')) {
		last = now;
		now = input();
		}
}
{STRUCT} { 	
			#ifdef debug 
			printf("STRUCT\n"); 
			#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 4;
				yylval.node->name = malloc(strlen("STRUCT")+1);
				strcpy(yylval.node->name, "STRUCT");
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return STRUCT;
			}
{RETURN} {
			#ifdef debug 
			printf("RETURN\n"); 
			#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 4;
				yylval.node->name = malloc(strlen("RETURN")+1);
				strcpy(yylval.node->name, "RETURN");
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return RETURN;
			}
{IF} {
	#ifdef debug 
	printf("IF\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 4;
				yylval.node->name = malloc(strlen("IF")+1);
				strcpy(yylval.node->name, "IF");
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return IF;
			}
{ELSE} {
	#ifdef debug 
	printf("ELSE\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 4;
				yylval.node->name = malloc(strlen("ELSE")+1);
				strcpy(yylval.node->name, "ELSE");
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return ELSE;
}
{WHILE} {
	#ifdef debug 
	printf("WHILE\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 4;
				yylval.node->name = malloc(strlen("WHILE")+1);
				strcpy(yylval.node->name, "WHILE");
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return WHILE;
}
{type} {
	#ifdef debug 
	printf("TYPE\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 5;
				yylval.node->name = malloc(strlen(yytext)+1);
				strcpy(yylval.node->name, yytext);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return TYPE;
}
{id} {
	#ifdef debug 
	printf("ID\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 1;
				yylval.node->name = malloc(strlen(yytext)+1);
				strcpy(yylval.node->name, yytext);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				return ID;
}
{pfloat} {
	#ifdef debug 
	printf("POINTFLOAT\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 3;
				yylval.node->type_float = atof(yytext);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				yylval.node->name = malloc(strlen("FLOAT")+1);
				strcpy(yylval.node->name, "FLOAT");
				return FLOAT;
			}
{efloat} {
	#ifdef debug 
	printf("EXPONENTFLOAT\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 3;
				yylval.node->type_float = atof(yytext);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				yylval.node->name = malloc(strlen("FLOAT")+1);
			 	strcpy(yylval.node->name, "FLOAT");
				return FLOAT;}
{notfloat} {lexical_flag = 1; printf("Error type A at line %d: Illegal floating point number \'%s\'\n", yylineno, yytext); return FLOAT;}
{octint} {
	#ifdef debug 
	printf("OCTINT\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 2;
				yylval.node->type_int = strtol(yytext, NULL, 8);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				yylval.node->name = malloc(strlen("OCTINT")+1);
				strcpy(yylval.node->name, "OCTINT");
				return INT;}
{notoctint} {lexical_flag = 1; printf("Error type A at line %d: Illegal octonary integer \'%s\'\n", yylineno, yytext); return INT;}
{hexint} {
	#ifdef debug 
	printf("HEXINT\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 2;
				yylval.node->type_int = strtol(yytext, NULL, 16);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				yylval.node->name = malloc(strlen("HEXINT")+1);
				strcpy(yylval.node->name, "HEXINT");
				return INT;}
{nothexint} {lexical_flag = 1; printf("Error type A at line %d: Illegal hexadecimal integer \'%s\'\n", yylineno, yytext); return INT;}
{frontdecint}|{latterdecint}|"-2147483648" {
	#ifdef debug 
	printf("DECINT\n"); 
	#endif
				yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
				yylval.node->lineno = yylineno;
				yylval.node->type = 2;
				yylval.node->type_int = atoi(yytext);
				yylval.node->child = NULL;
				yylval.node->sibling = NULL;
				yylval.node->name = malloc(strlen("INT")+1);
				strcpy(yylval.node->name, "INT");
				return INT;}
{notint} {lexical_flag = 1; printf("Error type A at line %d: Illegal decimal integer \'%s\'\n", yylineno, yytext); return INT;}
";" {
	#ifdef debug 
	printf("SEMI\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("SEMI")+1);
			strcpy(yylval.node->name, "SEMI");
			return SEMI;}
"," {
	#ifdef debug 
	printf("COMMA\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("COMMA")+1);
			strcpy(yylval.node->name, "COMMA");
			return COMMA;}
"=" {
	#ifdef debug 
	printf("ASSIGNOP\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("ASSIGNOP")+1);
			strcpy(yylval.node->name, "ASSIGNOP");
			return ASSIGNOP;}
{relop} {
	#ifdef debug 
	printf("RELOP\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("RELOP")+1);
			strcpy(yylval.node->name, "RELOP");
			return RELOP;}
"+" {
	#ifdef debug 
	printf("PLUS\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->name = malloc(strlen("PLUS")+1);
			strcpy(yylval.node->name, "PLUS");
			return PLUS;}
"-" {
	#ifdef debug 
	printf("MINUS\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("MINUS")+1);
			strcpy(yylval.node->name, "MINUS");
			return MINUS;}
"*" {
	#ifdef debug 
	printf("STAR\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("STAR")+1);
			strcpy(yylval.node->name, "STAR");
			return STAR;}
"&&" {
	#ifdef debug 
	printf("AND\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("AND")+1);
			strcpy(yylval.node->name, "AND");
			return AND;}
"||" {
	#ifdef debug 
	printf("OR\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("OR")+1);
			strcpy(yylval.node->name, "OR");
			return OR;}
"." {
	#ifdef debug 
	printf("DOT\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("DOT")+1);
			strcpy(yylval.node->name, "DOT");
			return DOT;}
"!" {
	#ifdef debug 
	printf("NOT\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("NOT")+1);
			strcpy(yylval.node->name, "NOT");
			return NOT;}
"(" {
	#ifdef debug 
	printf("LP\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->name = malloc(strlen("LP")+1);
			strcpy(yylval.node->name, "LP");
			return LP;}
")" {
	#ifdef debug 
	printf("RP\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("RP")+1);
			strcpy(yylval.node->name, "RP");
			return RP;}
"[" {
	#ifdef debug 
	printf("LB\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("LB")+1);
			strcpy(yylval.node->name, "LB");
			return LB;}
"]" {
	#ifdef debug 
	printf("RB\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("RB")+1);
			strcpy(yylval.node->name, "RB");
			return RB;}
"{" {
	#ifdef debug 
	printf("LC\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("LC")+1);
			strcpy(yylval.node->name, "LC");
			return LC;}
"}" {
	#ifdef debug 
	printf("RC\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("RC")+1);
			strcpy(yylval.node->name, "RC");
			return RC;}
{Blankspace} {printf("");}
"/" {
	#ifdef debug 
	printf("DIV\n"); 
	#endif
			yylval.node = (struct TreeNode *)malloc(sizeof(TreeNode));
			yylval.node->lineno = yylineno;
			yylval.node->type = 4;
			yylval.node->child = NULL;
			yylval.node->sibling = NULL;
			yylval.node->name = malloc(strlen("DIV")+1);
			strcpy(yylval.node->name, "DIV");
			return DIV;}
\n {yycolumn = 1; }
. { lexical_flag = 1; printf("Error type A at line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%