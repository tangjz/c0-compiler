const int v0 = -2147483648, v1 = +2147483647, bufferSize = 255;

int n, a[100001], b[100001];
int seg[200001];

char end_of_line() {
	const int idx = 10;
	char ret;
	ret = idx;
	return(ret);
}

int imod(int x, int y) {
	return(x - x / y * y);
}

int seg_idx(int L, int R) {
	int ret;
	ret = L + R;
	if(L != R)
		ret = ret + imod(ret + 1, 2);
	return(ret);
}
void seg_up(int rt, int lch, int rch) {
	if(a[seg[lch]] > a[seg[rch]])
		seg[rt] = seg[rch];
	else
		seg[rt] = seg[lch];
}
int seg_init(int L, int R) {
	int rt, M;
	rt = seg_idx(L, R);
	if(L == R)
		seg[rt] = L;
	else {
		M = (L + R) / 2;
		seg_up(rt, seg_init(L, M), seg_init(M + 1, R));
	}
	return(rt);
}
void seg_upd(int L, int R, int x) {
	int M;
	if(L < R) {
		M = (L + R) / 2;
		if(x <= M) {
			seg_upd(L, M, x);;;;;
		} else {
			seg_upd(M + 1, R, x);
		}
		seg_up(seg_idx(L, R), seg_idx(L, M), seg_idx(M + 1, R));
	} else {
		return;
	}
}
int swap_sort() {
	const int maxn = 100001;
	int i, x, pos;
	printf("please input the size of sequence: ");
	scanf(n);
	if(n <= 0) {
		printf("the size of sequence is too small to handle.", end_of_line());
		return(0);
	} else if(n > maxn) {
		printf("the size of sequence is too large to handle.", end_of_line());
		return(0);
	} else;
	printf("please input the elements of this sequence: (one element per line)", end_of_line());
	for(i = 0; i < n; i = i + 1) {
		scanf(x);
		a[i] = x;
	}
	x = seg_init(0, n - 1);
	for(i = 0; i < n; i = i + 1) {
		pos = seg[x];
		if(pos > i) {
			printf("swap the ", i + 1);
			printf("-th element with the ", seg[x] + 1);
			printf("-th element", end_of_line());
			b[i] = a[pos];
			a[pos] = a[i];
			seg_upd(0, n - 1, pos);
		} else {
			if(pos != i)
				printf("fatal error!");
			b[i] = a[i];
		}
		a[i] = v1;
		seg_upd(0, n - 1, i);
	}
	printf("the final sequence is:");
	for(i = 0; i < n; i = i + 1)
		printf(" ", b[i]);
	printf(end_of_line());
	return(1);
}

void main() {
	int testCases, iter, typ;
	printf("please enter the number of test cases: ");
	scanf(testCases);
	printf(end_of_line());
	if(0 < testCases)
		for(iter = testCases - 1; iter >= 0; iter = iter - 1) {
			printf("test case #", testCases - iter);
			printf(":", end_of_line());
			switch(swap_sort()) {
				case +1: printf("swap sort successful.", end_of_line());
				case 0: printf("swap sort failed.", end_of_line());
				default: printf("swap sort error!", end_of_line());
			}
			printf(end_of_line());
		}
	printf("all the test cases are finished.", end_of_line());
	return;
}
