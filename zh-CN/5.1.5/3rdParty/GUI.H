#ifndef _GUI_H_
#define _GUI_H_

BOOL GetTrayWndRect(RECT& TrayRect);
BOOL DoAnimation();
void MinToTray(const HWND hWnd);
void RestoreFromTray(const HWND hWnd, BOOL bForceMax = FALSE);
BOOL IsObscured(const HWND hWnd);

#endif  // _GUI_H_
