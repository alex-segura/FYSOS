/*
 *                             Copyright (c) 1984-2020
 *                              Benjamin David Lunt
 *                             Forever Young Software
 *                            fys [at] fysnet [dot] net
 *                              All rights reserved
 * 
 * Redistribution and use in source or resulting in  compiled binary forms with or
 * without modification, are permitted provided that the  following conditions are
 * met.  Redistribution in printed form must first acquire written permission from
 * copyright holder.
 * 
 * 1. Redistributions of source  code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in printed form must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 3. Redistributions in  binary form must  reproduce the above copyright  notice,
 *    this list of  conditions and the following  disclaimer in the  documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE, DOCUMENTATION, BINARY FILES, OR OTHER ITEM, HEREBY FURTHER KNOWN
 * AS 'PRODUCT', IS  PROVIDED BY THE COPYRIGHT  HOLDER AND CONTRIBUTOR "AS IS" AND
 * ANY EXPRESS OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED
 * WARRANTIES  OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  OWNER OR CONTRIBUTOR BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER  CAUSED AND ON
 * ANY  THEORY OF  LIABILITY, WHETHER  IN  CONTRACT,  STRICT  LIABILITY,  OR  TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN  ANY WAY  OUT OF THE USE OF THIS
 * PRODUCT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  READER AND/OR USER
 * USES AS THEIR OWN RISK.
 * 
 * Any inaccuracy in source code, code comments, documentation, or other expressed
 * form within Product,  is unintentional and corresponding hardware specification
 * takes precedence.
 * 
 * Let it be known that  the purpose of this Product is to be used as supplemental
 * product for one or more of the following mentioned books.
 * 
 *   FYSOS: Operating System Design
 *    Volume 1:  The System Core
 *    Volume 2:  The Virtual File System
 *    Volume 3:  Media Storage Devices
 *    Volume 4:  Input and Output Devices
 *    Volume 5:  ** Not yet published **
 *    Volume 6:  The Graphical User Interface
 *    Volume 7:  ** Not yet published **
 *    Volume 8:  USB: The Universal Serial Bus
 * 
 * This Product is  included as a companion  to one or more of these  books and is
 * not intended to be self-sufficient.  Each item within this distribution is part
 * of a discussion within one or more of the books mentioned above.
 * 
 * For more information, please visit:
 *             http://www.fysnet.net/osdesign_book_series.htm
 */

/*
 *  FONTEDIT.EXE
 *   The purpose of this code is to create a font or fonts for the example
 *    demontration of a gui included with this book.
 * 
 *   This code allows the user to toggle squares to make a bitmap of characters
 *    then save this data to the media for loading into the GUI demo.
 *
 *  Assumptions/prerequisites:
 *   *** Please Note ***
 *   This code is for the Windows platform, specifically Windows XP.  However,
 *    it does not use MFC or other version specific code.  It should be able
 *    to be compiled for most any version of Windows, and possible ported
 *    to a different platform.
 *
 *   When using the Visual Studio IDE, do not edit the resource.rc file within
 *    the IDE's editor.  It will add items that will break this build.
 *
 *  Last updated: 13 July 2020
 *
 *   This code was built with Visual Studio 6.0 (for 32-bit Windows XP)
 *    and Visual Studio 2019 (for 64-bit Windows 10)
 *
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <limits.h>

#include <stdio.h>

#include "resource.h"

#include "..\include\ctype.h"
#include "fontedit.h"


OPENFILENAME ofn;
char szFileName[512];
char szClassName[] = "Font Editor";
char szStr[512];
char szFontName[24] = "";

HMENU menu;
MENUINFO mi;
HBRUSH hbrush;

HDC hdc;
RECT rect;

bit8u grid[MAXH][MAXW];
bit8u clip[MAXH][MAXW];
bit8u back[MAXH][MAXW];
int curh, curw;
int tcurh, tcurw;
bool tfixed = 0, tfixed_enable = 0;
int tstart, tending;
int cur_char = 0;
int index;
int clipx, clipy, clipw;

bool isdirty = FALSE;
bool drawgrid = FALSE;
bool drawnumbers = TRUE;

HWND hButtonPrev, hButtonNext, hButtonFirst, hButtonLast;
HWND hButtonUp, hButtonDown, hButtonLeft, hButtonRight;
HWND hButtonMinus, hButtonPlus;
HWND hButtonInvert, hButtonHFlip, hButtonVFlip;
HWND hButtonRestore;
//HWND hCheckDrop;

HWND hSliderX, hSliderY, hSliderW;

// Start of Program Entry point
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
  HWND hwnd;               // This is the handle for our window
  MSG messages;            // Here messages to the application are saved
  WNDCLASSEX wincl;        // Data structure for the windowclass
  
  // The Window structure
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure;      // This function is called by windows
  wincl.style = CS_DBLCLKS;                 // Catch double-clicks
  wincl.cbSize = sizeof(WNDCLASSEX);
  
  // Use default icon and mouse-pointer
  wincl.hIcon = LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
  wincl.hIconSm = LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
  wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
  wincl.lpszMenuName = MAKEINTRESOURCE(hMenu);
  wincl.cbClsExtra = 0;                      // No extra bytes after the window class
  wincl.cbWndExtra = 0;                      // structure or the window instance
  
  // Use Windows's default colour as the background of the window
  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
  //wincl.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(169,208,241));
  
  // Register the window class, and if it fails quit the program
  if (!RegisterClassEx (&wincl))
    return 0;
  
  // Get the owner window rectangle.
  GetWindowRect(GetDesktopWindow(), &rect);
  
  // The class is registered, let's create the program
  hwnd = CreateWindowEx(
           0,                   // Extended possibilites for variation
           szClassName,         // Classname
           szClassName,         // Title Text
           WS_OVERLAPPEDWINDOW, // default window
           (rect.right - APP_WIDTH) / 2,
           (rect.bottom - APP_HEIGHT) / 2,
           APP_WIDTH,           // The programs width
           APP_HEIGHT,          // and height in pixels
           HWND_DESKTOP,        // The window is a child-window to desktop
           NULL,                // menu
           hThisInstance,       // Program Instance handler
           NULL                 // No Window Creation data
         );
  
  // Create the Buttons
  hButtonPrev    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "Pr&ev", 50, 25);
  hButtonNext    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Next", 50, 25);
  hButtonFirst   = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&First", 50, 25);
  hButtonLast    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "L&ast", 50, 25);
  hButtonUp      = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&U", 25, 25);
  hButtonDown    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&D", 25, 25);
  hButtonLeft    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&L", 25, 25);
  hButtonRight   = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&R", 25, 25);
  hButtonMinus   = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Minus", 75, 25);
  hButtonPlus    = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Plus", 75, 25);
  hButtonInvert  = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Invert", 75, 25);
  hButtonHFlip   = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Horz Flip", 75, 25);
  hButtonVFlip   = CreateButton(hwnd, BS_DEFPUSHBUTTON, "&Vert Flip", 75, 25);
  hButtonRestore = CreateButton(hwnd, BS_DEFPUSHBUTTON, "Re&store", 75, 25);
//  hCheckDrop     = CreateButton(hwnd, BS_AUTOCHECKBOX | BS_LEFTTEXT, "Dr&op Character", 125, 25);
  
  hSliderX = CreateTrackBar(hwnd, SLIDERW, SLIDERH);
  hSliderY = CreateTrackBar(hwnd, SLIDERW, SLIDERH);
  hSliderW = CreateTrackBar(hwnd, SLIDERW, SLIDERH);
  
  // Make the window visible on the screen
  ShowWindow(hwnd, SW_SHOW);
  DisableItems(menu);
  
  // Run the message loop. It will run until GetMessage() returns 0
  while (GetMessage(&messages, NULL, 0, 0)) {
    // Translate virtual-key messages into character messages
    TranslateMessage(&messages);
    // Send message to WindowProcedure
    DispatchMessage(&messages);
  }
  
  // The program return-value is 0 - The value that PostQuitMessage() gave
  return (int) messages.wParam;
}

// This is the call back function for the size dialog
LRESULT CALLBACK GotoDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  HWND hwnd;
  RECT rcOwner;
  
  switch (message) {
    case WM_INITDIALOG:
      SetWindowLongPtr(hDlg, GWL_STYLE, WS_DLGFRAME);
      hwnd = GetWindow(hDlg, GW_OWNER);
      GetWindowRect(hwnd, &rcOwner);
      SetWindowPos(hDlg, HWND_TOP, rcOwner.left + 35, rcOwner.top + 65, 0, 0, SWP_NOSIZE);
      SetDlgItemInt(hDlg, IDC_INDEX, index, FALSE);
      // select the text
      hwnd = GetDlgItem(hDlg, IDC_INDEX);
      SendMessage(hwnd, EM_SETSEL, 0, -1);
      break;
      
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK:
          index = GetDlgItemInt(hDlg, IDC_INDEX, NULL, FALSE);
          EndDialog(hDlg, IDOK);
          return TRUE;
          
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          return TRUE;
      }
  }
  
  return DefWindowProc(hDlg, message, wParam, lParam);
}

// This is the call back function for the size dialog
LRESULT CALLBACK SizeDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  HWND hOwner;
  RECT rcOwner;
  
  switch (message) {
    case WM_INITDIALOG:
      SetWindowLongPtr(hDlg, GWL_STYLE, WS_DLGFRAME);
      hOwner = GetWindow(hDlg, GW_OWNER);
      GetWindowRect(hOwner, &rcOwner);
      SetWindowPos(hDlg, HWND_TOP, rcOwner.left + 35, rcOwner.top + 65, 0, 0, SWP_NOSIZE);
      
      SetDlgItemInt(hDlg, IDC_WIDTH, DIAG_VALUE(tcurw), FALSE);
      SetDlgItemInt(hDlg, IDC_HEIGHT, DIAG_VALUE(tcurh), FALSE);
      SetDlgItemInt(hDlg, IDC_START, DIAG_VALUE(tstart), FALSE);
      SetDlgItemInt(hDlg, IDC_ENDING, DIAG_VALUE(tending), FALSE);
      if (tfixed)
        SendMessage(GetDlgItem(hDlg, IDC_FIXED), BM_SETCHECK, BST_CHECKED, 0);
      EnableWindow(GetDlgItem(hDlg, IDC_WIDTH), DIAG_ENABLED(tcurw));
      EnableWindow(GetDlgItem(hDlg, IDC_HEIGHT), DIAG_ENABLED(tcurh));
      EnableWindow(GetDlgItem(hDlg, IDC_START), DIAG_ENABLED(tstart));
      EnableWindow(GetDlgItem(hDlg, IDC_ENDING), DIAG_ENABLED(tending));
      EnableWindow(GetDlgItem(hDlg, IDC_FIXED), tfixed_enable);
      SetDlgItemText(hDlg, IDC_FONT_NAME, szFontName);
      break;
      
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK:
          tcurw = GetDlgItemInt(hDlg, IDC_WIDTH, NULL, FALSE);
          tcurh = GetDlgItemInt(hDlg, IDC_HEIGHT, NULL, FALSE);
          tstart = GetDlgItemInt(hDlg, IDC_START, NULL, FALSE);
          tending = GetDlgItemInt(hDlg, IDC_ENDING, NULL, FALSE);
          tfixed = (SendMessage(GetDlgItem(hDlg, IDC_FIXED), BM_GETCHECK, 0, 0) == BST_CHECKED);
          GetDlgItemText(hDlg, IDC_FONT_NAME, szFontName, 16);
          EndDialog(hDlg, IDOK);
          return TRUE;
          
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          return TRUE;
      }
  }
  
  return DefWindowProc(hDlg, message, wParam, lParam);
}

// This function is called by the Windows function DispatchMessage()
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  static struct FONT *font = NULL;
  struct FONT_INFO *info;
  bit8u *data;
  
  PAINTSTRUCT ps;
  static int x, y, x1, y1;
  BOOL result;
  LRESULT ret;
  
  // only calculate it if font is allocated
  if (font) {
    info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
    data = (bit8u *) ((bit8u *) info + (sizeof(struct FONT_INFO) * font->count));
  }
  
  switch (message) { // handle the messages
    case WM_CREATE:
      // get pointer to our Menu
      menu = GetMenu(hwnd);
      CheckMenuItem(menu, ID_VIEW_SHOW_NUMS, drawnumbers ? MF_CHECKED : MF_UNCHECKED);
      
      // set the color of the background of the menu
      hbrush = CreateSolidBrush(GetSysColor(COLOR_MENUBAR));
      //hbrush = CreateSolidBrush(RGB(149,198,238));
      mi.cbSize = sizeof(mi); 
      mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS; 
      mi.hbrBack = hbrush;
      SetMenuInfo(menu, &mi);
      
      font = NULL;
      cur_char = 0;
      isdirty = FALSE;
      
      strcpy(szFileName, "");
      return 0;
      
    case WM_LBUTTONDOWN:
      if (drawgrid) {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        if (((x > (GRIDSTART + 10)) && (x < (GRIDSTART + 10 + (curw * BOXSIZE)))) &&
            ((y > 6) && (y < (6 + (curh * BOXSIZE))))) {
          x = ((x - 10 - GRIDSTART) / BOXSIZE);
          y = ((y - 10) / BOXSIZE);
          grid[y][x] ^= 1;
          InvalidateRect(hwnd, NULL, FALSE);
          isdirty = TRUE;
        }
      }
      return 0;
      
    case WM_KEYDOWN: {
      SHORT state = GetKeyState(VK_CONTROL);
      switch (wParam) {
        case VK_DELETE:
          SendMessage(hwnd, WM_COMMAND, ID_EDIT_CLEAR, 0);
          break;
        case 'C':
          if (state & 0x8000)
            SendMessage(hwnd, WM_COMMAND, ID_EDIT_COPY, 0);
          break;
        case 'X':
          if (state & 0x8000)
            SendMessage(hwnd, WM_COMMAND, ID_EDIT_CUT, 0);
          break;
        case 'G':
          if (state & 0x8000)
            SendMessage(hwnd, WM_COMMAND, ID_EDIT_GOTO, 0);
          break;
        case 'E':
          SendMessage(hButtonPrev, BM_CLICK, 0, 0);
          break;
        case 'N':
          SendMessage(hButtonNext, BM_CLICK, 0, 0);
          break;
        case 'F':
          SendMessage(hButtonFirst, BM_CLICK, 0, 0);
          break;
        case 'A':
          SendMessage(hButtonLast, BM_CLICK, 0, 0);
          break;
        case 'U':
          SendMessage(hButtonUp, BM_CLICK, 0, 0);
          break;
        case 'D':
          SendMessage(hButtonDown, BM_CLICK, 0, 0);
          break;
        case 'L':
          SendMessage(hButtonLeft, BM_CLICK, 0, 0);
          break;
        case 'R':
          SendMessage(hButtonRight, BM_CLICK, 0, 0);
          break;
        case 'M':
          SendMessage(hButtonMinus, BM_CLICK, 0, 0);
          break;
        case 'P':
          SendMessage(hButtonPlus, BM_CLICK, 0, 0);
          break;
        case 'I':
          SendMessage(hButtonInvert, BM_CLICK, 0, 0);
          break;
        case 'H':
          SendMessage(hButtonHFlip, BM_CLICK, 0, 0);
          break;
        case 'V':
          if (state & 0x8000) {
            if (!(GetMenuState(menu, ID_EDIT_PASTE, MF_BYCOMMAND) & (MF_DISABLED | MF_GRAYED)))
              SendMessage(hwnd, WM_COMMAND, ID_EDIT_PASTE, 0);
          } else
            SendMessage(hButtonVFlip, BM_CLICK, 0, 0);
          break;
        case 'S':
          SendMessage(hButtonRestore, BM_CLICK, 0, 0);
          break;
//        case 'O':
//          SendMessage(hCheckDrop, BM_CLICK, 0, 0);
//          break;
      }
      // we need to set the focus to the hwnd so it will still catch the keypresses
      SetFocus(hwnd);
    } return 0;
      
    case WM_HSCROLL:
      // the trackbars were moved, so update the display
      InvalidateRect(hwnd, NULL, FALSE);
      SetFocus(hwnd);
      isdirty = TRUE;
      break;
      
    case WM_COMMAND:
      switch LOWORD(wParam) {
        case ID_FILE_NEW:
          if (isdirty)
            if (MessageBox(hwnd, "Save Current File?", "Warning!!!", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
              SaveFile(hwnd, font);
          
          tstart = 0;
          tending = 127;
          tcurw = 8;
          tcurh = 8;
          tfixed = 0;
          tfixed_enable = TRUE;
          strcpy(szFontName, "A font name goes here");
          ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIZE), hwnd, (DLGPROC) SizeDialogProc);
          SetFocus(hwnd); // make sure our main window has focus
          
          if (ret == IDCANCEL)
            return 0;
          
          strcpy(szFileName, "");
          SetWindowText(hwnd, szClassName);
          
          // initialize the font data
          curw = tcurw;
          curh = tcurh;
          font = InitFontData(font, curw, curh, tstart, tending - tstart + 1, tfixed, szFontName);
          
          // initialize our grid
          for (y=0; y<MAXH; y++)
            for (x=0; x<MAXW; x++)
              grid[y][x] = back[y][x] = 0;
          
          cur_char = 0;
          drawgrid = TRUE;
          EnableItems(menu, font);
          EnableMenuItem(menu, ID_FILE_SAVE, MF_GRAYED);
          
          InvalidateRect(hwnd, NULL, TRUE);
          isdirty = FALSE;
          return 0;
          
        case ID_FILE_OPEN:
          if (drawgrid)
            SaveCurChar(font, cur_char);
          
          if (isdirty)
            if (MessageBox(hwnd, "Save Current File?", "Warning!!!", MB_ICONEXCLAMATION | MB_YESNO) == IDYES) {
              if (strlen(szFileName))
                SaveFile(hwnd, font);
              else
                SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
            }
          strcpy(szFileName, "");
          
          result = OpenFileDialog(hwnd, szFileName, (TCHAR *) "Open a Font File.");
          if (result) {
            sprintf(szStr, "%s -- %s", szClassName, szFileName);
            SetWindowText(hwnd, szStr);
            font = OpenFile(hwnd, font);
            if (font == NULL) {
              MessageBox(hwnd, "Error Loading File", "Warning!!!", MB_ICONEXCLAMATION | MB_OK);
              drawgrid = FALSE;
              DisableItems(menu);
              InvalidateRect(hwnd, NULL, TRUE);
              return 0;
            }
            cur_char = 0;
            curw = LoadCurChar(hwnd, font, 0);
            curh = font->height;
            drawgrid = TRUE;
            EnableItems(menu, font);
            EnableMenuItem(menu, ID_FILE_SAVE, MF_ENABLED);
            
            InvalidateRect(hwnd, NULL, TRUE);
            isdirty = FALSE;
          }
          return 0;
          
        case ID_FILE_SAVE:
          SaveCurChar(font, cur_char);
          SaveFile(hwnd, font);
          return 0;
          
        case ID_FILE_SAVEAS:
          SaveCurChar(font, cur_char);
          result = SaveFileDialog(hwnd, szFileName, (TCHAR *) "Save the Font File.", (TCHAR *) "Font Files (*.fnt)\0*.fnt\0\0");
          if (result) {
            SaveFile(hwnd, font);
            sprintf(szStr, "%s -- %s", szClassName, szFileName);
            SetWindowText(hwnd, szStr);
            EnableMenuItem(menu, ID_FILE_SAVE, MF_ENABLED);
          }
          return 0;
          
        case ID_FILE_EXIT:
          if (drawgrid)
            SaveCurChar(font, cur_char);
          
          if (isdirty) {
            ret = MessageBox(hwnd, "Save Current File?", "Warning!!!", MB_ICONEXCLAMATION | MB_YESNOCANCEL);
            if (ret == IDCANCEL)
              return 0;
            if (ret == IDYES)
              SaveFile(hwnd, font);
          }
          
          PostQuitMessage(0);
          return 0;
          
        case ID_EDIT_CUT:
          SendMessage(hwnd, WM_COMMAND, ID_EDIT_COPY, 0);
          SendMessage(hwnd, WM_COMMAND, ID_EDIT_CLEAR, 0);
          EnableMenuItem(menu, ID_EDIT_PASTE, MF_ENABLED);
          break;
          
        case ID_EDIT_COPY:
          for (y=0; y<MAXH; y++)
            for (x=0; x<MAXW; x++)
              clip[y][x] = 0;
          for (y=0; y<curh; y++)
            for (x=0; x<curw; x++)
              clip[y][x] = grid[y][x];
          clipx = (int) SendMessage(hSliderX, TBM_GETPOS, 0, 0);
          clipy = (int) SendMessage(hSliderY, TBM_GETPOS, 0, 0);
          clipw = (int) SendMessage(hSliderW, TBM_GETPOS, 0, 0);
          EnableMenuItem(menu, ID_EDIT_PASTE, MF_ENABLED);
          return 0;
          
        case ID_EDIT_CLEAR:
          // initialize our grid
          for (y=0; y<MAXH; y++)
            for (x=0; x<MAXW; x++)
              grid[y][x] = 0;
          InvalidateRect(hwnd, NULL, FALSE);
          isdirty = TRUE;
          return 0;
          
        case ID_EDIT_PASTE:
          for (y=0; y<curh; y++)
            for (x=0; x<curw; x++)
              grid[y][x] = clip[y][x];
          SendMessage(hSliderX, TBM_SETPOS, TRUE, clipx);
          SendMessage(hSliderY, TBM_SETPOS, TRUE, clipy);
          SendMessage(hSliderW, TBM_GETPOS, TRUE, clipw);
          InvalidateRect(hwnd, NULL, FALSE);
          isdirty = TRUE;
          return 0;
          
        case ID_EDIT_GOTO:
          index = font->start;
          ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GOTO), hwnd, (DLGPROC) GotoDialogProc);
          SetFocus(hwnd); // make sure our main window has focus
          if (ret == IDCANCEL)
            return 0;
          
          index -= font->start;
          if ((index >= 0) && (index < (int) font->count)) {
            SaveCurChar(font, cur_char);
            cur_char = index;
            curw = LoadCurChar(hwnd, font, cur_char);
            InvalidateRect(hwnd, NULL, TRUE);
            // we need to set the focus to the hwnd so it will still catch the keypresses
            SetFocus(hwnd);
          }
          
          return 0;
          
        case ID_VIEW_SHOW_NUMS:
          drawnumbers ^= 1;
          CheckMenuItem(menu, ID_VIEW_SHOW_NUMS, drawnumbers ? MF_CHECKED : MF_UNCHECKED);
          InvalidateRect(hwnd, NULL, TRUE);
          break;
          
        case IDC_CONV_CUR:
          SaveCurChar(font, cur_char);
          tstart = DIAG_DISABLE(font->start);
          tending = DIAG_ENABLE(font->start + font->count - 1);
          tcurw = DIAG_ENABLE(font->max_width);
          tcurh = DIAG_ENABLE(font->height);
          tfixed = (font->flags & FLAGS_FIXED_WIDTH) > 0;
          tfixed_enable = TRUE;
          strcpy(szFontName, font->name);
          ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIZE), hwnd, (DLGPROC) SizeDialogProc);
          SetFocus(hwnd); // make sure our main window has focus
          
          if (ret == IDCANCEL)
            return 0;
          
          result = SaveFileDialog(hwnd, szFileName, (TCHAR *) "Save the converted Font File.", (TCHAR *) "Font Files (*.fnt)\0*.fnt\0\0");
          if (result) {
            font = ConvertFont(hwnd, font, tcurh, tcurw, tfixed, tending, szFontName);
            SaveFile(hwnd, font);
            curw = LoadCurChar(hwnd, font, cur_char);
            sprintf(szStr, "%s -- %s", szClassName, szFileName);
            SetWindowText(hwnd, szStr);
            InvalidateRect(hwnd, NULL, TRUE);
          }
          break;
          
        case IDC_NAME_CUR:
          tstart = DIAG_DISABLE(font->start);  // mark negative to disable item
          tending = DIAG_DISABLE(font->count - 1);
          tcurw = DIAG_DISABLE(font->max_width);
          tcurh = DIAG_DISABLE(font->height);
          tfixed = (font->flags & FLAGS_FIXED_WIDTH) > 0;
          tfixed_enable = FALSE;
          strcpy(szFontName, font->name);
          ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIZE), hwnd, (DLGPROC) SizeDialogProc);
          SetFocus(hwnd); // make sure our main window has focus
          
          if (ret == IDCANCEL)
            return 0;
          
          strcpy(font->name, szFontName);
          isdirty = TRUE;
          break;
          
        case IDC_DUMP:
          // must save first to calculate some of the values to dump.
          // TODO: might want to warn the user first.
          SaveCurChar(font, cur_char);
          if (strlen(szFileName))
            SaveFile(hwnd, font);
          else
            SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
          DumpFont(hwnd, font);
          break;
          
        case IDC_ABOUT:
          MessageBox(hwnd, VERSION_INFO, "About Font Edit", MB_OK);
          break;
          
        case BN_CLICKED:
          if (drawgrid) {
            tcurw = curw;
            if (lParam == (LPARAM) hButtonPrev) {
              if (cur_char > 0) {
                SaveCurChar(font, cur_char);
                cur_char--;
                curw = LoadCurChar(hwnd, font, cur_char);
              }
            } else if (lParam == (LPARAM) hButtonNext) {
              if (cur_char < (int) (font->count - 1)) {
                SaveCurChar(font, cur_char);
                cur_char++;
                curw = LoadCurChar(hwnd, font, cur_char);
              }
            } else if (lParam == (LPARAM) hButtonFirst) {
              SaveCurChar(font, cur_char);
              cur_char = 0;
              curw = LoadCurChar(hwnd, font, cur_char);
            } else if (lParam == (LPARAM) hButtonLast) {
              SaveCurChar(font, cur_char);
              cur_char = (font->count - 1);
              curw = LoadCurChar(hwnd, font, cur_char);
            } else if (lParam == (LPARAM) hButtonUp) {
              for (y=0; y<(curh - 1); y++)
                for (x=0; x<curw; x++)
                  grid[y][x] = grid[y+1][x];
              for (x=0; x<curw; x++)
                grid[y][x] = 0;
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonDown) {
              for (y=(curh - 1); y; y--)
                for (x=0; x<curw; x++)
                  grid[y][x] = grid[y-1][x];
              for (x=0; x<curw; x++)
                grid[0][x] = 0;
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonLeft) {
              for (y=0; y<curh; y++) {
                for (x=0; x<(curw-1); x++)
                  grid[y][x] = grid[y][x+1];
                grid[y][x] = 0;
              }
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonRight) {
              for (y=0; y<curh; y++) {
                for (x=(curw - 1); x; x--)
                  grid[y][x] = grid[y][x-1];
                grid[y][0] = 0;
              }
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonInvert) {
              for (y=0; y<curh; y++)
                for (x=0; x<curw; x++)
                  grid[y][x] ^= 1;
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonVFlip) {
              y1 = curh - 1;
              for (y=0; y<=y1; y++) {
                for (x=0; x<curw; x++) {
                  x1 = grid[y][x];
                  grid[y][x] = grid[y1][x];
                  grid[y1][x] = x1;
                }
                y1--;
              }
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonHFlip) {
              x1 = curw - 1;
              for (x=0; x<=x1; x++) {
                for (y=0; y<curh; y++) {
                  y1 = grid[y][x];
                  grid[y][x] = grid[y][x1];
                  grid[y][x1] = y1;
                }
                x1--;
              }
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonRestore) {
              for (y=0; y<curh; y++)
                for (x=0; x<curw; x++)
                  grid[y][x] = back[y][x];
              isdirty = TRUE;
            } else if (lParam == (LPARAM) hButtonMinus) {
              if (!(font->flags & FLAGS_FIXED_WIDTH)) {
                if (curw > 0) {
                  // can we narrow the window
                  if (curw == 16)
                    SetWindowPos(hwnd, NULL, 0, 0, APP_WIDTH, APP_GET_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
                  y = ((((curw * curh) + 7) & ~7) / 8);  // bytes for this char
                  curw--;
                  y1 = ((((curw * curh) + 7) & ~7) / 8); // bytes for this char after decrementing
                  // decrement the width of this char in the font
                  info[cur_char].width--;
                  if (y - y1) {
                    FontMoveData(font, cur_char + 1, -(y - y1));
                    // decrement the index of each char after in the font
                    for (x=cur_char+1; x<(int) font->count; x++)
                      info[x].index -= (y - y1);
                  }
                  isdirty = TRUE;
                  EnableWindow(hButtonRestore, FALSE);  // can't restore now that the size is different
                }
              }
            } else if (lParam == (LPARAM) hButtonPlus) {
              if (!(font->flags & FLAGS_FIXED_WIDTH)) {
                if (curw < MAXW) {
                  // do we need to widen the window?
                  if (curw == 15)
                    SetWindowPos(hwnd, NULL, 0, 0, APP_WIDTH_WIDE, APP_GET_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
                  // we need to clear out the column of bits that we will be adding.
                  for (y=0; y<curh; y++)
                    grid[y][curw] = 0;
                  // calculate the width in bytes before the increase
                  y = ((((curw * curh) + 7) & ~7) / 8);  // bytes for this char
                  curw++;
                  // and again after the increase
                  y1 = ((((curw * curh) + 7) & ~7) / 8); // bytes for this char after incrementing
                  // increment the width of this char in the font
                  info[cur_char].width++;
                  if (y1 - y) {
                    FontMoveData(font, cur_char + 1, (y1 - y));
                    // increment the index of each char after in the font
                    for (x=cur_char+1; x<(int) font->count; x++)
                      info[x].index += (y1 - y);
                  }
                  isdirty = TRUE;
                  EnableWindow(hButtonRestore, FALSE);  // can't restore now that the size is different
                }
              }
            }
            InvalidateRect(hwnd, NULL, (tcurw != curw));
            
            // enable/disable Prev/Next depending on where we are in the font
            EnableWindow(hButtonPrev, cur_char > 0);
            EnableWindow(hButtonNext, cur_char < (font->count - 1));
            
            // we need to set the focus to the hwnd so it will still catch the keypresses
            SetFocus(hwnd);
          }
          break;
      }
      
    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      SetBkMode(hdc, TRANSPARENT);
      //SetTextColor(hdc, RGB(0, 0, 0));
      
      // draw the grid?
      if (drawgrid) {
        // draw the box to hold the current char number, etc.
        sprintf(szStr, "Current char:  %i", cur_char + font->start);
        DrawTextBox(hdc, WHITENESS, BUTTONSRT, 10, TEXTBOXW, TEXTBOXH, szStr);
        
        // draw the border
        DrawTextBox(hdc, NOTSRCERASE, 6 + GRIDSTART, 6, (BOXSIZE * curw) + 6, (BOXSIZE * curh) + 6, NULL);
        
        // draw the grid
        for (y=0; y<curh; y++) {
          for (x=0; x<curw; x++) {
            x1 = (x * BOXSIZE) + 10;
            y1 = (y * BOXSIZE) + 10;
            DrawTextBox(hdc, (grid[y][x]) ? BLACKNESS : WHITENESS, x1 + GRIDSTART, y1, BOXSIZE - 5, BOXSIZE - 5, NULL);
          }
          
          // Row Number (y)
          if (drawnumbers) {
            sprintf(szStr, "% 2i", (curh - y));
            rect.left = x1 + GRIDSTART + 33;
            rect.top = y1 + 5;
            rect.right = rect.left + 20;
            rect.bottom = rect.top + 16;
            DrawText(hdc, szStr, -1, &rect, DT_RIGHT);
          }
        }
        
        // Column Number (x)
        if (drawnumbers) {
          y1 = (curh * BOXSIZE) + 15;
          for (x=0; x<curw; x++) {
            sprintf(szStr, "% 2i", x + 1);
            rect.left = GRIDSTART + (x * BOXSIZE) + 12;
            rect.top = y1;
            rect.right = rect.left + 20;
            rect.bottom = rect.top + 16;
            DrawText(hdc, szStr, -1, &rect, DT_RIGHT);
          }
        }
        
        // Slider box text
        rect.left = BUTTONSRT;
        rect.top = 10 + TEXTBOXH + 255;
        rect.right = rect.left + 120;
        rect.bottom = rect.top + 16;
        DrawText(hdc, "Horizontal Delta", -1, &rect, DT_CENTER);
#ifdef _WIN64
        sprintf(szStr, "%lli", SendMessage(hSliderX, TBM_GETPOS, 0, 0));
#else
        sprintf(szStr, "%i", SendMessage(hSliderX, TBM_GETPOS, 0, 0));
#endif
        DrawTextBox(hdc, WHITENESS, BUTTONSRT - 10 + SLIDERW + 10, 10 + TEXTBOXH + 275, BOXSIZE + 5, SLIDERH - 1, szStr);
        
        rect.top += SLIDERH + 30;
        rect.bottom = rect.top + 16;
        DrawText(hdc, "Vertical Delta", -1, &rect, DT_CENTER);
#ifdef _WIN64
        sprintf(szStr, "%lli", SendMessage(hSliderY, TBM_GETPOS, 0, 0));
#else
        sprintf(szStr, "%i", SendMessage(hSliderY, TBM_GETPOS, 0, 0));
#endif
        DrawTextBox(hdc, WHITENESS, BUTTONSRT - 10 + SLIDERW + 10, 10 + TEXTBOXH + 275 + SLIDERH + 30, BOXSIZE + 5, SLIDERH - 1, szStr);
        
        rect.top += SLIDERH + 30;
        rect.bottom = rect.top + 16;
        DrawText(hdc, "Width Delta", -1, &rect, DT_CENTER);
#ifdef _WIN64
        sprintf(szStr, "%lli", SendMessage(hSliderW, TBM_GETPOS, 0, 0));
#else
        sprintf(szStr, "%i", SendMessage(hSliderW, TBM_GETPOS, 0, 0));
#endif
        DrawTextBox(hdc, WHITENESS, BUTTONSRT - 10 + SLIDERW + 10, 10 + TEXTBOXH + 275 + ((SLIDERH + 30) * 2), BOXSIZE + 5, SLIDERH - 1, szStr);
      }
      
      EndPaint(hwnd, &ps);
      return 0;
      
    case WM_DESTROY:
      PostQuitMessage(0);       // send a WM_QUIT to the message queue
      break;
  }
  
  return DefWindowProc(hwnd, message, wParam, lParam);
}

void DrawTextBox(HDC hdc, DWORD fill, const int x, const int y, const int w, const int h, const char *str) {
  HPEN white, black;
  black = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
  white = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  
  // draw the box
  BitBlt(hdc, x + 2, y + 2, w - 3, h - 3, NULL, 0, 0, fill);
  SelectObject(hdc, white);
  MoveToEx(hdc, x, y, (LPPOINT) NULL);
  LineTo(hdc, x, y + h);
  LineTo(hdc, x + w, y + h);
  SelectObject(hdc, black);
  LineTo(hdc, x + w, y);
  LineTo(hdc, x, y);
  
  // display the text (if any)
  if (str) {
    RECT r;
    r.left = x + 4;
    r.top = y + 4;
    r.right = x + w - 3;
    r.bottom = y + w - 3;
    DrawText(hdc, str, -1, &r, DT_LEFT);
  }
  
  DeleteObject(white);
  DeleteObject(black);
}


void DisableItems(HMENU menu) {
  EnableMenuItem(menu, ID_FILE_SAVE, MF_GRAYED);
  EnableMenuItem(menu, ID_FILE_SAVEAS, MF_GRAYED);
  EnableMenuItem(menu, ID_EDIT_CUT, MF_GRAYED);
  EnableMenuItem(menu, ID_EDIT_COPY, MF_GRAYED);
  EnableMenuItem(menu, ID_EDIT_CLEAR, MF_GRAYED);
  EnableMenuItem(menu, ID_EDIT_GOTO, MF_GRAYED);
  EnableMenuItem(menu, ID_VIEW_SHOW_NUMS, MF_GRAYED);
  EnableMenuItem(menu, IDC_CONV_CUR, MF_GRAYED);
  EnableMenuItem(menu, IDC_NAME_CUR, MF_GRAYED);
  EnableMenuItem(menu, IDC_DUMP, MF_GRAYED);
  
  // hide buttons
  ShowWindow(hButtonPrev, SW_HIDE);
  ShowWindow(hButtonNext, SW_HIDE);
  ShowWindow(hButtonFirst, SW_HIDE);
  ShowWindow(hButtonLast, SW_HIDE);
  ShowWindow(hButtonUp,   SW_HIDE);
  ShowWindow(hButtonDown, SW_HIDE);
  ShowWindow(hButtonLeft, SW_HIDE);
  ShowWindow(hButtonRight, SW_HIDE);
  ShowWindow(hButtonMinus, SW_HIDE);
  ShowWindow(hButtonPlus, SW_HIDE);
  ShowWindow(hButtonInvert, SW_HIDE);
  ShowWindow(hButtonHFlip, SW_HIDE);
  ShowWindow(hButtonVFlip, SW_HIDE);
  ShowWindow(hButtonRestore, SW_HIDE);
  ShowWindow(hSliderX, SW_HIDE);
  ShowWindow(hSliderY, SW_HIDE);
  ShowWindow(hSliderW, SW_HIDE);

//  ShowWindow(hCheckDrop, SW_HIDE);
}

void EnableItems(HMENU menu, struct FONT *font) {
  EnableMenuItem(menu, ID_FILE_SAVEAS, MF_ENABLED);
  EnableMenuItem(menu, ID_EDIT_COPY, MF_ENABLED);
  EnableMenuItem(menu, ID_EDIT_CUT, MF_ENABLED);
  EnableMenuItem(menu, ID_EDIT_CLEAR, MF_ENABLED);
  EnableMenuItem(menu, ID_EDIT_GOTO, MF_ENABLED);
  EnableMenuItem(menu, ID_VIEW_SHOW_NUMS, MF_ENABLED);
  EnableMenuItem(menu, IDC_CONV_CUR, MF_ENABLED);
  EnableMenuItem(menu, IDC_NAME_CUR, MF_ENABLED);
  EnableMenuItem(menu, IDC_DUMP, MF_ENABLED);
  
  // show buttons
  SetWindowPos(hButtonPrev, HWND_TOP, BUTTONSRT, 10 + TEXTBOXH + 10, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonPrev, SW_SHOW);
  SetWindowPos(hButtonNext, HWND_TOP, BUTTONSRT + 75, 10 + TEXTBOXH + 10, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonNext, SW_SHOW);
  EnableWindow(hButtonPrev, cur_char > 0);
  EnableWindow(hButtonNext, cur_char < (font->count - 1));
  
  SetWindowPos(hButtonFirst, HWND_TOP, BUTTONSRT, 10 + TEXTBOXH + 40, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonFirst, SW_SHOW);
  SetWindowPos(hButtonLast, HWND_TOP, BUTTONSRT + 75, 10 + TEXTBOXH + 40, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonLast, SW_SHOW);
  
  SetWindowPos(hButtonUp, HWND_TOP, BUTTONSRT + 52, 10 + TEXTBOXH + 80, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonUp,   SW_SHOW);
  SetWindowPos(hButtonDown, HWND_TOP, BUTTONSRT + 52, 10 + TEXTBOXH + 130, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonDown, SW_SHOW);
  SetWindowPos(hButtonLeft, HWND_TOP, BUTTONSRT + 27, 10 + TEXTBOXH + 105, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonLeft, SW_SHOW);
  SetWindowPos(hButtonRight, HWND_TOP, BUTTONSRT + 52 + 23, 10 + TEXTBOXH + 105, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonRight, SW_SHOW);
  
  SetWindowPos(hButtonMinus, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 165, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonMinus, SW_SHOW);
  EnableWindow(hButtonMinus, (!(font->flags & FLAGS_FIXED_WIDTH))); // if this is a fixed width font, don't allow the user to plus or minus
  SetWindowPos(hButtonPlus, HWND_TOP, BUTTONSRT - 10 + 80, 10 + TEXTBOXH + 165, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonPlus, SW_SHOW);
  EnableWindow(hButtonPlus, (!(font->flags & FLAGS_FIXED_WIDTH))); // if this is a fixed width font, don't allow the user to plus or minus
  
  SetWindowPos(hButtonInvert, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 195, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonInvert, SW_SHOW);
  SetWindowPos(hButtonRestore, HWND_TOP, BUTTONSRT - 10 + 80, 10 + TEXTBOXH + 195, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonRestore, SW_SHOW);
  
  SetWindowPos(hButtonHFlip, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 225, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonHFlip, SW_SHOW);
  SetWindowPos(hButtonVFlip, HWND_TOP, BUTTONSRT - 10 + 80, 10 + TEXTBOXH + 225, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hButtonVFlip, SW_SHOW);

  SetWindowPos(hSliderX, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 275, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hSliderX, SW_SHOW);
  SetWindowPos(hSliderY, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 275 + SLIDERH + 30, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hSliderY, SW_SHOW);
  SetWindowPos(hSliderW, HWND_TOP, BUTTONSRT - 10, 10 + TEXTBOXH + 275 + ((SLIDERH + 30) * 2), 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
  ShowWindow(hSliderW, SW_SHOW);
  
//  SetWindowPos(hCheckDrop, HWND_TOP, BUTTONSRT, 10 + TEXTBOXH + 255, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//  ShowWindow(hCheckDrop, SW_SHOW);
}

HWND CreateButton(HWND hwnd, DWORD Style, const char *text, const int w, const int h) {
  HWND hButton = CreateWindowEx(
           WS_EX_CLIENTEDGE,    // Extended possibilites for variation
           TEXT("BUTTON"),      // Classname
           text,                // Title Text
           Style | WS_CHILD | WS_VISIBLE,
           CW_USEDEFAULT,       // Windows decides the position
           CW_USEDEFAULT,       //   where the window ends up on the screen
           w,                   // The button's width
           h,                   //  and height in pixels
           hwnd,                // The window is a child-window to hwnd
           NULL,                // menu
           GetModuleHandle(NULL), // Program Instance handler
           NULL                 // No Window Creation data
         );
  return hButton;
}

HWND CreateTrackBar(HWND hwnd, const int w, const int h) {
  HWND Slider = CreateWindowEx(
          WS_EX_CLIENTEDGE,
          TRACKBAR_CLASS,
          NULL,
          WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, 
          0, 0,
          w,                   // The button's width
          h,                   //  and height in pixels
          hwnd,                // The window is a child-window to hwnd
          NULL,                // menu
          GetModuleHandle(NULL), // Program Instance handler
          NULL                 // No Window Creation data
        );
  SendMessage(Slider, TBM_SETRANGE, TRUE, MAKELONG(-10, 10));
  SendMessage(Slider, TBM_SETPOS, TRUE, 0);
  SendMessage(Slider, TBM_SETTICFREQ, 2, 0);
  
  return Slider;
}

BOOL OpenFileDialog(HWND hwnd, LPTSTR pFileName ,LPTSTR pTitleName) {
  ZeroMemory(&ofn, sizeof(ofn));
  
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hInstance = GetModuleHandle(NULL);
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 0;

  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = pFileName;
  ofn.lpstrFileTitle = NULL;
  ofn.lpstrTitle = pTitleName;
  ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
  ofn.lpstrFilter = TEXT("Font Files (*.fnt)\0*.fnt\0\0");
  
  ofn.nMaxFile = 500;
  ofn.nMaxFileTitle = MAX_PATH;
  
  return GetOpenFileName(&ofn);
}

BOOL SaveFileDialog(HWND hwnd, LPTSTR pFileName, LPTSTR pTitleName, LPTSTR pFilter) {
  ZeroMemory(&ofn, sizeof(ofn));
  
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hInstance = GetModuleHandle(NULL);
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 0;
  
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = pFileName;
  ofn.lpstrFileTitle = NULL;
  ofn.lpstrTitle = pTitleName;
  ofn.lpstrDefExt = "fnt";
  ofn.Flags = OFN_EXPLORER|OFN_OVERWRITEPROMPT;
  ofn.lpstrFilter = pFilter;
  
  ofn.nMaxFile = 500;
  ofn.nMaxFileTitle = MAX_PATH;
  
  return GetSaveFileName(&ofn);
}

// width and height are in pixels
struct FONT *InitFontData(struct FONT *font, const int width, const int height, const int start, const int count, const int fixed, const char *name) {
  int i, w;
  
  // if font already allocated, free it first
  if (font)
    free(font);
  
  w = ((((width * height) + 7) & ~7) / 8); // bytes needed per char to store the whole char
  int sz = sizeof(struct FONT) + ((sizeof(struct FONT_INFO) + w) * count);
  font = (struct FONT *) calloc(sz + 4096, 1); // plus 4096 to compensate for widening characters
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  
  for (i=0; i<count; i++) {
    info[i].index = (w * i);
    info[i].width = width;
  }
  
  memcpy(font->sig, "FONT", 4);
  font->height = height;
  font->count = count;
  font->max_width = width;
  font->datalen = (w * count);
  font->total_size = sz;
  font->start = start;
  font->flags = (fixed) ? FLAGS_FIXED_WIDTH : 0;
  font->version = FILE_VERSION;
  font->type = FILE_TYPE;
  font->type_vers = TYPE_VERSION;
  strcpy(font->name, name);
  
  return font;
}

void SaveFile(HWND hwnd, struct FONT *font) {
  bit32u i, w;
  FILE *fp;
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  
  // open the file (truncating it if already exists)
  if ((fp = fopen(szFileName, "w+b")) == NULL)
    return;
  
  // the size might have changed a little, so lets re-calculate
  //  this size before we write it.
  w = ((((info[font->count - 1].width * font->height) + 7) & ~7) / 8); // bytes needed to store the whole char
  font->datalen = (info[font->count - 1].index + w);
  font->total_size = sizeof(struct FONT) + (sizeof(struct FONT_INFO) * font->count) + font->datalen;
  
  // also run through it to catch the widest char
  w = 0;
  for (i=0; i<font->count; i++)
    if (info[i].width > w)
      w = info[i].width;
  font->max_width = (bit8u) w;
  
  fwrite(font, 1, font->total_size, fp);
  fclose(fp);
  
  isdirty = FALSE;
}

struct FONT *OpenFile(HWND hwnd, struct FONT *font) {
  FILE *fp;
  int sz;
  
  // Open the file.
  if ((fp = fopen(szFileName, "r+b")) == NULL)
    return NULL;
  
  if (font)
    free(font);
  
  fseek(fp, 0, SEEK_END);
  sz = ftell(fp);
  rewind(fp);
  
  font = (struct FONT *) malloc(sz + 4096); // plus 4096 to compensate for widening characters
  
  if (fread(font, 1, sz, fp) != sz) {
    fclose(fp);
    return NULL;
  }
  
  if (memcmp(font->sig, "FONT", 4)) {
    MessageBox(hwnd, "File Signature Error", "Error", MB_OK);
    fclose(fp);
    return NULL;
  }

  
  
  // TODO:
  // just in case someone created a font before we added these version fields, 
  //   automatically adjust these fields when opening
  // This is highly unlikely that anyone did, but we do it anyway.
  //  We can remove these later, because as soon as someone saves a font, these fields are updated.
  font->version = FILE_VERSION;
  font->type = FILE_TYPE;
  font->type_vers = TYPE_VERSION;
  
  
  
  // check to file version.  We currently only support version 1.0
  if (font->version != FILE_VERSION) {
    MessageBox(hwnd, "Incorrect File Version Found", "Error", MB_OK);
    fclose(fp);
    return NULL;
  }

  // check to file type.  We currently only support type 0
  if (font->type != FILE_TYPE) {
    MessageBox(hwnd, "Incorrect File Type Found", "Error", MB_OK);
    fclose(fp);
    return NULL;
  }
  
  // check to type version.  We currently only support version 1.0 of type 0
  if (font->type_vers != TYPE_VERSION) {
    MessageBox(hwnd, "Incorrect Type Version Found", "Error", MB_OK);
    fclose(fp);
    return NULL;
  }
  
  fclose(fp);
  
  return font;
}

// returns the bit value of the bit 'i' away from 'p'
// i = 0 = bit 7 of first byte
bit8u GetBit(bit8u *p, const int i) {
  int byte = (i / 8);
  int bit = (i % 8);
  
  return (p[byte] & (0x80 >> bit)) ? 1 : 0;  
}

// convert font data to grid data
int LoadCurChar(HWND hwnd, struct FONT *font, const int ch) {
  int x, y, i;
  bit8u *p;
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  bit8u *data = (bit8u *) ((bit8u *) info + (sizeof(struct FONT_INFO) * font->count));
  
  i = 0;
  p = (bit8u *) (data + info[ch].index);
  for (y=0; y<(int) font->height; y++)
    for (x=0; x < info[ch].width; x++)
      back[y][x] =  // save the loaded grid to back[][] so we can restore it
        grid[y][x] = GetBit(p, i++);
  EnableWindow(hButtonRestore, TRUE);
  
  // set or clear the button if the flag is set
//  if (info[ch].flags & INFO_IS_DROPPED)
//    Button_SetCheck(hCheckDrop, BST_CHECKED);
//  else
//    Button_SetCheck(hCheckDrop, BST_UNCHECKED);

  SendMessage(hSliderX, TBM_SETPOS, TRUE, info[ch].deltax);
  SendMessage(hSliderY, TBM_SETPOS, TRUE, info[ch].deltay);
  SendMessage(hSliderW, TBM_SETPOS, TRUE, info[ch].deltaw);
  
  // do we need to widen the window
  int width = (info[ch].width >= 15) ? APP_WIDTH_WIDE : APP_WIDTH;
  SetWindowPos(hwnd, NULL, 0, 0, width, APP_GET_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
  
  return info[ch].width;
}

// stores the bit value of the bit 'i' away from 'p'
// i = 0 = bit 7 of first byte
void PutBit(bit8u *p, const int i, const char val) {
  int byte = (i / 8);
  int bit = (i % 8);
  
  p[byte] &= ~(0x80 >> bit);  // first clear it
  if (val)
    p[byte] |=  (0x80 >> bit);  // then set it (if val)
}

// convert grid data to font data
void SaveCurChar(struct FONT *font, const int ch) {
  bit32u x, y, i;
  bit8u *p;
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  bit8u *data = (bit8u *) ((bit8u *) info + (sizeof(struct FONT_INFO) * font->count));

  i = 0;
  p = (bit8u *) (data + info[ch].index);
  for (y=0; y<font->height; y++)
    for (x=0; x < info[ch].width; x++)
      PutBit(p, i++, grid[y][x]);
  
  // make sure that last part of the last byte is zeros
  while (i & 7)
    PutBit(p, i++, 0);
  
  info[ch].deltax = (char) ((int) SendMessage(hSliderX, TBM_GETPOS, 0, 0));
  info[ch].deltay = (char) ((int) SendMessage(hSliderY, TBM_GETPOS, 0, 0));
  info[ch].deltaw = (char) ((int) SendMessage(hSliderW, TBM_GETPOS, 0, 0));
  
  // set or clear the flag if the button is checked
//  if (Button_GetCheck(hCheckDrop) == BST_CHECKED)
//    info[ch].flags |= INFO_IS_DROPPED;
//  else
//    info[ch].flags &= ~INFO_IS_DROPPED;
}

void FontMoveData(struct FONT *font, bit32u ch, int delta) {
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  bit8u *data = (bit8u *) ((bit8u *) info + (sizeof(struct FONT_INFO) * font->count));
  
  if (ch < font->count) {
    memmove((void *) (data + info[ch].index + delta), (void *) (data + info[ch].index), font->datalen - info[ch].index);
    font->datalen += delta;
  }
}

struct FONT *ConvertFont(HWND hwnd, struct FONT *font, const int height, const int width, const int fixed, const int ending, const char *name) {
  int i, j, w, sz, dw = 0, dh = 0;
  int nCount = ending + 1 - font->start;
  int n = (nCount < font->count) ? nCount : (int) font->count;
  
  w = ((((width * height) + 7) & ~7) / 8); // bytes needed to store the whole char
  sz = sizeof(struct FONT) + ((sizeof(struct FONT_INFO) + w) * nCount);
  struct FONT *nFont = (struct FONT *) calloc(sz + 4096, 1); // plus 4096 to compensate for widening characters
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) nFont + sizeof(struct FONT));
  
  memcpy(nFont->sig, "FONT", 4);
  nFont->height = height;
  nFont->max_width = width;
  nFont->count = nCount;
  nFont->datalen = (w * height * nCount);
  nFont->total_size = sz;
  nFont->start = font->start;
  nFont->flags = (fixed) ? FLAGS_FIXED_WIDTH : 0;
  nFont->version = FILE_VERSION;
  nFont->type = FILE_TYPE;
  nFont->type_vers = TYPE_VERSION;
  strcpy(nFont->name, name);
  
  if (width < curw)
    dw = ((curw - width) + 1) / 2;
  if (height < curh)
    dh = ((curh - height) + 1) / 2;
  for (i=0; i<n; i++) {
    LoadCurChar(hwnd, font, i);
    for (j=0; j<dw; j++)
      SendMessage(hwnd, BN_CLICKED, 0, (LPARAM) hButtonLeft);
    for (j=0; j<dh; j++)
      SendMessage(hwnd, BN_CLICKED, 0, (LPARAM) hButtonUp);
    info[i].width = width;
    info[i].index = (w * i);
    SaveCurChar(nFont, i);
  }
  for (; i<nCount; i++) {
    for (int y=0; y<MAXH; y++)
      for (int x=0; x<MAXW; x++)
        grid[y][x] = back[y][x] = 0;
    info[i].width = width;
    info[i].index = (w * i);
    info[i].flags = 0;
    SaveCurChar(nFont, i);
    info[i].deltaw = 0;
    info[i].deltax = 0;
    info[i].deltay = 0;
  }
  
  curw = width;
  curh = height;
  free(font);
  
  return nFont;
}

void DumpFont(HWND hwnd, struct FONT *font) {
  FILE *fp;
  bit32u i, j, k, l;
  bit8u *p;
  
  struct FONT_INFO *info = (struct FONT_INFO *) ((bit8u *) font + sizeof(struct FONT));
  bit8u *data = (bit8u *) ((bit8u *) info + (sizeof(struct FONT_INFO) * font->count));
  
  strcpy(szStr, "");
  BOOL result = SaveFileDialog(hwnd, szStr, (TCHAR *) "Dump the Font File.", (TCHAR *) "Dump Files (*.txt)\0*.txt\0\0");
  if (result) {
    if ((fp = fopen(szStr, "w+b")) == NULL)
      return;
    
    // the font structure
    fprintf(fp, "struct FONT font%ix%i = {\r\n  {\r\n", font->max_width, font->height);
    
    fprintf(fp, "    '");
    for (i=0; i<4; i++)
      fprintf(fp, "%c", font->sig[i]);
    fprintf(fp, "',\r\n");
    fprintf(fp, "     % 5i,        // height of font in pixels\r\n"
                "     % 5i,        // max_width\r\n"
                "     % 5i,        // start\r\n"
                "     % 5i,        // count of chars in set\r\n"
                "     % 5i,        // data len\r\n"
                "     % 5i,        // total size\r\n"
                "     0x%08X,   // flags\r\n"
                "     0x%02X,         // version\r\n"
                "     0x%02X,         // type\r\n"
                "     0x%02X,         // type version\r\n"
                "     { 0, },       // reserved\r\n"
                "     '%s'      // name\r\n"
                "     { 0, },       // reserved\r\n },\r\n",
                font->height, font->max_width, font->start, font->count, font->datalen, font->total_size, font->flags, font->version, font->type, font->type_vers, font->name);
    
    // dump the font info
    fprintf(fp, "\r\n/* Font Info */\r\n   {\r\n");
    for (i=0; i<font->count; i++) {
      fprintf(fp, "     /* character %i */\r\n  ", i);
      if (i < ((bit32u) font->count - 1))
        fprintf(fp, "    { % 5i, % 3i, 0x%02X, % 2i, % 2i, % 2i, 0 },\r\n", info[i].index, info[i].width, info[i].flags, info[i].deltax, info[i].deltay, info[i].deltaw);
      else
        fprintf(fp, "    { % 5i, % 3i, 0x%02X, % 2i, % 2i, % 2i, 0 }\r\n", info[i].index, info[i].width, info[i].flags, info[i].deltax, info[i].deltay, info[i].deltaw);
    }
    fprintf(fp, "    },\r\n");
    
    // dump the data
    fprintf(fp, "\r\n/* Data */\r\n  {\r\n");
    for (i=0; i<font->count; i++) {
      fprintf(fp, "    /* character %i */\r\n  ", i);
      p = (bit8u *) (data + info[i].index);
      k = ((font->height * info[i].width) + 7) / 8;
      for (j=0; j<k; j++) {
        if ((i == (font->count - 1)) && (j == (font->height - 1)))
          fprintf(fp,   "0x%02X", p[j]);
        else
          fprintf(fp,   "0x%02X, ", p[j]);
      }
      fprintf(fp, "\r\n");
      fprintf(fp, "    /* Bitmap:\r\n    ");
      l = 0;
      for (j=0; j<font->height; j++) {
        for (k=0; k<info[i].width; k++)
          fprintf(fp, "%c", (GetBit(p, l++)) ? '1' : '.');
        fprintf(fp, "\r\n    ");
      }
      fprintf(fp, "*/\r\n");
    }
    fprintf(fp, "  }\r\n};\r\n\r\n");

    /*
    // dump the data
    for (i=0; i<font->count; i++) {
      fprintf(fp, "\r\n ; character %i\r\n", i);
      p = (bit8u *) (data + info[i].index);
      l = 0;
      for (j=0; j<font->height; j++) {
        fprintf(fp, "   db  ");
        for (k=0; k<info[i].width; k++)
          fprintf(fp, "%i", (GetBit(p, l++)) ? 1 : 0);
        fprintf(fp, "b\r\n");
      }
    }
    */

    

    fclose(fp);
  } else
    MessageBox(hwnd, "Was not able to create dump file...", "Error", MB_OK);
}


/*

  !"#$%&'()*+,-./
  0123456789
  :;<=>?@
  ABCDEFGHIJKLMNOPQRSTUVWXYZ
  [\]^_'
  abcdefghijklmnopqrstuvwxyz
  {|}~

*/