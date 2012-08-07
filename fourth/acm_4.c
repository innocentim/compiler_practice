#include <stdio.h>
#include <stdlib.h>

struct Tree {
	char name;
	struct Tree * l;
	struct Tree * r;
};

void create(struct Tree ** root) {
	char ch;
	ch = getchar();
	while (!('A' <= ch && ch <= 'Z' || ch == '#') && ch != '\n') {
		ch = getchar();
	}
	if (ch == '\n') {
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

void pre(struct Tree * root) {
	if (root == NULL) {
		return;
	}
	printf(" %c", root->name);
	pre(root->l);
	pre(root->r);
};

void mid(struct Tree * root) {
	struct Tree * queue[1000];
	int front = 0, rear = 0;
	if (root) {
		queue[rear++] = root;
	}
	while (front < rear) {
		struct Tree * temp = queue[front++];
		printf(" %c", temp->name);
		if (temp->l) {
			queue[rear++] = temp->l;
		}
		if (temp->r) {
			queue[rear++] = temp->r;
		}
	}
};

void reverse(struct Tree * root) {
	if (!root) {
		return;
	}
	struct Tree * temp = root->l;
	root->l = root->r;
	root->r = temp;
	reverse(root->l);
	reverse(root->r);
};

int main() {
	struct Tree * a;
	struct Tree * b;
	create(&a);
	getchar();
	create(&b);
	getchar();
	struct Tree * root = malloc(sizeof(struct Tree));
	root->l = a;
	root->r = b;
	root->name = getchar();

	printf("PreOrder:");
	pre(root);
	printf("\n");
	printf("LevelOrder:");
	mid(root);
	printf("\n");
	reverse(root);
	printf("PreOrder:");
	pre(root);
	printf("\n");
	printf("LevelOrder:");
	mid(root);
	printf("\n");

	return 0;
};
