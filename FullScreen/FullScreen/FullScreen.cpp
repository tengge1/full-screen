//实现全屏窗口，F1全屏，ESC取消全屏
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wcx;
	HWND hwnd;
	MSG msg;

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = TEXT("WndClass");
	wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wcx);

	hwnd = CreateWindow(TEXT("WndClass"), TEXT("窗口程序"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nShowCmd);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bFull = FALSE;//是否全屏
	static WINDOWPLACEMENT wpNormalScreen, wpFullScreen;//记录窗口显示状态信息
	static RECT rectWindow;//窗口坐标
	static RECT rectClient;//客户区坐标
	static RECT rectFullScreen;//屏幕全屏时窗口位置
	static POINT pointUpLeft, pointDownRight;//左上角和右下角坐标
	static int nScreenWidth, nScreenHeight;//显示器分辨率
	static MINMAXINFO *pmmi;//记录窗口最小最大信息

	switch (uMsg)
	{
	case WM_CREATE://窗口创建
		nScreenWidth = GetSystemMetrics(SM_CXSCREEN);//屏幕横向分辨率
		nScreenHeight = GetSystemMetrics(SM_CYSCREEN);//屏幕纵向分辨率
		break;
	case WM_KEYDOWN://按键消息，F1全屏，F2取消全屏
		switch (wParam)
		{
		case VK_F1://全屏
			if (bFull) break;
			bFull = TRUE;
			//保存全屏前窗口显示状态信息
			wpNormalScreen.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hWnd, &wpNormalScreen);
			//获取窗口和客户区大小
			GetWindowRect(hWnd, &rectWindow);
			GetClientRect(hWnd, &rectClient);
			pointUpLeft.x = rectClient.left;
			pointUpLeft.y = rectClient.top;
			pointDownRight.x = rectClient.right;
			pointDownRight.y = rectClient.bottom;
			ClientToScreen(hWnd, &pointUpLeft);
			ClientToScreen(hWnd, &pointDownRight);
			//计算全屏时窗口位置
			rectFullScreen.left = rectWindow.left - pointUpLeft.x;
			rectFullScreen.top = rectWindow.top - pointUpLeft.y;
			rectFullScreen.right = nScreenWidth + rectWindow.right - pointDownRight.x;
			rectFullScreen.bottom = nScreenHeight + rectWindow.bottom - pointDownRight.y;
			//调用SetWindowPlacement实现全屏
			wpFullScreen.length = sizeof(WINDOWPLACEMENT);
			wpFullScreen.flags = 0;
			wpFullScreen.showCmd = SW_SHOWNORMAL;
			wpFullScreen.rcNormalPosition = rectFullScreen;
			SetWindowPlacement(hWnd, &wpFullScreen);
			break;
		case VK_ESCAPE://取消全屏
			if (!bFull) break;
			bFull = FALSE;
			//恢复原来窗口大小
			SetWindowPlacement(hWnd, &wpNormalScreen);
			break;
		}
		break;
	case WM_GETMINMAXINFO://窗口最小最大信息，如果不处理，挡不住任务栏
		if (!bFull) break;
		//处理全屏最大大小
		pmmi = (MINMAXINFO *)lParam;
		pmmi->ptMaxPosition.x = rectFullScreen.left;
		pmmi->ptMaxPosition.y = rectFullScreen.top;
		pmmi->ptMaxSize.x = rectFullScreen.right - rectFullScreen.left;
		pmmi->ptMaxSize.y = rectFullScreen.bottom - rectFullScreen.top;
		pmmi->ptMaxTrackSize.x = rectFullScreen.right - rectFullScreen.left;
		pmmi->ptMaxTrackSize.y = rectFullScreen.right - rectFullScreen.left;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;;
	}
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
}
