//#include <windows.h>
//#include <iostream>
//
//// Helper function to find a child window by class name
//HWND FindChildWindow(HWND parent, const std::wstring& className) {
//    HWND child = FindWindowEx(parent, nullptr, className.c_str(), nullptr);
//    return child;
//}
//
//// Simulates a left click at a specific location
//void SimulateLeftClick(int x, int y) {
//    SetCursorPos(x, y);
//    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
//    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
//}
//
//// Main function to interact with the system tray
//void ExpandSystemTrayIcons() {
//    // Find the Shell_TrayWnd (Taskbar)
//    HWND hTray = FindWindow(L"Shell_TrayWnd", nullptr);
//    if (!hTray) {
//        std::cerr << "Could not find the Shell_TrayWnd." << std::endl;
//        return;
//    }
//
//    // Find the TrayNotifyWnd (Notification Area)
//    HWND hTrayNotify = FindChildWindow(hTray, L"TrayNotifyWnd");
//    if (!hTrayNotify) {
//        std::cerr << "Could not find the TrayNotifyWnd." << std::endl;
//        return;
//    }
//
//    // Find the Chevron (Button for hidden icons)
//    HWND hChevron = FindChildWindow(hTrayNotify, L"Button");
//    if (!hChevron) {
//        std::cerr << "Could not find the Chevron button." << std::endl;
//        return;
//    }
//
//    // Get the Chevron button's screen position
//    RECT rect;
//    if (GetWindowRect(hChevron, &rect)) {
//        int x = (rect.left + rect.right) / 2;
//        int y = (rect.top + rect.bottom) / 2;
//
//        // Simulate a left-click on the Chevron button
//        SimulateLeftClick(x, y);
//
//        std::cout << "System tray icons expanded." << std::endl;
//    }
//    else {
//        std::cerr << "Could not get the Chevron button's position." << std::endl;
//    }
//}
//
//int main() {
//    ExpandSystemTrayIcons();
//    return 0;
//}


#include <windows.h>
#include <Commctrl.h>
#include <iostream>
#include <string>
#include <vector>


// Helper function to find a toolbar with buttons
HWND GetToolbarWithButtons(HWND hTrayNotify) {
    HWND hToolbar = nullptr;
    while ((hToolbar = FindWindowEx(hTrayNotify, hToolbar, L"ToolbarWindow32", nullptr)) != nullptr) {
        int count = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
        if (count > 0) {
            return hToolbar;
        }
    }
    return nullptr;
}


// Helper function to find a child window by class name
HWND FindChildWindow(HWND parent, const std::wstring& className) {
    return FindWindowEx(parent, nullptr, className.c_str(), nullptr);
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

// Function to retrieve the tooltip text for a toolbar button
std::wstring GetToolbarButtonTooltip(HWND hToolbar, int index) {
    // Get the TBBUTTON structure
    TBBUTTON tbb = {};
    SendMessage(hToolbar, TB_GETBUTTON, index, (LPARAM)&tbb);

    // Create a TOOLINFO structure to query the tooltip
    HWND hTooltips = (HWND)SendMessage(hToolbar, TB_GETTOOLTIPS, 0, 0);
    if (!hTooltips) {
        std::cerr << "Tooltip control not found for button " << index << std::endl;
        return L"";
    }

    TOOLINFO toolInfo = {};
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hToolbar;
    toolInfo.uId = tbb.idCommand;
    wchar_t tooltipText[256] = {};

    toolInfo.lpszText = tooltipText;

    // Get the tooltip text
    if (SendMessage(hTooltips, TTM_GETTEXT, 0, (LPARAM)&toolInfo)) {
        return std::wstring(tooltipText);
    }

    return L"";
}

// Expand the system tray icons
bool ExpandSystemTray() {
    HWND hTray = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTray) {
        std::cerr << "Could not find the Shell_TrayWnd." << std::endl;
        return false;
    }

    HWND hTrayNotify = FindChildWindow(hTray, L"TrayNotifyWnd");
    if (!hTrayNotify) {
        std::cerr << "Could not find the TrayNotifyWnd." << std::endl;
        return false;
    }

    HWND hChevron = FindChildWindow(hTrayNotify, L"Button");
    if (!hChevron) {
        std::cerr << "Could not find the Chevron button." << std::endl;
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

    std::cerr << "Could not get the Chevron button's position." << std::endl;
    return false;
}

// Find and right-click on the Discord icon in the system tray
bool RightClickDiscordIcon() {
    HWND hNotifyIconOverflowWindow = FindWindow(L"NotifyIconOverflowWindow", nullptr);
    if (!hNotifyIconOverflowWindow) {
        std::cerr << "Could not find the NotifyIconOverflowWindow." << std::endl;
        return false;
    }

    // Find the ToolbarWindow32 with buttons
    HWND hToolbar = GetToolbarWithButtons(hNotifyIconOverflowWindow);
    if (!hToolbar) {
        std::cerr << "Could not find a ToolbarWindow32 with buttons." << std::endl;
        return false;
    }

    int count = SendMessage(hToolbar, TB_BUTTONCOUNT, 0, 0);
    printf("Button Count : %d\n", count);
    for (int i = 0; i < count; ++i) {
        std::wstring tooltip = GetToolbarButtonTooltip(hToolbar, i);
        wprintf(L"Button : %s\n", tooltip);

        if (tooltip.find(L"Discord") != std::wstring::npos) {
            wprintf(L"Found Discord icon with tooltip: %s\n", tooltip);

            // Get the button's rectangle
            RECT rect;
            SendMessage(hToolbar, TB_GETRECT, i, (LPARAM)&rect);
            MapWindowPoints(hToolbar, nullptr, (POINT*)&rect, 2);

            // Simulate a right-click
            int x = (rect.left + rect.right) / 2;
            int y = (rect.top + rect.bottom) / 2;
            SimulateRightClick(x, y);

            return true;
        }
    }

    std::cerr << "Discord icon not found in the system tray." << std::endl;
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

int main() {
    // 1. Expand system tray icons
    if (!ExpandSystemTray()) {
        return 1;
    }

    // 2. Right-click on Discord icon
    if (!RightClickDiscordIcon()) {
        return 1;
    }

    // 3. Select "Acknowledgements" menu item
    SelectAcknowledgements();

    return 0;
}
