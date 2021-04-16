#ifndef _RSA_H_
#define _RSA_H_

#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;
#define ull unsigned long long
//生成[left,right)的随机数
int random(int left,int right){
    int res=left+rand()%right;
    return res;
}
//模乘运算：(xy)%n
ull MulMod(ull a, ull b,ull n){
    return (a%n)*(b%n)%n;
}
//模幂运算：(base^pow)%n
ull PowMod(ull base,ull pow,ull n){
    ull a=base,b=pow,c=1;
    while(b){
        while(!(b&1)){
            b>>=1;
            a=MulMod(a,a,n);
        }
        b--;
        c=MulMod(a,c,n);
    }
    return c;
}
//RabinMiller检验，生成随机大质数
long RabinMillerKnl(ull &n){
    ull a,q,k,v;
    q=n-1;
    k=0;
    //只要q不是奇数
    while(!(q&1)){
        ++k;
        q>>=1;
    }
    a=random(2,n-1);
    v=PowMod(a,q,n);
    if(v==1)return 1;//是质数
    //循环检验是否为质数
    for(int j=0;j<k;++j){
        unsigned int z=1;
        for(int w=0;w<j;++w){
            z<<=1;
        }
        if(PowMod(a,z*q,n)==n-1){
            return 1;
        }
    }
    return 0;//不是质数
}
//多次重复检验，保证可靠性
long RabinMiller(ull &n,long loop=100){
    for(long i=0;i<loop;++i){
        if(!RabinMillerKnl(n)){
            return 0;
        }
    }
    return 1;
}
//质数生成的入口
ull RandomPrime(char bits){
    ull base;
    //重复，直到获得奇数
    do{
        base=(unsigned long)1<<(bits-1);//保证最高位是1  
        base+=random(0,base);
        base|=1;//随机生成一个奇数
    }while(!RabinMiller(base,30));
    return base;
}

//辗转相除法求最大公约数
ull Gcd(ull &p,ull &q){
    ull a=p>q?p:q;
    ull b=p<q?p:q;
    ull t;
    if(p==q){
        return p;
    }
    else{
        while(b){
            a=a%b;
            t=a;
            a=b;
            b=t;
        }
        return a;
    }
}

//根据d*e%phi(n)=1求出私钥d-n的d，phi_n是欧拉函数
ull Euclid(ull e,ull phi_n){
    ull Max=0xffffffffffffffff-phi_n;//阈值
    ull i=1;
    //不断尝试phi_n的倍数
    while(1){
        if((i*phi_n+1)%e==0){//i*phi(n)=d*e-1
            return (i*phi_n+1)/e;//返回d
        }
        ++i;
        ull tmp=(i+1)*phi_n;
        if(tmp>Max){//超过阈值，没找到d
            return 0;
        }
    }
    return 0;
}

//加密：C=M^e%n
ull Encry(unsigned short M,ull e,ull n){
    return PowMod(M,e,n);
}
//解密：M=C^d%n
unsigned short Decry(ull C,ull d,ull n){
    ull nRes=PowMod(C,d,n);//64bit
    unsigned short *pRes=(unsigned short*)&(nRes);//把nRes分割成四个16bit的数组
    //明文最多只有16bit
    if(pRes[1]||pRes[2]||pRes[3]){
        return 0;
    }
    else{
        return pRes[0];
    }
}

struct RSA{
    ull p,q,n,f,e,d,s;
    void print(){
        cout<<"p="<<p<<endl;
        cout<<"q="<<q<<endl;
        cout<<"n="<<n<<endl;
        cout<<"f="<<f<<endl;
        cout<<"e="<<e<<endl;
        cout<<"d="<<d<<endl;
        cout<<"s="<<s<<endl;
    }
};
//生成RSA公钥en-私钥dn所需的所有信息
RSA RsaGetParam(){
    RSA rsa;
    rsa.p=RandomPrime(16);
    rsa.q=RandomPrime(16);
    rsa.n=rsa.p*rsa.q;
    rsa.f=(rsa.p-1)*(rsa.q-1);//欧拉函数，phi_n
    do{
        rsa.e=random(0,65536);
        rsa.e|=1;
    }while(Gcd(rsa.e,rsa.f)!=1);

    rsa.d=Euclid(rsa.e,rsa.f);
    rsa.s=0;//n的位数
    ull t=rsa.n>>1;
    while(t){
        rsa.s++;
        t>>=1;
    }
    return rsa;
}

#endif