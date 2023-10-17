
// MFCWebFileDownDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCWebFileDown.h"
#include "MFCWebFileDownDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <wininet.h>
#include <string>
#include <vector>
#include <fstream>
#define READ_BUF_SIZE    1024

#include "unzip.h"
#include "zip.h"
//1) CreateZip: *fn 문자열로 Zip파일을 생성한다.* password에는 파일의 암호를 추가할 수 있다.
//2) ZipAdd: CreateZip으로 생성한 Zip 파일에* dstzn 문자열의 이름으로* fn 문자열 위치에 있는 파일을 추가한다.
//3) CloseZip: 생성한 Zip파일의 핸들을 닫는다.
//1) Open: *fn 문자열 경로에 있는 Zip 파일을 불러온다.* password에는 파일의 암호를 입력한다.
//2) UnzipItem: 불러온 Zip 파일에서 index 숫자의 인덱스에 있는 파일을* fn 문자열 경로로 압축해제한다.
//3) CloseZip: Zip파일의 핸들을 닫는다.

// CMFCWebFileDownDlg 대화 상자



CMFCWebFileDownDlg::CMFCWebFileDownDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCWEBFILEDOWN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCWebFileDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCWebFileDownDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMFCWebFileDownDlg 메시지 처리기

BOOL CMFCWebFileDownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 파일 다운 받기
	// 만약 postman으로 보낼 주소가 (https://www2.atpos.co.kr/DownLoad/00002289.zip)이라면,
	// 다운받을 파일 이름과 포맷 지정
	m_szFileName = "00002289.zip";
	getFileFromHttp(_T("http://www2.atpos.co.kr/DownLoad/00002289.zip"), m_szFileName);

	// zip파일 압출 풀기
	UnzipProcess();



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCWebFileDownDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCWebFileDownDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CMFCWebFileDownDlg::getFileFromHttp(CString pszUrl, char* pszFile)
{
	// 1. 인터넷 연결
	HINTERNET hSession = InternetOpen(L"Test", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	// 입력된 HTTP주소를 열기
	HINTERNET hUrl = InternetOpenUrl(hSession, pszUrl, NULL, 0, 0, 0);
	// 인터넷 세션의 핸들
	// URL
	// HTTP server에 보내는 해더
	// 해더 사이즈
	// Flag
	// Context

	FILE* fp;
	DWORD dwReadSize = 0;

	// 다운로드할 파일 만들기
	if ((fopen_s(&fp, pszFile, "wb")) == 0)
	{
		TCHAR    szBuff[READ_BUF_SIZE];
		DWORD    dwSize;
		DWORD    dwDebug = 10;

		do {
			// 웹상의 파일 읽기
			InternetReadFile(hUrl, szBuff, READ_BUF_SIZE, &dwSize);

			// 웹상의 파일을 만들어진 파일에 써넣기
			fwrite(szBuff, dwSize, 1, fp);

			dwReadSize += dwSize;
		} while ((dwSize != 0) || (--dwDebug != 0));

		fclose(fp);
	}

	// 인터넷 핸들 닫기
	InternetCloseHandle(hUrl);

	// 인터넷 핸들 닫기
	InternetCloseHandle(hSession);

	return(dwReadSize);
}

void CMFCWebFileDownDlg::UnzipProcess()
{
	HZIP hz;
	ZRESULT zr;
	TCHAR strReadIni[200] = { 0 };
	CString strTotal = _T("");
	int nTotal = 0, nLen = 0, nPos = 0, nRet = 0;
	CString strTmp;

	//1
	hz = OpenZip(_T("D:\\Projec\\MFC\\MFCWebFileDown\\MFCWebFileDown\\00002289.zip"), 0);
	if (hz == 0)
	{
		AfxMessageBox(_T("Error: Failed to open Zip"));
		return;
	}

	//2
	zr = UnzipItem(hz, 0, _T("D:\\Projec\\MFC\\MFCWebFileDown\\MFCWebFileDown"));

	if (zr != ZR_OK)
	if (zr != ZR_OK)
	{
		AfxMessageBox(_T("UnZip Error"));
		zr = CloseZip(hz);
		return;
	}
}