#pragma once


// choice4pause �Ի���

class choice4pause : public CDialogEx
{
	DECLARE_DYNAMIC(choice4pause)

public:
	choice4pause(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~choice4pause();

// �Ի�������
	enum { IDD = IDD_choice4pause };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
