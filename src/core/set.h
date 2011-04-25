#ifndef windsoul_set_h
#define windsoul_set_h

struct set;

#define SET_EXIST 0
#define SET_PUSH 1
#define SET_POP 2
#define SET_PUSHONCE 3

struct set* setCreate(void);
void setRelease(struct set *);

/*
	SET_EXIST 
		return 0 : not existed
		return 1+ : reference count
	SET_PUSH
		return 0+ : reference count before push
	SET_POP
		return 0+ : reference count after pop
	SET_PUSHONCE : call push only when not exited
		return 0+ : reference count before push
 */
int setOperate(struct set *, void *, int op);

void* setTraverse(struct set *, void ** state);

#endif
