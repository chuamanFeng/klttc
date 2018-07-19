// choice4pause.cpp : 实现文件
//

#include "stdafx.h"
#include "KLT.h"
#include "choice4pause.h"
#include "afxdialogex.h"


// choice4pause 对话框

IMPLEMENT_DYNAMIC(choice4pause, CDialogEx)

choice4pause::choice4pause(CWnd* pParent /*=NULL*/)
	: CDialogEx(choice4pause::IDD, pParent)
{

}

choice4pause::~choice4pause()
{
}

void choice4pause::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(choice4pause, CDialogEx)
END_MESSAGE_MAP()


// choice4pause 消息处理程序
