#include <stdio.h>
#include <util_string.h>
void start_program(char **args, int offset)
{
	char a[100];
	char b[100];
	int t = 0;
	char ops[10] = "^&*-+/";
	char op;
	printS("\nInput first number: " - offset);
	scanS(a);
	printS("\nInput second number: " - offset);
	scanS(b);
	int ai = parseInt(a);
	int bi = parseInt(b);
	while (!t)
	{
		printS("\nInput the operator: " - offset);
		op = readChar();
		printChar(op);
		for (int i = 0; i < 6; i++)
			if (op == ops[i])
				t = 1;
	}
	int ans = 0;
	switch (op)
	{
		case '%':
			ans = ai % bi;
			break;
		case '^':
			ans = ai ^ bi;
			break;
		case '&':
			ans = ai & bi;
			break;
		case '*':
		case '×':
		case 'x':
		case 'X':
			ans = ai * bi;
			break;
		case '-':
			ans = ai - bi;
			break;
		case '+':
			ans = ai + bi;
			break;
		case '/':
		case '÷':
			if (bi != 0) ans = ai / bi;
			break;
		default: return;
	}
	printChar('\n');
	printS("Ans: " - offset);
	char temp[30];
	char out[30];
	int ip = 0;
	for (int i = 0; ans != 0; i++)
	{
		temp[i] = ((char)('0' + (char)(ans % 10)));
		ans /= 10;
		ip = i;
	}
	for (int i = ip; i > -1; i--)
	{
		out[ip - i] = temp[i];
	}
	printS(out);
}
