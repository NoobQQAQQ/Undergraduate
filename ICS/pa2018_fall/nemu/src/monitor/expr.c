#include "nemu.h"
#include "cpu/reg.h"
#include "cpu/cpu.h"
#include "memory/memory.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUM, REG, SYMB,
	SHR, SHL, NEQ, GE, LE, LA, LO, NEG, DEREF, REF, HEX

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// white space
	{"\\+", '+'},
	{"\\-", '-'}, // neg or sub
	{"\\*", '*'}, // multi or deref
	{"\\/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"\\[", '['},
	{"\\]", ']'},
	{">>", SHR},
	{"<<", SHL},
	{"%", '%'},

	{"==", EQ},
	{"!=", NEQ},
	{">=", GE},
	{"<=", LE},
	{">", '>'},
	{"<", '<'},
	{"!", '!'},
	{"\\^", '^'},
	{"~", '~'},
	{"&&", LA},
	{"\\|\\|", LO},
	{"&", '&'}, // ref or bitand
	{"\\|", '|'},
	{"0[xX][a-fA-F0-9]+", HEX},
	{"[0-9]+", NUM},
	{"\\$e[acdb]x", REG},
	{"\\$esp", REG},
	{"\\$esi", REG},
	{"\\$ebp", REG},
	{"\\$edi", REG},
	{"\\$eip", REG},
	{"[_a-zA-Z]+[_a-zA-Z0-9]*", SYMB},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//printf("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				if (rules[i].token_type == NOTYPE) // skip space
					break;

				if (substr_len < 32)
				{
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = 0;
				}
				else
				{
					printf("a token's length exceeds upper bound\n");
					return false;
				}

				switch(rules[i].token_type) {
					default: tokens[nr_token].type = rules[i].token_type;
							 nr_token ++;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}


int priority[32];

bool check_parentheses(int s, int e, bool *success)
{
	int delta = 0; // left - right
	// for the whole process, delta should always be nonnegative, at last delta should be 0

	// check legality
	for (int i = s; i <= e; i++)
	{
		if (tokens[i].type == '(')
			delta++;
		if (tokens[i].type == ')')
			delta--;
		
		if (delta < 0)
		{
			*success = false;
			return 0;
		}
	}
	if (delta != 0)
	{
		*success = false;
		return 0;
	}

	// check match
	*success = true;
	delta = 0;
	if ((tokens[s].type != '(') || (tokens[e].type != ')'))
		return 0;
	else
	{
		for (int j = s; j <= e; j++)
		{
			if (tokens[j].type == '(')
				delta++;
			if (tokens[j].type == ')')
				delta--;
			if (delta == 0)
			{
				if (j == e)
					return 1;
				else
					return 0;
			}
		}
	}
	return 0;
}

bool isOperator(int type)
{
	if (type == NUM || type == HEX || type == REG || type == SYMB)
		return false;
	return true;
}

bool isMono(int type)
{
	if (type == NEG || type == DEREF || type == REF || type == '~' || type == '!')
		return true;
	return false;
}

int getDoperator(int s, int e) // return index of dominant operator in tokens
{
	int help[32];
	// initialize help array
	for (int i = s; i <= e; i++)
		help[i] = priority[i];

	// check parentheses and modify priority of some tokens
	for (int i = s; i <= e; )
	{
		if (tokens[i].type == '(')
		{
			// find matched )
			int delta = 0;
			int j;
			for (j = i; j <= e; j++)
			{
				if (tokens[j].type == '(')
					delta++;
				if (tokens[j].type == ')')
					delta--;
				
				if (delta == 0) // find match
					break;
			}
			for (int k = i; k <= j; k++) //skip the operators surrounded by ()
				help[k] = -1;
			i = j + 1;
		}
		else
			i++;
	}
	for (int i = s; i <= e; i++)
		printf("%d ", help[i]);
	int maxp, maxp_index;
	maxp = maxp_index = 0;
	for (int i = s; i <= e; i++)
	{
		if (help[i] != -1) // isOperator == true && not surrounded by ()
		{
			if (help[i] >= maxp) // the smallest priority is 1
			{
				maxp = help[i]; maxp_index = i;
			}
		}
		// skip Num, hex, reg, symb
	}
	return maxp_index;
}


int htoi(char *str) // hex str to dec int
{
	int len = strlen(str);
	int sum = 0;
	int base = 1;
	for (int i = len - 1; i >= 2; i--)
	{
		int value = 0;

		if (isdigit(str[i]))
			value = str[i] - '0';
		else if (isupper(str[i]))
		{
			str[i] = tolower(str[i]);
			value = str[i] - 'a' + 10;
		}
		else if (islower(str[i]))
			value = str[i] - 'a' + 10;
		
		sum += value * base;
		base *= 16;
	}
	return sum;
}

extern uint32_t look_up_symtab(char *sym, bool *success);

uint32_t eval(int s, int e, bool *success)
{
	if (*success == true)
	{
		if (s > e)
		{
			*success = false;
			return 0;
		}
		else if (s == e)
		{
			if (isOperator(tokens[s].type))
			{
				*success = false;
				return 0;
			}
			else
			{
				switch(tokens[s].type)
				{
					case NUM:
						return atoi(tokens[s].str);
					case HEX:
						return htoi(tokens[s].str);
					case REG:
						if (!strcmp(tokens[s].str, "$eax")) 
							return cpu.eax;
						else if (!strcmp(tokens[s].str, "$ecx"))
							return cpu.ecx;
						else if (!strcmp(tokens[s].str, "$edx"))
							return cpu.edx;
						else if (!strcmp(tokens[s].str, "$ebx"))
							return cpu.ebx;
						else if (!strcmp(tokens[s].str, "$ebp"))
							return cpu.ebp;
						else if (!strcmp(tokens[s].str, "$esp"))
							return cpu.esp;
						else if (!strcmp(tokens[s].str, "$esi"))
							return cpu.esi;
						else if (!strcmp(tokens[s].str, "$edi"))
							return cpu.edi;
						else if (!strcmp(tokens[s].str, "$eip"))
							return cpu.eip;
					case SYMB:
						return look_up_symtab(tokens[s].str, success);
				}
			}
		}
		else if(check_parentheses(s, e, success))
		{
			return eval(s + 1, e - 1, success);
		}
		else
		{
			if (*success == false)
				return 0;
			int op = getDoperator(s, e);
			printf("dominant operator is %s, at %d\n", tokens[op].str, op);
			uint32_t val1;
			uint32_t val2;
			if (isMono(tokens[op].type)) // monocular operator
			{
				val1 = eval(op + 1, e, success);
				switch(tokens[op].type)
				{
					case NEG: return -val1;
					case DEREF: return vaddr_read(val1, SREG_DS, 4);
					//case REF: return (uint32_t)&val1;
					case '!': return !val1;
					case '~': return ~val1;
					default : assert(0);
				}
			}
			else 
			{
				val1 = eval(s, op - 1, success);
				val2 = eval(op + 1, e, success);
				printf("v1 = %d, v2 = %d\n", val1, val2);
				switch(tokens[op].type)
				{
					case '+': return val1 + val2;
					case '-': return val1 - val2;
					case '*': return val1 * val2;
					case '/': return val1 / val2;
					case '^': return val1 ^ val2;
					case '&': return val1 & val2;
					case '|': return val1 | val2;
					case '%': return val1 % val2;
					case '>': return val1 > val2;
					case '<': return val1 < val2;
					case SHR: return val1 >> (val2 % 32);
					case SHL: return val1 << (val2 % 32);
					case EQ: return val1 == val2;
					case NEQ: return val1 != val2;
					case GE: return val1 >= val2;
					case LE: return val1 <= val2;
					case LA: return val1 && val2;
					case LO: return val1 || val2;
					default : assert(0);
				}
			}
		}
	}
	else
		return 0;

	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}


	//printf("\nPlease implement expr at expr.c\n");
	//assert(0);

	//return 0;

	// initialize priority
	for (int i = 0; i < 32; i++)
		priority[i] = -1;
	// scan tokens to change priority and some types(such as -, *, &...)
	for (int i = 0; i < nr_token; i++)
	{
		if (isOperator(tokens[i].type))
		{
			switch (tokens[i].type)
			{
				case '+' : priority[i] = 4; break;
				case '-' : 
					if ((i == 0) || ( (isOperator(tokens[i - 1].type)) && (tokens[i - 1].type != ')') ) )
					{
						tokens[i].type = NEG; priority[i] = 2;
					}
					else
						priority[i] = 4;
					break;
				case '*' :
					if ((i == 0) || ( (isOperator(tokens[i - 1].type)) && (tokens[i - 1].type != ')') ) )
					{
						tokens[i].type = DEREF; priority[i] = 2;
					}
					else
						priority[i] = 3;
					break;
				case '/' : priority[i] = 3; break;
				case '(' : priority[i] = 1; break;
				case ')' : priority[i] = 1; break;
				case '[' : priority[i] = 1; break;
				case ']' : priority[i] = 1; break;
				case SHR : priority[i] = 5; break;
				case SHL : priority[i] = 5; break;
				case '%' : priority[i] = 3; break;
				case EQ : priority[i] = 7; break;
				case NEQ : priority[i] = 7; break;
				case GE : priority[i] = 6; break;
				case LE : priority[i] = 6; break;
				case '>' : priority[i] = 6; break;
				case '<' : priority[i] = 6; break;
				case '!' : priority[i] = 2; break;
				case '^' : priority[i] = 9; break;
				case '~' : priority[i] = 2; break;
				case LA : priority[i] = 11; break;
				case LO : priority[i] = 12; break;
				case '&' : 
					if ((i == 0) || ( (isOperator(tokens[i - 1].type)) && (tokens[i - 1].type != ')') ) )
					{
						tokens[i].type = REF; priority[i] = 2;
					}
					else
						priority[i] = 8;
					break;
				case '|' : priority[i] = 10; break;
				default : break;
			}
		}
	}
	// calculate expression
	*success = true;
	return eval(0, nr_token - 1, success);
}
