#include <string.h>
#include <malloc.h>
#include <tree.h>
//·ÇÆ½ºâÊ÷
T_Tree *Tree_Add( T_Tree *sp, void *content, int len,
	int (*Cmp_rec)(void *sp_content,void *content,int len),
	int (*user_add_tree)(T_Tree *sp,void *content,int len))
{
int rc;
    if(!sp){
	sp=(T_Tree *)malloc(sizeof(T_Tree));
	if(!sp){
	    return sp;
	}
	sp->Content=(void *)malloc(len+5);
	if(!sp->Content){
		free(sp);
		return 0;
	}
	memcpy(sp->Content,content,len);
	sp->Left=sp->Right=0;
	sp->Ldepth=sp->Rdepth=0;
    } else {
	rc=Cmp_rec(sp->Content,content,len);
	if(rc>0) {
		sp->Left=Tree_Add( sp->Left, content, len,
			Cmp_rec, user_add_tree);
		return sp;
	} else if(rc<0) {
		sp->Right=Tree_Add( sp->Right, content, len,
			Cmp_rec, user_add_tree);
		return sp;
	} else { 
	    if(user_add_tree){
	        rc=user_add_tree(sp,content,len);
	    }
	}
    }
    return sp;
}
