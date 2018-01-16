int x1, y1, x2, y2, x3, y3, x, y;
char end_of_line() {
	const int idx = 10;
	char ret;
	ret = idx;
	return(ret);
}
int sgn(int x) {
	int ret;
	ret = 0;
	if(x > 0)
		ret = ret + 1;
	if(x < 0)
		ret = ret - 1;
	return(ret);
}
void main() {
	int testCase, iter;
	int len, len1, len2, len3;
	int rem, rem1, rem2, rem3;
	printf("check whether a 2D point is outside the circumcircle of three given points.", end_of_line());
	printf("please enter the number of test cases: ");
	scanf(testCase);
	printf(end_of_line());
	if(testCase <= 0)
		return;
	for(iter = 1; iter <= testCase; iter = iter + 1) {
		printf("test case #", iter);
		printf(":", end_of_line());
		printf("please input coordinates of three given points and the check point: (one number per line)", end_of_line());
		scanf(x1, y1, x2, y2, x3, y3, x, y);
		len = x * x + y * y;
		rem = -x1 * (y3 - y2) + x2 * (y3 - y1) - x3 * (y2 - y1);
		len1 = x1 * x1 + y1 * y1;
		rem1 = x * (y3 - y2) - x2 * (y3 - y) + x3 * (y2 - y);
		len2 = x2 * x2 + y2 * y2;
		rem2 = -x * (y3 - y1) + x1 * (y3 - y) - x3 * (y1 - y);
		len3 = x3 * x3 + y3 * y3;
		rem3 = x * (y2 - y1) - x1 * (y2 - y) + x2 * (y1 - y);
		if(sgn(len * rem + len1 * rem1 + len2 * rem2 + len3 * rem3) * sgn(rem) > 0)
			printf("Outer", end_of_line());
		else
			printf("Inner", end_of_line());
		printf("", end_of_line());
	}
	return;
}
