#include "DxWindown.h"
#include <algorithm>
DxWindown::DxWindown(const char* wndName,unsigned int width,unsigned int hight,unsigned int posLeft,unsigned int posTop)
	:m_wnd_name(wndName),m_hight(hight),m_width(width),m_pos_left(posLeft),m_pos_top(posTop)
{
	CreateWnd();
}

DxWindown::~DxWindown()
{

};


void DxWindown::CreateWnd()
{
// 	HINSTANCE hInst=::GetModuleHandle(NULL);
// 	WNDCLASSEX wc;
// 	wc.cbSize=sizeof(wc);
// 	wc.style=CS_HREDRAW|CS_VREDRAW;//水平 竖直移动时 重绘
// 	wc.lpfnWndProc=StaticWndPoc;
// 	wc.cbClsExtra=0; //指定一个数值,记录窗口类额外的信息,默认为0
// 	wc.cbWndExtra=sizeof(this);
// 	wc.hInstance=hInst;//标识了该窗口类的窗口过程所在的模块实例的句柄，不能为NULL
// 	wc.hIcon=0;//窗口类的图标,此参数ICON资源句柄,如果设置为NULL,操作系统将提供一个默认的图标
// 	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
// 	wc.hbrBackground=static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
// 	wc.lpszClassName=m_wnd_name.c_str();
// 	wc.lpszMenuName=NULL;
// 	wc.hIconSm=NULL;
// 
// 	if (!::RegisterClassEx(&wc))
// 	{
// 		MessageBox(0,"CreateWindown Error","Error",0);
// 		return ;
// 	}
// 
// 	unsigned int style=WS_OVERLAPPEDWINDOW;
// 	RECT rc={0,0,m_width,m_hight};
// 	::AdjustWindowRect(&rc,style,false);
// 
// 	m_hwnd=::CreateWindowA(m_wnd_name.c_str(),m_wnd_name.c_str(),style,m_pos_left,m_pos_top,rc.right-rc.left,rc.bottom-rc.top,0,0,hInst,0);
// 	if (m_hwnd==NULL)
// 	{
// 		MessageBox(0,"CreateWnd Error","Error",0);
// 		return ;
// 	}
// 
// 	::GetClientRect(m_hwnd,&rc);
// 	m_hight=rc.bottom-rc.top;
// 	m_width=rc.right-rc.left;
// 	::SetWindowLongPtr(m_hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));
// 
// 
// 	::ShowWindow(m_hwnd,SW_SHOWNORMAL);
// 	::UpdateWindow(m_hwnd);
// 
// 	DragAcceptFiles(m_hwnd,true);
	HINSTANCE hInst =::GetModuleHandle(NULL);

	WNDCLASSEX wc;
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= DxWindown::StaticWndPoc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(this);
	wc.hInstance		= hInst;
	wc.hIcon			= NULL;
	wc.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_wnd_name.c_str();
	wc.hIconSm			= NULL;

	if( !::RegisterClassEx( &wc ) )
	{
		DWORD dwError = GetLastError();
		//ReportErrWithLastErr(dwError);
		return;
	}
	unsigned int style= WS_OVERLAPPEDWINDOW;
	RECT rc = { 0, 0, m_width,m_hight};
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = ::CreateWindowA(
		m_wnd_name.c_str(), 
		m_wnd_name.c_str(),
		WS_OVERLAPPEDWINDOW, 
		m_pos_left, 
		m_pos_top,
		rc.right - rc.left, 
		rc.bottom - rc.top, 
		0, 
		0, 
		hInst, 
		0);

	if( m_hwnd == NULL )
	{
		DWORD dwError = GetLastError();
		//ReportErrWithLastErr(dwError);
		return ;
	}
	//长宽应该为客户区大小
	::GetClientRect(m_hwnd, &rc);
	m_width = rc.right - rc.left;
	m_hight = rc.bottom - rc.top;
	::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	::ShowWindow(m_hwnd, SW_SHOWNORMAL);
	::UpdateWindow(m_hwnd);

	//设置能够接受拖拽文件
	DragAcceptFiles( m_hwnd,TRUE );
}


LRESULT CALLBACK DxWindown::StaticWndPoc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	//这是什么用法？？？
	DxWindown* pWin=(DxWindown*)(::GetWindowLongPtr(hwnd,GWLP_USERDATA));
	if (pWin)
		return pWin->DxWndProc(hwnd,uMsg,wParam,lParam);
	else
		return ::DefWindowProc(hwnd,uMsg,wParam,lParam);
}

LRESULT DxWindown::DxWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (dx_msg_proc)
	{
		if (dx_msg_proc(uMsg,wParam,lParam))
			return 1;
	}

	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
		EndPaint(hwnd,&ps);
		break;

	case WM_KEYDOWN:
		std::for_each(key_down_event.begin(),key_down_event.end(),[&](KeyCallback& e){e((unsigned int)wParam);});
		break;
	case  WM_KEYUP:
		std::for_each(key_up_envent.begin(),key_up_envent.end(),[&](KeyCallback& e){e((unsigned int)wParam);});
		break;
	case WM_LBUTTONDOWN:
		std::for_each(mouse_left_down.begin(),mouse_left_down.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_LBUTTONUP:
		std::for_each(mouse_left_up.begin(),mouse_left_up.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_RBUTTONDOWN:
		std::for_each(mouse_right_down.begin(),mouse_right_down.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_RBUTTONUP:
		std::for_each(mouse_right_up.begin(),mouse_right_up.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_MBUTTONDOWN:
		std::for_each(mouse_mid_down.begin(),mouse_mid_down.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_MBUTTONUP:
		std::for_each(mouse_mid_up.begin(),mouse_mid_up.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_MOUSEWHEEL:
		std::for_each(mouse_whell_event.begin(),mouse_whell_event.end(),[&](WheelCallBack& e){e(Win32MsgBox::GetWheelEvnetFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_MOUSEMOVE:
		std::for_each(mouse_move_event.begin(),mouse_move_event.end(),[&](MouseCallBack& e){e(Win32MsgBox::GetMouseEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_SIZE:
		std::for_each(wnd_size_event.begin(),wnd_size_event.end(),[&](WndSizeCallBack& e){e(Win32MsgBox::GetSizeEventFromMsg(uMsg,wParam,lParam));});
		break;
	case WM_DROPFILES:
		break;
	default:
		return	::DefWindowProc(hwnd,uMsg,wParam,lParam);
	}
return 0;
}



void DxWindown::RigesterWndSizeEvent(WndSizeCallBack cb)
{
	wnd_size_event.push_back(cb);
}

void DxWindown::RigsterMouseWheelEvent(WheelCallBack cb)
{
	mouse_whell_event.push_back(cb);
}

void DxWindown::RigeterMouseMiddleUp(MouseCallBack cb)
{
	mouse_mid_up.push_back(cb);
}

void DxWindown::RigisterDropEvent(DropFileCallback cb)
{
	drop_file_event.push_back(cb);
}

void DxWindown::RigisterKeyDown(KeyCallback cb)
{
	key_down_event.push_back(cb);
}

void DxWindown::RigisterKeyUp(KeyCallback cb)
{
	key_up_envent.push_back(cb);
}

void DxWindown::RigsterMouseLeftDown(MouseCallBack cb)
{
	mouse_left_down.push_back(cb);
}

void DxWindown::RigisterMouseLeftUp(MouseCallBack cb)
{
	mouse_left_up.push_back(cb);
}

void DxWindown::RigisterMouseRightDown(MouseCallBack cb)
{
	mouse_right_down.push_back(cb);
}

void DxWindown::RigisterMouseRightUp(MouseCallBack cb)
{
	mouse_right_up.push_back(cb);
}

void DxWindown::RigisterMouseMoveEvent(MouseCallBack cb)
{
	mouse_move_event.push_back(cb);
}