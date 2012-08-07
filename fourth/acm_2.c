#include <stdio.h>
#include <stdlib.h>

enum traverse_style {
	PRE,
	IN,
	POST,
};

struct Tree {
	char name;
	struct Tree * l;
	struct Tree * r;
};

void create(struct Tree ** root) {
	char ch;
	ch = getchar();
	while (ch == ' ' && ch != EOF) {
		ch = getchar();
	}
	if (ch == EOF) {
		return;
	}
	if (ch == '#') {
		*root = NULL;
	} else {
		*root = malloc(sizeof(struct Tree));
		(*root)->name = ch;
		create(&(*root)->l);
		create(&(*root)->r);
	}
};

void trav(struct Tree * root, int method) {
	if (root == NULL) {
		return;
	}
	if (method == PRE) {
		printf(" %c", root->name);
	}
	trav(root->l, method);
	if (method == IN) {
		printf(" %c", root->name);
	}
	trav(root->r, method);
	if (method == POST) {
		printf(" %c", root->name);
	}
};

int node_n = 0;
int leave_n = 0;
int one_n = 0;
int h = 0, hnow = 0;

void stat(struct Tree * root) {
	if (root == NULL) {
		return;
	}
	if (++hnow > h) {
		h = hnow;
	}
	node_n++;
	if (!root->l && !root->r) {
		leave_n++;
	} else if (!(root->l && root->r)) {
		one_n++;
	}
	stat(root->l);
	stat(root->r);
	hnow--;
};

int main() {
	struct Tree * root;
	create(&root);
	printf("PreOrder:");
	trav(root, PRE);
	printf("\n");
	printf("InOrder:");
	trav(root, IN);
	printf("\n");
	printf("PostOrder:");
	trav(root, POST);
	printf("\n");
	stat(root);
	printf("%d\n%d %d %d\n", h, node_n, leave_n, one_n);

	return 0;
};
