const int _global_constInt = 7;
const char _message_to_the_world = '*';

int _global_int_arr[10], _global_int;
char _global_char_arr[10], _global_char;

int fib(int k)
{
    if (k > 1)
        return (fib(k-1)+fib(k-2)+-1*0);
    if (k == 0)
    {
        return (1);
    }
    if (k == 1)
    {
        return (1);
    }
    
}

void if_TEST()
{
    if (1)
    {}
    if (0)
    {;}
    if (1);
    if (fib(2))
    {
        if (fib(3) + 1)
        {
            printf("if test  ok");
        }
    }
    
}

int loop_TEST(int loop_initVar, int loop_STEP, int loop_CNT)
{
    int i, j;
    int Mark_cnt;
    i = loop_initVar;
        Mark_cnt = loop_CNT;
    j = i;
    for(loop_CNT = loop_CNT; loop_CNT > 0; loop_CNT = loop_CNT - 1)
    {
        i = i + loop_STEP;
    } ;
    
    for(j = j; j > 0; j = j - 1);    
    ;
    
    if (j == 0)
    {
        if (i == loop_initVar + loop_STEP * Mark_cnt)
        {
            printf("loop test ok");
        }
    }


    return (i);
}

char myToUpper(char InputChar)
{
    if (InputChar == 'a')
    {
        return ('A');
    }
    if (InputChar == 'b')
        return ('B');
    if (InputChar == 'c')
    {
        return ('C');
    }
    if (InputChar == 'd')
    {
        return ('D');
    }
    if (InputChar == 'e')
        return ('E');
}

void case_TEST()
{
    int TEST_num;
    char test_CHAR;
    printf("please input int(0-9) & char values(a-e):");
    scanf(TEST_num, test_CHAR);
    switch (TEST_num)
    {
        case 0:
            printf(TEST_num+test_CHAR);
        case 1:
        {
            printf(TEST_num+test_CHAR);
        }
        case 2:
            printf(TEST_num+ test_CHAR);
        case 3:
        {
            printf(TEST_num+test_CHAR) ;
        }
        case 4:
            printf(TEST_num+test_CHAR);
        case 5:
        {
            printf(TEST_num+test_CHAR);
        }
        case 6:
            printf(TEST_num+test_CHAR);
        case 7:
            printf(TEST_num+test_CHAR);
        case 8 :
            printf(TEST_num+test_CHAR);
        case 9 :
            printf(TEST_num+test_CHAR);
		default: {}
    }

    switch (test_CHAR)
    {
        case 'a':
        {
            printf(myToUpper(test_CHAR));
        }
        case 'b':
            printf(myToUpper(test_CHAR));
        case 'c':
        {
            printf(myToUpper(test_CHAR));
        }
        case 'd':
            printf(myToUpper(test_CHAR));
        case 'e':
            printf(myToUpper(test_CHAR));    
		default:;
    }

}

void setAverage()
{
    int SUM, RET, i;
    SUM = 0;
    i = 0;
    for(i = i; i < 10; i = i + 1)
    {
        SUM = SUM + _global_char_arr[i];
    };
    RET = SUM / 10;
    i = 0;
    _global_int = RET;
    
}

void array_TEST()
{
    int i;
    i = 0;

    for(i = i; i < 10; i = i + 1)
    {
        _global_char_arr[i] = 'a';
    }  ;

    setAverage();
}

void main()
{
    int loop_initvar, loop_step, loop_cnt;
    if_TEST();
    printf("please input 3 int(the 3rd int must >0):");
    scanf(loop_initvar, loop_step, loop_cnt);
    loop_initvar = loop_TEST(loop_initvar, loop_step, loop_cnt);
    case_TEST();
    array_TEST();
    if (_global_int == 97)
    {
        printf("passed");
    }
}
