#pragma once


// notice4ROI �Ի���

class notice4ROI : public CDialogEx
{
	DECLARE_DYNAMIC(notice4ROI)

public:
	notice4ROI(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~notice4ROI();

// �Ի�������
	enum { IDD = IDD_notice4ROI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
