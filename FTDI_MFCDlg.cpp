
// FTDI_MFCDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FTDI_MFC.h"
#include "FTDI_MFCDlg.h"
#include "afxdialogex.h"
#include "ftd2xx.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFTDIMFCDlg dialog



CFTDIMFCDlg::CFTDIMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTDI_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFTDIMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFTDIMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CFTDIMFCDlg message handlers

BOOL CFTDIMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	FT_HANDLE ftHandle;
	FT_STATUS ftStatus;
	ftStatus = FT_Open(0, &ftHandle);
	ftStatus |= FT_SetUSBParameters(ftHandle, 4096, 4096); // Set USB transfer sizes   
	ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0); // Disable event characters   
	ftStatus |= FT_SetTimeouts(ftHandle, 5000, 5000); // Set read/write timeouts to 5 sec   
	ftStatus |= FT_SetLatencyTimer(ftHandle, 16); // Latency timer at default 16ms  
	ftStatus |= FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0x11, 0x13); // No flow control  
	ftStatus |= FT_SetBaudRate(ftHandle, 9600);  // Baud rate = 9600  
	ftStatus |= FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
	if (ftStatus != FT_OK)
		//			printf("ftStatus not ok %d\n", ftStatus); //check for error  
		TRACE1("ftStatus not ok %d\n", ftStatus);
	else
	{
		char data_out[12] = "Hello World";
		DWORD w_data_len = 12;
		DWORD data_written;
		ftStatus = FT_Write(ftHandle, data_out, w_data_len, &data_written);

		char data_in[12];
		DWORD r_data_len = 12;
		DWORD data_read;
		ftStatus = FT_Read(ftHandle, data_in, r_data_len, &data_read);
		if (ftStatus != FT_OK)
			//printf("ftStatus not ok %d\n", ftStatus);
			TRACE1("ftStatus not ok %d\n", ftStatus);
		else
			TRACE1("Data Read:  %s\n", data_in);
		//printf("Data Read:  %s\n", data_in);
	}
	ftStatus = FT_Close(ftHandle);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFTDIMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTDIMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFTDIMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

