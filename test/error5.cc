int intVar;
char charVar;

char end_of_line() {
	const int idx = 10;
	char ret;
	ret = idx;
	return(ret);
}

void showGlobalVariable() {
	printf("global variable show:", end_of_line());
	printf("int intVar = ", intVar);
	printf("", end_of_line());
	printf("char charVar = ", charVar);
	printf("", end_of_line());
}

void setGlobalVariable() {
	intVar = 19260817;
	charVar = 'Z';
	printf("global variable should be changed!", end_of_line());
}

void fakeSetGlobalVariable(int intVar, char charVar) {
	intVar = 26191708;
	charVar = 'z';
	printf("global variable should not be changed!", end_of_line());
}

int fakeSetGlobalVariable() {
	intVar = 19260817;
	charVar = 'Z';
	printf("this function should not be created!", end_of_line());
}

void fake_function_test(char fake_function_test) {
	fake_function_test = fake_function_test(fake_function_test);
}

void fake_function_test() {
	const char fakeVar = 'F';
	int fakeVar;
	fakeVar = 'F' + 'A' - 'K' * 'E' / '_';
	printf("this function should not be created too!", end_of_line());
}

void fake_function_test_2(int fakeVar) {
	char fakeVar;
	fakeVar = 'T';
}

void main() {
	const char charVar = 'a';
	const int intVar = 123456789;

	setGlobalVariable();
	showGlobalVariable();

	fakeSetGlobalVariable(intVar, charVar);
	showGlobalVariable();

	charVar = 'J';
	intVar = 20481024;

	fakeSetGlobalVariable();
	showGlobalVariable();
	
	printf("local variable show:", end_of_line());
	printf("int intVar = ", intVar);
	printf("", end_of_line());
	printf("char charVar = ", charVar);
	printf("", end_of_line());

	fake_function_test();
	fake_function_test_2();

	return;
}
