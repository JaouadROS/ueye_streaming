#pragma once
#include "uEye.h"
#include "afxwin.h"

class CuEyeSimpleTriggerDlg : public CDialog
{
 private:
	virtual void DoDataExchange(CDataExchange* pDX);

	HIDS	    m_hCam;				
	HWND	    m_hWndDisplay;		
	INT		    m_nColorMode;		// Y8/RGB16/RGB24/REG32
	INT		    m_nBitsPerPixel;	
	INT		    width_image;
	INT		    height_image;
	INT		    m_nSizeY;			
	INT		    left_offset;			
	INT		    right_offset;	
    INT         nFrame;
    BOOL        live;     
	CAMINFO		m_ci;


	// Memory needed for live display while using DIB
	INT		    m_lMemoryId;		// camera memory - buffer ID
	char*	    m_pcImageMemory;	// camera memory - pointer to buffer
	SENSORINFO  m_sInfo;			// sensor information struct
    INT         m_nRenderMode;		// render  mode
    INT         m_nFlipHor;			// horizontal flip flag
    INT         m_nFlipVert;		// vertical flip flag
	
	int			counter;

	bool OpenCamera();
    void ExitCamera();
    int  InitDisplayMode();

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	afx_msg void OnBnClickedButtonLoadParameter();
	afx_msg void OnButtonExit();
	LRESULT OnUEyeMessage(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

 public:

    CuEyeSimpleTriggerDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_UEYESIMPLETRIGGER_DIALOG };

    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonResetCounters();
};

