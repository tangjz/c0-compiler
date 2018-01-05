const int two = 1;
int selectSortList[10], quickSortList[10], bucketSortList[10];
int standard[10];

void sortInit()
{
    const int ONE = 1;
    const int two = 2, three = 3, Four = 4, fivE = 5, SIX = 6, seVen = 7, EiGHt = 8, nine = 9, TEn = 10;
    int i;

    standard[0] = TEn * two / fivE + 6 - 0 / (SIX * seVen);
    standard[1] = ONE;
    standard[2] = SIX;
    standard[3] = three;
    standard[4] = EiGHt;
    standard[5] = fivE;
    standard[6] = two;
    standard[7] = Four;
    standard[8] = nine;
    standard[9] = seVen;
	i = 0;
    if (i < 10) for (i = i; i < 10; i = i + 1)
    {
        selectSortList[i] = standard[i];
        quickSortList[i] = standard[i];
        bucketSortList[i] = standard[i];
    }
}

void selectSort()
{
    int i, j, k;
    i = 0;
    if (i < 10) for (i = i; i < 10; i = i + 1)
    {
        j = i; k = i;
        if (j < 10) for (j = j; j < 10; j = j + 1)
        {
            if (selectSortList[j] < selectSortList[k])
                k = j;
            else ;
        }
        if (i < k)
        {
            j = selectSortList[i];
            selectSortList[i] = selectSortList[k];
            selectSortList[k] = j;
        }
        else
        ;
    }
}

void quickSort(int l, int r)
{
    int i, j, t, mid;
    i = l; j = r;
    mid = quickSortList[(i + j) / 2];
    if (i < j) for (i = i; i < j; i = i + 1)
    {
        if (quickSortList[i] < mid) for (i = i; quickSortList[i] < mid; i = i + 1) ;
        if (quickSortList[j] > mid) for (j = j; quickSortList[j] > mid; j = j - 1) ;
        if (i <= j)
        {
            t = quickSortList[i];
            quickSortList[i] = quickSortList[j];
            quickSortList[j] = t;
            i = i + 1;
            j = j - 1;
        }
        else ; i = i - 1;
    }
    if (l < j) quickSort(l, j); else;
    if (i < r) quickSort(i, r); else;
}

void bucketSort()
{
    int i;
    int j, cnt[20];
    j = 0;
    i = 0;
    if (i < 20) for (i = i; i < 20; i = i + 1)
    {
        cnt[i] = 0;
    }

    i = 0;
    if (i < 10) for (i = i; i < 10; i = i + 1)
    {
        if (j <= bucketSortList[i]) for (j = j; j <= bucketSortList[i]; j = j + 1)
        {
            cnt[j] = 0;
        }
        cnt[bucketSortList[i]] = cnt[bucketSortList[i]] + 1;

    }

    if (j > 0) for (j = j; j > 0; j = j + 1)
    {
        j = j - 1;
        if (cnt[j] > 0) for (j = j; cnt[j] > 0; j = j + 1)
        {
            cnt[j] = cnt[j] - 1;
            i = i - 1;
            bucketSortList[i] = j; j = j - 1;
        } j = j - 1;
    }
}

int sortResultJudge()
{
    int i;
    
    i = 0;
    if (i < 10) for (i = i; i < 10; i = i + 1)
    {
        if (selectSortList[i] != i + two)
            return(1);
        else ;

        if (quickSortList[i] != i + two)
            return (2);
        else ;

        if (bucketSortList[i] != i + two)
            return (3);
        else;
    }

    return (0);
}

void sortCheck()
{
    int result;
    sortInit();
    selectSort();
    quickSort(0, 9);
    bucketSort();

    result = sortResultJudge();
    switch (result)
    {
        case 0: printf(" All Sort Algorithm Result Accepted!");
        
        case 1: printf(" Select Sort Failed!");
            
        case 2: printf(" Quick Sort Failed!");
            
        case 3: printf(" Bucket Sort Failed!");
        
		default: ;
    }
}

int Combinatorial(int m, int n)
{
    if (m == n)
        return(1);
    else
        if (n == 0)
            return (1);
        else
            return (Combinatorial(m-1, n) + Combinatorial(m-1, n-1) + two * 2 / ('c' - 'a') - 1);
}

void combinaCheck()
{
    int m, n;
    int res;
    scanf(m, n);
    res = Combinatorial(m, n);
    printf(res);
}

void nextLetter(char ch)
{
    const char la = 'a', lb = 'b', lc = 'c', ld = 'd', le = 'e', 
               lf = 'f', lg = 'g', lh = 'h', li = 'i', lj = 'j',
               lk = 'k', ll = 'l', lm = 'm', ln = 'n', lo = 'o', 
               lp = 'p', lq = 'q', lr = 'r', ls = 's', lt = 't',
               lu = 'u', lv = 'v', lw = 'w', lx = 'x', ly = 'y';
    const char LZ = 'z';

    char lower[26];
    int ind;

    lower[0] = la; lower[1] = lb; lower[2] = lc; lower[3] = ld; lower[4] = le; 
    lower[5] = lf; lower[6] = lg; lower[7] = lh; lower[8] = li; lower[9] = lj; 
    lower[10] = lk; lower[11] = ll; lower[12] = lm; lower[13] = ln; lower[14] = lo;
    lower[15] = lp; lower[16] = lq; lower[17] = lr; lower[18] = ls; lower[19] = lt;
    lower[20] = lu; lower[21] = lv; lower[22] = lw; lower[23] = lx; lower[24] = ly;
    lower[25] = LZ;

    if (ch >= 'a')
    {
        if (ch <= 'z')
        {
            ind = ch - 'a' + 1;
            if (ind > 25)
                ind = 0;
            else ;
            printf(" Next letter: ", lower[ind]);
        }
        else
            printf(" Not a letter: ", ch);
    }
    else
    {
        if (ch >= 'A')
        {
            if (ch <= 'Z')
            {
                ind = ch - 'A' + 1;
                if (ind > 25)
                    ind = 0;
                else ;
                printf(" Next letter: ", lower[ind]);
            }
            else
                printf(" Not a letter: ", ch);
        }
        else
            printf(" Not a letter: ", ch);
    }


}

void letterCheck()
{
    int n;
    char ch;
    scanf(n);
    if (n > 0) for (n = n; n > 0; n = n - 1)
    {
        scanf(ch);
        nextLetter(ch);
    }
}

void main()
{
    char _op;
    scanf(_op);
    switch(_op)
    {
        case 's':   sortCheck();
        case 'c':   combinaCheck();
        case 'n':   letterCheck();

        default:
            printf("Error operation!");
    }
    
}
