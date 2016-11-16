// pl0_ll1_3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

# define STACK_INIT_SIZE 100 //堆栈最大长度 
# define STACKINCREMENT 10 //堆栈增加长度
#define LeadExp struct Lead_expression //产生式结构体 


int norw = 12;
int num;
const int lineLength = 82;  // 行缓冲区长度
char  line[lineLength];
int codeNo = 0;
bool sourceEnd = false;
const int al = 10; //标识符的长度
typedef char alfa[al + 1];  // alfa类型用于标识符
alfa  a;                         // 词法分析器中用于临时存放正在分析的词
int kk = al;
alfa  id;                        // 存放读入的词
								 //关键字、运算符及界限符表 
char ch; //读取的字符
int cc = 0, ll = 0; //字符的数量和行的长度
FILE *fin, *fin_yufa;
FILE *fout, *fout_yufa;
int sym;
char charTable[27][10] = { "var","procedure", "begin","end","read","write","call","while","do","if","then","else"
">",">=","<","<=","=","#",":=","+","-","*","/",",",";","(",")" };
int ssym[100];


//标识符的常量形式
enum KEY {
	ID = 1, VAR, PEOCEDURE, BEGIN, END, READ, WRITE, CALL, WHILE, DO, IF, THEN, ELSE, GT, GE,
	LSS, LE, EQU, NEQ, IS, ADD, MINUS, MUL, DIV, COM, SEMI, LP, RP, OVER
};

//初始化一些符号
//29个终结符
//在LL（1）文法中用到的非终结符的常量表示 
enum NTERMINAL { //26个非终结符
	Program = 100, Program1, Procedure, Proname, ProMain, VarStat, VariTable, VarStat1,
	Block, Statement, Statement1, ReadStat, WriteStat, AssignStat, IfStat,
	ElseStat, WhileStat, Exp, AssignExp1, Comop, AssignExp, AssignExp2, Term,
	Term1, Factor, CallStat
};
LeadExp //表示文法的数据结构
{ int left; //左部符号 
int length; //文法右部的长度 
int right[48][5]; //文法右部的字符序列 
};
LeadExp leadExp[48]; //分配48条文法的空间 
typedef struct
//堆栈结构体 
{
	int *base; int *top; int stacksize;
}Stack;
//分析堆栈; 
Stack s;
void InitStack(Stack &S) //初始化堆栈
{
	S.base = (int *)malloc(STACK_INIT_SIZE*sizeof(int));
	if (!S.base)
	{
		printf("分配空间失败!");
		exit(0);
	}
	S.top = S.base;
	S.stacksize = STACK_INIT_SIZE;
}
int GetTop(Stack S) //得到栈顶元素 
{
	int e; if (S.top == S.base)
	{
		printf("是个空栈!");
		exit(0);
	} e = *(S.top - 1); return e;
}

void Push_S(Stack &S, int e) //将元素压入堆栈
{
	if (S.top - S.base>S.stacksize) {
		S.base = (int *)realloc(S.base, (S.stacksize + STACKINCREMENT*sizeof(int)));
		if (!S.base)
		{
			printf("溢出!"); exit(0);
		}
		S.top = S.base + S.stacksize; S.stacksize += STACKINCREMENT;
	}
	*S.top++ = e;
} 
void Pop_S(Stack &S) //退栈
{
	int e; if (S.top == S.base)
	{
		printf("It's a empty Stack!"); exit(0);
	}
	e = *--S.top;
}

void PushLeadExp(int a)
{
	for (int i = 4; i >= 0; i--)
	{
		if (leadExp[a].right[a][i] != 0)
			Push_S(s, leadExp[a].right[a][i]);
	}
}
////////////初始化文法,将之存入文法表中 
void InitSyntax() { //25个产生式的右部 
	int right[48][5] = { { Procedure, Program1, 0, 0, 0 },
	{ Program,0,0,0,0 },
	{ 0, 0, 0, 0, 0 },
	{ PEOCEDURE,Proname,SEMI,ProMain,0 },
	{ ID, 0, 0, 0, 0 },
	{ VarStat, Block,0,0,0 },
	{ VAR,VariTable,SEMI,0,0 },
	{ 0,0,0,0,0 },
	{ ID, VarStat1,0,0,0 },
	{ COM,VariTable,0,0,0 },
	{ 0, 0, 0, 0, 0 },
	{ BEGIN,Statement , END, 0, 0 },
	{ Statement1,Statement,0,0,0 },
	{ 0, 0, 0, 0, 0 },
	{ ReadStat,0, 0, 0, 0 },
	{ WriteStat,0, 0, 0, 0 },
	{ AssignStat, 0, 0, 0, 0 },
	{ IfStat, 0, 0, 0, 0 },
	{ WhileStat, 0, 0, 0, 0 },
	{ CallStat, 0, 0, 0, 0 },
	{ READ,LP,ID,RP,0 },
	{ WRITE,LP,Exp,RP,0 },
	{ ID,IS,Exp,0,0 },
	{ IF, Exp, THEN,Statement, ElseStat },
	{ ELSE, Statement, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ WHILE, Exp,DO,Statement,0 },
	{ AssignExp,AssignExp1, 0, 0, 0 },
	{ Comop, AssignExp, 0, 0, 0 },
	{ 0, 0, 0, 0 ,0 },
	{ LSS, 0, 0, 0, 0 },
	{ LE, 0, 0, 0, 0 },
	{ GT, 0, 0, 0, 0 },
	{ GE, 0, 0, 0, 0 },
	{ EQU, 0, 0, 0, 0 },
	{ NEQ, 0, 0, 0, 0 },
	{ Term, AssignExp2, 0, 0, 0 },
	{ MINUS, Term, AssignExp2, 0, 0 },
	{ ADD, Term, AssignExp2, 0 ,0 },
	{ MINUS, Term, AssignExp2, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ Factor, Term1, 0, 0, 0 },
	{ MUL, Factor, Term1, 0, 0 },
	{ DIV, Factor, Term1, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ LP, Exp, RP },
	{ ID, 0, 0, 0, 0 },
	{ CALL, ID, 0, 0, 0 } };
	//产生式的左部 
	int left[48] =
	{ Program,Program1,Program1,Procedure,Proname,ProMain,VarStat,VarStat,VariTable,VarStat1,VarStat1,Block,Statement,Statement,
		Statement1,Statement1,Statement1,Statement1,Statement1,Statement1,ReadStat,WriteStat,AssignStat,IfStat,ElseStat,ElseStat,WhileStat,
		Exp,AssignExp1,AssignExp1,Comop,Comop,Comop,Comop,Comop,Comop,AssignExp,AssignExp,AssignExp2,AssignExp2,AssignExp2,Term,Term1,
		Term1,Term1,Factor,Factor,CallStat };
	//产生式的左部长度 
	int length[48] = { 2,1,0,4, 1,2,3,0,2,2,0,3,2,0,1,1,1,1,1,1,3,3,3,5,2,0,4,2,2,0,1,1,1,1,1,1,2,3,3,3,0,
		2,3,3,0,3,1,2 };
	for (int i = 0; i < 48; i++) //文法的赋值, 由左部符号,右部长度和右部符号序列构成 
	{
		leadExp[i].left = left[i];
		leadExp[i].length = length[i];
		for (int j = 0; j < 5; j++)
		{
			leadExp[i].right[i][j] = right[i][j];
		}
	}
}

void getch()
{
	if (cc == ll)
	{
		if (feof(fin))
		{
			sourceEnd = true;
			return;
		}
		cc = 0;
		fgets(line, lineLength, fin);
		codeNo++;
		ll = strlen(line);
		if (line[ll - 1] == 10) ll--;
	}
	ch = line[cc];
	cc = cc + 1;
}


void getWord()
{
	if (sourceEnd)
		return;
	int i, j, k;
	while (ch == ' ' || ch == 9)
		getch();    
	if (isalpha(ch))
	{
		k = 0;
		memset(a, 0, al + 1);
		// 检测一个单词长度
		do {
			if (k < al)
			{
				a[k] = ch;
				k = k + 1;
			}
			getch();
			if (sourceEnd)
				return;
		} while (isalpha(ch) || isdigit(ch));
		if (k >= kk)
			kk = k;
		else
		{
			do {
				a[kk] = ' ';
				kk = kk - 1;
			} while (kk > k);
		}
		strcpy_s(id, a);
		//fprintf(fout, "%s\n",id);
		i = 1;
		j = norw;
		for (int i = 0; i < 12; i++)
		{
			if (strcmp(charTable[i], id) == 0)
			{
				sym = i + 2;
				break;
			}
			else
			{
				sym = 1;
			}
		}

	}
	else if (isdigit(ch))  
	{
		k = 0;
		num = 0;
		//sym = NUMBER;
		do {
			num = 10 * num + ch - '0';
			k = k + 1;
			getch();
		} while (isdigit(ch));
		//fprintf(fout, "%d\n",num);
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			//fprintf(fout, "%s\n", ":=");
			sym = IS;
			getch();
		}
		else
		{
			//fprintf(fout, "%s\n", "=");
			//sym = NUL;
		}
	}
	else if (ch == '<')  
	{
		getch();
		if (ch == '=')
		{
			//fprintf(fout, "%s\n", "<=");
			sym = LE;
			getch();
		}
		else
		{
			//cout << "<" << endl;
			//fprintf(fout, "%s\n", "<");
			sym = LSS;
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			//cout << ">=" << endl;
			//fprintf(fout, "%s\n", ">=");
			sym = GE;
			getch();
		}
		else
		{
			//fprintf(fout, "%s\n", ">");
			sym = GT;
		}
	}
	else                // end of extra stuff
	{
		//cout << ch << endl;
		//fprintf(fout, "%s\n", ch);
		sym = ssym[ch];  // 其它符号的赋值
		getch();
	}
}
void getSymWrite()
{
	if (sourceEnd)
		return;
	int i, j, k;
	while (ch == ' ' || ch == 9)
		getch();    
	if (isalpha(ch)) 
	{
		k = 0;
		memset(a, 0, al + 1);
		// 检测一个单词长度
		do {
			if (k < al)
			{
				a[k] = ch;
				k = k + 1;
			}
			getch();
			if (sourceEnd)
				return;
		} while (isalpha(ch) || isdigit(ch));
		if (k >= kk)
			kk = k;
		else
		{
			do {
				a[kk] = ' ';
				kk = kk - 1;
			} while (kk > k);
		}
		strcpy_s(id, a);
		fprintf(fout, "%s\n", id);
		i = 1;
		j = norw;
		for (int i = 0; i < 12; i++)
		{
			if (strcmp(charTable[i], id) == 0)
			{
				sym = i + 2;
				break;
			}
			else
			{
				sym = 1;
			}
		}

	}
	else if (isdigit(ch))  // number
	{
		k = 0;
		num = 0;
		do {
			num = 10 * num + ch - '0';
			k = k + 1;
			getch();
		} while (isdigit(ch));
		fprintf(fout, "%d\n", num);
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			fprintf(fout, "%s\n", ":=");
			sym = IS;
			getch();
		}
		else
		{
			fprintf(fout, "%s\n", "=");
			//sym = NUL;
		}
	}
	else if (ch == '<')  
	{
		getch();
		if (ch == '=')
		{
			fprintf(fout, "%s\n", "<=");
			sym = LE;
			getch();
		}
		else
		{
			//cout << "<" << endl;
			fprintf(fout, "%s\n", "<");
			sym = LSS;
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			//cout << ">=" << endl;
			fprintf(fout, "%s\n", ">=");
			sym = GE;
			getch();
		}
		else
		{
			fprintf(fout, "%s\n", ">");
			sym = GT;
		}
	}
	else                
	{
		//cout << ch << endl;
		fprintf(fout, "%c\n", ch);
		sym = ssym[ch];  // 其它符号的赋值
		getch();
	}
}
void produce_wenfa()
{
	while (1)
	{
		if (sourceEnd)
			return;
		getSymWrite();
	}
}
//语法分析的过程 
void YuFaAnalyze() {


	InitStack(s); //获得词法分析的结果
	int LL1Table[26][29] = {
		{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//Program
		{ 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },//Program1
		{ 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },//Procedure
		{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 7, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 0, 0, 0 },
		{ 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 13, 0, 0, 0, 14, 13, 13, 13, 13, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 17, 0, 0, 0, 0, 15, 16, 20, 19, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 26, 0, 0, 0, 26, 26, 26, 26, 26, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 28, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 34, 31, 32, 35, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 0, 0 },
		{ 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 41, 0, 41, 41, 41, 41, 41, 41, 0, 39, 40, 0, 0, 41, 41, 0, 0, 0 },
		{ 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 37, 38, 39, 40, 41, 0, 0, 0, 42, 0, 0, 0 },
		{ 0 ,0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 45, 0, 45, 45, 45, 45, 45, 45, 0, 45, 45, 43, 44, 0, 45, 0, 0, 0 },
		{ 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 0, 0, 0 },
		{ 0 ,0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

	Push_S(s, OVER);
	Push_S(s, Program); //将识别符压入栈中
	while (true) //单词没有分析完
	{
		if (sourceEnd)
		{
			printf("匹配完成\n");
			return;
		}
			
		int type = sym; //取得单词的类别 
		int top = GetTop(s); //取得栈顶元素
		if (1 <= top && top <= 29) //top是终结符
		{
			if (top == type) //匹配
			{
				if (top == OVER) //分析成功,退出
				{
					break;
					//exit(0); 
				}
				else //没有分析完 
				{
					Pop_S(s); //弹出栈顶
					fprintf(fout_yufa,"匹配%d\n成功", type);
					getWord();
					//分析下一个单词 
				}
			}
			else {
				//匹配出错，退出
				fprintf(fout_yufa, "匹配出错，退出");
				break;
				//exit(0); 
			}
		}
		else //栈顶不是终结符
		{ //LL1分析表中对应的产生式编号
			int lead = LL1Table[top - 100][type - 1];
			if (lead != 0) {
				Pop_S(s); //弹出栈顶元素
				PushLeadExp(lead - 1); //逆序压入产生式右部
			}
			else //出错,退出 
			{
				break; //exit(0);
			}
		}
	}
	//fclose(fpout1);
}

int main() {
	//初始化一些符号，便于词法分析使用
	ssym['>'] = GT;
	ssym['<'] = LSS;
	ssym['='] = EQU;
	ssym['#'] = NEQ;
	ssym['+'] = ADD;
	ssym['-'] = MINUS;
	ssym['*'] = MUL;
	ssym['/'] = DIV;
	ssym[','] = COM;
	ssym[';'] = SEMI;
	ssym['('] = LP;
	ssym[')'] = RP;
	//fopen_s(&fin,"testPas1.txt", "r");
	//fopen_s(&fout,"result.txt", "w");
	//输出文法结果
	fin = fopen("testPas1.txt", "r");
	fout = fopen("result_wenfa.txt", "w");
	//getch();
	getWord();
	produce_wenfa();
	fclose(fin);
	fclose(fout);
	sourceEnd = false;
	//输出语法结果
	fin_yufa = fopen("testPas1.txt", "r");
	fout_yufa = fopen("result_yufa.txt", "w");
	
	//fclose(fin);
	//fclose(fout);
	getch();
	getWord();
	InitSyntax();
	YuFaAnalyze();
	fclose(fout_yufa);
	fclose(fin_yufa);
	system("pause");
	return 0;
}

