#include <windows.h>
#include <Commctrl.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <oleacc.h>
#pragma comment(lib, "oleacc.lib")

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

// Helper function to find a child window by class name
HWND FindChildWindow(HWND parent, const tstring& className) {
	return FindWindowEx(parent, NULL, className.c_str(), NULL);
}

// Simulate a mouse right-click at the given screen coordinates
void SimulateRightClick(int x, int y) {
	SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

// Simulate a left-click at the given screen coordinates
void SimulateLeftClick(int x, int y) {
	SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

// Expand the system tray icons
bool ExpandSystemTray() {
	HWND hTray = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (!hTray) {
		std::cout << "[ExpandSystemTray] Could not find the Shell_TrayWnd." << std::endl;
		return false;
	}

	HWND hTrayNotify = FindChildWindow(hTray, _T("TrayNotifyWnd"));
	if (!hTrayNotify) {
		std::cout << "[ExpandSystemTray] Could not find the TrayNotifyWnd." << std::endl;
		return false;
	}

	HWND hChevron = FindChildWindow(hTrayNotify, _T("Button"));
	if (!hChevron) {
		std::cout << "[ExpandSystemTray] Could not find the Chevron button." << std::endl;
		return false;
	}

	RECT rect;
	if (GetWindowRect(hChevron, &rect)) {
		int x = (rect.left + rect.right) / 2;
		int y = (rect.top + rect.bottom) / 2;

		SimulateLeftClick(x, y);
		Sleep(1000); // Wait for the icons to expand
		return true;
	}

	std::cout << "[ExpandSystemTray] Could not get the Chevron button's position." << std::endl;
	return false;
}

//================================================================================================


// Helper function to find a toolbar with buttons
HWND GetToolbarWithButtons(HWND hTrayNotify) {
	HWND hToolbar = NULL;
	while ((hToolbar = FindWindowEx(hTrayNotify, hToolbar, _T("ToolbarWindow32"), NULL)) != NULL) {
		int count = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
		if (count > 0) {
			return hToolbar;
		}
	}
	return NULL;
}

void printButtonInfo(TBBUTTON* tbb) {

	if (!tbb) {
		std::cout << "[printButtonInfo] Invalid TBBUTTON pointer\n";
		return;
	}

	// print button info
	std::cout << "[printButtonInfo] Button Info\n";
	std::cout << "[printButtonInfo] iBitmap : " << tbb->iBitmap << std::endl;
	std::cout << "[printButtonInfo] idCommand : " << tbb->idCommand << std::endl;
	std::cout << "[printButtonInfo] fsState : " << tbb->fsState << std::endl;
	std::cout << "[printButtonInfo] fsStyle : " << tbb->fsStyle << std::endl;
	std::cout << "[printButtonInfo] dwData : " << tbb->dwData << std::endl;
	std::cout << "[printButtonInfo] iString : " << tbb->iString << std::endl;

	//print =============
	std::cout << "[printButtonInfo] =======================================\n";

	return;
}

// Function to retrieve the button tooltip
void GetToolbarButtonInfo(HWND hToolbar, int index, TBBUTTON* tbb) {
	if (!hToolbar) {
		std::cout << "[GetToolbarButtonInfo] Invalid Toolbar handle\n";
		return;
	}

	if (!tbb) {
		std::cout << "[GetToolbarButtonInfo] Invalid TBBUTTON pointer\n";
		return;
	}

	// Retrieve TBBUTTON structure
	if (-1 == SendMessage(hToolbar, TB_GETBUTTON, index, (LPARAM)tbb)) {
		std::cout << "[GetToolbarButtonInfo] Failed to get button at index " << index << std::endl;
		return;
	}

	return;
}


//================================================================================================

// 컨텍스트 메뉴 창에서 MSAA를 사용하여, 지정한 이름(targetName)의 메뉴 항목을 찾아 DefaultAction을 호출하는 함수
bool ClickMenuItemByName(HWND hContextMenu, const TCHAR* targetName)
{
	IAccessible* pAcc = nullptr;
	// OBJID_MENU를 사용하여 컨텍스트 메뉴의 IAccessible 인터페이스를 얻음
	HRESULT hr = AccessibleObjectFromWindow(hContextMenu, OBJID_MENU, IID_IAccessible, (void**)&pAcc);
	if (FAILED(hr) || !pAcc)
	{
		std::cout << "[ClickMenuItemByName] AccessibleObjectFromWindow failed." << std::endl;
		return false;
	}

	long childCount = 0;
	hr = pAcc->get_accChildCount(&childCount);
	if (FAILED(hr))
	{
		std::cout << "[ClickMenuItemByName] get_accChildCount failed." << std::endl;
		pAcc->Release();
		return false;
	}

	// MSAA에서는 자식 항목은 1부터 번호가 매겨집니다.
	bool clicked = false;
	VARIANT varChild;
	VariantInit(&varChild);
	for (long i = 1; i <= childCount; i++)
	{
		varChild.vt = VT_I4;
		varChild.lVal = i;
		BSTR bstrName = nullptr;
		hr = pAcc->get_accName(varChild, &bstrName);
		if (SUCCEEDED(hr) && bstrName)
		{
			// 메뉴 항목 이름 비교 (예: "열기")
#ifdef _UNICODE
			if (_tcscmp(bstrName, _T("열기")) == 0)
#else
			if (strcmp(bstrName, "열기") == 0)
#endif
			{
				hr = pAcc->accDoDefaultAction(varChild);
				if (SUCCEEDED(hr))
				{
					clicked = true;
					SysFreeString(bstrName);
					break;
				}
			}
			SysFreeString(bstrName);
		}
	}
	pAcc->Release();
	return clicked;
}

// 컨텍스트 메뉴 창(hContextMenu)에서 Win32 메뉴 API를 이용하여,
// 이름이 targetName인 메뉴 항목을 찾아 그 항목의 영역에서 좌클릭 시뮬레이션하는 함수
bool ClickMenuItemByNameNoMSAA(HWND hContextMenu, const TCHAR* targetName)
{
	// 시도: 컨텍스트 메뉴 창에 붙은 HMENU를 비공식적으로 얻음
	HMENU hMenu = (HMENU)GetProp(hContextMenu, _T("Menu"));
	if (!hMenu)
	{
		std::cout << "[ClickMenuItemByNameNoMSAA] Failed to get HMENU from context menu." << std::endl;
		return false;
	}

	int itemCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < itemCount; i++)
	{
		TCHAR menuText[256] = { 0 };
		// MF_BYPOSITION로 메뉴 항목의 텍스트를 읽음
		int len = GetMenuString(hMenu, i, menuText, 256, MF_BYPOSITION);
		if (len == 0)
			continue;
		std::wcout << L"[ClickMenuItemByNameNoMSAA] Menu item " << i << L": " << menuText << std::endl;
		if (_tcscmp(menuText, targetName) == 0)
		{
			// GetMenuItemRect를 사용하여 메뉴 항목의 영역을 구함.
			// hWnd 파라미터는 메뉴를 소유한 창으로, 여기서는 GetForegroundWindow()를 사용합니다.
			RECT rcItem = {};
			HWND hOwner = GetForegroundWindow();
			if (!GetMenuItemRect(hOwner, hMenu, i, &rcItem))
			{
				std::cout << "[ClickMenuItemByNameNoMSAA] GetMenuItemRect failed." << std::endl;
				return false;
			}
			POINT ptCenter;
			ptCenter.x = (rcItem.left + rcItem.right) / 2;
			ptCenter.y = (rcItem.top + rcItem.bottom) / 2;
			// 대상 메뉴 항목 중앙에서 좌클릭 시뮬레이션
			SetCursorPos(ptCenter.x, ptCenter.y);
			Sleep(100);
			INPUT inputs[2] = {};
			inputs[0].type = INPUT_MOUSE;
			inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			inputs[1].type = INPUT_MOUSE;
			inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
			if (SendInput(2, inputs, sizeof(INPUT)) != 2)
			{
				std::cout << "[ClickMenuItemByNameNoMSAA] SendInput failed." << std::endl;
				return false;
			}
			return true;
		}
	}
	std::cout << "[ClickMenuItemByNameNoMSAA] Menu item '" << targetName << "' not found." << std::endl;
	return false;
}


// "DNF" 이름을 가진 버튼(Tray 아이콘)을 찾은 후, 해당 버튼에 마우스 포인터를 이동시키고,
// 오른쪽 클릭하여 컨텍스트 메뉴를 연 다음, 메뉴 항목 중 "열기"가 있을 것으로 가정하여 그 항목을 클릭하는 함수
bool RightClickIconInTrayAndSelectOpen(int index, int CountMenu, int IndexMenu)
{
	// 1. NotifyIconOverflowWindow (시스템 트레이) 찾기
	HWND hNotifyIconOverflowWindow = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
	if (!hNotifyIconOverflowWindow)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Could not find NotifyIconOverflowWindow." << std::endl;
		return false;
	}

	// 2. 시스템 트레이 내 버튼들이 있는 ToolbarWindow32 찾기
	HWND hToolbar = GetToolbarWithButtons(hNotifyIconOverflowWindow);
	if (!hToolbar)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Could not find a ToolbarWindow32 with buttons." << std::endl;
		return false;
	}

	// 3. TB_BUTTONSTRUCTSIZE 메시지로 toolbar에 TBBUTTON 구조체 크기를 알림
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	// 4. 대상 toolbar의 프로세스 핸들 열기 (예: explorer.exe)
	DWORD processId = 0;
	GetWindowThreadProcessId(hToolbar, &processId);
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);
	if (!hProcess)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to open target process." << std::endl;
		return false;
	}

	//// 5. 버튼 개수 얻기
	//int btnCount = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
	//_tprintf(_T("[RightClickIconInTrayAndSelectOpen] Button Count: %d\n"), btnCount);

	// 6. RECT 구조체 크기만큼 원격 메모리 할당 (TB_GETITEMRECT에 사용)
	LPVOID remoteRectBuffer = VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
	if (!remoteRectBuffer)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to allocate memory for RECT in target process." << std::endl;
		CloseHandle(hProcess);
		return false;
	}

	if (SendMessage(hToolbar, TB_GETITEMRECT, index, (LPARAM)remoteRectBuffer) == 0)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to get item rect for button at index " << index << std::endl;
		return false;
	}

	RECT rect = {};
	SIZE_T bytesRead = 0;
	if (!ReadProcessMemory(hProcess, remoteRectBuffer, &rect, sizeof(RECT), &bytesRead) ||
		bytesRead != sizeof(RECT))
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to read item rect for button at index " << index << std::endl;
		return false;
	}

	// 8. 아이콘 영역의 중앙 좌표 계산 (정사각형의 중점)
	POINT ptCenter;
	ptCenter.x = (rect.left + rect.right) / 2;
	ptCenter.y = (rect.top + rect.bottom) / 2;

	// 툴바 클라이언트 좌표를 화면 좌표로 변환
	if (!ClientToScreen(hToolbar, &ptCenter))
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to convert client to screen coordinates." << std::endl;
		return false;
	}

	VirtualFreeEx(hProcess, remoteRectBuffer, 0, MEM_RELEASE);
	CloseHandle(hProcess);

	// 9. 해당 중앙 좌표로 마우스 포인터 이동
	SetCursorPos(ptCenter.x, ptCenter.y);
	Sleep(1000); // 1초 대기하여 툴팁(혹은 내부 확인용)이 나타날 시간을 확보

	// 10. 대상 버튼 영역에서 마우스 우클릭 시뮬레이션 (오른쪽 클릭으로 컨텍스트 메뉴 오픈)
	INPUT inputs[2] = {};
	inputs[0].type = INPUT_MOUSE;
	inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	inputs[1].type = INPUT_MOUSE;
	inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	if (SendInput(2, inputs, sizeof(INPUT)) != 2)
	{
		std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to simulate right-click." << std::endl;
		return false;
	}

	Sleep(500); // 컨텍스트 메뉴가 뜰 시간을 확보

	// 11. 컨텍스트 메뉴(팝업 메뉴) 창 찾기 (일반적으로 클래스 이름 "#32768")
	HWND hContextMenu = FindWindowEx(NULL, NULL, _T("#32768"), NULL);
	if (!hContextMenu)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Context menu not found." << std::endl;
		return false;
	}

	// 12. 컨텍스트 메뉴의 위치(전체 창의 RECT)를 얻기
	RECT menuRect = {};
	if (!GetWindowRect(hContextMenu, &menuRect))
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to get context menu rect." << std::endl;
		return false;
	}

	// 13. "열기" 메뉴 항목이 첫 번째 항목이라고 가정하고, 그 영역의 중앙 좌표 계산  
	// (예: 메뉴 전체 높이의 1/3 정도가 첫 항목의 영역이라 가정)
	int menuWidth = menuRect.right - menuRect.left;
	int menuHeight = menuRect.bottom - menuRect.top;
	// 첫 항목 영역의 대략적인 높이를 1/3로 가정 (환경에 따라 달라질 수 있음)
	POINT openPt;
	openPt.x = menuRect.left + menuWidth / 2;
	openPt.y = menuRect.top + IndexMenu * (menuHeight / CountMenu);

	// 14. "열기" 메뉴 항목을 클릭하기 위해 마우스 좌클릭 시뮬레이션
	SetCursorPos(openPt.x, openPt.y);
	Sleep(200); // 클릭 전 잠시 대기
	INPUT clickInputs[2] = {};
	clickInputs[0].type = INPUT_MOUSE;
	clickInputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	clickInputs[1].type = INPUT_MOUSE;
	clickInputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	if (SendInput(2, clickInputs, sizeof(INPUT)) != 2)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to simulate left-click on context menu item." << std::endl;
		return false;
	}

	//// 8. 컨텍스트 메뉴에서 "열기" 항목을 이름으로 찾아 선택
	//if (!ClickMenuItemByName(hContextMenu, _T("열기")))
	//{
	//	std::cout << "[RightClickDNFAndSelectOpen] Failed to click menu item '열기'." << std::endl;
	//	return false;
	//}

	//// 9. 컨텍스트 메뉴에서 "열기" 항목을 이름으로 찾아 좌클릭 시뮬레이션 (MSAA 미사용)
	//if (!ClickMenuItemByNameNoMSAA(hContextMenu, _T("열기")))
	//{
	//	std::cout << "[RightClickDNFAndSelectOpen] Failed to click menu item '열기'." << std::endl;
	//	return false;
	//}

	std::cout << "[RightClickIconInTrayAndSelectOpen] Successfully performed right-click and selected '열기' on the tray button." << std::endl;
	return true;

	//================================================================================================
	//================================================================================================
	//================================================================================================
	//================================================================================================

	//bool found = false;
	//POINT targetPt = { 0 };
	//for (int i = 0; i < btnCount; ++i)
	//{
	//	// 7. 각 버튼의 영역(RECT)을 TB_GETITEMRECT 메시지로 가져오기
	//	if (SendMessage(hToolbar, TB_GETITEMRECT, i, (LPARAM)remoteRectBuffer) == 0)
	//	{
	//		std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to get item rect for button at index " << i << std::endl;
	//		continue;
	//	}

	//	RECT rect = {};
	//	SIZE_T bytesRead = 0;
	//	if (!ReadProcessMemory(hProcess, remoteRectBuffer, &rect, sizeof(RECT), &bytesRead) ||
	//		bytesRead != sizeof(RECT))
	//	{
	//		std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to read item rect for button at index " << i << std::endl;
	//		continue;
	//	}

	//	// 8. 아이콘 영역의 중앙 좌표 계산 (정사각형의 중점)
	//	POINT ptCenter;
	//	ptCenter.x = (rect.left + rect.right) / 2;
	//	ptCenter.y = (rect.top + rect.bottom) / 2;

	//	// 툴바 클라이언트 좌표를 화면 좌표로 변환
	//	if (!ClientToScreen(hToolbar, &ptCenter))
	//	{
	//		std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to convert client to screen coordinates." << std::endl;
	//		continue;
	//	}

	//	// 9. 해당 중앙 좌표로 마우스 포인터 이동
	//	SetCursorPos(ptCenter.x, ptCenter.y);
	//	Sleep(1000); // 1초 대기하여 툴팁(혹은 내부 확인용)이 나타날 시간을 확보

	//	// 여기서 추가로 현재 버튼이 "KakaoTalk"인지 확인할 수 있는 방법(예: 툴팁 텍스트)을 사용할 수 있으나,
	//	// Inspect 정보를 기반으로 "KakaoTalk" 버튼임을 알고 있다고 가정합니다.
	//	// (또는 이전 단계에서 "KakaoTalk" 버튼을 미리 선별하는 방법을 사용할 수 있음)
	//	// 본 예제에서는 첫 번째 "KakaoTalk" 버튼을 대상으로 진행합니다.
	//	// 만약 조건에 맞다면 targetPt에 해당 좌표 저장 후 break.
	//	// (조건 확인 생략 - 필요시 TB_GETBUTTONTEXT 등으로 추가 확인 가능)
	//	targetPt = ptCenter;
	//	found = true;
	//	break;
	//}

	//VirtualFreeEx(hProcess, remoteRectBuffer, 0, MEM_RELEASE);
	//CloseHandle(hProcess);

	//if (!found)
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] 'KakaoTalk' tray button not found." << std::endl;
	//	return false;
	//}

	//// 10. 대상 버튼 영역에서 마우스 우클릭 시뮬레이션 (오른쪽 클릭으로 컨텍스트 메뉴 오픈)
	//INPUT inputs[2] = {};
	//inputs[0].type = INPUT_MOUSE;
	//inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	//inputs[1].type = INPUT_MOUSE;
	//inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	//if (SendInput(2, inputs, sizeof(INPUT)) != 2)
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to simulate right-click." << std::endl;
	//	return false;
	//}

	//Sleep(500); // 컨텍스트 메뉴가 뜰 시간을 확보

	//// 11. 컨텍스트 메뉴(팝업 메뉴) 창 찾기 (일반적으로 클래스 이름 "#32768")
	//HWND hContextMenu = FindWindowEx(NULL, NULL, _T("#32768"), NULL);
	//if (!hContextMenu)
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Context menu not found." << std::endl;
	//	return false;
	//}

	//// 12. 컨텍스트 메뉴의 위치(전체 창의 RECT)를 얻기
	//RECT menuRect = {};
	//if (!GetWindowRect(hContextMenu, &menuRect))
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to get context menu rect." << std::endl;
	//	return false;
	//}

	//// 13. "열기" 메뉴 항목이 첫 번째 항목이라고 가정하고, 그 영역의 중앙 좌표 계산  
	//// (예: 메뉴 전체 높이의 1/3 정도가 첫 항목의 영역이라 가정)
	//int menuWidth = menuRect.right - menuRect.left;
	//int menuHeight = menuRect.bottom - menuRect.top;
	//// 첫 항목 영역의 대략적인 높이를 1/3로 가정 (환경에 따라 달라질 수 있음)
	//POINT openPt;
	//openPt.x = menuRect.left + menuWidth / 2;
	//openPt.y = menuRect.top + menuHeight / 6;

	//// 14. "열기" 메뉴 항목을 클릭하기 위해 마우스 좌클릭 시뮬레이션
	//SetCursorPos(openPt.x, openPt.y);
	//Sleep(200); // 클릭 전 잠시 대기
	//INPUT clickInputs[2] = {};
	//clickInputs[0].type = INPUT_MOUSE;
	//clickInputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	//clickInputs[1].type = INPUT_MOUSE;
	//clickInputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	//if (SendInput(2, clickInputs, sizeof(INPUT)) != 2)
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to simulate left-click on context menu item." << std::endl;
	//	return false;
	//}

	//std::cout << "[RightClickKakaoTalkAndSelectOpen] Successfully performed right-click and selected '열기' on the 'KakaoTalk' tray button." << std::endl;
	//return true;
}


//================================================================================================


// Select the "Acknowledgements" option from the context menu
void SelectAcknowledgements() {
	INPUT inputs[2] = {};

	// Simulate DOWN key
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_DOWN;

	// Simulate ENTER key
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_RETURN;

	SendInput(2, inputs, sizeof(INPUT));
	std::cout << "Acknowledgements menu selected." << std::endl;
}


// Select the "Exit" option from the context menu
void SelectExitOption() {
	INPUT inputs[2] = {};

	// Simulate DOWN key
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_DOWN;

	// Simulate ENTER key
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_RETURN;

	SendInput(2, inputs, sizeof(INPUT));
	std::cout << "Exit menu option selected." << std::endl;
}

// Select the "Exit" option from the context menu
void SelectInformationOption() {
	INPUT inputs[2] = {};

	// Simulate DOWN key
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_DOWN;

	// Simulate ENTER key
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_RETURN;

	SendInput(2, inputs, sizeof(INPUT));
	std::cout << "Information menu option selected." << std::endl;
}


int main() {
	// 1. Expand system tray icons
	if (!ExpandSystemTray()) {
		return 1;
	}

	// 2. Right-click on Discord icon
	tstring discordIconName;
	//if (!GetDiscordIconName(discordIconName)) {
	//	return 1;
	//}
	//if (!GetDiscordIconName(discordIconName)) {
	//	return 1;
	//}

	if (!RightClickIconInTrayAndSelectOpen(2, 8, 2))
	{
		std::cout << "[Main] Operation failed." << std::endl;
	}

	//// 3. Select "Acknowledgements" menu item
	//SelectExitOption();

	return 0;
}