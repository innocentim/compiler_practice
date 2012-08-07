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

int main() {
	struct Tree * root;
	create(&root);

	struct Tree * queue[1000];
	int front = 0, rear = 0;
	if (root) {
		queue[rear++] = root;
	}
	printf("LevelOrder:");
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
	printf("\n");

	return 0;
};
