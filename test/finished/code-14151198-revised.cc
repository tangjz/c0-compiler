const int cst1 = 1, cst2 = 2, cst3 = 3;
const char csta = 'a', cstb = 'b', cstc = 'c';
int g_a, g_b, g_arr[2048];
char g_stra[1024], g_strb[2048];


void const_define(){
    const int a = 1;
    const int b = 0, c = -1;
    const char cst23='x';
    const char cst24='z', constA0='A', cstA1='1', cst_div = '/';
    printf(a);   
    printf(cst_div);
}

void const_define_test(){
    const_define();
}

void var_define(){
    int a, arr[1], _a, _arr[2], a1, arr1[3];
    char b, strb[1], _b, _bstr[2], _b1, _bstr1[3];
    int c;
    char z;
}
void var_define_test(){
    var_define();
}

void define(){
    const int int1 = 1;
    const char cst24='z', constA0='A';
    int val1;
    char b, strb[1];
}

void define_test(){
    var_define_test();
    const_define_test();
    define();
    printf("define test passed");
}

void operator(){
    int a, b, c, d, e, f;
    a = 1; b = 1 + 2; c = 3 * 1; d = -4; e = 5; f = 6;
    a = (b * 3) + c - d / 4 + a / a;
    b = + 0 + a + (a * 3) + c - d / 4 + e * f;
    e = -(a) + b * 0 - (1 - 0);
}

void operator_test(){
    operator();
    printf("operator test passed");
}
void string(){
    printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
    printf("!#$%&'()*+,-./:;<=>?@[]^_`{|}~");
}
void string_test(){
    string();

}
void dowhile(){
    int i;
    i = 100;

    for(i = i; i >= 10; i = i + 1) {
        if (i >= 50) 
            i = i / 2;
        if (i < 50)
            i = (i + 10) / 10;
        i = i - 1;
	}
    for(i = i; i - i; i = i + 1) {
        i = i - 1;
        printf("dowhile test passed");
		i = i - 1;

    }
}

void dowhile_test(){
    dowhile();
}

void ifcondition(){
    int a, rightnumber;
    a = 1;
    rightnumber = 0;
    if (a == 1) rightnumber = rightnumber + 1;
    a = 2;
    if (a > 1) rightnumber = rightnumber + 1;
    a = 0;
    if (a < 1) rightnumber = rightnumber + 1;
    if (a != 1) rightnumber = rightnumber + 1;
    if (a >= 0) rightnumber = rightnumber + 1;
    if (a <= 0) rightnumber = rightnumber + 1;
    if (rightnumber !=  (1 + 1 + 1 + 1 * 3))
        printf("ifcondition clause error!");
    if (rightnumber != rightnumber)
        printf("ifcondition clause error");
}


int switchcodintionans(int x){
    switch (x) {
        case 1 : return (2);
        case 2 : return (3);
        case 3 : return (4);
        default: ;
    }
    return (5);
}


int switchcodintion(){
    int i, rightnumber;
    i = 1;
    rightnumber = 0;
    for(i = i; i < 5; i = i + 1) {
        if ( i + 1 == switchcodintionans(i))
            rightnumber = rightnumber + 1;
    }
    if (rightnumber != 4)
        printf("switchcodintion clause  error!");
    if (rightnumber)
        return (i);
    printf("switchcodintion clause  error!");
}

void condition(){
    const int intermax = 5;
    int iter;
    iter = 0;
    for(iter = iter; iter < intermax; iter = iter + 1) {
        switch (iter){
            case 0: if (iter != 0) printf("error 0");
            case 1: if (iter != 1) printf("error 1");
            case 2: if (iter != 2) printf("error 2");
            default: ;
        }
    }
}

void condition_test(){
    ifcondition();
    switchcodintion();
    condition();
    printf("condition test passed");
}


void miscellaneous(){
    int _, a, array[23], i, endid;
    char ch1, buf[256];
    _ = 1; a = 2; i= 0;
    for(i = i; i < 23; i = i + 1) {
        array[i] = i;
    }
    ch1 = 'a';
    i = 127;
    endid = i;
    _ = 0;

    for(_ = _; _ < endid; _ = _ + 1) {
        buf[ (_ * 1 - _ - 1 + 2 / 1 - 1) / 2] =  'a';
    }

    ;;;;;;;;;{;}{;};;;;;{;}{;}{;;;;;;};;;
    if (buf[0] == 'a')
    printf("miscellaneous test passed");
}
void miscellaneous_test(){
    miscellaneous();
}


int fibonaci(int n){
    switch (n){
        case 0: return (0);
        case 1: return (1);
        case 2: return (1);
		default: ;
    }
    return (fibonaci(n - 1) + fibonaci(n - 2));
}

void fibonaci_test(){
    int n;
    printf("Fibonaci: Please input a unsigned positive integer ?");
    scanf(n);
    for(n = n; n < 0; n = n + 1){
        if (n < 0){
            printf("Input error, try again:");
            scanf(n);
        }
        if (n >= 0){
            printf("The fibonaci's answer is ", fibonaci(n));   
        }
		n = n - 1;
    }
    
}
int mod(int n, int m){
    return (n - n / m * m);
}
int gcd(int n, int m){
    int rem;
    rem = 0;
    if (n == 0)
        return (m);
    if (m == 0)
        return (n);
    return (gcd(m, mod(n, m)));
}

int isWrongGcd(int n, int m){
    if (n <= 0)
        return (1);
    if (m <= 0)
        return (1);
    return (0);
}
void gcd_test(){
    int n, m;
    printf("GCD: Please input two unsigned positive integer ?");
    scanf(n, m);
    if (isWrongGcd(n, m)){
        for(n = n; isWrongGcd(n, m); n = n + 1) {
            printf("Input error, try Again: ");
            scanf(n, m);
			n = n - 1;
        }
    }
    printf("The gcd's answer is ", gcd(n, m));
}

int is_prime(int x){
    int i;
    i = 2;
    if (x == 2)
        return (1);
    for(i = i; i * i <= x; i = i + 1) {
        if (mod(x, i) == 0)
            return (0);
    }
    return (1);
}

void prime_factorization(int n){
    int i, first;
    i = 2;
    first = 0;
    if (n == 1){
        printf("1");
        return ;
    }
    for(i = i; i * i <= n; i = i + 1) {
        if (is_prime(i) == 1) {
            if (mod(n, i) == 0){
                for(n = n; mod(n, i) == 0; n = n + 1) {
                    n = n / i;
                    if (first == 1)
                        printf("*");
                    first = 1;
                    printf(i);
					n = n - 1;
                }
            }
        }
    }

    if (n != 1){
        if (first == 1)
            printf("*");
        printf(n);
    }
}

void prime_factorization_test(){
    int n;
    printf("prime_factorization: Please input a unsigned positive integer(<2^31-1) ?");
    scanf(n);
    for(n = n; n < 0; n = n + 1) {
        if (n < 0){
            printf("Input error, try again:");
            scanf(n);
        }
		n = n - 1;
    }
    printf("The factor of n is ");
    prime_factorization(n);
}
int isWrong(char A){
    if ('E' == A)
        return (0);
    if ('M' == A)
        return (0);
    if ('H' == A)
        return (0);
    return (1);
}

void main(){
    char kind;
    printf("Please input Test Type:E(Easy), (M)Middle, H(Hard): ");
    scanf(kind);
    if (isWrong(kind)){
        for(kind = kind; isWrong(kind); kind = kind + 1){
            printf("Try Again: ");
            scanf(kind);
			kind = kind - 1;
        }
    }
    switch(kind){
    case 'E': {
                define_test();
                operator_test();
                fibonaci_test();
            }
    case 'M': {
                dowhile_test();
                condition_test();
                string_test();
                gcd_test();
            }
    case 'H': {
            define_test();
            operator_test();
            dowhile_test();
            condition_test();
            string_test();
            miscellaneous_test();
            prime_factorization_test();
        }
    default: ;
    }
    return ;
}
