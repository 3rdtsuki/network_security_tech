#include"des.h"
#include"rsa.h"
#include"tool.h"
#include<Winsock2.h> 
#include<stdio.h>
#include<string.h>
#include<iostream>
#pragma comment(lib,"ws2_32")
#define SERVER_PORT 6666
using namespace std;

int main(){
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);
	
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);//创建服务器端socket
	struct sockaddr_in server_addr;//服务器端地址
	
	char buffer[500];
	int addrlen=sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);//服务器端口号定为6666
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockSrv,(SOCKADDR*)&server_addr,sizeof(SOCKADDR));//套接字绑定服务器端地址
	listen(sockSrv,5);//队列的最大长度为5
	
	
    printf("监听端口：%d\n",SERVER_PORT);
    struct sockaddr_in addrClient;//客户端地址
    SOCKET client = accept(sockSrv, (SOCKADDR*)&addrClient, &addrlen);//创建新套接字连接客户
    cout<<"当前客户的端口号："<<addrClient.sin_port<<endl;//输出当前客户的端口号

    
    //生成RSA公钥e-n和私钥d-n
    RSA rsa;
    srand(time(NULL));
    rsa=RsaGetParam();
    //把公钥发给客户端
    char e_n[40];
    en2str(e_n,rsa.e,rsa.n);//将ull型的RSA公钥转为字符串，存在e_n
    send(client, e_n, strlen(e_n), 0);//将公钥发给客户端
    cout<<"已将公钥发给客户端，e="<<rsa.e<<", n="<<rsa.n<<endl;
    Sleep(100);//等一下对面发送 
    
    //接收公钥加密后的DES密钥并用RSA私钥解密
    int len=recv(client, buffer, sizeof(buffer), 0);
    buffer[len]='\0';//这个必须要加，否则会输出奇怪的东西qwq 
    cout<<"收到加密后的DES密钥为"<<buffer<<endl;
    
    char key[9];//RSA解密后的DES密钥
    len=strlen(buffer);
    for(int i=0;i<len;i+=20){//20个字符对应一个ull（即密钥的一个字符的asc码）
        char temp[20];
        for(int j=0;j<20;++j){
        	if(buffer[i+j]==' ')continue;//去掉前面的空格 
        	get_substr(temp,buffer,i+j,i+20);
        	break;
		}
        //转为ull
        ull C=str2ull(temp);
        cout<<"C="<<C<<endl;
        //RSA解密
		unsigned short asc=Decry(C,rsa.d,rsa.n);
		cout<<asc<<endl;
        key[i/20]=(char)asc;
    }
    
    key[8]='\0'; 
    cout<<"DES密钥解密后为"<<key<<endl;
    Des_SetKey(key);

    while(1){
        len=recv(client, buffer, sizeof(buffer), 0);
        buffer[len]='\0';//后面的不要了
        //DES解密
        DES_Decrypt(buffer,buffer);

        if(strcmp(buffer,"quit")==0)break;
        printf("接收到消息：%s",buffer);
        printf("\n");	


        printf("发送消息：");
        scanf("%s",buffer);
        if(strcmp(buffer,"quit")==0)break;
        //DES加密
        DES_Encrypt(buffer,buffer);

        send(client, buffer, strlen(buffer), 0);
        cout<<"等待接收信息中...\n"; 
    }
	
	closesocket(sockSrv);
	WSACleanup();

}
//g++ server.cpp -o server.exe -lwsock32
