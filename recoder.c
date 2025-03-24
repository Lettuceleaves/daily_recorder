#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>

// Global variables to count keyboard presses and mouse clicks
int keyPressCount = 0;
int mouseClickCount = 0;

// Log file name
const char* LOG_FILE = "C:\\records\\daily_input_log.txt";

// Callback function to handle keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) { // Ensure nCode is valid
        // Increment count for each keyboard press event
        keyPressCount++;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Callback function to handle mouse events
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) { // Ensure nCode is valid
        // Detect mouse click events (left, right, or middle button)
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
            mouseClickCount++;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Check if the log entry is for today
int IsTodayRecord(const char* dateStr) {
    time_t now = time(NULL);
    struct tm* nowInfo = localtime(&now);
    char todayStr[11];
    strftime(todayStr, sizeof(todayStr), "%Y-%m-%d", nowInfo);
    return strncmp(dateStr, todayStr, 10) == 0;
}

// Update or append today's data
void UpdateOrAppendLog() {
    FILE* logFile;
    time_t now;
    struct tm* timeInfo;
    char timeBuffer[20];
    char dateStr[11];
    char timeStr[9];
    int keyCount, mouseCount;
    int foundToday = 0;
    char line[256];

    // Get current time
    now = time(NULL);
    timeInfo = localtime(&now);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeInfo);

    // Open log file in read mode
    logFile = fopen(LOG_FILE, "r+");
    if (logFile == NULL) {
        // If file does not exist, create a new one
        logFile = fopen(LOG_FILE, "w");
        if (logFile == NULL) {
            printf("Cannot create log file!\n");
            return;
        }
        fprintf(logFile, "%s - Keyboard presses: %d, Mouse clicks: %d\n", timeBuffer, keyPressCount, mouseClickCount);
        fclose(logFile);
        keyPressCount = 0;
        mouseClickCount = 0;
        return;
    }

    // Use a buffer to store updated content
    char buffer[1024];
    int bufferSize = 0;

    // Read each line from the log file
    while (fgets(line, sizeof(line), logFile)) {
        // Ensure the line is properly terminated
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        printf("Reading line: %s\n", line); // Debugging output
        int result = sscanf(line, "%[^ ] %[^ ] - Keyboard presses: %d, Mouse clicks: %d", dateStr, timeStr, &keyCount, &mouseCount);
        printf("sscanf result: %d, dateStr: %s, timeStr: %s\n", result, dateStr, timeStr); // Debugging output
        if (result == 4) {
            printf("Parsed: %s %s - Keyboard presses: %d, Mouse clicks: %d\n", dateStr, timeStr, keyCount, mouseCount); // Debugging output
            if (IsTodayRecord(dateStr)) {
                keyCount += keyPressCount;
                mouseCount += mouseClickCount;
                foundToday = 1;
            }
            snprintf(buffer + bufferSize, sizeof(buffer) - bufferSize, "%s %s - Keyboard presses: %d, Mouse clicks: %d\n", dateStr, timeStr, keyCount, mouseCount);
            bufferSize += strlen(buffer + bufferSize);
        } else {
            printf("Failed to parse line: %s\n", line); // Debugging output
        }
    }

    // If today's record is not found, append a new record
    if (!foundToday) {
        snprintf(buffer + bufferSize, sizeof(buffer) - bufferSize, "%s - Keyboard presses: %d, Mouse clicks: %d\n", timeBuffer, keyPressCount, mouseClickCount);
    }

    // Clear and write back to the original file
    rewind(logFile);
    fputs(buffer, logFile);
    fclose(logFile);

    // Reset counters
    keyPressCount = 0;
    mouseClickCount = 0;
}

int main() {
    HHOOK hKeyHook, hMouseHook;
    MSG msg;
    time_t lastUpdateTime = time(NULL);

    // Install keyboard hook
    hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (hKeyHook == NULL) {
        printf("Cannot install keyboard hook!\n");
        return 1;
    }

    // Install mouse hook
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);
    if (hMouseHook == NULL) {
        printf("Cannot install mouse hook!\n");
        return 1;
    }

    // Message loop
    while (1) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update data every 5 seconds
        time_t now = time(NULL);
        if (difftime(now, lastUpdateTime) >= 5) { // 5 seconds
            UpdateOrAppendLog();
            lastUpdateTime = now;
        }
    }

    // Remove hooks
    UnhookWindowsHookEx(hKeyHook);
    UnhookWindowsHookEx(hMouseHook);

    return 0;
}
