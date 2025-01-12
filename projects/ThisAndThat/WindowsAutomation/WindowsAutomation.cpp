#include <windows.h>
#include <Commctrl.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif


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

// Function to retrieve the button text directly from the Toolbar
tstring GetToolbarButtonText(HWND hToolbar, int index) {
	// 1. Retrieve the TBBUTTON structure for the specified index
	TBBUTTON tbb = {};
	TCHAR buttonText[256] = {};

	if (!SendMessage(hToolbar, TB_GETBUTTON, index, (LPARAM)&tbb)) {
		std::cout << "Failed to get button at index " << index << std::endl;
		return _T("");
	}

	// 2. If the button has text, get the text associated with the button
	if (tbb.iString != -1) {
		LRESULT result = SendMessage(hToolbar, TB_GETBUTTONTEXT, tbb.idCommand, (LPARAM)buttonText);
		if (result > 0) {
			std::cout << "Get Button Text!\n";
			return tstring(buttonText);
		}
	}

	std::cout << "Fail to get button text!\n";
	// 3. If the button has no text, return an empty string
	return _T("");
}

// Function to retrieve the button tooltip
tstring GetToolbarButtonTooltip(HWND hToolbar, int index) {
	if (!hToolbar) {
		std::cout << "Invalid Toolbar handle\n";
		return _T("");
	}

	int buttonCount = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
	if (index < 0 || index >= buttonCount) {
		std::cout << "INVALID INDEX!\n";
		return _T("");
	}

	TBBUTTON tbb = {};
	memset(&tbb, 0, sizeof(TBBUTTON));
	TCHAR tooltipText[256] = {};

	// Retrieve TBBUTTON structure
	if (!SendMessage(hToolbar, TB_GETBUTTON, index, (LPARAM)&tbb)) {
		std::cout << "Failed to get button at index " << index << std::endl;
		return _T("");
	}

	// Get tooltip text
	HWND hTooltips = (HWND)SendMessage(hToolbar, TB_GETTOOLTIPS, 0, 0);
	if (!hTooltips) {
		std::cout << "Tooltip control not found for button " << index << std::endl;
		return _T("");
	}

	TOOLINFO toolInfo = {};
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hToolbar;
	toolInfo.uId = tbb.idCommand;
	toolInfo.lpszText = tooltipText;

#ifdef _UNICODE
	//toolInfo.cchTextMax = sizeof(tooltipText) / sizeof(TCHAR);
#endif

	if (SendMessage(hTooltips, TTM_GETTEXT, 0, (LPARAM)&toolInfo)) {
		return tstring(tooltipText);
	}

	return _T("");
}


// Expand the system tray icons
bool ExpandSystemTray() {
	HWND hTray = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (!hTray) {
		std::cout << "Could not find the Shell_TrayWnd." << std::endl;
		return false;
	}

	HWND hTrayNotify = FindChildWindow(hTray, _T("TrayNotifyWnd"));
	if (!hTrayNotify) {
		std::cout << "Could not find the TrayNotifyWnd." << std::endl;
		return false;
	}

	HWND hChevron = FindChildWindow(hTrayNotify, _T("Button"));
	if (!hChevron) {
		std::cout << "Could not find the Chevron button." << std::endl;
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

	std::cout << "Could not get the Chevron button's position." << std::endl;
	return false;
}

// Find and right-click on the Discord icon in the system tray
bool RightClickVMWareIcon() {
	HWND hNotifyIconOverflowWindow = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
	if (!hNotifyIconOverflowWindow) {
		std::cout << "Could not find the NotifyIconOverflowWindow." << std::endl;
		return false;
	}

	// Find the ToolbarWindow32 with buttons
	HWND hToolbar = GetToolbarWithButtons(hNotifyIconOverflowWindow);
	if (!hToolbar) {
		std::cout << "Could not find a ToolbarWindow32 with buttons." << std::endl;
		return false;
	}

	int count = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
	_tprintf(_T("Button Count : %d\n"), count);
	for (int i = 0; i < count; ++i) {
		//tstring tooltip = GetToolbarButtonText(hToolbar, i);
		tstring tooltip = GetToolbarButtonTooltip(hToolbar, i);
		_tprintf(_T("Button : %s\n"), tooltip);

		if (tooltip.find(_T("VMware Tools")) != tstring::npos) {
			_tprintf(_T("This Button is VMware Tools\n"));

			// Get the button's rectangle
			RECT rect;
			SendMessage(hToolbar, TB_GETRECT, i, (LPARAM)&rect);
			MapWindowPoints(hToolbar, NULL, (POINT*)&rect, 2);

			// Simulate a right-click
			int x = (rect.left + rect.right) / 2;
			int y = (rect.top + rect.bottom) / 2;
			SimulateRightClick(x, y);

			return true;
		}

		_tprintf(_T("This Button is not VMware Tools\n"));
	}

	std::cout << "VMware Tools icon not found in the system tray." << std::endl;
	return false;
}


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
	if (!RightClickVMWareIcon()) {
		return 1;
	}

	//// 3. Select "Acknowledgements" menu item
	//SelectExitOption();

	return 0;
}