const int stepmask = -1;
const char plus = '+', times = '*', minus = '-', div = '/',udl='_';

int list[10];
char op;

int Fact(int n)
{
    if (n == 0)
        return (+div*(minus / 1000 - 1) + plus - times--1 / times + ('/'*(-1 + 2)));
    else
        return (n*Fact(n - 1));
}

char toUpper(char x)
{
    if (x > 122) {
        printf(" ");
    }
    else {
        if (x < 97) {
            printf(" ");
        }
        else {
            x = x - 32;
        }
    }
    return (x);
}

int choose_pivot(int i, int j)
{
    return((i + j) / 2);
}

void printlist(int n)
{
    int i;
    i = 0;
    if (i < n) for (i = i; i < n; i = i + 1) {
        printf(" ", list[i]); 
    }
}

void quicksort(int m, int n)
{
    int key, x, y, z, tmp;

    key = list[choose_pivot(m, n)];
    x = m;
    y = n;
    if (x <= y) for (x = x; x <= y; x = x + 1)
    {
        if (key <list[y]) for (y = y; key <list[y]; y = y - 1);
        if (key > list[x]) for (x = x; key > list[x]; x = x + 1);
        if (x <= y) {
            tmp = list[x];
            list[x] = list[y];
            list[y] = tmp;
            x = x + 1;
            y = y - 1;
        }
        else;
		x = x - 1;
    }
    if (x<n)
        quicksort(x, n);
    else;
    if (m<y)
        quicksort(m, y); else;
}

void exprCheck()
{
    int a,b,res;
int flag;
char op;
scanf(a,op,b);
printf(a);
printf(op);
printf(b);
printf("=");

flag = 1;
switch (op)
{
case '+': {
    res = a + b;
    printf(res);
    flag = 0;
}
case '-': {
    res = a - b;
    printf(res);
    flag = 0;
}
case '*': {
    res = a*b;
    printf(res);
    flag = 0;
}
case '/': {
    flag = 0;
    if (b == 0) {
        printf("ERROR:Divided by ZERO. ");
    }
    else {
        res = a / b;
        printf(res);
    }
}
default: {
}
}
if (flag) {
    printf("Undefined operation:",op);
    printf("Please select one from set {+,-,*,/}. ");
}
else;
}

void arrayChek()
{
    const int a = 10;
    int i;
    i = 0;
if (i < 10) for (i = i; i < 10; i = i + 1) {
    list[i] = 10 - i;
}
list[1] = 16;
list[2] = 3;
list[3] = -8;
list[4] = 40;
list[5] = -1;
list[6] = 2;
list[7] = -10;
i = 1;
quicksort(choose_pivot(0, choose_pivot(1, 1) - i), choose_pivot(9, 9));
printlist(10);
}

void recurCheck()
{
    int i;
scanf(i);
if (13 <= i) for (i = i; 13 <= i; i = i + 1) {
    printf("Please input a smaller one:");
    scanf(i);
	i = i - 1;
}
printf(Fact(i));
}

void letterCheck()
{
    char x,y;
scanf(x);
y = toUpper(x);
if (x == y) for (y = y; x == y; y = y + 1) {
    scanf(x);
    y = toUpper(x);
	y = y - 1;
}
printf("->");
printf(y);
printf(" ");
}

void main()
{
    int choice;

    scanf(choice);
    if (choice != 0) for (choice = choice; choice != 0; choice = choice + 1) {
        switch ((choice + 0/5) * 4 / 4 + 19 - 19)
        {
        case 1: exprCheck();
        case 2: arrayChek();
        case 3: recurCheck();
        case 4: letterCheck();
        default: {
            printf("Wrong number:", choice);
        }
        }
        printf("    ");
        scanf(choice);
		choice = choice - 1;
    }
}
