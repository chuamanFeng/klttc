#pragma once


// choice4pause 对话框

class choice4pause : public CDialogEx
{
	DECLARE_DYNAMIC(choice4pause)

public:
	choice4pause(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~choice4pause();

// 对话框数据
	enum { IDD = IDD_choice4pause };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
