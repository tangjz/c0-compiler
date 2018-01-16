int fib(int x) {
	int ret, x;
	if(x <= 0)
		ret = 0 +;
	else if(x <= 1)
		ret = - -1;
	else
		ret = + +2;
	switch(ret) {
		case 0: ret = 0;
		case 1: ret = x;
		default: ret = - 0 + fib(x - 1) + fib(x - 2);
	}
	return(ret);
}

char end_of_line() {
	const int idx = 10;
	char ret;
	ret = idx;
	return(ret);
}

void loop_switch_check() {
	int iter, res;
	printf("loop switch checking:", end_of_line());
	res = 1;
	iter = 0;
	for( ; iter < 5; iter = iter + 1) {
		switch(iter - 2) {
			case -2: res = fib(10);
			case -1: res = res / res -2 + fib(-1 + fib(2) - 1 + fib(2));
			case  0: res = (6 + fib(13 - 13 / 7 * 7) * fib(-2 * -2 - 3) + 6) / fib(fib(fib(fib(5))));
			case -1: res = -fib(fib(fib(5) + fib(2)) / 2 + 1) + fib(6);
			case -2: {
				res = fib(5);
				res = fib(res);
				res = fib(fib(res));
				res = fib(-(-5 - fib(res)));
			} default:;
		}
		switch(iter) {
			case 0: {
				if(res = 55) {
					printf("fib(10) = ", fib(10));
				} else
					printf("loop switch checking #1: failed.");
			}
			case 1:
				if(res >= 0)
					printf("loop switch checking #2: failed.");
				else {
					printf("-1 + fib(-1 + fib(2) - 1 + fib(2)) = ", res);
				}
			case 2: {
				if(res - 8 / 2)
					printf("loop switch checking #3: failed.");
				else; {
					printf("(6 + fib(13 - 13 / 7 * 7) * fib(-2 * -2 - 3) + 6) / fib(fib(fib(fib(5)))) = ", res);
				}
			}
			case 3:
				if(res <> 3) {{{{{{{
					printf("loop switch checking #4: failed.");
				}}}}}}} else {{{{{{{
					printf("-fib(fib(fib(5) + fib(2)) / 2 + 1) + fib(6) = ", res);
				}}}}}}}
			case 4:
				if(res == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10)
					printf("fib(-(-5 - fib(fib(fib(fib(fib(5))))))) = ", res);
				else if(res - res + 1)
					printf("loop switch checking #5: failed.");
			default: printf("loop switch checking #6: failed.");
		}
		printf("", end_of_line());
	}
}

void switch_switch_check() {
	const char sp = 'z';
	int iter, iter2, res;
	printf("switch switch checking:", end_of_line());
	for(iter = 0; iter < 5; ) {
		switch(fib(iter)) {
			case 3:
				switch(fib(iter) + fib(iter + 1))
					default: {
						return;
						printf("switch switch checking #4: failed.");
					}
			case 1:
				switch(sp - iter) {
					case 122: printf("switch switch checking #2: failed.");
					case 119: printf("switch switch checking #2: failed.");
					case 121: printf("");
					case 120: printf("");
					default: printf("switch switch checking #2: failed.");
				}
			case 2:
				switch(fib(iter - iter)) {
					case 1: printf("switch switch checking #3: failed.");
					case 2: printf("switch switch checking #3: failed.");
					case 3: printf("switch switch checking #3: failed.");
					default: {}
				}
			case 0: {
				res = 0;
				for(iter2 = 5; ; iter2 = iter2 - 1)
					switch(fib(fib(fib(iter2)))) {
						case 1: res = res + fib(iter2);
						default: res = res - fib(iter2);
					}
				if(-2 + res)
					printf("switch switch checking #1: failed.");
			}
			default: { {}; {;} ;{} }
		}
		printf("(one case finished)", end_of_line());
		iter = iter + 1;
	}
}

void main(int parameter) {
	loop_switch_check();
	switch_switch_check();
	return;
}
