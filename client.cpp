#include"des.h"
#include"rsa.h"
#include"tool.h"
#include<Winsock2.h> 
#include<stdio.h>
#include<string.h>
#pragma comment(lib,"ws2_32")
#define SERVER_PORT 6666
RSA rsa;
void setRSA(char *e_n){
    ull e,n;
    for(int i=0;i<20;++i){
        if(e_n[i]==' ')continue;
        char e_str[20];
        get_substr(e_str,e_n,i,20);
        e=str2ull(e_str);
        break;
    }
    for(int i=20;i<40;++i){
        if(e_n[i]==' ')continue;
        char n_str[20];
        get_substr(n_str,e_n,i,40);
        n=str2ull(n_str);
        break;
    }
    rsa.e=e;
    rsa.n=n;
}
int main(){
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);

    char sendbuf[200],recvbuf[200];
    int serverSocket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//去和6666端口服务器连接
    connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    //生成DES密钥
    char key[]="abcdefgh";
    Des_SetKey(key);
    //接收RAS公钥，并进行解析，前20位是e，后20位是n
    char e_n[40];
    recv(serverSocket, e_n, 40, 0); 
    setRSA(e_n);//解析，将en写入全局rsa
    cout<<"已收到RAS公钥，e="<<rsa.e<<", n="<<rsa.n<<endl;

    //用RAS公钥加密DES密钥
    ull C=0;//密文
    unsigned short M;//因为明文是ushort型，2B（16bit），所以要将8字符密钥（64bit）分割来RAS加密，为了简便每个字符一组
    char C_str[500]="",temp[25];
    for(int i=0;i<8;++i){
        M=(unsigned short)key[i];
        C=Encry(M,rsa.e,rsa.n);
        ull2str(temp,C);
        cout<<"C="<<temp<<endl;
        strcat(C_str,temp);
    }
    cout<<"加密后的DES密钥为："<<C_str<<endl;
    send(serverSocket, C_str, strlen(C_str), 0);//发送DES密钥的RSA密文
    cout<<"已发送加密后的DES密钥"<<endl;
    
    
    while(1)
	{
		printf("发送消息：");
        scanf("%s",sendbuf);
		if(strcmp(sendbuf, "quit") == 0) break;
        //DES加密
        DES_Encrypt(sendbuf,sendbuf);

		send(serverSocket, sendbuf, strlen(sendbuf), 0); //向服务端发送消息
        cout<<"等待接收信息中...\n"; 


		printf("接受到消息：");
		int len=recv(serverSocket, recvbuf, 200, 0); //接收服务端发来的消息
		recvbuf[len]='\0';//后面的不要了
        //DES解密
        DES_Decrypt(recvbuf,recvbuf);
		printf("%s\n", recvbuf);
	}
    closesocket(serverSocket);
	WSACleanup();
}
