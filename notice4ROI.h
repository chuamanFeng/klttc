#pragma once


// notice4ROI 对话框

class notice4ROI : public CDialogEx
{
	DECLARE_DYNAMIC(notice4ROI)

public:
	notice4ROI(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~notice4ROI();

// 对话框数据
	enum { IDD = IDD_notice4ROI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
