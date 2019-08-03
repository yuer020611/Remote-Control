#include <winsock2.h>
#include<stdio.h>
#include<iostream>
#include<string>
#include <cstring>
#include <fstream>
using namespace std;
#pragma comment(lib,"ws2_32.lib")//链接这个库 

//TCP服务器端 

void RecvFile(SOCKET sClient, string SaveFileName);
int connect();

int main()
{
	int temp = 10;
	while (true)
	{
		if (temp == 10)
		{
			temp = connect();
		}
		else
		{
		}
	}
	return 0;
}


int connect()
{
	//调用winsock 
	WORD sockVersion = MAKEWORD(2, 2);//请求使用的winsock版本 
	WSADATA wsaData;   // 实际返回的winsock版本  
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
	}
	//创建socket 
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//参数分别为协议族，类型，协议号 AF_INET代表TCP/IP 
	if (slisten == INVALID_SOCKET)//异常处理 
	{
		printf("scoket error!\n");
		return 10;
	}
	//bind
	sockaddr_in sin; //服务器端点地址 
	sin.sin_family = AF_INET; //协议族 
	sin.sin_port = htons(8888); //端口号， htons函数将本地字节顺序变为网络字节顺序（16位） 
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//服务器bind时需要使用地址通配
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)//LPSOCKADDR是类型强制转换 
	{
		printf("bind error !\n");
		return 10;
	}
	//listen
	if (listen(slisten, 5) == SOCKET_ERROR)//5为queuesize,缓存区大小 
	{ 
		printf("listen error !\n");
		return 10;
	}
	//由于使用的是TCP ，socket stream,要循环接收数据
	SOCKET sClient;//声明变量
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[1024];//buffer
	printf("waiting for connect...\n\n");
	while (true)
	{
		printf("command：");
		string data;
		getline(cin, data);//键盘读入数据
		const char* sendData;
		sendData = data.c_str(); //string变为const char*
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);//accept会新建一个socket 
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;//重新开始循环 
		}
		printf("someone ip: %s\r\n", inet_ntoa(remoteAddr.sin_addr)); //inet将ip地址结构转成字符串 ， \r是回车 
		//发送数据		
		send(sClient, sendData, strlen(sendData), 0);
		//接收数据 

		//接收文件
		if (data.find("pass") != string::npos)//pass-D:\\001.txt-D:\\002.txt
		{
			string savepath = data.substr(data.find_last_of("-") + 1);
			RecvFile(sClient, savepath);
		}
		//接收字符串
		else
		{
			int ret = recv(sClient, revData, 1024, 0);
			printf("feedback：\n");
			while (true)
			{
				if (ret > 0)
				{
					revData[ret] = 0x00;
					printf(revData);
					ret = recv(sClient, revData, 1024, 0);
				}
				break;
			}
			printf("\n\n");
		}
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
}


void RecvFile(SOCKET sClient, string SaveFileName)
{
	cout << "receive start" << endl;
	const int bufferSize = 1024;
	char buffer[bufferSize] = { 0 };
	int readLen = 0;
	//string SaveFileName ; //这是服务器要接收的文件的保存路径
	ofstream desFile;
	desFile.open(SaveFileName.c_str(), ios::binary);
	if (!desFile)
	{
		return;
	}
	do
	{
		readLen = recv(sClient, buffer, bufferSize, 0);
		if (readLen == 0)
		{
			break;
		}
		else
		{
			desFile.write(buffer, readLen);
			cout << "receiving" <<  endl;
		}
	} while (true);
	cout << "receive over\n\n" << endl;
	desFile.close();
}