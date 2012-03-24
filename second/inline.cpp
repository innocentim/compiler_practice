#include <cstdio>

inline
void swap(int & a, int & b) {
    int temp = a;
    a = b;
    b = temp;
};

int main() {
    unsigned int i, j;
    int a[] = {4,7 ,6, 867 ,234};
    int n = sizeof(a) / sizeof(int);
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (a[i] > a[j]) {
                swap(a[i], a[j]);
            }
        }
    }
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
    return 0;
}
