const int maxs=2000;
const char xpstrt='a';
int begin,end,res,ps;char _s[300],temp;
char ca,cb;
int _aSfunc_1(int a,char c)
{
    int aa;
    int bb;
    char cc;
    aa=a;
    cc=c;
    bb=1;
    if(aa>bb)
    {
        switch(cc-'i')
        {
            case 0:return (a*_aSfunc_1(aa-bb,'i'));
            default:{return (a*_aSfunc_1(aa-1,'c'));}    
        }
    }
    else
    {
        switch(cc)
        {
            case 'i':printf("The result of case i is done!");
            default:printf("The result of case c is done!");
        }
        return (1);
    }
}
int min(int a,int b)
{
    if(a < b)
    {
        return (a);
    }
    else
    {
        if(a == b)
        {
            return (a);
        }
        else
        {
            return (b);
        }
    }
}
int max(int a,int b)
{
    if(a > b)
    {
        return (a);
    }
    else
    {
        return (b);
    }
}

void no_Sfc(int a,int b)
{
    int aa,bb;
    aa=a;
    bb=b;
    if(aa > maxs)
    {
        res = -1;
        return;
    }
    else
    {
        ;
    }
    begin = min(aa,bb);
    end = max(aa,bb);
    res = (begin + end)*(end - begin +1)/2;
}
void print1(int res1,int res2)
{
    printf("result one is:");
    printf(res1);
    printf("result two is:",+res2+2-1-1);
}
void print2()
{
    if(res != -1)
    {
        printf("The sum of numbers between two results is:",res);
    }
    else
    {
        printf("no result!");
    }    
}
void main()
{
    const int first=1,second=2;
    const char p1='+',p2='-',p3='*',p4='/';
    int s_firi,s_seci,ni,nc,resultI,resultC,i,j,k,flag;
    char s_firc,s_secc,p_fir,p_sec;
    scanf(s_firi,s_seci,s_firc,s_secc);
    if(s_firi < 0)
    {
        printf("wrong input!");
        return;
    }
    else
    {
        if(s_seci < 0)
        {
            printf("wrong input!");
            return;
        }
        else
        {
            ;
        }
    }
    if(s_firi >= s_seci)
    {
        ni = s_firi;  
    }
    else
    {
        ni = s_seci;    
    }
    if(ni <= 0)
    {
        if(ni <= +0) for(ni = ni; ni <= +0; ni = ni + 1)
        {
            
        }
    }
    else
    {
        ni = ni + 1;
    }
    switch( +s_firc - s_secc )
    {
        case 1:nc = 1;
        case 2:nc = 2;
        case -1:nc = 3;
        case -2:nc = 4;
        default:nc = 5;

    }
    resultI = _aSfunc_1(ni,'i');
    resultC = _aSfunc_1(nc,'c');
    print1(resultI,resultC);
    no_Sfc(resultI,resultC);
    print2();
    return;
}
