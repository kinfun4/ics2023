/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 0 ,
  TK_NUM,
  TK_EQ,
  TK_PLU,
  TK_MIN,
  TK_MUL,
  TK_DIV,
  TK_MOD, 
  TK_LEF, 
  TK_RIG,
	TK_HEX,
	TK_REG,
	TK_AND,
	TK_NEQ,
	TK_UCI,
	TK_SUB,
  /* TODO: Add more token types */

};

enum {
	P_NOTYPE = 0, 
	P_AND = 0,
	P_EQ=1, P_NEQ=1,
	P_PLU=2, P_MIN=2, 
	P_MOD=3, 
	P_MUL=4, P_DIV=4, 
	P_UCI=5, P_SUB=5,
	P_NUM=6, P_HEX=6, P_REG=6,
	P_LEF=7, P_RIG=7,

};

bool *suc;
static struct rule {
  const char *regex;
  int token_type;
  int token_priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE, P_NOTYPE},    // spaces
  {"\\+", TK_PLU, P_PLU},         // plus
  {"-", TK_MIN, P_MIN},							  // minus
  {"==", TK_EQ, P_EQ},        // equal
  {"\\*", TK_MUL, P_MUL},						  // multiply
  {"/", TK_DIV, P_DIV},			// divide 
  {"%", TK_MOD, P_MOD},						  //mod
	{"0x[0-9|a-e]+", TK_HEX, P_HEX},		 //hex-number
	{"\\$[0-9|a-z|\\$]+", TK_REG, P_REG},		// reg name
	{"!=", TK_NEQ, P_NEQ},  // no euqal
	{"&&", TK_AND, P_AND}, // and
  {"\\(", TK_LEF, P_LEF}, // "("
  {"\\)", TK_RIG, P_RIG}, // ")"
  {"[0-9]+", TK_NUM, P_NUM},  //number
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char* str;
  int priority;
  word_t num;
} Token;

static Token tokens[100] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token (char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] !=  '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i  ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a n ew token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		if(nr_token>=100)panic("Too many tokens!");

		if(rules[i].token_type!=0){
			tokens[nr_token].type=rules[i].token_type;
			tokens[nr_token].priority=rules[i].token_priority;
			tokens[nr_token].str=substr_start;
		}

        switch (rules[i].token_type) {
					case TK_NOTYPE:break;											//space
					case TK_NUM:sscanf(substr_start,"%u",&tokens[nr_token].num);  //number 
						   nr_token++;
							 break;
					case TK_HEX:sscanf(substr_start,"0x%x",&tokens[nr_token].num); // hex-number
							 nr_token++;
							 break;
					case TK_REG:tokens[nr_token].num=isa_reg_str2val(substr_start+1,suc);
							 nr_token++;
							 break;
					default:nr_token++;break;
        }
		break;
      }
    }

    if (i == NR_REGEX) { 
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int q){
	int cnt = 0;
	for(int i=p;i<=q;i++)
	{
		if(tokens[i].type==TK_LEF)cnt++;
		if(tokens[i].type==TK_RIG)cnt--;
		if(cnt<0)return false;
	}
	if(cnt!=0)return false;
	return true;
}

bool check_unary_op(int p) {
	if(tokens[p].type==TK_MIN||tokens[p].type==TK_MUL){
		if(p==0||(tokens[p-1].type!=TK_NUM && tokens[p-1].type!=TK_HEX && tokens[p-1].type!=TK_REG && tokens[p-1].type != TK_RIG)){
				return true;
		}
	}
	return false;
}

static word_t get_addr(word_t addr){
	return isa_mmu_translate(addr,4,0);
}

word_t eval(int p,int q)
{
	if(p>q){ 
		Log("Invalid expression because p>q where p=%d, q=%d \n",p,q);
		*suc=false;
		return 0;
	}
	else if(p==q){ 
		if(tokens[p].type!=TK_NUM && tokens[p].type!=TK_HEX && tokens[p].type!=TK_REG){
			Log("Invalid expression at position %d, string = %s \n" ,p, tokens[p].str);
			*suc=false;
			return 0;
		}
		return tokens[p].num;
	}
	else if(check_parentheses(p,q)==false){
		Log("check_parentheses(p,q)	is false, p = %d, q = %d\n",p,q);
		*suc=false;
		return 0;
	}
	else if(tokens[p].type== TK_LEF && tokens[q].type==TK_RIG)	{
		return eval(p+1,q-1);
	}
	else{
		int cnt=0,pri=1e7;
		int op=0;
		for(int i=p;i<=q;i++)
		{
			if((pri>=tokens[i].priority+cnt*P_RIG && tokens[i].priority<P_SUB) || pri>tokens[i].priority+cnt*P_RIG ){
				op=i;
				pri=tokens[i].priority+cnt*P_RIG;
			}
			if(tokens[i].type==TK_LEF){
				cnt++;
			}
			if(tokens[i].type==TK_RIG){
				cnt--;
			}
		}
		Log("op=%d\n",op);
		word_t val1=0,val2=0;
		if(op!=p)val1=eval(p,op-1);
		if(op!=q)val2=eval(op+1,q);
		switch (tokens[op].type) {
			case TK_PLU: return val1+val2;break;
			case TK_MIN: return val1-val2;break;
			case TK_MUL: return val1*val2;break;
			case TK_DIV: return val1/val2;break;
			case TK_MOD: return val1%val2;break;
			case TK_EQ: return val1==val2;break;
			case TK_NEQ: return val1!=val2;break; 
			case TK_AND: return val1 && val2; break;
			case TK_SUB: return -val2;break;
			case TK_UCI: return get_addr(val2);break; 
			default: Log("Invalid expression at position %d,string = %s\n",op, tokens[op].str);*suc=false; return 0;
		}
	}
	return 0;
}

word_t expr(char *e, bool *success) {
	suc=success;
	*suc=true;
  if (!make_token(e)) {
    *suc = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
	for(int i=0;i<nr_token;i++)
	{
		if(check_unary_op(i)){
			if(tokens[i].type==TK_MIN){
				tokens[i].type=TK_SUB;
				tokens[i].priority=P_SUB;
			}
			if(tokens[i].type==TK_MUL){
				tokens[i].type=TK_UCI;
				tokens[i].priority=P_UCI;
			}
		}
	}
  Log("nr_token = %d\n",nr_token);
  return eval(0,nr_token-1);
}
