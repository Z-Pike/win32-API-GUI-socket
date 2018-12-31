#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "MainDlg.h"
#include <WinSock2.h>
#include <commdlg.h>
#include<process.h>
#include "commctrl.h" //�߼��ؼ���Ҫ�Ӹ�ͷ�ļ� 
#pragma comment(lib, "comctl32")  
#pragma comment(lib,"Ws2_32.lib")
/********************/
//IDC_DIALOG1��IDC_DIALOG2����������
//Styles�µ�Style��ΪChild��Border��ΪNone
HINSTANCE hinstance;  //����ʵ�����  
HWND hDlg_intab[2]; //����Ҫ���뵽TAB�ؼ��еĶԻ�����  
HWND htabctrl;  // TAB�ؼ��ľ��  

BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//�����Ӵ��ڵĴ��ڴ�����̺�������  
BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  
typedef BOOL(WINAPI *DIALOGPROC)(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam); //����һ�� ����ָ��  
DIALOGPROC DlgProc[2]={tab1_dlg_proc,tab2_dlg_proc}; 
/********************/
int filesize(char * filepathname)//ͨ��·���ļ�������ȡ�ļ���С
{
	FILE * p=fopen(filepathname,"rb");
	if(!p)
	{
		return -1;
	}
	fseek(p,0L,SEEK_END);
	int filesize=ftell(p);
	fclose(p);
	return filesize;//�ֽ���
}
struct Zmain//��Ҫ�ṹ�� �׽��� ��� ���ƽ���ѭ���ı�ǩ
{
	SOCKET a;//�׽���
	HWND hc;//���
	int lab;//���ƽ���ѭ���ı�ǩ
	char filename[200];//��������ʹ�õĴ洢�ļ���������
};
struct Zmain Tb1m;//���������ȫ������
struct Zmain Tb2m;//���ս����ȫ������
unsigned int __stdcall Tb1function(LPVOID p)//����������߳� �������ΪZmain�ṹ��
{
	//((struct Zmain *)p)-> ���������÷�ʽ
	char tb1rcvbuf[1024];//��Ҫ���ջ�����
	ZeroMemory(tb1rcvbuf,1024);//�ڴ�����
	char tb1headcheck[32];//�ļ�����ͷ��Ϣ
	char tb1filehouzhui[32];//�����ļ��ĺ�׺��
	char tb1filesize[32];//�����ļ��Ĵ�С
	char filestartkey[]="filestar7";//Լ�����ļ�����ͷ��Ϣ
	int tb1fileheadcheck=1;//���ڸ����ȶ��ļ�����ͷ��Ϣ
	//char rant[10];//�ļ����仺����
	int tb1filetranscheck=0; //��ȡ�ļ������׽���״̬
	int tb1socketcheck = 0;//���ڻ�ȡ�׽���״̬
	while(((struct Zmain *)p)->lab)
	{
		tb1socketcheck=recv(((struct Zmain *)p)->a, tb1rcvbuf, 1024, 0);
		if((0==tb1socketcheck)||(SOCKET_ERROR==tb1socketcheck))
		{
			MessageBox(((struct Zmain *)p)->hc,TEXT("�����ѶϿ�"),"warning",MB_OK);
			break;
		}
		sscanf(tb1rcvbuf,"$%[^?]?%[^#]#%s",tb1headcheck,tb1filehouzhui,tb1filesize);
		tb1fileheadcheck=strcmp(filestartkey,tb1headcheck);
		if(0==tb1fileheadcheck)//�����ļ�
		{
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT5,TEXT("��ʼ�����ļ�"));
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


	// ������׽��ֵ�һ�����ص�ַ
	if(::bind(((struct Zmain *)p)->a, (LPSOCKADDR)&jie, sizeof(jie)) == SOCKET_ERROR)
	{
		MessageBox(NULL,TEXT("bind() Failed"),TEXT("Failed"),MB_OK);
		return 0;
	}
		
	// �������ģʽ
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
	char tb2headcheck[32];//�ļ�����ͷ��Ϣ
	char tb2filehouzhui[32];//�����ļ��ĺ�׺��
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
		if((0==tb2socketcheck)||(SOCKET_ERROR==tb2socketcheck))//�����ļ�
		{
			MessageBox(((struct Zmain *)p)->hc,TEXT("�����ѶϿ�"),"warning",MB_OK);
			break;
		}
		//MessageBox(NULL,sebf,"bbb",MB_OK);
		sscanf(tb2rcvbuf,"$%[^?]?%[^#]#%s",tb2headcheck,tb2filehouzhui,tb2filesize);
		tb2fileheadcheck=strcmp(filestartkey,tb2headcheck);
		if(0==tb2fileheadcheck)
		{
			SetDlgItemText(((struct Zmain *)p)->hc,IDC_EDIT5,TEXT("��ʼ�����ļ�"));
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
	SendMessage( hwndPB, PBM_SETRANGE,(WPARAM)0, (LPARAM)(MAKELPARAM(0,1000000)));//��������Χ0-100000(100000-10M)(10000-1M)(1000-1k)
	//SendMessage( hwndPB, PBM_SETRANGE,(WPARAM)0, (LPARAM)(MAKELPARAM(0,sendfilesize)));
	SendMessage( hwndPB, PBM_GETRANGE,(WPARAM)TRUE,(LPARAM)&range );
	/****************************************************/
	ZeroMemory(sendfilebuf,1024);
	ZeroMemory(filehouzhui,32);
	ZeroMemory(headmessage,256);
	wsprintf(filehouzhui,"%s",houzhuiming);
	wsprintf(headmessage,"$filestar7?%s#%d",filehouzhui,sendfilesize);
	send(((struct Zmain *)p)->a,headmessage,lstrlen(headmessage),0);//����ͷ��Ϣ
	char endmessage[10]="#fi1eover";
	FILE * sendfilep = fopen(((struct Zmain *)p)->filename, "rb");  //windows����"rb",��ʾ��һ��ֻ���Ķ������ļ�				
	while ((frdcount = fread(sendfilebuf, 1, 1024, sendfilep )) > 0)
	{
		if (send(((struct Zmain *)p)->a, sendfilebuf, frdcount, 0) < 0)
		{
			MessageBox(NULL,TEXT("Send Fileʧ��"),TEXT("ʧ��"),MB_OK);
			break;
		}
		else//���ý�����(WPARAM)(100*(frdcount/sendfilesize))
		{
			SendMessage(hwndPB,PBM_DELTAPOS,(WPARAM)((1000000*frdcount)/sendfilesize),(LPARAM)0);//����������
			//SendMessage(hwndPB,PBM_DELTAPOS,(WPARAM)frdcount,(LPARAM)0);
		}
		ZeroMemory(sendfilebuf,1024);
	}
	fclose(sendfilep);
	MessageBox(((struct Zmain *)p)->hc,TEXT("Transfer Successful! wait few seconds for EndMessage"),TEXT("Successful!"),MB_OK);
	SendMessage(hwndPB,PBM_SETPOS,(WPARAM)0,(LPARAM)0);//����������
	send(((struct Zmain *)p)->a, endmessage, 9, 0) ;//must wait!!!!!!!!  why?
	return 0;
}
BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/********************/
	hinstance=GetModuleHandle(NULL);//��ȡ����ʵ��?����ȫ�ֱ���hinstance
	/********************/

    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);

		/********************/
		case WM_NOTIFY:      //TAB�ؼ��л�����ʱ���͵���Ϣ  
		{     
			if((INT)wParam==IDC_TAB1) //����Ҳ������һ��NMHDR *nm = (NMHDR *)lParam���ָ������ȡ ������¼�  
			{                   //���߿����в���NMHDR�ṹ  
				if(((LPNMHDR)lParam)->code==TCN_SELCHANGE) //��TAB��ǩת����ʱ����TCN_SELCHANGE��Ϣ  
				{     
						
					int sel=TabCtrl_GetCurSel(htabctrl);  
					switch(sel)   //��������ֵ������Ӧ�ı�ǩֵ������Ӧ������  
					{     
							
						case 0: //TAB1��ǩʱ������Ҫ��ʾ tab1��ǩҳ��  
						{  
							ShowWindow(hDlg_intab[0],TRUE); //��ʾ������ShowWindow����  
							ShowWindow(hDlg_intab[1],FALSE);  
							break;  
						}  
						case 1://TAB2��ǩʱ������Ҫ��ʾ tab2��ǩҳ��  
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
	////////////////////////////////////////////////////////////  ��ʾtab��ǩ  
    htabctrl=GetDlgItem(hwnd,IDC_TAB1);  
    TCITEM tie;//����tab��ǩ������  
    LPSTR tabname[2]={"����","����"}; //����һ����ά���� ���tab��ǩ����  
    //���忪ʼ���� tie���ֶ� Mask psztext,ccxtextmax,image,lparam  
    tie.mask=TCIF_TEXT|TCIF_IMAGE;//psztext�ֶ���Ч  
      tie.iImage = -1;   
    for(INT i=0;i<2;i++)  
    {  
        tie.pszText=tabname[i];  
        TabCtrl_InsertItem(htabctrl,i,&tie);  
    }  
    //////////////////////////////////////////////////////////////  
     
    //��tab��ǩ����ʾ ��Ŀؼ�  
    RECT rect;//���tab�ؼ�������λ��  
    GetClientRect(htabctrl,&rect);  
    // ������������ tab�ؼ�λ���ƶ�  
    for(i=0;i<2;i++)  
    {  
        hDlg_intab[i]=CreateDialog(hinstance,MAKEINTRESOURCE(IDD_DIALOG1+i),htabctrl,DlgProc[i]);  
        MoveWindow(hDlg_intab[i],2,29,rect.right - rect.left-6,rect.bottom - rect.top-35,FALSE);  
    }  
  
    ShowWindow(hDlg_intab[0],SW_SHOW);
	/********************/
	WSADATA text1;//��ʼ���׽��ֻ���
    WORD ban=MAKEWORD(2,0);
    WSAStartup(ban,&text1);

    return TRUE;
}

void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)//��������Ϣѭ��
{
	
    switch(id)
    {
		case MID_XIAOXI:
		{
			MessageBox(NULL,TEXT("�ļ���ʼ��Ϣ$filestar7?��׺#��С\n�ļ�������Ϣ#fi1eover"),TEXT("�ڲ���Ϣ"),MB_OK);
			break;
		}
		case MID_JIESHAO:
		{
			MessageBox(NULL,TEXT("�˳����ܴ����ı���Ϣ���ļ���\n��Ҫ��Ϊ������,��������롣\n�������֣�������Ŀ�������Ȼ���䡣\n���벿�֣��ȴ������������Ӳ����䡣"),TEXT("���������"),MB_OK);
			break;
		}
        //break;
    	//default:
		//break;
    }
}

void Main_OnClose(HWND hwnd)//���ϽǺ��
{
	Tb1m.lab=0;
	Tb2m.lab=0;
    closesocket(Tb1m.a);
    closesocket(Tb2m.a);
    WSACleanup();
    EndDialog(hwnd, 0);
}

/********************/

BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//����������Ϣѭ��
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
							MessageBox(hwnd,TEXT("�����׽���ʧ��"),TEXT("ʧ��"),MB_OK);
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
							MessageBox(hwnd,TEXT("connectʧ��"),TEXT("ʧ��"),MB_OK);
							return 0;
						}
						else
						{
							_beginthreadex(NULL,0,Tb1function,(LPVOID)&Tb1m,0,NULL);
							MessageBox(hwnd,TEXT("���ӳɹ�"),TEXT("OK"),MB_OK);
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
							wsprintf(nowmessage,"ѡ��%s",Tb1m.filename);
							SetDlgItemText(hwnd,IDC_EDIT5,nowmessage);
						}
						break;
					}
					case IDC_BUTTON4://���̲߳��������������ļ�Ҳ�ô����߳�
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
						SetDlgItemText(hwnd,IDC_EDIT3,TEXT("�ѶϿ�"));
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
							MessageBox(NULL,TEXT("�����׽���ʧ��"),TEXT("ʧ��"),MB_OK);
							return 0;
						}
						else
						{
							SetDlgItemText(hwnd,IDC_EDIT3,TEXT("��׼�����"));
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
							wsprintf(nowmessage,"ѡ��%s",Tb2m.filename);
							SetDlgItemText(hwnd,IDC_EDIT5,nowmessage);
						}
						break;  
					}  
					case IDC_BUTTON4: //�����ļ� 
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
						SetDlgItemText(hwnd,IDC_EDIT3,TEXT("�ر�״̬"));
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

