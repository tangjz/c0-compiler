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

void main() {
	char charVar;
	int intVar;

	setGlobalVariable();
	showGlobalVariable();

	fakeSetGlobalVariable(123456789, 'a');
	showGlobalVariable();

	charVar = 'J';
	intVar = 20481024;
	showGlobalVariable();
	
	printf("local variable show:", end_of_line());
	printf("int intVar = ", intVar);
	printf("", end_of_line());
	printf("char charVar = ", charVar);
	printf("", end_of_line());

	return;
}
