#include<iostream>
#include<cstring>
using namespace std;
#define ull unsigned long long
//将ull型的公钥e-n转为字符串out，ull是8B，十进制下最多20bit，所以Out需要40个字符
//例如：Out="               18089          2312472469"
void en2str(char *Out,ull e,ull n){
    for(int i=0;i<40;++i){//前面的位用空格填充
        Out[i]=' ';
    }
    int Lcnt=19,Rcnt=39;
    while(e){
        Out[Lcnt--]=e%10+'0';
        e/=10;
    }
    while(n){
        Out[Rcnt--]=n%10+'0';
        n/=10;
    }
}
//获得子串[left,right)
void get_substr(char *Out,char *In,int left,int right){
    int cnt=0;
    for(int i=left;i<right;++i){
        Out[cnt++]=In[i];
    }
    Out[cnt]='\0';
}
//字符串数字转ull "526"->526
ull str2ull(const char *str){
    int n=strlen(str);
    ull res=0;
    for(int i=0;i<n;++i){
        res=res*10+str[i]-'0';
    }
    return res;
}
//ull转为20位字符串Out
void ull2str(char *Out,ull In){
    for(int i=0;i<20;++i){//前面的位用空格填充
        Out[i]=' ';
    }
    int cnt=19;
    while(In){
        Out[cnt--]=In%10+'0';
        In/=10;
    }
    Out[20]='\0';
}