#include <cstdio>

class person {
public:
	const char * name;
	person(const char * name) : name(name) {};
	virtual void print_name() = 0;
	virtual void shout() = 0;
};

class student : public person {
public:
	student(const char * name) : /* interesting : call the constructor of super class */ person(name) {};
	void print_name() {
		printf("i'm %s\n", name);
	};

	void shout() {
		printf("i'm a student\n");
	};
};

class child : public person {
public:
	child(const char * name) : person(name) {};

	void print_name() {
		printf("i'm %s\n", name);
	};
	
	void shout() {
		printf("i'm a child\n");
	};
};

void work(person * a) {
	a->print_name();
	a->shout();
	printf("\n");
};

int main() {
	work(new student("Prince Arthas, apprentice of Uther"));
	work(new child("Timmy the Ghoul"));
	
	return 0;
}
