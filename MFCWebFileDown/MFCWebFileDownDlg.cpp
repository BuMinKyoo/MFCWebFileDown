
// MFCWebFileDownDlg.cpp: 구현 파일



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
#define FILE_DOWNLOAD_FILE_DIR "C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\Download\\"

#include "XUnzip.h"
#include "XZip.h"
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
	//m_szFileName = "C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\Download\\00002289.zip";
	//getFileFromHttp(_T("http://www2.atpos.co.kr/DownLoad/00002289.zip"), m_szFileName);

	//Sleep(100);

	//// zip파일 압출 풀기
	//CString csTargetDir = _T("C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\Download\\00002289.zip");
	//CString csUnzipDir = _T("C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\Download");
	//DoUnzipFile(csTargetDir, csUnzipDir);

	//Sleep(100);

	AllFileMove();

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

BOOL CMFCWebFileDownDlg::DoUnzipFile(CString sTargetDir, CString sUnzipDir)
{
	if (GetFileAttributes(sTargetDir) == 0xFFFFFFFF)
		return FALSE;

	if (GetFileAttributes(sUnzipDir) == 0xFFFFFFFF)
		CreateDirectory(sUnzipDir, NULL);

	ZRESULT zr = 0;
	TCHAR szCurrentdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurrentdir);
	SetCurrentDirectory(sUnzipDir);

	HZIP hz = OpenZip((TCHAR*)(LPCTSTR)sTargetDir, 0, ZIP_FILENAME);
	if (hz)
	{
#ifdef _UNICODE
		ZIPENTRYW ze;
#else
		ZIPENTRY ze;
#endif
		ZeroMemory(&ze, sizeof(ze));
		GetZipItem(hz, -1, &ze);

		int numitems = ze.index;
		for (int i = 0; i < numitems; i++)
		{
			zr = GetZipItem(hz, i, &ze);
			if (zr == ZR_OK)
			{
				zr = UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
				if ((ze.attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					// 압축 해제 파일 내역을 출력하고 싶은 경우 ze.name으로 ..
				}
			}
		}
		CloseZip(hz);
	}

	SetCurrentDirectory(szCurrentdir);
	return TRUE;
}

int CMFCWebFileDownDlg::AllFileMove()
{
	CFileFind FileFind;
	BOOL	bIsFileRemain = FALSE;
	BOOL	bExist = TRUE;
	char	szDirFile[MAX_PATH + 1] = { 0, };
	CString csFileExtension;
	CString strFileName;

	// 파일이 있는지 확인
	strcpy_s(szDirFile, FILE_DOWNLOAD_FILE_DIR);
	strcat_s(szDirFile, "*.*");

	int length = strlen(szDirFile) + 1; // NULL 종료 문자 포함
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, szDirFile, length, NULL, 0);
	TCHAR* tcharString = new TCHAR[bufferSize];
	MultiByteToWideChar(CP_UTF8, 0, szDirFile, length, tcharString, bufferSize);

	bIsFileRemain = FileFind.FindFile(tcharString);

	if (!bIsFileRemain)
	{
		FileFind.Close();
		return FALSE;
	}


	int nCnt = 1;
	CString csDateTime = _T("2023-10-17");
	CString csBkDir = _T("");
	CString csfullPath = _T("");

	// Backup할 디렉토리명을 만든다.
	// 만들때는 Backup폴더를 확인해서 같은 날자면 뒤에 번호붙이기

	csBkDir += FILE_DOWNLOAD_FILE_DIR;
	csBkDir += csDateTime;

	do
	{
		CString csTemp = _T("");
		csTemp.Format(_T("%s_%d"), csBkDir, nCnt);
		DWORD dwAttrib = GetFileAttributes(csTemp);
		
		// 존재하지 않으면 생성
		bExist = dwAttrib != 0xffffffff;
		nCnt++;
	} while (bExist);

	csBkDir.Format(_T("%s_%d"), csBkDir, nCnt - 1);

	// 형변환..
	CStringA csBkDirA(csBkDir);
	char* pchar = csBkDirA.GetBuffer();

	// Backup할 디렉토리를 생성시켜 준다.
	CreateDirectory(csBkDir, 0);

	while (bIsFileRemain)	// 복사할 파일 남아 있을 때까지 
	{
		bIsFileRemain = FileFind.FindNextFile();
		strFileName = FileFind.GetFileName(); // 잡아온 파일명

		// 잡아온 파일이 현재 디렉토리에 있는지 확인
		if (!FileFind.IsDirectory())
		{
			csFileExtension = strFileName.Right(3);

			// 그게 exe파일이면
			if (csFileExtension.CompareNoCase(_T("exe")) == 0)
			{
				if (!FileMove(1, strFileName, pchar))
				{
					AfxMessageBox(_T("파일 이동중 오류가 발생하였습니다.!"));
					break;
				}
			}
			else if (csFileExtension.CompareNoCase(_T("pdb")) == 0)   // 그게 pdb파일이면
			{
				if (!FileMove(2, strFileName, pchar))
				{
					AfxMessageBox(_T("파일 이동중 오류가 발생하였습니다.!"));
					break;
				}
			}
			else if (csFileExtension.CompareNoCase(_T("zip")) == 0)   // 그게 pdb파일이면
			{
				if (!FileMove(3, strFileName, pchar))
				{
					AfxMessageBox(_T("파일 이동중 오류가 발생하였습니다.!"));
					break;
				}
			}
			//else
			//{
			//	if (!FileMove(0, strFileName, (char*)(LPWSTR)(LPCTSTR)csBkDir))
			//	{
			//		AfxMessageBox(_T("파일 이동중 오류가 발생하였습니다.!"));
			//		break;
			//	}
			//}
		}
	}

	FileFind.Close();

	return TRUE;
}


BOOL CMFCWebFileDownDlg::FileMove(short psnFileType, LPCTSTR lpszFileName, LPCSTR lpszBkDir)
{
	short	bExist = TRUE;
	char	szDirFile[MAX_PATH + 1] = { 0, };
	char	szTemp[MAX_PATH + 1] = { 0, };
	char	szBackupDir[MAX_PATH + 1] = { 0, };
	CString strFileDest, strFileSrc, strBackupFile;

	strBackupFile += lpszBkDir;
	strBackupFile += "\\";
	strBackupFile += lpszFileName;

	static LPCTSTR EXE_DIR = _T("C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\exe\\");
	static LPCTSTR PDB_DIR = _T("C:\\minkyoo\\MFC\\MFCWebFileDown\\MFCWebFileDown\\pdb\\");

	strcpy_s(szDirFile, FILE_DOWNLOAD_FILE_DIR);

	// exe 폴더 생성
	DWORD dwAttrib = GetFileAttributes(EXE_DIR);
	// 존재하지 않으면 생성
	if (dwAttrib == 0xffffffff)
		::CreateDirectory(EXE_DIR, NULL);

	// pdb 폴더 생성
	dwAttrib = GetFileAttributes(PDB_DIR);
	// 존재하지 않으면 생성
	if (dwAttrib == 0xffffffff)
		::CreateDirectory(PDB_DIR, NULL);

	if (psnFileType == 1)
	{
		strFileDest.Format(_T("%s%s"), EXE_DIR, lpszFileName);
	}
	else if (psnFileType == 2)
	{
		strFileDest.Format(_T("%s%s"), PDB_DIR, lpszFileName);
	}

	//else
	//{
	//	strFileDest.Format("%s%s", FILE_ASTEMS_EXE_DIR, lpszFileName);
	//}

	// 파일이름
	strFileSrc += FILE_DOWNLOAD_FILE_DIR;
	strFileSrc += lpszFileName;

	// zip파일은 다른 곳에 복사하지 않고, 해당하는 다운로드 날자 폴더에 복사한다
	if (psnFileType != 3)
	{
		if (!CopyFile(strFileSrc, strFileDest, TRUE))		// 파일을 복사 한다. 
		{
			//
		}
	}

	// Download폴더 에도 다운받은 것을 복사한다
	if (!CopyFile(strFileSrc, strBackupFile, TRUE))
	{
		////
	}

	CStringA csFileSrcA(strFileSrc);
	char* szFileSrc = csFileSrcA.GetBuffer();
	int r = remove(szFileSrc);	// 파일을 삭제한다. 
	DWORD errorCode = GetLastError();

	return TRUE;
}