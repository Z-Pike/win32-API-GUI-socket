#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "MainDlg.h"
#include <WinSock2.h>
#include <commdlg.h>
#include<process.h>
#include "commctrl.h" //高级控件都要加该头文件 
#pragma comment(lib, "comctl32")  
#pragma comment(lib,"Ws2_32.lib")
/********************/
//IDC_DIALOG1和IDC_DIALOG2的属性设置
//Styles下的Style设为Child，Border设为None
HINSTANCE hinstance;  //程序实例句柄  
HWND hDlg_intab[2]; //两个要载入到TAB控件中的对话框句柄  
HWND htabctrl;  // TAB控件的句柄  

BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//两个子窗口的窗口处理过程函数申明  
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  
typedef BOOL(WINAPI *DIALOGPROC)(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam); //定义一个 函数指针  
DIALOGPROC DlgProc[2]={tab1_dlg_proc,tab2_dlg_proc}; 
/********************/
int filesize(char * filepathname)//通过路径文件名，获取文件大小
{
	FILE * p=fopen(filepathname,"rb");
	if(!p)
	{
		return -1;
	}
	fseek(p,0L,SEEK_END);
	int filesize=ftell(p);
	fclose(p);
	return filesize;//字节数
}
struct Zmain//主要结构体 套接字 句柄 控制接收循环的标签
{
	SOCKET a;//套接字
	HWND hc;//句柄
	int lab;//控制接收循环的标签
	char filename[200];//发出部分使用的存储文件名的数组
};
struct Zmain Tb1m;//发出界面的全局数据
struct Zmain Tb2m;//接收界面的全局数据
unsigned int __stdcall Tb1function(LPVOID p)//发出界面分线程 传入参数为Zmain结构体
{
	//((struct Zmain *)p)-> 传入后的引用方式
	char tb1rcvbuf[1024];//主要接收缓冲区
	ZeroMemory(tb1rcvbuf,1024);//内存清零
	char tb1headcheck[32];//文件传输头消息
	char tb1filehouzhui[32];//传输文件的后缀名
	char tb1filesize[32];//传输文件的大小
	char filestartkey[]="filestar7";//约定的文件传输头消息
	int tb1fileheadcheck=1;//用于辅助比对文件传输头消息
	//char rant[10];//文件传输缓冲区
	int tb1filetranscheck=0; //获取文件传输套接字状态
	int tb1socketcheck = 0;//用于获取套接字状态
	while(((struct Zmain *)p)->lab)
	{
		tb1socketcheck=recv(((struct Zmain *)p)->a, tb1rcvbuf, 1024, 0);
		if((0==tb1socketcheck)||(SOCKET_ERROR==tb1socketcheck))
		{
			MessageBox(((struct Zmain *)p)->hc,TEXT("连接已断开"),"warning",MB_OK);
			break;
		}
		sscanf(tb1rcvbuf,"$%[^?]?%[^#]#%s",tb1headcheck,tb1filehouzhui,tb1filesize);
		tb1fileheadcheck=strcmp(filestartkey,tb1headcheck);
		if(0==tb1fileheadcheck)//接收文件
		{
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT5,TEXT("开始接收文件"));
			char tname[32];
			wsprintf(tname,"recv%s",tb1filehouzhui);
			//int tda=atoi(daxiao);
			FILE * tb1rfp = fopen(tname, "wb");
			char fileendkey[10]="#fi1eover";
			int tb1fileendcheck=1;
			while ((tb1filetranscheck = recv(((struct Zmain *)p)->a, tb1rcvbuf, 1024, 0)) > 0)//still waiting,no ending.
			{
				tb1fileendcheck=strcmp(tb1rcvbuf,fileendkey);
				if(0==tb1fileendcheck)
				{
					break;
				}
				if (fwrite(tb1rcvbuf, 1, tb1filetranscheck, tb1rfp) < tb1filetranscheck)
				{
					MessageBox(NULL,"Write Failed","Failed",MB_OK);
					break;
				}
			ZeroMemory(tb1rcvbuf,1024);
			}
			fclose(tb1rfp);
			
			//MessageBox(NULL,TEXT("recver Successful!"),TEXT("Successful!"),MB_OK);
			tb1fileheadcheck=1;
			ZeroMemory(tb1rcvbuf,1024);
			ZeroMemory(tb1headcheck,32);
			ZeroMemory(tb1filesize,32);
			ZeroMemory(tb1filehouzhui,32);

		}
		else
		{
			//recv(((struct Zmain *)p)->a, rcbf, 1024, 0);
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT1,tb1rcvbuf);
			ZeroMemory(tb1rcvbuf,1024);	
		}

	}
	//SOCKET tf=((struct Zmain *)p)->a;
	//HWND th=((struct Zmain *)p)->hc;
/**********************************************************************************************************************/
	return 0;
}
unsigned int __stdcall Tb2function(LPVOID p)
{
	char jpor[64];
	int svp;
	ZeroMemory(jpor,64);
	GetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT4,jpor,64);
	svp=atoi(jpor);
	sockaddr_in jie;
	jie.sin_family=AF_INET;
	jie.sin_addr.S_un.S_addr=INADDR_ANY;
	jie.sin_port=htons(svp);


	// 绑定这个套节字到一个本地地址
	if(::bind(((struct Zmain *)p)->a, (LPSOCKADDR)&jie, sizeof(jie)) == SOCKET_ERROR)
	{
		MessageBox(NULL,TEXT("bind() Failed"),TEXT("Failed"),MB_OK);
		return 0;
	}
		
	// 进入监听模式
	if(::listen(((struct Zmain *)p)->a, 2) == SOCKET_ERROR)
	{
		MessageBox(NULL,TEXT("listen() Failed"),TEXT("Failed"),MB_OK);
		return 0;
	}
	sockaddr_in acp;
	int lec=sizeof(acp);
	SOCKET sc;
	sc= ::accept(((struct Zmain *)p)->a, (SOCKADDR*)&acp, &lec);
	if(sc== INVALID_SOCKET)
	{
		MessageBox(NULL,TEXT("accept() Failed"),TEXT("Failed"),MB_OK);
	}
	SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT3,inet_ntoa(acp.sin_addr));
	closesocket(((struct Zmain *)p)->a);
	((struct Zmain *)p)->a=sc;

    char tb2rcvbuf[1024];
	ZeroMemory(tb2rcvbuf,1024);
	char tb2headcheck[32];//文件传输头消息
	char tb2filehouzhui[32];//传输文件的后缀名
	char tb2filesize[32];
	char filestartkey[]="filestar7";
	int tb2fileheadcheck=1;
	//char ant[10];
	int tb2filetranscheck=0; 
	int tb2socketcheck = 0;
	while(((struct Zmain *)p)->lab)
	{
		//MessageBox(NULL,sebf,"aaa",MB_OK);
		tb2socketcheck=recv(((struct Zmain *)p)->a, tb2rcvbuf, 1024, 0);
		if((0==tb2socketcheck)||(SOCKET_ERROR==tb2socketcheck))//接收文件
		{
			MessageBox(((struct Zmain *)p)->hc,TEXT("连接已断开"),"warning",MB_OK);
			break;
		}
		//MessageBox(NULL,sebf,"bbb",MB_OK);
		sscanf(tb2rcvbuf,"$%[^?]?%[^#]#%s",tb2headcheck,tb2filehouzhui,tb2filesize);
		tb2fileheadcheck=strcmp(filestartkey,tb2headcheck);
		if(0==tb2fileheadcheck)
		{
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT5,TEXT("开始接收文件"));
			char tname[32];
			wsprintf(tname,"recv%s",tb2filehouzhui);
			//int tda=atoi(sdaxiao);
			FILE * tb2rfp = fopen(tname, "wb"); //windows??"wb",??????????????
			
			char fileendkey[10]="#fi1eover";
			int tb2fileendcheck=1;

			while ((tb2filetranscheck = recv(((struct Zmain *)p)->a, tb2rcvbuf, 1024, 0)) > 0)//still waiting,no ending.
			{
				tb2fileendcheck=strcmp(tb2rcvbuf,fileendkey);
				if(0==tb2fileendcheck)
				{
					break;
				}
				if (fwrite(tb2rcvbuf, sizeof(char), tb2filetranscheck, tb2rfp) < tb2filetranscheck)
				{
					MessageBox(NULL,"Write Failed","Failed",MB_OK);
					break;
				}
			ZeroMemory(tb2rcvbuf,1024);
			}
			fclose(tb2rfp);
			
			//MessageBox(NULL,TEXT("recver Successful!"),TEXT("Successful!"),MB_OK);
			tb2fileheadcheck=1;
			ZeroMemory(tb2rcvbuf,1024);
			ZeroMemory(tb2headcheck,32);
			ZeroMemory(tb2filehouzhui,32);
			ZeroMemory(tb2filesize,32);
		}
		else
		{
			//recv(sc, sebf, 1024, 0);
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT1,tb2rcvbuf);
			ZeroMemory(tb2rcvbuf,1024);
		}
			
	}
	return 0;
}
unsigned int __stdcall SendFileThread(LPVOID p)
{
	
	int frdcount=0;
	int sendfilesize=filesize(((struct Zmain *)p)->filename);
	char *houzhuiming=strchr(((struct Zmain *)p)->filename,'.');
	char filehouzhui[32];
	char sendfilebuf[1024];
	char headmessage[256];
	/****************************************************///progressbar
	HWND hwndPB=GetDlgItem(((struct Zmain *)p)->hc,IDC_PROGRESS1);
	PBRANGE range;
	SendMessage( hwndPB, PBM_SETRANGE,(WPARAM)0, (LPARAM)(MAKELPARAM(0,1000000)));//进度条范围0-100000(100000-10M)(10000-1M)(1000-1k)
	//SendMessage( hwndPB, PBM_SETRANGE,(WPARAM)0, (LPARAM)(MAKELPARAM(0,sendfilesize)));
	SendMessage( hwndPB, PBM_GETRANGE,(WPARAM)TRUE,(LPARAM)&range );
	/****************************************************/
	ZeroMemory(sendfilebuf,1024);
	ZeroMemory(filehouzhui,32);
	ZeroMemory(headmessage,256);
	wsprintf(filehouzhui,"%s",houzhuiming);
	wsprintf(headmessage,"$filestar7?%s#%d",filehouzhui,sendfilesize);
	send(((struct Zmain *)p)->a,headmessage,lstrlen(headmessage),0);//发送头消息
	char endmessage[10]="#fi1eover";
	FILE * sendfilep = fopen(((struct Zmain *)p)->filename, "rb");  //windows下是"rb",表示打开一个只读的二进制文件				
	while ((frdcount = fread(sendfilebuf, 1, 1024, sendfilep )) > 0)
	{
		if (send(((struct Zmain *)p)->a, sendfilebuf, frdcount, 0) < 0)
		{
			MessageBox(NULL,TEXT("Send File失败"),TEXT("失败"),MB_OK);
			break;
		}
		else//设置进度条(WPARAM)(100*(frdcount/sendfilesize))
		{
			SendMessage(hwndPB,PBM_DELTAPOS,(WPARAM)((1000000*frdcount)/sendfilesize),(LPARAM)0);//进度条增长
			//SendMessage(hwndPB,PBM_DELTAPOS,(WPARAM)frdcount,(LPARAM)0);
		}
		ZeroMemory(sendfilebuf,1024);
	}
	fclose(sendfilep);
	MessageBox(((struct Zmain *)p)->hc,TEXT("Transfer Successful! wait few seconds for EndMessage"),TEXT("Successful!"),MB_OK);
	SendMessage(hwndPB,PBM_SETPOS,(WPARAM)0,(LPARAM)0);//进度条归零
	send(((struct Zmain *)p)->a, endmessage, 9, 0) ;//must wait!!!!!!!!  why?
	return 0;
}
BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/********************/
	hinstance=GetModuleHandle(NULL);//获取程序实例?传给全局变量hinstance
	/********************/

    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);

		/********************/
		case WM_NOTIFY:      //TAB控件切换发生时传送的消息  
		{     
			if((INT)wParam==IDC_TAB1) //这里也可以用一个NMHDR *nm = (NMHDR *)lParam这个指针来获取 句柄和事件  
			{                   //读者可自行查找NMHDR结构  
				if(((LPNMHDR)lParam)->code==TCN_SELCHANGE) //当TAB标签转换的时候发送TCN_SELCHANGE消息  
				{     
						
					int sel=TabCtrl_GetCurSel(htabctrl);  
					switch(sel)   //根据索引值查找相应的标签值，干相应的事情  
					{     
							
						case 0: //TAB1标签时，我们要显示 tab1标签页面  
						{  
							ShowWindow(hDlg_intab[0],TRUE); //显示窗口用ShowWindow函数  
							ShowWindow(hDlg_intab[1],FALSE);  
							break;  
						}  
						case 1://TAB2标签时，我们要显示 tab2标签页面  
						{  
							ShowWindow(hDlg_intab[0],FALSE);  
							ShowWindow(hDlg_intab[1],TRUE);  
							break;  
						}  
					}  
				}  
			}  
			break;  
		}
		/********************/
    }

    return FALSE;
}

BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	/********************/
	////////////////////////////////////////////////////////////  显示tab标签  
    htabctrl=GetDlgItem(hwnd,IDC_TAB1);  
    TCITEM tie;//设置tab标签的属性  
    LPSTR tabname[2]={"发出","接入"}; //定义一个二维数组 存放tab标签名字  
    //具体开始设置 tie的字段 Mask psztext,ccxtextmax,image,lparam  
    tie.mask=TCIF_TEXT|TCIF_IMAGE;//psztext字段有效  
      tie.iImage = -1;   
    for(INT i=0;i<2;i++)  
    {  
        tie.pszText=tabname[i];  
        TabCtrl_InsertItem(htabctrl,i,&tie);  
    }  
    //////////////////////////////////////////////////////////////  
     
    //在tab标签中显示 别的控件  
    RECT rect;//存放tab控件的区域位置  
    GetClientRect(htabctrl,&rect);  
    // 将两个窗口往 tab控件位置移动  
    for(i=0;i<2;i++)  
    {  
        hDlg_intab[i]=CreateDialog(hinstance,MAKEINTRESOURCE(IDD_DIALOG1+i),htabctrl,DlgProc[i]);  
        MoveWindow(hDlg_intab[i],2,29,rect.right - rect.left-6,rect.bottom - rect.top-35,FALSE);  
    }  
  
    ShowWindow(hDlg_intab[0],SW_SHOW);
	/********************/
	WSADATA text1;//初始化套接字环境
    WORD ban=MAKEWORD(2,0);
    WSAStartup(ban,&text1);

    return TRUE;
}

void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)//主界面消息循环
{
	
    switch(id)
    {
		case MID_XIAOXI:
		{
			MessageBox(NULL,TEXT("文件开始消息$filestar7?后缀#大小\n文件结束消息#fi1eover"),TEXT("内部消息"),MB_OK);
			break;
		}
		case MID_JIESHAO:
		{
			MessageBox(NULL,TEXT("此程序能传输文本信息与文件。\n主要分为两部分,发出与接入。\n发出部分，建立对目标的连接然后传输。\n接入部分，等待接收外来连接并传输。"),TEXT("介绍与帮助"),MB_OK);
			break;
		}
        //break;
    	//default:
		//break;
    }
}

void Main_OnClose(HWND hwnd)//右上角红叉
{
	Tb1m.lab=0;
	Tb2m.lab=0;
    closesocket(Tb1m.a);
    closesocket(Tb2m.a);
    WSACleanup();
    EndDialog(hwnd, 0);
}

/********************/

BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//发出界面消息循环
{   
	
    switch(uMsg)  
    {     
        case WM_INITDIALOG:  
            return TRUE;  
        case WM_COMMAND:
			{
				
				switch(wParam)  
				{  
					case IDC_BTN1:  
					{ 
						Tb1m.a=socket(AF_INET,SOCK_STREAM,0);
						Tb1m.hc=hwnd;
						Tb1m.lab=1;
						if(Tb1m.a== INVALID_SOCKET)
						{
							MessageBox(hwnd,TEXT("创建套接字失败"),TEXT("失败"),MB_OK);
							return 0;
						}
						char tb1ip[128];//target ip
						char tb1port[64];//target port
						ZeroMemory(tb1ip,128);
						ZeroMemory(tb1port,64);
						int tb1portnum;//char port to int port
						GetDlgItemText(hwnd,IDC_EDIT3,tb1ip,128);
						GetDlgItemText(hwnd,IDC_EDIT4,tb1port,64);
						tb1portnum=atoi(tb1port);
						sockaddr_in xun;
						xun.sin_family=AF_INET;
						xun.sin_addr.S_un.S_addr=inet_addr(tb1ip);
						xun.sin_port=htons(tb1portnum);
						if(connect(Tb1m.a, (sockaddr*)&xun, sizeof(xun)) == -1)
						{
							MessageBox(hwnd,TEXT("connect失败"),TEXT("失败"),MB_OK);
							return 0;
						}
						else
						{
							_beginthreadex(NULL,0,Tb1function,(LPVOID)&Tb1m,0,NULL);
							MessageBox(hwnd,TEXT("连接成功"),TEXT("OK"),MB_OK);
						}
						break;  
					}
					case IDC_BUTTON1:
					{
						
						char tb1sendbuf[1024];
						ZeroMemory(tb1sendbuf,1024);
						GetDlgItemText(hwnd,IDC_EDIT2,tb1sendbuf,1024);
						send(Tb1m.a,tb1sendbuf,lstrlen(tb1sendbuf),0);
						break;
					}
					case IDC_BUTTON3:
					{
						OPENFILENAME tb1fs;
						ZeroMemory(&tb1fs,sizeof(tb1fs));
						tb1fs.lStructSize=sizeof(tb1fs);
						tb1fs.lpstrFile=Tb1m.filename;
						tb1fs.lpstrFile[0]=TEXT('\0');
						tb1fs.nMaxFile=200;
						tb1fs.lpstrFilter=TEXT("ALL\0*.*\0Text\0*.TXT\0cpp\0*.cpp\0");
						tb1fs.nFilterIndex=1;
						tb1fs.lpstrFileTitle=NULL;
						tb1fs.nMaxFileTitle=0;
						tb1fs.lpstrInitialDir=NULL;
						tb1fs.hwndOwner=hwnd;
						tb1fs.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
						if(GetOpenFileName(&tb1fs))
						{
							char nowmessage[210];
							wsprintf(nowmessage,"选中%s",Tb1m.filename);
							SetDlgItemText(hwnd,IDC_EDIT5,nowmessage);
						}
						break;
					}
					case IDC_BUTTON4://主线程不可阻塞，发送文件也得创建线程
					{
					
						_beginthreadex(NULL,0,SendFileThread,(LPVOID)&Tb1m,0,NULL);
						break;
					}
					case IDC_BUTTON5://MD5
					{
						char buffer[128];
	
						getFileMD5(Tb1m.filename, buffer);
						SetDlgItemText(hwnd,IDC_EDIT6,buffer);
						break;
					}
					case IDC_BUTTON2:
					{
						Tb1m.lab=0;
						closesocket(Tb1m.a);
						SetDlgItemText(hwnd,IDC_EDIT3,TEXT("已断开"));
						break;
					}
				}  
				break;  
			}    
        case WM_CLOSE:  
            {
				EndDialog(hwnd,0);    
				return FALSE;
			}
    }  
    return FALSE;  
}  
 
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)    
{  
    switch(uMsg)  
    {     
        case WM_INITDIALOG:  
            return TRUE;  
        case WM_COMMAND:  
			{
				 
				 switch(wParam)  
				{  
					case IDC_BTN2:  
					{  
						Tb2m.a=socket(AF_INET,SOCK_STREAM,0);
						Tb2m.hc=hwnd;
						Tb2m.lab=1;
						if(Tb2m.a== INVALID_SOCKET)
						{
							MessageBox(NULL,TEXT("创建套接字失败"),TEXT("失败"),MB_OK);
							return 0;
						}
						else
						{
							SetDlgItemText(hwnd,IDC_EDIT3,TEXT("已准许接入"));
							_beginthreadex(NULL,0,Tb2function,(LPVOID)&Tb2m,0,NULL);
						}	
						break;  
					}  
					case IDC_BUTTON2:  
					{  
						char tb2sendbuf[1024];
						ZeroMemory(tb2sendbuf,1024);
						GetDlgItemText(hwnd,IDC_EDIT2,tb2sendbuf,1024);
						send(Tb2m.a,tb2sendbuf,lstrlen(tb2sendbuf),0);
							
						break;  
					}  
					case IDC_BUTTON3:  
					{ 
						OPENFILENAME as;
						ZeroMemory(&as,sizeof(as));
						as.lStructSize=sizeof(as);
						as.lpstrFile=Tb2m.filename;
						as.lpstrFile[0]=TEXT('\0');
						as.nMaxFile=200;
						as.lpstrFilter=TEXT("ALL\0*.*\0Text\0*.TXT\0cpp\0*.cpp\0");
						as.nFilterIndex=1;
						as.lpstrFileTitle=NULL;
						as.nMaxFileTitle=0;
						as.lpstrInitialDir=NULL;
						as.hwndOwner=hwnd;
						as.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
						if(GetOpenFileName(&as))
						{
							char nowmessage[210];
							wsprintf(nowmessage,"选中%s",Tb2m.filename);
							SetDlgItemText(hwnd,IDC_EDIT5,nowmessage);
						}
						break;  
					}  
					case IDC_BUTTON4: //发送文件 
					{  
					  
						_beginthreadex(NULL,0,SendFileThread,(LPVOID)&Tb2m,0,NULL);
						break;  
					}  
					case IDC_BUTTON5://MD5
					{
						char abuffer[128];
	
						getFileMD5(Tb2m.filename, abuffer);
						SetDlgItemText(hwnd,IDC_EDIT6,abuffer);
						break;
						
						break;
					}
					case IDC_BUTTON1:  
					{ 
						Tb2m.lab=0;
						closesocket(Tb2m.a);
						SetDlgItemText(hwnd,IDC_EDIT3,TEXT("关闭状态"));
						break;  
					}  
				
				}  
				break;
			}
              
        case WM_CLOSE:
			{
				EndDialog(hwnd,0);  
				return FALSE;  
			}
            
    }  
    return FALSE;  
}

