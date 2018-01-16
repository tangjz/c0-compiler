const char _a = 'A', _b = 'B', _c = 'C', _d = 'D', _e = 'E', _f = 'F', _g = 'G', _h = 'H',
_i = 'I', _j = 'J', _k = 'K', _l = 'L', _m = 'M', _n = 'N', _o = 'O', _p = 'P', _q = 'Q',
_r = 'R', _s = 'S', _t = 'T', _u = 'U', _v = 'V', _w = 'W', _x = 'X', _y = 'Y', _z = 'Z';

char end_of_line() {
	const int idx = 10;
	char ret;
	ret = idx;
	return(ret);
}

void putchar(char ch) {
	printf(ch);
}

void test_of_characters() {
	printf("supported characters in this compiler are listed in the following:", end_of_line());
	printf("", end_of_line());
	printf(" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~", end_of_line());
	printf("", end_of_line());
}

int is_alpha(char ch) {
	if(ch >= 'a')
		if(ch <= 'z')
			return(1);
		else
			return(0);
	else if(ch <= 'Z')
		if(ch >= 'A')
			return(1);
		else
			return(0);
	return(0);
}

int is_digit(char ch) {
	const char fst = '0', lst = '9';
	char iter;
	for(iter = fst; iter <= lst; iter = iter + 1)
		if(iter == ch)
			return(1);
	return(0);
}

void single_character_test() {
	int x;
	char op1, op2;
	x = ((((((((((((((((((((((-'1' + '2' - '3' + '4'
		+'5' - '6' + '7' - '8'))))))))))))))))))))));
	if(x) {
		printf("this branch should not be accessed!", end_of_line());
		x = 0;
	}
	x = '0' - _a + _c;
	if(is_digit(x)) {
		printf("ascii ", x);
		printf(" is digit ");
		putchar(x);
		printf(end_of_line());
	} else {
		printf("ascii ", x);
		printf(" is not digit", end_of_line());
	}
	x = 'z' + '0' - '9';
	if(is_alpha(x)) {
		printf("ascii ", x);
		printf(" is alpha ");
		putchar(x);
		printf(end_of_line());
	} else {
		printf("ascii ", x);
		printf(" is not alpha", end_of_line());
	}
	x = _c - '4' + '1';
	if(is_alpha(x)) {
		printf("ascii ", x);
		printf(" is alpha ");
		putchar(x);
		printf(end_of_line());
	} else {
		printf("ascii ", x);
		printf(" is not alpha", end_of_line());
	}
}

void main() {
	test_of_characters();
	single_character_test();
	return;
}
