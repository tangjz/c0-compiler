const int cona=10;
const char conb='a',conc='6';
int _cha,chb[6];
char chc,chd;
int f(int x,int y){
    const int z=1;
    if(x>1) return (x+z);
    x=x+1;f(x,y);
    return (x+y);
}
int compare(int a,int b){
        if(a>b) return (1);
        if(a<=b) return (-1);
}

void line(int n,int m){
    int i,j,stpos[100],posst[100];
    int p,q,move,end,po1,po2,st2;
    i=1;j=1;
    for(i=i;i<=n;i=i+1){
        stpos[i]=i;
        posst[i]=i;
    }
    for(i=1;i<=m;i=i+1){
        if(i==1){
                  p=3;q=2;
         }
          if(i==2){
              p=8;q=-3;
          }
          if(i==3){
              p=3;q=-2;
          }
        if(q!=0){
            move=compare(q,0);
            end=move*q;
            po1=stpos[p];
            for(j=j;j<=end;j=j+1){
                st2=posst[po1+move];
                po2=stpos[st2];
                posst[po1]=st2;
                stpos[st2]=po1;
                po1=po2;
            }
            posst[po2]=p;
            stpos[p]=stpos[p]+q;
        }
    }
        printf(posst[1]);
    for(i=2;i<=n;i=i+1){
        printf(posst[i]);
       }
    return;
}
char f2(char x){
        int z2;
        z2=1;
    x=x+z2;
        return (x);
}
int fib(int n){
        int result;
        if(n==0) return(0);
        if(n==1) return(1);
        result=fib(n-1)+fib(n-2);
        return (result);

}
void nest(){
    int a[15];
    int b[15];
    a[0]=1;
    b[1]=3;
    a[3]=4;
    b[4]=6;
    a[6]=8;
    b[a[b[a[b[a[0]]]]]]=2333;
    printf(a[0]+b[8]);
    return;
}
void t(char x){
    if(x>'c') return;
    x=x+1;
    printf(x);
    t(x);
    return;
}
void main()
{
        const int coz=3;
        int a,b,c,d,key,i,sz2[5];
    int cho;
        char sz[5],e;
        chc='+';
        chd='-';
        b=+3;
        i=0;
    c=-2;
    key=1;
    scanf(cho);
    switch(cho){
        case 0:{

            printf("test line:");
                line(8,3);

        }
        case 1:{
            printf("test fib:");
                d=fib(coz);
                printf("fib=",d);

        }
        case 2:{
            for(i=i;i<5;i=i+1){
                       sz[i]='a';
                    sz2[i]=i;
                }


        
            scanf(key,a);
                if(key<3){
                    a=a+b;
                printf(a);
                }
                if(key<=3) {
                    b=b-a;
                    sz2[0]=sz2[0]+b;
                    sz[0]=sz[0]+1;
                printf(b);
                printf(sz[0]);
                printf(sz2[0]);
               }
                if(key>6) {
                    c=a*b;
                    sz2[1]=sz2[1]+c;
                    sz[1]=sz[1]+1;
                printf(c);
                printf(sz[1]);
                printf(sz2[1]);
                }
                if(key>=6) {
                    d=+a/b;
                    sz2[2]=sz2[2]+d;
                sz[2]=sz[2]+1;
                printf(d);
                printf(sz[2]);
                printf(sz2[2]);
                }
                if(key!=4) {
                    a=a-1/coz;
                    sz2[3]=sz2[3]+a+'c';
                    sz[3]=sz[3]+1;
                printf(a);
                printf(sz[3]);
                printf(sz2[3]);
                }
                if(key==4) {
                    a=-a+2*coz;
                    sz[4]=sz[4]+1;
                printf(a);
                printf(sz[4]);
                printf(sz2[4]);
                }    
                if(key){
                    printf(+a);
                }
        }
        case 3:{
            printf("test function:");
            if(key) sz2[4]=f(-2,3)+(5+1)*coz;
                t(conb);
            printf(f2('a'));
            printf(sz2[4]);
        }
        case 4:{
            printf("test switch:");
            scanf(key);
                switch(key*1){
                    case 0:printf("key is 0");
                    case 1:printf("key is ",key);
                    case 2:printf("key is 2");
					default: ;
                }
            scanf(e);
                switch(e){
                    case 'a':printf(++2);
                    case 'b':printf(+2+-2);
					default: ;
                }

        }
        default: {
		}
    }


        nest();
        return ;
}
