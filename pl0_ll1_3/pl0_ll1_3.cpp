// pl0_ll1_3.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

# define STACK_INIT_SIZE 100 //��ջ��󳤶� 
# define STACKINCREMENT 10 //��ջ���ӳ���
#define LeadExp struct Lead_expression //����ʽ�ṹ�� 


int norw = 12;
int num;
const int lineLength = 82;  // �л���������
char  line[lineLength];
int codeNo = 0;
bool sourceEnd = false;
const int al = 10; //��ʶ���ĳ���
typedef char alfa[al + 1];  // alfa�������ڱ�ʶ��
alfa  a;                         // �ʷ���������������ʱ������ڷ����Ĵ�
int kk = al;
alfa  id;                        // ��Ŷ���Ĵ�
								 //�ؼ��֡�����������޷��� 
char ch; //��ȡ���ַ�
int cc = 0, ll = 0; //�ַ����������еĳ���
FILE *fin, *fin_yufa;
FILE *fout, *fout_yufa;
int sym;
char charTable[27][10] = { "var","procedure", "begin","end","read","write","call","while","do","if","then","else"
">",">=","<","<=","=","#",":=","+","-","*","/",",",";","(",")" };
int ssym[100];


//��ʶ���ĳ�����ʽ
enum KEY {
	ID = 1, VAR, PEOCEDURE, BEGIN, END, READ, WRITE, CALL, WHILE, DO, IF, THEN, ELSE, GT, GE,
	LSS, LE, EQU, NEQ, IS, ADD, MINUS, MUL, DIV, COM, SEMI, LP, RP, OVER
};

//��ʼ��һЩ����
//29���ս��
//��LL��1���ķ����õ��ķ��ս���ĳ�����ʾ 
enum NTERMINAL { //26�����ս��
	Program = 100, Program1, Procedure, Proname, ProMain, VarStat, VariTable, VarStat1,
	Block, Statement, Statement1, ReadStat, WriteStat, AssignStat, IfStat,
	ElseStat, WhileStat, Exp, AssignExp1, Comop, AssignExp, AssignExp2, Term,
	Term1, Factor, CallStat
};
LeadExp //��ʾ�ķ������ݽṹ
{ int left; //�󲿷��� 
int length; //�ķ��Ҳ��ĳ��� 
int right[48][5]; //�ķ��Ҳ����ַ����� 
};
LeadExp leadExp[48]; //����48���ķ��Ŀռ� 
typedef struct
//��ջ�ṹ�� 
{
	int *base; int *top; int stacksize;
}Stack;
//������ջ; 
Stack s;
void InitStack(Stack &S) //��ʼ����ջ
{
	S.base = (int *)malloc(STACK_INIT_SIZE*sizeof(int));
	if (!S.base)
	{
		printf("����ռ�ʧ��!");
		exit(0);
	}
	S.top = S.base;
	S.stacksize = STACK_INIT_SIZE;
}
int GetTop(Stack S) //�õ�ջ��Ԫ�� 
{
	int e; if (S.top == S.base)
	{
		printf("�Ǹ���ջ!");
		exit(0);
	} e = *(S.top - 1); return e;
}

void Push_S(Stack &S, int e) //��Ԫ��ѹ���ջ
{
	if (S.top - S.base>S.stacksize) {
		S.base = (int *)realloc(S.base, (S.stacksize + STACKINCREMENT*sizeof(int)));
		if (!S.base)
		{
			printf("���!"); exit(0);
		}
		S.top = S.base + S.stacksize; S.stacksize += STACKINCREMENT;
	}
	*S.top++ = e;
} 
void Pop_S(Stack &S) //��ջ
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
////////////��ʼ���ķ�,��֮�����ķ����� 
void InitSyntax() { //25������ʽ���Ҳ� 
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
	//����ʽ���� 
	int left[48] =
	{ Program,Program1,Program1,Procedure,Proname,ProMain,VarStat,VarStat,VariTable,VarStat1,VarStat1,Block,Statement,Statement,
		Statement1,Statement1,Statement1,Statement1,Statement1,Statement1,ReadStat,WriteStat,AssignStat,IfStat,ElseStat,ElseStat,WhileStat,
		Exp,AssignExp1,AssignExp1,Comop,Comop,Comop,Comop,Comop,Comop,AssignExp,AssignExp,AssignExp2,AssignExp2,AssignExp2,Term,Term1,
		Term1,Term1,Factor,Factor,CallStat };
	//����ʽ���󲿳��� 
	int length[48] = { 2,1,0,4, 1,2,3,0,2,2,0,3,2,0,1,1,1,1,1,1,3,3,3,5,2,0,4,2,2,0,1,1,1,1,1,1,2,3,3,3,0,
		2,3,3,0,3,1,2 };
	for (int i = 0; i < 48; i++) //�ķ��ĸ�ֵ, ���󲿷���,�Ҳ����Ⱥ��Ҳ��������й��� 
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
		// ���һ�����ʳ���
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
		sym = ssym[ch];  // �������ŵĸ�ֵ
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
		// ���һ�����ʳ���
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
		sym = ssym[ch];  // �������ŵĸ�ֵ
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
//�﷨�����Ĺ��� 
void YuFaAnalyze() {


	InitStack(s); //��ôʷ������Ľ��
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
	Push_S(s, Program); //��ʶ���ѹ��ջ��
	while (true) //����û�з�����
	{
		if (sourceEnd)
		{
			printf("ƥ�����\n");
			return;
		}
			
		int type = sym; //ȡ�õ��ʵ���� 
		int top = GetTop(s); //ȡ��ջ��Ԫ��
		if (1 <= top && top <= 29) //top���ս��
		{
			if (top == type) //ƥ��
			{
				if (top == OVER) //�����ɹ�,�˳�
				{
					break;
					//exit(0); 
				}
				else //û�з����� 
				{
					Pop_S(s); //����ջ��
					fprintf(fout_yufa,"ƥ��%d\n�ɹ�", type);
					getWord();
					//������һ������ 
				}
			}
			else {
				//ƥ������˳�
				fprintf(fout_yufa, "ƥ������˳�");
				break;
				//exit(0); 
			}
		}
		else //ջ�������ս��
		{ //LL1�������ж�Ӧ�Ĳ���ʽ���
			int lead = LL1Table[top - 100][type - 1];
			if (lead != 0) {
				Pop_S(s); //����ջ��Ԫ��
				PushLeadExp(lead - 1); //����ѹ�����ʽ�Ҳ�
			}
			else //����,�˳� 
			{
				break; //exit(0);
			}
		}
	}
	//fclose(fpout1);
}

int main() {
	//��ʼ��һЩ���ţ����ڴʷ�����ʹ��
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
	//����ķ����
	fin = fopen("testPas1.txt", "r");
	fout = fopen("result_wenfa.txt", "w");
	//getch();
	getWord();
	produce_wenfa();
	fclose(fin);
	fclose(fout);
	sourceEnd = false;
	//����﷨���
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

