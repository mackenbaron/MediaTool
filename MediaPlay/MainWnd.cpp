#include "stdafx.h"
#include "Resource.h"
#include "MainWnd.h"
#include "FileDialogEx.h"

#include "Poco/ThreadPool.h"

using Poco::ThreadPool;


CMainWndDlg* g_pMainWndDlg = nullptr;

//////////////////////////////////////////////////////////////////////////
///
DUI_BEGIN_MESSAGE_MAP(CMainWndDlg, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_WINDOWINIT, OnWindowInit)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SETFOCUS, OnSetFocus)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_VALUECHANGED, OnValueChanged)
DUI_END_MESSAGE_MAP()

CMainWndDlg::CMainWndDlg(void)
{
	g_pMainWndDlg = this;

	m_pCloseBtn = nullptr;
	m_pMaxBtn = nullptr;
	m_pRestoreBtn = nullptr;
	m_pMinBtn = nullptr;

	m_pDateEdit = nullptr;
	m_pTimeEdit = nullptr;
	m_pAddrEdit = nullptr;
	m_pLongitudeEdit = nullptr;
	m_pLatitudeEdit = nullptr;

	m_pMediaCtrl = nullptr;
	m_pPlayProgress = nullptr;
	m_pStartTimeLabel = nullptr;
	m_pEndTimeLabel = nullptr;

	m_pPlayBtn = nullptr;
	m_pStopBtn = nullptr;
	m_pOpenFileBtn = nullptr;
	m_pLogRichEdit = nullptr;

	m_pMediaWnd = nullptr;
	m_pAvi = nullptr;
	m_spPlayTask = nullptr;
}

CMainWndDlg::~CMainWndDlg(void)
{
	m_mapString.clear();
}

LPCTSTR CMainWndDlg::GetWindowClassName() const
{
	return _T("MediaPlay");
}

CDuiString CMainWndDlg::GetSkinFile()
{
	return _T("main_wnd.xml");
}

CDuiString CMainWndDlg::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath() + _T("Skin"));
}

void CMainWndDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CMainWndDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// �����˵����ر���Ϣ
	if (wParam == SC_CLOSE)
	{
		Close();
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}

	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

void CMainWndDlg::InitWindow()
{
	m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("closeBtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("maxBtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("restoreBtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("minBtn")));

	m_pDateEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("dateEdit")));
	m_pTimeEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("timeEdit")));
	m_pAddrEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("addrEdit")));
	m_pLongitudeEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("longitudeEdit")));
	m_pLatitudeEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("latitudeEdit")));

	m_pLogOption = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("logOption")));

	m_pMediaCtrl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("mediaCtrl")));
	m_pPlayProgress = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("playProgress")));
	m_pStartTimeLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("startTimeLabel")));
	m_pEndTimeLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("endTimeLabel")));

	m_pPlayBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("playBtn")));
	m_pStopBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("stopBtn")));
	m_pOpenFileBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("openFileBtn")));
	m_pLogRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logRichEdit")));

	WindowImplBase::InitWindow();
}

CControlUI* CMainWndDlg::CreateControl( LPCTSTR pstrClassName )
{
	if (_tcsicmp(pstrClassName, _T("WndMediaDisplay")) == 0)
	{
		CWndUI *pUI = new CWndUI;
		HWND   hWnd = CreateWindow(_T("#32770"), _T("WndMediaDisplay"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), (HMENU)0, NULL, NULL);
		pUI->Attach(hWnd);
		return pUI;
	}

	return nullptr;
}

LRESULT CMainWndDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE; // ��Ϣ���ش�����������ϵͳ������
	switch (uMsg)
	{
	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_RESTORED: // ���񵽴��ڻ�ԭ��Ϣ
			{
				if (m_PaintManager.GetRoot() != NULL)
				{
					if (m_pMaxBtn != NULL) m_pMaxBtn->SetVisible(true);
					if (m_pRestoreBtn != NULL) m_pRestoreBtn->SetVisible(false);
				}
			}
			break;
		case SIZE_MAXIMIZED: // ���񵽴��������Ϣ
			{
				if (m_PaintManager.GetRoot() != NULL)
				{
					if (m_pMaxBtn != NULL) m_pMaxBtn->SetVisible(false);
					if (m_pRestoreBtn != NULL) m_pRestoreBtn->SetVisible(true);
				}
			}
			break;
		default:
			{
				
			}
			break;
		}
		bHandled = FALSE;
		break;
	case WM_PAINT:
		{
			if (m_pMediaWnd != nullptr && m_pMediaCtrl)
			{
				m_pMediaWnd->SetPos(m_pMediaCtrl->GetPos());
			}
		}
		bHandled = FALSE;
		break;
	case WM_MENUEXCLICK:
		{
			CControlUI * pControl = (CControlUI *)lParam;
			if (pControl != NULL)
			{
				m_PaintManager.SendNotify(pControl, DUI_MSGTYPE_MENUCLICK, wParam, lParam);
			}
		}
		break;
	case WM_TIMER:
		{
			this->OnTimer((UINT_PTR)wParam);
			bHandled = FALSE;
		}
		break;
	case WM_LOGOUTPUT:
		{
			this->OnLogOutput(wParam, lParam);
		}
		break;
	case WM_UPDATE_PROGRESS:
		{
			this->UpdateProgress();
		}
		bHandled = FALSE;
		break;
	default:
		bHandled = FALSE; // ����ϵͳ����
	}

	return 0;
}

void CMainWndDlg::Notify(TNotifyUI& msg)
{
	return WindowImplBase::Notify(msg);
}

void CMainWndDlg::OnWindowInit( TNotifyUI& msg )
{
	g_logCallBack = CMainWndDlg::LogMsgShow;

	this->InitConfig();

	// ����Ĭ���̳߳ص�����
	ThreadPool::defaultPool().addCapacity(320);

	// ��������۲���
	m_taskManager.addObserver(Observer<CMainWndDlg, TaskStartedNotification>(*this, &CMainWndDlg::taskStarted));
	m_taskManager.addObserver(Observer<CMainWndDlg, TaskCancelledNotification>(*this, &CMainWndDlg::taskCancelled));
	m_taskManager.addObserver(Observer<CMainWndDlg, TaskFailedNotification>(*this, &CMainWndDlg::taskFailed));
	m_taskManager.addObserver(Observer<CMainWndDlg, TaskFinishedNotification>(*this, &CMainWndDlg::taskFinished));
	m_taskManager.addObserver(Observer<CMainWndDlg, TaskProgressNotification>(*this, &CMainWndDlg::taskProgress));
}

void CMainWndDlg::OnClick( TNotifyUI& msg )
{
	if (msg.pSender == m_pCloseBtn)
	{
		Close();
		::PostQuitMessage(0L);
		return;
	}
	else if (msg.pSender == m_pMinBtn)
	{
		this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender == m_pMaxBtn)
	{
		this->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if (msg.pSender == m_pRestoreBtn)
	{
		this->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else if (msg.pSender == m_pPlayProgress)
	{
	}
	else if (msg.pSender == m_pPlayBtn)
	{
		if (m_pPlayBtn->GetToolTip().CompareNoCase(STR_PLAY.c_str()) == 0)
		{
			if (m_pPlayProgress->GetValue() == 100)
			{
				return;
			}
			this->ClickPlayBtn(STR_PAUSE, true);
		}
		else
		{
			this->ClickPlayBtn(STR_PLAY, true);
		}
	}
	else if (msg.pSender == m_pStopBtn)
	{
		this->ClickStopBtn();
	}
	else if (msg.pSender == m_pOpenFileBtn)
	{
		this->ClickOpenFileBtn();
	}
	else if (msg.pSender == m_pPlayProgress)
	{
		::MessageBox(NULL, _T(""), _T(""), 0);
	}
}

void CMainWndDlg::InitConfig()
{
	//m_pPlayBtn->SetEnabled(false);
	this->InitD3D();
}


void CMainWndDlg::LoadConfig(void)
{
	m_mapString.clear();

	/*	�����ļ���	*/
	tstring strFileName = CPath::GetModulePath(CPaintManagerUI::GetInstance());
	strFileName = CStringUtil::Format(_T("%s\\%s"), strFileName.c_str(), cstrConfName.c_str());

	//////////////////////////////////////////////////////////////////////////
	/*	��ȡ�ļ�����	*/

	/*	��ȡ [CWREG] SECTION	*/
	TCHAR szAutoExe[MAX_PATH] = { 0 };
	GetPrivateProfileString(_T("CWREG"), cstrAutoRun.c_str(), _T(""), szAutoExe, MAX_PATH, strFileName.c_str());
	m_mapString.insert(std::make_pair(cstrAutoRun, szAutoExe));

	//////////////////////////////////////////////////////////////////////////
}

void CMainWndDlg::OnSelectChanged( TNotifyUI& msg )
{

}

void CMainWndDlg::ClickPlayBtn( tstring strText, bool bEnabled /*= true*/ )
{
	m_pPlayBtn->SetEnabled(bEnabled);
	if (strText.compare(STR_PLAY) == 0)
	{
		m_pPlayBtn->SetNormalImage(_T("file='res/btn_play.png' source='0,0,42,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetHotImage(_T("file='res/btn_play.png' source='42,0,84,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetPushedImage(_T("file='res/btn_play.png' source='84,0,126,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetDisabledImage(_T("file='res/btn_play.png' source='126,0,168,27' corner='2,2,2,2'"));
	}
	else
	{
		m_pPlayBtn->SetNormalImage(_T("file='res/btn_pause.png' source='0,0,42,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetHotImage(_T("file='res/btn_pause.png' source='42,0,84,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetPushedImage(_T("file='res/btn_pause.png' source='84,0,126,27' corner='2,2,2,2'"));
		m_pPlayBtn->SetDisabledImage(_T("file='res/btn_pause.png' source='126,0,168,27' corner='2,2,2,2'"));

		
		::ShowWindow(m_pMediaWnd->GetHWND(), SW_NORMAL);
		::SetTimer(m_hWnd, 1, 500, NULL);
	}

	m_pPlayBtn->SetToolTip(strText.c_str());
}

void CMainWndDlg::ClickStopBtn()
{
	::KillTimer(m_hWnd, 1);

	this->ClickPlayBtn(STR_PLAY, false);

	/*	ȡ�����������Դ	*/
	m_taskManager.cancelAll();
	m_spPlayTask = nullptr;
	m_d3d.Clear();
	FastMutex::ScopedLock lock(m_mciMutex);
	if (m_pAvi != NULL)
	{
		AVI_close(m_pAvi);
		m_pAvi = NULL;
	}

	
	if (m_pMediaWnd != nullptr)
	{
		::ShowWindow(m_pMediaWnd->GetHWND(), SW_HIDE);
	}
	m_pPlayProgress->SetEnabled(false);
	m_pPlayProgress->SetValue(0);

	m_pStartTimeLabel->SetText(_T(""));
	m_pEndTimeLabel->SetText(_T(""));
}

void CMainWndDlg::ClickOpenFileBtn()
{
	this->ClickStopBtn();

	CFileDialogEx openFileDlg;
	openFileDlg.SetTitle(_T("����Ƶ�ļ�"));
	openFileDlg.SetFilter(_T("��Ƶ�ļ�\0**.avi;\0\0"));
	openFileDlg.SetParentWnd(m_hWnd);
	if (openFileDlg.ShowOpenFileDlg())
	{
		m_strPathName = openFileDlg.GetPathName();
		if (m_pMediaCtrl != nullptr)
		{
			m_pMediaCtrl->SetVisible(true);

			m_taskManager.start(new CParseTask);
		}
	}
}

void CMainWndDlg::OnTimer( UINT_PTR nIDEvent )
{
	if (nIDEvent == 1)
	{
		this->UpdateProgress();
	}
}


void CMainWndDlg::UpdateProgress()
{
	if (m_pAvi == nullptr)
	{
		return;
	}

	FastMutex::ScopedLock lock(m_mciMutex);

	m_pPlayProgress->SetEnabled(true);
	if (m_pPlayProgress->GetValue() == 100 && m_d3d.GetPos() == 0)
	{
		::KillTimer(m_hWnd, 1);
		this->ClickPlayBtn(STR_PLAY, true);
		return;
	}

	long lTotalFrames = m_pAvi->video_frames;
	double fFps = m_pAvi->fps;

	/*	�����ܹ�ʱ��	*/
	if (fFps <= 0)
	{
		fFps = 1.0f;
	}
	long lTotalSeconds = (double)lTotalFrames/fFps;
	if (lTotalSeconds/60 >= 100)
	{
		m_pEndTimeLabel->SetText(CStringUtil::Format(_T("%03d:%02d"), lTotalSeconds/60, lTotalSeconds%60).c_str());
	}
	else
	{
		m_pEndTimeLabel->SetText(CStringUtil::Format(_T("%02d:%02d"), lTotalSeconds/60, lTotalSeconds%60).c_str());
	}
	

	/*	���µ�ǰ����ʱ��	*/
	long lCurPos = m_d3d.GetPos();
	long lCurSeconds = (double)lCurPos/fFps;
	m_pStartTimeLabel->SetText(CStringUtil::Format(_T("%02d:%02d"), lCurSeconds/60, lCurSeconds%60).c_str());

	/*	���½�����	*/
	if (lTotalSeconds > 0)
	{
		m_pPlayProgress->SetValue(lCurSeconds*100/lTotalSeconds);
	}
	else
	{
		m_pPlayProgress->SetValue(0);
	}
}

void CMainWndDlg::OnSetFocus( TNotifyUI& msg )
{
	if (msg.pSender == m_pPlayProgress)
	{
	}
}

DWORD WINAPI CMainWndDlg::ParseMediaFile( LPVOID lpvData )
{
	CMainWndDlg* pMainDlg = static_cast<CMainWndDlg*>(lpvData);
	tstring strPath = pMainDlg->m_strPathName;


	/*	��ȡ��������Ϣ	*/
	H264_LIBINFO_S    lib_info;
	if ( 0 == Hi264DecGetInfo(&lib_info) )
	{
		LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("�汾: %s\n��Ȩ: %s\n\n������: 0x%x\n"), CStringUtil::AnsiToTStr(lib_info.sVersion).c_str(), CStringUtil::AnsiToTStr(lib_info.sCopyRight).c_str(), lib_info.uFunctionSet);
	}
	else
	{
		LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("��ѯ�����汾��Ϣʧ��"));
	}

	pMainDlg->m_pAvi = AVI_open_input_file((CStringUtil::TStrToAnsi(strPath)).c_str(), 1);
	if (nullptr == pMainDlg->m_pAvi)
	{
		LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("�޷��� H264 �ļ� %s \n"), strPath.c_str());
		goto exit;
	}
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("�����ļ�: %s"), strPath.c_str());
	pMainDlg->ClickPlayBtn(STR_PAUSE, true);

	/*	���ý���������	*/
	H264_DEC_ATTR_S   dec_attrbute;
	memset(&dec_attrbute, 0x00, sizeof(dec_attrbute));
	dec_attrbute.uPictureFormat = 0x00;		/*	���ͼ���ʽ��0x00��ʾ4:2:0	*/
	dec_attrbute.uBufNum        = 16;		/*	�ο�֡�Ļ�������Ŀ			*/
	dec_attrbute.uPicWidthInMB  = 120;		/*	1080p	*/
	dec_attrbute.uPicHeightInMB = 68;		/*	1080p	*/
	dec_attrbute.uStreamInType  = 0x00;		/* ����������ʽ  "00 00 01" or "00 00 00 01"	*/
	dec_attrbute.pUserData  = nullptr;   // ���û�����
	
	dec_attrbute.uWorkMode = 0x31;		/* bit0 = 1: H.264 ���ģʽ; bit0 = 0: �������ģʽ */
	//dec_attrbute.uWorkMode |= 0x10;	/* bit4 = 1:  �������;    bit4 = 0: disable ������ */
	//dec_attrbute.uWorkMode |= 0x20;		/* �������߳�	*/
	

	HI_HDL hDecoder = Hi264DecCreate(&dec_attrbute);		/*	����������	*/
	if(nullptr ==  hDecoder)
	{
		LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("����������ʧ��"));
		goto exit;
	}
	
	LARGE_INTEGER lpFrequency;
	QueryPerformanceFrequency(&lpFrequency);
	LARGE_INTEGER timeStart;
	QueryPerformanceCounter(&timeStart);

	HI_S32 end = 0;
	HI_U8  buf[BUFF_LEN];	/*	����������	*/
	H264_DEC_FRAME_S  dec_frame;
	HI_U32 pic_cnt = 0;
	HI_U32 ImageEnhanceEnable = 1;
	HI_U32 StrenthCoeff = 40;	/*	ͼ����ǿϵ��	*/
	HI_U32 tmpW = 0;
	HI_U32 tmpH = 0;
	FILE*  yuv = fopen(CStringUtil::TStrToAnsi(CPath::GetModulePath() + _T("temp.yuv")).c_str(), "w+b");
	ASSERT(yuv != nullptr);
	/* ���� h264 ���ļ� */
	while (!end)
	{
		HI_U32	len = AVI_read_frame(pMainDlg->m_pAvi, (char*)buf);
		HI_U32  flags = (len > 0) ? 0 : 1;		/*	�����Ƿ������־	*/

		/*	�������һ���������н��벢��֡���ͼ��	*/
		HI_S32 result = Hi264DecFrame(hDecoder, buf, len, 0, &dec_frame, flags);
		while(HI_H264DEC_NEED_MORE_BITS  !=  result)
		{
			if(HI_H264DEC_NO_PICTURE ==  result)   //flush over and all the remain picture are output
			{
				end = 1;		/*	������������ͼ����������������	*/
				break;
			}

			if(HI_H264DEC_OK == result)   /*	��һ֡ͼ�����	*/
			{
				if (ImageEnhanceEnable)    //	ͼ����ǿ
				{
					Hi264DecImageEnhance(hDecoder, &dec_frame, StrenthCoeff);
				}

				const HI_U8 *pY = dec_frame.pY;
				const HI_U8 *pU = dec_frame.pU;
				const HI_U8 *pV = dec_frame.pV;
				HI_U32 width    = dec_frame.uWidth;
				HI_U32 height   = dec_frame.uHeight - dec_frame.uCroppingBottomOffset;
				HI_U32 yStride  = dec_frame.uYStride;
				HI_U32 uvStride = dec_frame.uUVStride;

				pic_cnt++;
				while (pMainDlg->m_d3d.Size() >= MAX_MEMORY_POOL)
				{
					Sleep(50);
				}
				USERDATA userData;
				userData.pszData = new char[height*yStride + height* uvStride/2 + height* uvStride/2];
				ASSERT(userData.pszData != nullptr);
				memcpy(userData.pszData, pY, height*yStride);
				memcpy(userData.pszData + height*yStride, pU, height* uvStride/2);
				memcpy(userData.pszData + height*yStride + height* uvStride/2, pV, height* uvStride/2);
				userData.lPos = pMainDlg->m_pAvi->video_pos;
				pMainDlg->m_d3d.Push(userData);

				if (tmpW != dec_frame.uWidth || tmpH != dec_frame.uHeight)
				{
					LogMsg(WT_EVENTLOG_WARNING_TYPE, _T("Size change-->width: %d, height: %d, stride: %d\n"), dec_frame.uWidth, dec_frame.uHeight, dec_frame.uYStride);
					tmpW = dec_frame.uWidth;
					tmpH = dec_frame.uHeight;
				}

				/*	�洢һ��ͼ��	*/
				if(nullptr !=  yuv )
				{
					/*LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("PicBytes: %8d, I4:%4d, I8:%4d, 16:%4d, Pskip:%4d, P16:%4d, P16x8:%4d, P8x16:%4d, P8:%4d"), 
					dec_frame.pFrameInfo->uPicBytes,
					dec_frame.pFrameInfo->uI4MbNum,
					dec_frame.pFrameInfo->uI8MbNum,
					dec_frame.pFrameInfo->uI16MbNum,
					dec_frame.pFrameInfo->uPskipMbNum,
					dec_frame.pFrameInfo->uP16MbNum,
					dec_frame.pFrameInfo->uP16x8MbNum,
					dec_frame.pFrameInfo->uP8x16MbNum,
					dec_frame.pFrameInfo->uP8MbNum);*/

					
					#pragma region λͼ�洢
					/*	 λͼ�ļ�ͷ		*/
/*BITMAPFILEHEADER bmpFileHeader;
					ZeroMemory(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
					bmpFileHeader.bfType = 0x4d42;
					bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 + width*height;
					bmpFileHeader.bfReserved1 = 0;
					bmpFileHeader.bfReserved2 = 0;
					bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;

					/*	λͼ��Ϣͷ		*/
					/*BITMAPINFOHEADER bmp_infoheader;
					ZeroMemory(&bmp_infoheader, sizeof(BITMAPINFOHEADER));
					bmp_infoheader.biSize = sizeof(BITMAPINFOHEADER);
					bmp_infoheader.biWidth = width;
					bmp_infoheader.biHeight = height;
					bmp_infoheader.biPlanes = 1;
					bmp_infoheader.biBitCount = 8;
					bmp_infoheader.biCompression = BI_RGB;
					bmp_infoheader.biSizeImage = 0;			//ͼ���С����ѹ�������ݣ��������Ϊ0
					bmp_infoheader.biXPelsPerMeter = 0;
					bmp_infoheader.biYPelsPerMeter = 0;
					bmp_infoheader.biClrUsed = 0;
					bmp_infoheader.biClrImportant = 0;

					//	����Ҷ�ͼ�ĵ�ɫ��
					RGBQUAD rgbquad[256];
					for(int i = 0; i < 256; i++)
					{
						rgbquad[i].rgbBlue = i;
						rgbquad[i].rgbGreen = i;
						rgbquad[i].rgbRed = i;
						rgbquad[i].rgbReserved = 0;
					}

					fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, yuv);
					fwrite(&bmp_infoheader, sizeof(BITMAPINFOHEADER), 1, yuv);
					fwrite(&rgbquad, sizeof(RGBQUAD)*256, 1, yuv);
					fwrite(pY, height*width, 1, yuv);*/
					#pragma endregion λͼ�洢

					/*fwrite(pY, 1, height* yStride, yuv);
					fwrite(pU, 1, height* uvStride/2, yuv);
					fwrite(pV, 1, height* uvStride/2, yuv);*/
				}
			}
			/* ��������ʣ������� */
			result = Hi264DecFrame(hDecoder, NULL, 0, 0, &dec_frame, flags);
		}
	}

	LARGE_INTEGER timeEnd;
	QueryPerformanceCounter(&timeEnd);
	HI_U32 time = (HI_U32)((timeEnd.QuadPart - timeStart.QuadPart)*1000/lpFrequency.QuadPart);
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("time= %d ms \nframes = %d \nfps = %d"), time, pic_cnt, pic_cnt*1000/(time+1));

	/* ���ٽ��������ͷž�� */
	Hi264DecDestroy(hDecoder);
	hDecoder = nullptr;

exit:

	if (pMainDlg->m_pAvi != nullptr)
	{
		AVI_close(pMainDlg->m_pAvi);
	}
	if (yuv != nullptr)
	{
		fclose(yuv);
		yuv = nullptr;
	}

	return 0;
}

void CMainWndDlg::LogMsgShow( WT_EVENTLOG_TYPE emType, SYSTEMTIME stTime, LPCTSTR lpszMsg )
{
	if (g_pMainWndDlg != NULL)
	{
		LOG_ITEM_DATA* pLogItemData = new LOG_ITEM_DATA();
		pLogItemData->emType = emType;
		pLogItemData->stTime = stTime;
		pLogItemData->strMsg = lpszMsg;

		if (!g_pMainWndDlg->PostMessage(WM_LOGOUTPUT, (WPARAM)pLogItemData, NULL)) 
		{ 
			// ������Ϣ�������ᵼ�·���ʧ��
			delete pLogItemData;
			pLogItemData = NULL;
		}
	}
}

LRESULT CMainWndDlg::OnLogOutput( WPARAM wParam, LPARAM lParam )
{
	LOG_ITEM_DATA* pLogItemData = (LOG_ITEM_DATA*)wParam;
	if (pLogItemData != NULL)
	{
		tstring strEventType;
		switch (pLogItemData->emType)
		{
		case WT_EVENTLOG_SUCCESS_TYPE:
			strEventType = _T("�ɹ�");
			break;
		case WT_EVENTLOG_ERROR_TYPE:
			strEventType = _T("����");
			break;
		case WT_EVENTLOG_WARNING_TYPE:
			strEventType = _T("����");
			break;
		case WT_EVENTLOG_INFORMATION_TYPE:
			strEventType = _T("��Ϣ");
			break;
		default:
			break;
		}

		tstring strMsgHeader, strMsgContent;

		strMsgHeader = CStringUtil::Format(_T("%04d-%02d-%02d %02d:%02d:%02d [%s]\r\n"), pLogItemData->stTime.wYear, 
			pLogItemData->stTime.wMonth, pLogItemData->stTime.wDay, pLogItemData->stTime.wHour, 
			pLogItemData->stTime.wMinute, pLogItemData->stTime.wSecond, strEventType.c_str());

		CHARFORMAT2 cfHeader;
		ZeroMemory(&cfHeader, sizeof(CHARFORMAT2));
		cfHeader.cbSize = sizeof(CHARFORMAT2);
		cfHeader.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		_tccpy(cfHeader.szFaceName, _T("����")); // ��������
		cfHeader.dwEffects = 0;
		cfHeader.yHeight = 210; // ���ָ߶�
		cfHeader.crTextColor = RGB(0, 110, 254); // ������ɫ

		CHARFORMAT2 cfContent;
		ZeroMemory(&cfContent, sizeof(CHARFORMAT2));
		cfContent.cbSize = sizeof(CHARFORMAT2);
		cfContent.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		_tccpy(cfContent.szFaceName, _T("����")); // ��������
		cfContent.dwEffects = 0;
		cfContent.yHeight = 180; // ���ָ߶�
		cfContent.crTextColor = RGB(0, 0, 0); // ������ɫ

		PARAFORMAT2 pfHeader;
		ZeroMemory(&pfHeader, sizeof(PARAFORMAT2));
		pfHeader.cbSize = sizeof(PARAFORMAT2);
		pfHeader.dwMask =  PFM_STARTINDENT | PFM_OFFSET;
		pfHeader.dxStartIndent = 0;
		pfHeader.dxOffset = 0;

		PARAFORMAT2 pfContent;
		ZeroMemory(&pfContent, sizeof(PARAFORMAT2));
		pfContent.cbSize = sizeof(PARAFORMAT2);
		pfContent.dwMask =  PFM_STARTINDENT | PFM_OFFSET;
		pfContent.dxStartIndent = 200;
		pfContent.dxOffset = 0;

		if (pLogItemData->strMsg.length() > 0 && pLogItemData->strMsg[pLogItemData->strMsg.length()-1]  == _T('\n'))
		{
			strMsgContent = CStringUtil::Format(_T("%s"), pLogItemData->strMsg.c_str());
		}
		else
		{
			strMsgContent = CStringUtil::Format(_T("%s\r\n"), pLogItemData->strMsg.c_str());
		}


		m_pLogRichEdit->SetSel(-1, -1);
		m_pLogRichEdit->SetSelectionCharFormat(cfHeader);
		m_pLogRichEdit->SetParaFormat(pfHeader);
		m_pLogRichEdit->ReplaceSel(strMsgHeader.c_str(), false);

		switch (pLogItemData->emType)
		{
		case WT_EVENTLOG_SUCCESS_TYPE:
			cfContent.crTextColor = RGB(60, 180, 117); // ��ɫ
			break;
		case WT_EVENTLOG_ERROR_TYPE:
			cfContent.crTextColor = RGB(254, 0, 2); // ��ɫ
			break;
		case WT_EVENTLOG_WARNING_TYPE:
			cfContent.crTextColor = RGB(218, 69, 39); // ��ɫ
			break;
		case WT_EVENTLOG_INFORMATION_TYPE:
			break;
		default:
			break;
		}
		m_pLogRichEdit->SetSel(-1, -1);
		m_pLogRichEdit->SetSelectionCharFormat(cfContent);
		m_pLogRichEdit->SetParaFormat(pfContent);
		m_pLogRichEdit->ReplaceSel(strMsgContent.c_str(), false);

		/*if (m_pLogAutoClearOption->IsSelected())
		{
			if (m_pLogRichEdit->GetLineCount() >= 3000)
			{
				int nSelEnd = m_pLogRichEdit->LineIndex(1000); 
				m_pLogRichEdit->SetSel(0, nSelEnd);
				m_pLogRichEdit->ReplaceSel(_T(""), false);
			}
		}*/

		//if (m_pLogAutoScrollOption->IsSelected())
		{
			m_pLogRichEdit->EndDown();
		}

		delete pLogItemData;
	}

	return 0;
}



void CMainWndDlg::InitD3D()
{
	m_pMediaWnd = static_cast<CWndUI*>(m_PaintManager.FindControl(_T("mediaCtrl")));
	ASSERT(m_pMediaWnd != nullptr);

	m_pMediaWnd->SetPos(m_pMediaCtrl->GetPos());

	//m_d3d.InitD3D(m_pMediaWnd->GetHWND(), 1920, 1080);


	//m_sdlWnd.m_emStatus = CSdlWnd::E_INIT;
	//if (m_sdlWnd.Init() < 0)
	//{
	//	LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("SDL ��ʼ��ʧ�� [%s]"), CStringUtil::AnsiToTStr(SDL_GetError()).c_str());
	//	m_sdlWnd.Quit();
	//	return;
	//}

	//CWndUI* pWnd = static_cast<CWndUI*>(m_PaintManager.FindControl(_T("mediaCtrl")));
	//ASSERT(pWnd != nullptr);
	//m_sdlWnd.m_pWnd = m_sdlWnd.CreateWnd(pWnd->GetHWND());
	//if (m_sdlWnd.m_pWnd == nullptr)
	//{
	//	LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("��������ʧ��	[%s]"), CStringUtil::AnsiToTStr(SDL_GetError()).c_str());
	//	m_sdlWnd.Quit();
	//	return;
	//}

	//m_sdlWnd.m_pRender = m_sdlWnd.CreateRender();
	//if (m_sdlWnd.m_pRender == nullptr)
	//{
	//	LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("������Ⱦ��ʧ�� [%s]"), CStringUtil::AnsiToTStr(SDL_GetError()).c_str());
	//	m_sdlWnd.Quit();
	//	return;
	//}

	//m_sdlWnd.m_pTexture = m_sdlWnd.CreateTexture();
	//if (m_sdlWnd.m_pTexture == nullptr)
	//{
	//	LogMsg(WT_EVENTLOG_ERROR_TYPE, _T("��������ʧ��	[%s]"), CStringUtil::AnsiToTStr(SDL_GetError()).c_str());
	//	m_sdlWnd.Quit();
	//	return;
	//}

	////�����߳�ģ����Ƶ���ݸ���  
	////SDL_Thread *video_thread = SDL_CreateThread(VideoThread, NULL, this);  
	//std::thread video_thread(VideoThread, this);

	////��Ϣѭ��  
	//SDL_Event event;  
	//while (m_sdlWnd.m_emStatus != CSdlWnd::E_STOP)
	//{  
	//	SDL_WaitEvent(&event);  
	//	switch (event.type)  
	//	{  
	//	case SDL_QUIT:  
	//		m_sdlWnd.m_emStatus = CSdlWnd::E_REQ;  
	//		break;  
	//	}  
	//}  
}

void CMainWndDlg::taskStarted( TaskStartedNotification* pNf )
{
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("Started"));
	pNf->release();
}

void CMainWndDlg::taskCancelled( TaskCancelledNotification* pNf )
{
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("Cancelled"));
	pNf->release();
}

void CMainWndDlg::taskFinished( TaskFinishedNotification* pNf )
{
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("Finished"));
	pNf->release();
}

void CMainWndDlg::taskFailed( TaskFailedNotification* pNf )
{
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("Failed"));
	pNf->release();
}

void CMainWndDlg::taskProgress( TaskProgressNotification* pNf )
{
	LogMsg(WT_EVENTLOG_INFORMATION_TYPE, _T("Progress:%f"), pNf->progress());
	pNf->release();
}

void CMainWndDlg::OnValueChanged( TNotifyUI& msg )
{
	if (msg.pSender == m_pPlayProgress)
	{
		if (m_pPlayProgress->IsEnabled())
		{
			FastMutex::ScopedLock lock(m_mciMutex);

			POINT pt = msg.ptMouse;
			RECT rect = m_pPlayProgress->GetPos();
			if (pt.x >= rect.left && pt.x <= rect.right)
			{
				/*	���ý�����ֵ	*/
				long lDis = pt.x - rect.left;
				long lWid = rect.right - rect.left;
				double d = ((double)lDis)/((double)lWid);
				int nPer = d*100.0;
				m_pPlayProgress->SetValue(nPer);

				m_d3d.Clear();
				/*	���ò��Ż���	*/
				m_lCurFrame = m_pAvi->video_frames*d;
				AVI_set_video_position(m_pAvi, m_lCurFrame, nullptr);
				HI_U8  buf[BUFF_LEN];
				HI_U32	len = -1;
				while ((len = AVI_read_frame(m_pAvi, (char*)buf)) > 0)
				{
					if ((buf[4] & 0x1f) == 0x07)	//	��SPS
					{
						break;
					}
				}

				/*	�����û�����	*/
				m_pAddrEdit->SetText(_T("��֮��"));
			}
			this->ClickPlayBtn(STR_PAUSE, true);
		}
		else
		{
			m_pPlayProgress->SetValue(0);
		}
	}
	else
	{
		m_pAddrEdit->SetText(_T(""));
	}
}