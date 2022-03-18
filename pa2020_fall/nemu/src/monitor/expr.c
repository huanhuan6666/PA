#include "nemu.h"
#include "cpu/reg.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "elf.h"
#include "trap.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256, //white space
	EQ,           //equal '=='
	NEQ,          //not equal '!='
	GE,           //>=
	LE,           //<=
	
	AND,          //&&
	OR,           //||
	RM,           //>>
	LM,           //<<
	
	NUM,          //a dec number
	HEX,          //a hex number
	REG,          //regesiter
	SYMB,          //a symbol name in symbol table
	
	NEG,           //tobe negative 
    DEREF          //jieyinyong
	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE},        // white space
	
	{"0[xX][0-9a-fA-F]+", HEX}, //hex number begin of '0X(x)'
	{"[0-9a-fA-F]+[Hh]", HEX},     //hex number end of 'H'
	{"[0-9]+", NUM},  //dec number
	
	{"\\$e[acdb]x", REG},       //reg eax ecx edx ebx
	{"\\$e[sbi]p", REG},         //reg esp ebp eip
	{"\\$e[sd]i", REG},         //reg esi edi
	{"[_a-zA-Z]+[0-9a-zA-Z_]*", SYMB}, //a symbol name
	
	{"\\+", '+'}, 
	{"\\-", '-'},
	{"\\*", '*'}, 
	{"/", '/'},
	{"\\(", '('},  
	{"\\)", ')'}, 
	
	{"==", EQ},            //==
	{"!=", NEQ},           //!=
	{">=", GE},            //>=
	{"<=", LE},            //<=
	{"&&", AND},           //&&
	{"\\|\\|", OR},        //||
	{"!", '!'},
	
	{"&", '&'},
	{"\\|", '|'},
	{"\\^", '^'},
	{"~", '~'},
	{">>", RM},            //>>
	{"<<", LM}             //<<
	
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				case NOTYPE: break;//white space jump directly
				case HEX://need to be a string
				case NUM:
				case REG:
				case SYMB:
				{
				    int j = 0;
				    for(j = 0; j < substr_len; j++)
				        tokens[nr_token].str[j] = *(substr_start + j);
				    tokens[nr_token].str[j] = '\0';//to be a string to store in tokens[]
				}
				default:
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)//no type of this token maybe a bad input
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

bool check_parentheses(int p, int q, bool *success)
{
    int counts = 0;
    if(tokens[p].type == '(' && tokens[q].type == ')')//must be in (...)
    {
        for(int i = p + 1; i < q; i++)
        {
            if(tokens[i].type == '(')
                counts++;
            else if(tokens[i].type == ')')
                counts--;
            if(counts < 0) 
            {
                *success = false;
                //printf("infoa :%d %d %d\n",p,q,*success);
                return false; //if ) is more than ( a bad input
            }
        }
        if(counts == 0) {
            //printf("infob :%d %d %d\n",p,q,*success);
            return true;
        }
    }
    //printf("infoc :%d %d %d\n",p,q,*success);
    return false;
}

bool if_operand(int i)
{
    if(tokens[i].type == NUM || tokens[i].type == HEX || tokens[i].type == REG || tokens[i].type == SYMB || tokens[i].type == NOTYPE
    || tokens[i].type == ')' || tokens[i].type == '(') return false;
    else
        return true;
}

int priority(int i)
{
    if(tokens[i].type == DEREF || tokens[i].type == NEG || tokens[i].type == '!' || tokens[i].type == '~')
        return 1;
    else if(tokens[i].type == '*' || tokens[i].type == '/')
        return 2;
    else if(tokens[i].type == '+' || tokens[i].type == '-')
        return 3;
    else if(tokens[i].type == LM || tokens[i].type == RM)
        return 4;
    else if(tokens[i].type == GE || tokens[i].type == LE || tokens[i].type == '>' || tokens[i].type == '<')
        return 5;
    else if(tokens[i].type == EQ || tokens[i].type == NEQ)
        return 6;
    else if(tokens[i].type == '&' || tokens[i].type == '|' || tokens[i].type == '^')
        return 7;
    else if(tokens[i].type == OR || tokens[i].type == AND)
        return 8;
    else return 9;
}

uint32_t look_up_symtab(char* sym, bool* success);

uint32_t eval(int p, int q, bool* success)//calculate the tokens array and return the res
{
    if(p > q)//a bad input
    {
        //printf("0 ");
        *success = false;
        return 0;
    }
    else if(p == q)//if is singal tokens
    {
        //printf("1 ");
        if(tokens[p].type == NUM)
        {
           uint32_t res = 0;
           sscanf(tokens[p].str, "%d", &res);
           return res;
        }
        else if(tokens[p].type == HEX)
        {
            uint32_t res = 0;
            sscanf(tokens[p].str, "%x", &res);
            return res;
        }
        else if(tokens[p].type == REG)
        {
            if(tokens[p].str[3] == 'x')//$e(acdb)x
            {
                if(tokens[p].str[2] == 'a')
                    return cpu.eax;
                else if(tokens[p].str[2] == 'c')
                    return cpu.ecx;
                else if(tokens[p].str[2] == 'd')
                    return cpu.edx;
                else if(tokens[p].str[2] == 'b')
                    return cpu.ebx;
            }
            else if(tokens[p].str[3] == 'p')//$e(sbi)p
            {
                if(tokens[p].str[2] == 's')
                    return cpu.esp;
                else if(tokens[p].str[2] == 'b')
                    return cpu.ebp;
                else if(tokens[p].str[2] == 'i')
                    return cpu.eip;
            }
            else if(tokens[p].str[3] == 'i')//$e(sd)i
            {
                if(tokens[p].str[2] == 's')
                    return cpu.esi;
                else if(tokens[p].str[2] == 'd')
                    return cpu.edi;
            }
            else return cpu.eip;
        }
        else if(tokens[p].type == SYMB)//a name of a symbol
            return look_up_symtab(tokens[p].str, success);
    }
    else if(check_parentheses(p, q, success))//if is (****)
    {
        //printf("2 ");
        return eval(p + 1, q - 1, success);
    }
    else//normal condition
    {
        //printf("3 ");
        int ope_position = -1; //the operand between
        int ope_priority = 0;
        int flags = 0;
        for(int i = p; i <= q; i++)
        {
            if(tokens[i].type == '(') flags++;
            else if(tokens[i].type == ')') flags--;
            if(flags == 0 && if_operand(i))//the op's position if it's a operand and must not in ()!
            {
                //printf("4 ");
                if(priority(i) >= ope_priority)//the tokens[i]'s priority is after now
                {
                    ope_position = i;
                    ope_priority = priority(i);
                    //printf("op is %d\n",ope_position);
                }
            }
        }
        
        uint32_t val1 = 0, val2 = 0;//two opcode
        int op_type = tokens[ope_position].type;
        
        if(op_type != DEREF && op_type != NEG && op_type != '!' && op_type != '~')//the - or * is at begin like -NUM
            val1 = eval(p, ope_position - 1, success);
        val2 = eval(ope_position + 1, q, success);
        
        switch(op_type)
        {
            //+ - * /
            case '+': return val1 + val2;break;
            case '-': return val1 - val2;break;
            case '*': return val1 * val2;break;
            case '/': return val1 / val2;break;
            //== != > < >= <= !
            case EQ: return val1 == val2;break;
            case NEQ: return val1 != val2;break;
            case GE:  return val1 >= val2;break;
            case LE: return val1 <= val2;break;
            case '!': return !val2;break;
            case AND: return val1 && val2; break;
            case OR: return val1 || val2; break;
            //| & ~ ^ >> <<
            case '~': return ~val2;break;
            case '&': return val1 & val2;break;
            case '^': return val1 ^ val2;break;
            case '|': return val1 | val2;break;
            case LM: return val1 << val2;break;
            case RM: return val1 >> val2;break;
            //* - plus
            case DEREF: return vaddr_read(val2, SREG_CS, 4);break;
            case NEG: return -val2;break;
            default: printf("opcode operands hit bad\n"); 
            assert(0);
        }
    }
    printf("opcode operands hit bad2\n");
    return 0;
}

bool if_certain(int i)
{
    if(tokens[i].type == NUM || tokens[i].type == HEX || tokens[i].type == REG || tokens[i].type == SYMB || tokens[i].type == NOTYPE
    || tokens[i].type == ')') return false;
    else return true;//after a operand or '(' is another meaning for '-' or '*'
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		//printf("5 ");
		return 0;
	}
	for(int i = 0; i < nr_token; i++)//make_token plus
	{
	    if(tokens[i].type == '*' && (i == 0 || if_certain(i - 1)))
	        tokens[i].type = DEREF;//now '*' is pointer jieyinyong
	    else if(tokens[i].type == '-' && (i == 0 || if_certain(i - 1)))
	        tokens[i].type = NEG;
	}
	return eval(0, nr_token - 1, success);
}
