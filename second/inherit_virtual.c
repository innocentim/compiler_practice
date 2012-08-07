#include <stdio.h>
#include <stdlib.h>

// class person, an abstruct class
struct person {
	void * vtab;
	const char * name;
};

struct vtab_person_t { // "virtual table of person" type
	void (*print_name)(struct person * this);
	void (*shout)(struct person * this);
} vtab_person[1];

// class student
struct student {
	struct person _super; // inherit
};

void student_print_name(struct student * this) { // override
	printf("i'm %s\n", ((struct person *)this)->name); // why can i directly use static cast? Please be aware of the memory layout (advanced topic)
};

void student_shout(struct student * this) { // override
	printf("i'm a student\n");
};

struct vtab_student_t {
	void (*print_name)(struct student * this);
	void (*shout)(struct student * this);
} vtab_student[1];

// class child
struct child {
	struct person _super; // inherit
};

void child_print_name(struct child * this) { // override
	printf("i'm %s\n", ((struct person *)this)->name);
};

void child_shout(struct child * this) { // override
	printf("i'm a child\n");
};

struct vtab_child_t {
	void (*print_name)(struct child * this);
	void (*shout)(struct child * this);
} vtab_child[1];

// main

void work(struct person * p) {
	((struct vtab_person_t *)p->vtab)->print_name(p);
	((struct vtab_person_t *)p->vtab)->shout(p);
	printf("\n");
};

int main() {
	// initialization
	vtab_person->shout = NULL;
	vtab_person->print_name = NULL;
	vtab_student->shout = student_shout;
	vtab_student->print_name = student_print_name;
	vtab_child->shout = child_shout;
	vtab_child->print_name = child_print_name;

	// work
	struct student * stu;
	{ // constructor of student
		stu = malloc(sizeof(struct student));
		((struct person *)stu)->vtab = vtab_student;
		((struct person *)stu)->name = "Prince Arthas, apprentice of Uther";
	}
	work((struct person*)stu);

	struct child * ch;
	{ // constructor of child
		ch = malloc(sizeof(struct child));
		((struct person *)ch)->vtab = vtab_child;
		((struct person *)ch)->name = "Timmy the Ghoul";
	}
	work((struct person*)ch);

	return 0;
};
