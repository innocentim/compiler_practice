#include <stdlib.h>

#define __list_node(T) _list_node_##T
#define _list_node(T) __list_node(T)
#define _list(T) list_##T
#define list(T) _list(T)
#define __insert(T) _insert_##T
#define _insert(T) __insert(T)
#define __remove(T) _remove_##T
#define _remove(T) __remove(T)
#define _list_init(T) list_init_##T
#define list_init(T) _list_init(T)
#define _list_push_front(T) list_push_front_##T
#define list_push_front(T) _list_push_front(T)
#define _list_pop_front(T) list_pop_front##T
#define list_pop_front(T) _list_pop_front(T)
#define _list_push_back(T) list_push_back_##T
#define list_push_back(T) _list_push_back(T)
#define _list_pop_back(T) list_pop_back_##T
#define list_pop_back(T) _list_pop_back(T)
#define _list_destory(T) list_destory_##T
#define list_destory(T) _list_destory(T)

struct _list_node(TEMPLATE) {
	TEMPLATE data;
	struct _list_node(TEMPLATE) * next;
	struct _list_node(TEMPLATE) * prev;
};

struct list(TEMPLATE) {
	struct _list_node(TEMPLATE) head[1];
};

void _insert(TEMPLATE)(struct list(TEMPLATE) * this, TEMPLATE _data, struct _list_node(TEMPLATE) * now) {
	struct _list_node(TEMPLATE) * new_node = malloc(sizeof(struct _list_node(TEMPLATE)));
	new_node->data = _data;
	new_node->next = now;
	new_node->prev = now->prev;
	new_node->prev->next = new_node;
	new_node->next->prev = new_node;
};

TEMPLATE _remove(TEMPLATE)(struct list(TEMPLATE) * this, struct _list_node(TEMPLATE) * deleted) {
	TEMPLATE ret = this->head->next->data;
	if (deleted != this->head) {
		deleted->prev->next = deleted->next;
		deleted->next->prev = deleted->next;
		free(deleted);
	}
	return ret;
};

void list_init(TEMPLATE)(struct list(TEMPLATE) * this) {
	this->head->next = this->head;
	this->head->prev = this->head;
};

void list_push_front(TEMPLATE)(struct list(TEMPLATE) * this, TEMPLATE _data) {
	_insert(TEMPLATE)(this, _data, this->head);
};

TEMPLATE list_pop_front(TEMPLATE)(struct list(TEMPLATE) * this) {
	return _remove(TEMPLATE)(this, this->head->next);
};

void list_push_back(TEMPLATE)(struct list(TEMPLATE) * this, TEMPLATE _data) {
	_insert(TEMPLATE)(this, _data, this->head->prev);
};

TEMPLATE list_pop_back(TEMPLATE)(struct list(TEMPLATE) * this) {
	return _remove(TEMPLATE)(this, this->head->prev);
};

void list_destory(TEMPLATE)(struct list(TEMPLATE) * this) {
	struct _list_node(TEMPLATE) * temp;
	for (temp = this->head->next; temp != this->head;) {
		struct _list_node(TEMPLATE) * next = temp->next;
		free(temp);
		temp = next;
	}
};

#undef __list_node
#undef _list_node
#undef _list
#undef list
#undef __insert
#undef _insert
#undef __remove
#undef _remove
#undef _list_init
#undef list_init
#undef _list_push_front
#undef list_push_front
#undef _list_pop_front
#undef list_pop_front
#undef _list_push_back
#undef list_push_back
#undef _list_pop_back
#undef list_pop_back
#undef _list_destory
#undef list_destory
