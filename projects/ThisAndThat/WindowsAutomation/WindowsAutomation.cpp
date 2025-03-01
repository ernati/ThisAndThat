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

// ���ؽ�Ʈ �޴� â���� MSAA�� ����Ͽ�, ������ �̸�(targetName)�� �޴� �׸��� ã�� DefaultAction�� ȣ���ϴ� �Լ�
bool ClickMenuItemByName(HWND hContextMenu, const TCHAR* targetName)
{
	IAccessible* pAcc = nullptr;
	// OBJID_MENU�� ����Ͽ� ���ؽ�Ʈ �޴��� IAccessible �������̽��� ����
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

	// MSAA������ �ڽ� �׸��� 1���� ��ȣ�� �Ű����ϴ�.
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
			// �޴� �׸� �̸� �� (��: "����")
#ifdef _UNICODE
			if (_tcscmp(bstrName, _T("����")) == 0)
#else
			if (strcmp(bstrName, "����") == 0)
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

// ���ؽ�Ʈ �޴� â(hContextMenu)���� Win32 �޴� API�� �̿��Ͽ�,
// �̸��� targetName�� �޴� �׸��� ã�� �� �׸��� �������� ��Ŭ�� �ùķ��̼��ϴ� �Լ�
bool ClickMenuItemByNameNoMSAA(HWND hContextMenu, const TCHAR* targetName)
{
	// �õ�: ���ؽ�Ʈ �޴� â�� ���� HMENU�� ����������� ����
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
		// MF_BYPOSITION�� �޴� �׸��� �ؽ�Ʈ�� ����
		int len = GetMenuString(hMenu, i, menuText, 256, MF_BYPOSITION);
		if (len == 0)
			continue;
		std::wcout << L"[ClickMenuItemByNameNoMSAA] Menu item " << i << L": " << menuText << std::endl;
		if (_tcscmp(menuText, targetName) == 0)
		{
			// GetMenuItemRect�� ����Ͽ� �޴� �׸��� ������ ����.
			// hWnd �Ķ���ʹ� �޴��� ������ â����, ���⼭�� GetForegroundWindow()�� ����մϴ�.
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
			// ��� �޴� �׸� �߾ӿ��� ��Ŭ�� �ùķ��̼�
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


// "DNF" �̸��� ���� ��ư(Tray ������)�� ã�� ��, �ش� ��ư�� ���콺 �����͸� �̵���Ű��,
// ������ Ŭ���Ͽ� ���ؽ�Ʈ �޴��� �� ����, �޴� �׸� �� "����"�� ���� ������ �����Ͽ� �� �׸��� Ŭ���ϴ� �Լ�
bool RightClickIconInTrayAndSelectOpen(int index, int CountMenu, int IndexMenu)
{
	// 1. NotifyIconOverflowWindow (�ý��� Ʈ����) ã��
	HWND hNotifyIconOverflowWindow = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
	if (!hNotifyIconOverflowWindow)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Could not find NotifyIconOverflowWindow." << std::endl;
		return false;
	}

	// 2. �ý��� Ʈ���� �� ��ư���� �ִ� ToolbarWindow32 ã��
	HWND hToolbar = GetToolbarWithButtons(hNotifyIconOverflowWindow);
	if (!hToolbar)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Could not find a ToolbarWindow32 with buttons." << std::endl;
		return false;
	}

	// 3. TB_BUTTONSTRUCTSIZE �޽����� toolbar�� TBBUTTON ����ü ũ�⸦ �˸�
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	// 4. ��� toolbar�� ���μ��� �ڵ� ���� (��: explorer.exe)
	DWORD processId = 0;
	GetWindowThreadProcessId(hToolbar, &processId);
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);
	if (!hProcess)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to open target process." << std::endl;
		return false;
	}

	//// 5. ��ư ���� ���
	//int btnCount = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
	//_tprintf(_T("[RightClickIconInTrayAndSelectOpen] Button Count: %d\n"), btnCount);

	// 6. RECT ����ü ũ�⸸ŭ ���� �޸� �Ҵ� (TB_GETITEMRECT�� ���)
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

	// 8. ������ ������ �߾� ��ǥ ��� (���簢���� ����)
	POINT ptCenter;
	ptCenter.x = (rect.left + rect.right) / 2;
	ptCenter.y = (rect.top + rect.bottom) / 2;

	// ���� Ŭ���̾�Ʈ ��ǥ�� ȭ�� ��ǥ�� ��ȯ
	if (!ClientToScreen(hToolbar, &ptCenter))
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to convert client to screen coordinates." << std::endl;
		return false;
	}

	VirtualFreeEx(hProcess, remoteRectBuffer, 0, MEM_RELEASE);
	CloseHandle(hProcess);

	// 9. �ش� �߾� ��ǥ�� ���콺 ������ �̵�
	SetCursorPos(ptCenter.x, ptCenter.y);
	Sleep(1000); // 1�� ����Ͽ� ����(Ȥ�� ���� Ȯ�ο�)�� ��Ÿ�� �ð��� Ȯ��

	// 10. ��� ��ư �������� ���콺 ��Ŭ�� �ùķ��̼� (������ Ŭ������ ���ؽ�Ʈ �޴� ����)
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

	Sleep(500); // ���ؽ�Ʈ �޴��� �� �ð��� Ȯ��

	// 11. ���ؽ�Ʈ �޴�(�˾� �޴�) â ã�� (�Ϲ������� Ŭ���� �̸� "#32768")
	HWND hContextMenu = FindWindowEx(NULL, NULL, _T("#32768"), NULL);
	if (!hContextMenu)
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Context menu not found." << std::endl;
		return false;
	}

	// 12. ���ؽ�Ʈ �޴��� ��ġ(��ü â�� RECT)�� ���
	RECT menuRect = {};
	if (!GetWindowRect(hContextMenu, &menuRect))
	{
		std::cout << "[RightClickIconInTrayAndSelectOpen] Failed to get context menu rect." << std::endl;
		return false;
	}

	// 13. "����" �޴� �׸��� ù ��° �׸��̶�� �����ϰ�, �� ������ �߾� ��ǥ ���  
	// (��: �޴� ��ü ������ 1/3 ������ ù �׸��� �����̶� ����)
	int menuWidth = menuRect.right - menuRect.left;
	int menuHeight = menuRect.bottom - menuRect.top;
	// ù �׸� ������ �뷫���� ���̸� 1/3�� ���� (ȯ�濡 ���� �޶��� �� ����)
	POINT openPt;
	openPt.x = menuRect.left + menuWidth / 2;
	openPt.y = menuRect.top + IndexMenu * (menuHeight / CountMenu);

	// 14. "����" �޴� �׸��� Ŭ���ϱ� ���� ���콺 ��Ŭ�� �ùķ��̼�
	SetCursorPos(openPt.x, openPt.y);
	Sleep(200); // Ŭ�� �� ��� ���
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

	//// 8. ���ؽ�Ʈ �޴����� "����" �׸��� �̸����� ã�� ����
	//if (!ClickMenuItemByName(hContextMenu, _T("����")))
	//{
	//	std::cout << "[RightClickDNFAndSelectOpen] Failed to click menu item '����'." << std::endl;
	//	return false;
	//}

	//// 9. ���ؽ�Ʈ �޴����� "����" �׸��� �̸����� ã�� ��Ŭ�� �ùķ��̼� (MSAA �̻��)
	//if (!ClickMenuItemByNameNoMSAA(hContextMenu, _T("����")))
	//{
	//	std::cout << "[RightClickDNFAndSelectOpen] Failed to click menu item '����'." << std::endl;
	//	return false;
	//}

	std::cout << "[RightClickIconInTrayAndSelectOpen] Successfully performed right-click and selected '����' on the tray button." << std::endl;
	return true;

	//================================================================================================
	//================================================================================================
	//================================================================================================
	//================================================================================================

	//bool found = false;
	//POINT targetPt = { 0 };
	//for (int i = 0; i < btnCount; ++i)
	//{
	//	// 7. �� ��ư�� ����(RECT)�� TB_GETITEMRECT �޽����� ��������
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

	//	// 8. ������ ������ �߾� ��ǥ ��� (���簢���� ����)
	//	POINT ptCenter;
	//	ptCenter.x = (rect.left + rect.right) / 2;
	//	ptCenter.y = (rect.top + rect.bottom) / 2;

	//	// ���� Ŭ���̾�Ʈ ��ǥ�� ȭ�� ��ǥ�� ��ȯ
	//	if (!ClientToScreen(hToolbar, &ptCenter))
	//	{
	//		std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to convert client to screen coordinates." << std::endl;
	//		continue;
	//	}

	//	// 9. �ش� �߾� ��ǥ�� ���콺 ������ �̵�
	//	SetCursorPos(ptCenter.x, ptCenter.y);
	//	Sleep(1000); // 1�� ����Ͽ� ����(Ȥ�� ���� Ȯ�ο�)�� ��Ÿ�� �ð��� Ȯ��

	//	// ���⼭ �߰��� ���� ��ư�� "KakaoTalk"���� Ȯ���� �� �ִ� ���(��: ���� �ؽ�Ʈ)�� ����� �� ������,
	//	// Inspect ������ ������� "KakaoTalk" ��ư���� �˰� �ִٰ� �����մϴ�.
	//	// (�Ǵ� ���� �ܰ迡�� "KakaoTalk" ��ư�� �̸� �����ϴ� ����� ����� �� ����)
	//	// �� ���������� ù ��° "KakaoTalk" ��ư�� ������� �����մϴ�.
	//	// ���� ���ǿ� �´ٸ� targetPt�� �ش� ��ǥ ���� �� break.
	//	// (���� Ȯ�� ���� - �ʿ�� TB_GETBUTTONTEXT ������ �߰� Ȯ�� ����)
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

	//// 10. ��� ��ư �������� ���콺 ��Ŭ�� �ùķ��̼� (������ Ŭ������ ���ؽ�Ʈ �޴� ����)
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

	//Sleep(500); // ���ؽ�Ʈ �޴��� �� �ð��� Ȯ��

	//// 11. ���ؽ�Ʈ �޴�(�˾� �޴�) â ã�� (�Ϲ������� Ŭ���� �̸� "#32768")
	//HWND hContextMenu = FindWindowEx(NULL, NULL, _T("#32768"), NULL);
	//if (!hContextMenu)
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Context menu not found." << std::endl;
	//	return false;
	//}

	//// 12. ���ؽ�Ʈ �޴��� ��ġ(��ü â�� RECT)�� ���
	//RECT menuRect = {};
	//if (!GetWindowRect(hContextMenu, &menuRect))
	//{
	//	std::cout << "[RightClickKakaoTalkAndSelectOpen] Failed to get context menu rect." << std::endl;
	//	return false;
	//}

	//// 13. "����" �޴� �׸��� ù ��° �׸��̶�� �����ϰ�, �� ������ �߾� ��ǥ ���  
	//// (��: �޴� ��ü ������ 1/3 ������ ù �׸��� �����̶� ����)
	//int menuWidth = menuRect.right - menuRect.left;
	//int menuHeight = menuRect.bottom - menuRect.top;
	//// ù �׸� ������ �뷫���� ���̸� 1/3�� ���� (ȯ�濡 ���� �޶��� �� ����)
	//POINT openPt;
	//openPt.x = menuRect.left + menuWidth / 2;
	//openPt.y = menuRect.top + menuHeight / 6;

	//// 14. "����" �޴� �׸��� Ŭ���ϱ� ���� ���콺 ��Ŭ�� �ùķ��̼�
	//SetCursorPos(openPt.x, openPt.y);
	//Sleep(200); // Ŭ�� �� ��� ���
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

	//std::cout << "[RightClickKakaoTalkAndSelectOpen] Successfully performed right-click and selected '����' on the 'KakaoTalk' tray button." << std::endl;
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