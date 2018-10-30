#include "stdafx.h"
#include "uEye_header.h"
#include "uEyeStreaming.h"
#include ".\uEyeStreaming.h"
#include <wchar.h>


CuEyeSimpleTriggerDlg::CuEyeSimpleTriggerDlg(CWnd* pParent)
: CDialog(CuEyeSimpleTriggerDlg::IDD, pParent)
{
	//Hello
}


void CuEyeSimpleTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FRAME_EVENTS, nFrame);
}


BEGIN_MESSAGE_MAP(CuEyeSimpleTriggerDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_START,             OnButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_STOP,              OnButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_EXIT,              OnButtonExit)
    ON_MESSAGE(IS_UEYE_MESSAGE,                 OnUEyeMessage)
    ON_BN_CLICKED(IDC_BUTTON_LOAD_PARAMETER,    OnBnClickedButtonLoadParameter)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_RESET_COUNTERS, OnBnClickedButtonResetCounters)
END_MESSAGE_MAP()


BOOL CuEyeSimpleTriggerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    GetDlgItem(IDC_BUTTON_LOAD_PARAMETER)->EnableWindow(FALSE);
    m_hWndDisplay = GetDlgItem( IDC_DISPLAY )->m_hWnd;

    m_pcImageMemory = NULL;
    m_lMemoryId = 0;
    m_hCam = 0;
    m_nRenderMode = IS_RENDER_FIT_TO_WINDOW;
	left_offset = 0;
	right_offset = 0;
    m_nFlipHor = 0;
    m_nFlipVert = 0;

	nFrame = 0;

	

	counter = 1;

    UpdateData(FALSE);

    OpenCamera();

    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	INT nSupportedTriggerModes = is_SetExternalTrigger(m_hCam, IS_GET_SUPPORTED_TRIGGER_MODE);





    return true;
}


void CuEyeSimpleTriggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {

    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}


void CuEyeSimpleTriggerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

    }
    else
    {
        CDialog::OnPaint();
    }
}

void CuEyeSimpleTriggerDlg::OnButtonStart()
{ 
    if( m_hCam == 0 )
    {
        OpenCamera();
    }

    if( m_hCam != 0 )
    {
        is_CaptureVideo( m_hCam, IS_DONT_WAIT );

        GetDlgItem(IDC_BUTTON_LOAD_PARAMETER)->EnableWindow(FALSE);

		live = TRUE;

        GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
    }
}

void CuEyeSimpleTriggerDlg::OnButtonStop()
{
    if( m_hCam != 0 )
    {
        is_StopLiveVideo( m_hCam, IS_FORCE_VIDEO_STOP );

		live = FALSE;

        GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
    }

    GetDlgItem(IDC_BUTTON_LOAD_PARAMETER)->EnableWindow(TRUE);
}

void CuEyeSimpleTriggerDlg::OnBnClickedButtonResetCounters()
{
	nFrame = 0;
	UpdateData(FALSE);
}


void CuEyeSimpleTriggerDlg::OnBnClickedButtonLoadParameter()
{

    if ( m_hCam == 0 )
        OpenCamera();

    if ( m_hCam != 0 )
    {
        if( is_ParameterSet(m_hCam, IS_PARAMETERSET_CMD_LOAD_FILE, NULL, NULL) == IS_SUCCESS && m_pcImageMemory != NULL )
        {
            BOOL bWasLive = (BOOL)(is_CaptureVideo( m_hCam, IS_GET_LIVE ));
            if( bWasLive )
                is_StopLiveVideo(m_hCam, IS_FORCE_VIDEO_STOP);

            is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );

            IS_SIZE_2D imageSize;
            is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

            INT nAllocSizeX = 0;
            INT nAllocSizeY = 0;

			width_image = nAllocSizeX = imageSize.s32Width;
			height_image = nAllocSizeY = imageSize.s32Height;

            UINT nAbsPosX = 0;
            UINT nAbsPosY = 0;

            is_AOI(m_hCam, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&nAbsPosX , sizeof(nAbsPosX));
            is_AOI(m_hCam, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&nAbsPosY , sizeof(nAbsPosY));

            if (nAbsPosX)
            {
                nAllocSizeX = m_sInfo.nMaxWidth;
            }
            if (nAbsPosY)
            {
                nAllocSizeY = m_sInfo.nMaxHeight;
            }

            switch( is_SetColorMode( m_hCam, IS_GET_COLOR_MODE ) )
            {
            case IS_CM_RGBA12_UNPACKED:
            case IS_CM_BGRA12_UNPACKED:
                m_nBitsPerPixel = 64;
                break;

            case IS_CM_RGB12_UNPACKED:
            case IS_CM_BGR12_UNPACKED:
            case IS_CM_RGB10_UNPACKED:
            case IS_CM_BGR10_UNPACKED:
                m_nBitsPerPixel = 48;
                break;

            case IS_CM_RGBA8_PACKED:
            case IS_CM_BGRA8_PACKED:
            case IS_CM_RGB10_PACKED:
            case IS_CM_BGR10_PACKED:
            case IS_CM_RGBY8_PACKED:
            case IS_CM_BGRY8_PACKED:
                m_nBitsPerPixel = 32;
                break;

            case IS_CM_RGB8_PACKED:
            case IS_CM_BGR8_PACKED:
                m_nBitsPerPixel = 24;
                break;

            case IS_CM_BGR565_PACKED:
            case IS_CM_UYVY_PACKED:
            case IS_CM_CBYCRY_PACKED:
                m_nBitsPerPixel = 16;
                break;

            case IS_CM_BGR5_PACKED:
                m_nBitsPerPixel = 15;
                break;

            case IS_CM_MONO16:
            case IS_CM_SENSOR_RAW16:
            case IS_CM_MONO12:
            case IS_CM_SENSOR_RAW12:
            case IS_CM_MONO10:
            case IS_CM_SENSOR_RAW10:
                m_nBitsPerPixel = 16;
                break;

            case IS_CM_RGB8_PLANAR:
                m_nBitsPerPixel = 24;
                break;

            case IS_CM_MONO8:
            case IS_CM_SENSOR_RAW8:
            default:
                m_nBitsPerPixel = 8;
                break;
            }

            is_AllocImageMem( m_hCam, nAllocSizeX, nAllocSizeY, m_nBitsPerPixel, &m_pcImageMemory, &m_lMemoryId);

            is_SetImageMem(m_hCam, m_pcImageMemory, m_lMemoryId );

            imageSize.s32Width = width_image;
            imageSize.s32Height = height_image;

            is_AOI(m_hCam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));

            if( bWasLive )
                is_CaptureVideo(m_hCam, IS_DONT_WAIT);
        }
    }
}

void CuEyeSimpleTriggerDlg::OnButtonExit()
{
    ExitCamera();
    PostQuitMessage( 0 );
}

bool CuEyeSimpleTriggerDlg::OpenCamera()
{
	
	INT nRet = IS_NO_SUCCESS;
    ExitCamera();

    m_hCam = (HIDS) 0;
	nRet = is_InitCamera(&m_hCam, m_hWndDisplay);
	if (nRet == IS_SUCCESS)
    {
		nRet = is_GetCameraInfo(m_hCam, &m_ci);
        is_GetSensorInfo(m_hCam, &m_sInfo);
			
		SENSORINFO sInfo;
		is_GetSensorInfo(m_hCam, &sInfo);
		width_image = sInfo.nMaxWidth;
		height_image = sInfo.nMaxHeight;

        UpdateData(TRUE);
        nRet = InitDisplayMode();

        if (nRet == IS_SUCCESS)
        {
            is_EnableMessage(m_hCam,    IS_DEVICE_REMOVED,      GetSafeHwnd());
            is_EnableMessage(m_hCam,    IS_DEVICE_RECONNECTED,  GetSafeHwnd());
            is_EnableMessage(m_hCam,    IS_FRAME,               GetSafeHwnd());
            is_EnableMessage(m_hCam,    IS_TRIGGER,             GetSafeHwnd());

            if(is_CaptureVideo( m_hCam, IS_WAIT ) == IS_SUCCESS)
            {
				live = TRUE;
            }

			UINT nMode = IO_FLASH_MODE_OFF;
			nRet = is_IO(m_hCam, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode));

        }
        else
        {
            AfxMessageBox(TEXT("Initializing the display mode failed!"), MB_ICONWARNING );
        }

    }
    else
    {
        AfxMessageBox(TEXT("No uEye camera could be opened !"), MB_ICONWARNING );
        return false;
    }
}

LRESULT CuEyeSimpleTriggerDlg::OnUEyeMessage( WPARAM wParam, LPARAM lParam )
{
    UpdateData(FALSE);

    switch ( wParam )
    {
    case IS_DEVICE_REMOVED:
        Beep( 400, 50 );
        break;

    case IS_DEVICE_RECONNECTED:
        Beep( 400, 50 );
        break;

    case IS_FRAME:
        if( m_pcImageMemory != NULL )
        {
			nFrame++;
            is_RenderBitmap( m_hCam, m_lMemoryId, m_hWndDisplay, m_nRenderMode );

			char filename[256];

			sprintf(filename, "C:\\\\EssaieBlanc\\\\Phase_1\\\\1.4\\\\cam_%s_%d.png", m_ci.SerNo, counter);

			size_t origsize = strlen(filename) + 1;
			const size_t newsize = 100;
			size_t convertedChars = 0;
			wchar_t wcstring[newsize];
			mbstowcs_s(&convertedChars, wcstring, origsize, filename, _TRUNCATE);
			wcscat_s(wcstring, L"");

			IMAGE_FILE_PARAMS ImageFileParams;

			ImageFileParams.pwchFileName = wcstring;
			ImageFileParams.nFileType = IS_IMG_PNG;
			ImageFileParams.nQuality = 100;
			ImageFileParams.pnImageID = NULL;
			ImageFileParams.ppcImageMem = NULL;
			//int nRet = is_ImageFile(m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams, sizeof(ImageFileParams));

			counter = counter + 1;
			
			
			
					   
        }
        break;
    }
    return 0;
}

void CuEyeSimpleTriggerDlg::ExitCamera()
{
    if( m_hCam != 0 )
    {
        is_EnableMessage(m_hCam,    IS_DEVICE_REMOVED,      NULL);
        is_EnableMessage(m_hCam,    IS_DEVICE_RECONNECTED,  NULL);
        is_EnableMessage(m_hCam,    IS_FRAME,               NULL );
        is_EnableMessage(m_hCam,    IS_TRIGGER,             NULL );

        is_StopLiveVideo( m_hCam, IS_WAIT );

		UINT nMode = IO_FLASH_MODE_OFF;
		auto nRet = is_IO(m_hCam, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode));

        if( m_pcImageMemory != NULL )
        {
            is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
        }

        m_pcImageMemory = NULL;

        is_ExitCamera( m_hCam );
        m_hCam = NULL;
    }
}

int CuEyeSimpleTriggerDlg::InitDisplayMode()
{
    INT nRet = IS_NO_SUCCESS;

    if (m_hCam == NULL)
    {
        return IS_NO_SUCCESS;
    }

    if( m_pcImageMemory != NULL )
    {
        is_FreeImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
    }
    m_pcImageMemory = NULL;

    nRet = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);
    if (m_sInfo.nColorMode == IS_COLORMODE_BAYER)
    {
        is_GetColorDepth(m_hCam, &m_nBitsPerPixel, &m_nColorMode);
    }
    else if (m_sInfo.nColorMode == IS_COLORMODE_CBYCRY)
    {
        m_nColorMode = IS_CM_BGRA8_PACKED;
        m_nBitsPerPixel = 32;
    }
    else
    {
        m_nColorMode = IS_CM_MONO8;
        m_nBitsPerPixel = 8;
    }

    if (is_AllocImageMem(m_hCam, width_image, height_image, m_nBitsPerPixel, &m_pcImageMemory, &m_lMemoryId ) != IS_SUCCESS)
    {
        AfxMessageBox(TEXT("Memory allocation failed!"), MB_ICONWARNING );
    }
    else
    {
        is_SetImageMem( m_hCam, m_pcImageMemory, m_lMemoryId );
    }

    if (nRet == IS_SUCCESS)
    {
        is_SetColorMode(m_hCam, m_nColorMode);

        IS_SIZE_2D imageSize;
        imageSize.s32Width = width_image;
        imageSize.s32Height = height_image;

        is_AOI(m_hCam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
    }

    return nRet;
}


void CuEyeSimpleTriggerDlg::OnClose()
{
    ExitCamera();
    CDialog::OnClose();
}


