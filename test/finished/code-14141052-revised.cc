const int MAX=10;
const char A='a';
const char plus = '+', minus = '-', mult = '*', div = '/';

int n,x,y,kind,m,ans,_a,_b;
char ch,alphabet[26];

void testDefine()
{
    const int int0=0,int1=1,int10=10;

    const int int_0=0,int_1=-1,int_10=-10;

    const int _int0=0,_int1=+1,_int10=+10;

    const char char0='0',char1='1';

    int var0,var1,arr1[1],var11,arr11[11];
}

int fibo(int n)
{
    if (n<0) return(-1);
    else if (n==0) return(0);
    else if (n==1) return(1);
    else if (n>MAX) return(-2);
    else return(fibo(n-1)+fibo(n-2));
}

char upcase(char x)
{
    char y;
    if (x>='a')
        if (x<='z')
        {
            y = x - 'a' + 'A';
            return(y);
        }
        else return(x);
    else return(x);
}

char lowcase(char x)
{
    char y;
    if (x>='A')
        if (x<='Z')
        {
            y = x - 'A' + 'a';
            return(y);
        }
        else return(x);
    else return(x);
}

void printspace()
{
    printf(" ");
}

void display(int m)
{
    int i;
    char letter;
    if (m<=0)
        return;
    else if (m>26)
        m = 26;
    else {
        i = 0;
        letter = A;
		if(i<m)
        for(i=i;i<m;i=i+1)
        {
            alphabet[i] = upcase(letter);
            alphabet[i] = upcase(letter);
            printf(alphabet[i]);
            letter = letter + 1;
        }
        printspace();
        i = 0;
		if(i<m)
        for (i=i;i<m;i = i + 1)
        {
            alphabet[i] = lowcase(alphabet[i]);
            printf(alphabet[i]);
        }
        printspace();
    }
}

void expre()
{
    int x,y,z;
    scanf(x,y);
    z = +((x+y) - (-y*(-2)) + (-2*0));
    printf("expre:",z);
    printspace();
}

int mod(int x, int y)
{
    int z;
    z = x - x/y*y;
    return(z);
}

int gcd(int a, int b)
{
    if (b==0) return(0);
    else if (mod(a,b)==0)
        return(b);
    else return(gcd(b, mod(a,b)));
}

void process()
{
    char choice;
    scanf(choice);
    switch (choice)
    {
        case 'a':
            expre();
        case 'b':
        {
            scanf(n);
            ans = fibo(n);
            printf("fibo:",ans);
            printspace();
        }
        case 'd':
        {
            scanf(m);
            display(m);
        }
        case 'e':
        {
            scanf(_a,_b);
            printf("gcd:",gcd(_a,_b));
            printspace();
        }
        case 'f':
            printf("string:!#$%&'()*+,-./:;<=>?@[\]^_`{|}~");
		default: ;
    }
    {
        ;
    }
    ;
    {}
}

void main()
{
    int i,num;
    i = 0;
    testDefine();
    scanf(num);
	if(i<num)
    for (i=i;i<num;i=i+1)
    {
        i = i + 1;
        process();
		i=i-1;
    }
    return;
    printf("Test Not Return");
}
