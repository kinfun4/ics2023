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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static int p;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
uint32_t choose(uint32_t n) {
	return rand()%n;
}

static void gen(char* str) {
	for(char* i=str;*i!='\0';i++)
	{
		buf[p++]=*i;
	}
	buf[p]='\0';
}

static void gen_num() {
	char str[32];
	uint32_t maxn=100;
	snprintf(str, sizeof(str),"%u", choose(maxn));
	gen(str);
}

static void gen_rand_op() {
	switch(choose(4)) {
		case 0: gen("+");break;
		case 1: gen("-");break;
		case 2: gen("*");break;
		case 3: gen("/");break;
		default: printf("Invalid value when gen_rand_op()!\n");assert(0);
	}
}

static void gen_rand_expr() {
	int k=choose(3);
	if(p>100)k=0;
	switch (k) {
		case 0: gen_num();break;
		case 1: gen("(");gen_rand_expr();gen(")");break;
		case 2: gen_rand_expr();gen_rand_op();gen_rand_expr();break;
		default: printf("Invalid value when gen_rand_expr()!\n");assert(0);
	}
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 10;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	buf[0]='\0';
	p=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);
	if(ret==EOF)continue;

    printf("%u %s\n", result, buf);
  }
  return 0;
}
