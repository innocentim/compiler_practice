#include "list.hpp"
#include <cstdio>

int main() {
	while (1) {
		list<int> l;
		for (int i = 0; i < 100; i++){
			l.push_back(i);
		}
		// for (int i = 0; i < 100; i++){
		// printf("%d ", l.pop_back());
		// }
		// printf("\n");
	}

	return 0;
};
