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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int id;
	char* str;
	uint32_t rec;
	struct watchpoint *prev;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].id = i;
		wp_pool[i].prev = (i == 0 ? NULL : &wp_pool[i - 1]);
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP* ret;
	if(free_ == NULL)ret=NULL;
	else {
		ret=free_;
		free_=free_->next;
		if(free_!=NULL)free_->prev=NULL;
		if(head==NULL){
			ret->next=NULL;
		}
		else {
			head->prev=ret;
			ret->next=head;
		}
		ret->prev=NULL;
		head=ret;
	}
	return ret;
}

void free_wp(WP* wp) {
	for(WP* i=head;i!=NULL;i=i->next)
	{
		if(i==wp){
			if(i->prev!=NULL)i->prev->next=i->next;
			if(i->next!=NULL)i->next->prev=i->prev;
			if(head==i)head=i->next;
			if(free_==NULL)
			{
				i->next=NULL;
			}
			else {
				i->next=free_;
				free_->prev=i;
			}
			free_=i;
			i->prev=NULL;
			return;
		}
	}
	Log("No watchpoint with id=%d!\n",wp->id);
}

void add_wp(char* args){
	WP* wp=new_wp();	
	if(wp==NULL){
		Log("There is no more watchpoints!");
		return;
	}
	sscanf(args,"%s",wp->str);
	bool suc;
	wp->rec=expr(args,&suc);
	if(!suc){
		Log("Invalid expressions when use add_wp(char* args)!\n *args=%s\n",args);
	}
}

void delete_wp(int id){
	WP* wp=&wp_pool[id];
	wp->rec=0;
	wp->str=NULL;
	free_wp(wp);
}

bool check_wp(){
	bool suc,ret=false;
	for(WP* i=head;i!=NULL;i=i->next){
		uint32_t ans=expr(i->str,&suc);
		if(ans!=i->rec){
			ret=true;
			Log("The watchpoint %d (expression=%s) have changed, the formal value is %u, the present value is %u\n",i->id, i->str, i->rec, ans);
		}
		if(!suc){
			Log("Can not calculate the expressions %s\n!",i->str);
		}
	}
	return ret;
}

void list_wp(){
	if(head==NULL){
		Log("There is no watchpoint.\n");
	}
	for(WP* i=head;i!=NULL;i=i->next){
		printf("id=%-10d expressions=%-20s value=%-10u\n",i->id,i->str,i->rec);
	}
}
