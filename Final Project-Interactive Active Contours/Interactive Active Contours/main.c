#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)

{
MSG			msg;
HWND		hWnd;
WNDCLASS	wc;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="PLUS";

if (!RegisterClass(&wc))
  return(FALSE);

hWnd=CreateWindow("PLUS","Huzefa_Kagalwala_FinalProject",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,700,650,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

RubberBand = 0; RubberBandActive = 0;
Balloon = 0;
Neutral = 0;
Original = 1;
Sobel = 0;
Auto = 0;
Restore = 0;
Step = 0;
stepFlag = 1;

strcpy(filename,"");
OriginalImage=NULL;
ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}


LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam)

{
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
char				header[320], text[320];
int					BYTES, xPos, yPos, r, c, r2, c2, *indices;
TotalRegions = 0;
switch (uMsg)
  {
  case WM_COMMAND:
    switch (LOWORD(wParam))
      {
	  case ID_COLOR_YELLOW:
		Yellow = (Yellow + 1) % 2;
		if (Blue == 1 || Green == 1 || Red == 1 || Orange == 1)
		{
			Green = Blue = Red = Orange = 0;
		}
		if (Yellow == 1) { color2 = RGB(255, 255, 0); }
		break;
	  case ID_COLOR_BLUE:
		Blue = (Blue + 1) % 2;
		if (Red == 1 || Green == 1 || Yellow == 1 || Orange == 1)
		{
			Green = Red = Yellow = Orange = 0;
		}
		if (Blue == 1) { color2 = RGB(0, 0, 255); }
		break;
	  case ID_COLOR_ORANGE:
		Orange = (Orange + 1) % 2;
		if (Blue == 1 || Green == 1 || Yellow == 1 || Red == 1)
		{
			Green = Blue = Yellow = Red = 0;
		}
		if (Orange == 1) { color2 = RGB(255, 127, 0); }
		break;
	  case ID_COLOR_RED:
		Red = (Red + 1) % 2;
		if (Blue == 1 || Green == 1 || Yellow == 1 || Orange == 1)
		{
			Green = Blue = Yellow = Orange = 0;
		}
		if (Red == 1) { color2 = RGB(255, 0, 0); }
		break;
	  case ID_COLOR_GREEN:
		Green = (Green + 1) % 2;
		if (Red == 1 || Blue == 1 || Yellow == 1 || Orange == 1)
		{
			Red = Blue = Yellow = Orange = 0;
		}
		if (Green == 1) { color2 = RGB(0, 255, 0); }
		break;
	  case ID_REGIONGROWING_AUTO:
		  Auto = (Auto + 1) % 2;
		  if (Auto == 1) { Restore = Step = 0; stepFlag = 1; }
		  break;
	  case ID_ACTIVECONTOURS_RUBBERBAND:
		  RubberBand = (RubberBand + 1) % 2;
		  if (RubberBand == 1)
		  {
			  RubberBandActive = 1;
			  PaintImage();
			  Balloon = 0; BalloonActive = 0;
			  Neutral = 0;
			  idx = 0;
			  for (int x = 0; x < idx_init; x++)
			  {
				  if (x % 5 == 0)
				  {
					  rbx[idx] = rbx_init[x];
					  rby[idx] = rby_init[x];
					  idx++;
				  }
			  }
			  _beginthread(RubberBandThread, 0, MainWnd);
			  //memset(idx_init, 0, sizeof idx_init); memset(rbx_init, 0, sizeof rbx_init);
			  //memset(rby_init, 0, sizeof rby_init);
		  }
		  break;
	  case ID_ACTIVECONTOURS_BALLOON:
		  Balloon = (Balloon + 1) % 2;
		  if (Balloon == 1)
		  {
			  RubberBand = 0; RubberBandActive = 0;  Neutral = 0; BalloonActive = 1;
			  //PaintImage();
			  _beginthread(BalloonThread, 0, MainWnd);
		  }
		  break;
	  case ID_ACTIVECONTOURS_NEUTRAL:
		  Neutral = (Neutral + 1) % 2;
		  if (Neutral == 1) 
		  {
			  RubberBand = 0; Balloon = 0; 
			  PaintImage();
			  if (RubberBandActive == 1) { _beginthread(NeutralRBThread, 0, MainWnd); }
			  else if (BalloonActive == 1) { _beginthread(NeutralBalloonThread, 0, MainWnd); }
		  }
		  break;
	  case ID_DISPLAY_ORIGINAL:
		  Original = (Original + 1) % 2;
		  if (Original == 1)
		  {
			  Sobel = ThreadRunning = Auto = Step = RubberBand = 0;
			  memset(rbx, 0, sizeof rbx); memset(rby, 0, sizeof rby); memset(status, 0, sizeof status);
			  memset(rbx_init, 0, sizeof rbx_init); memset(rby_init, 0, sizeof rby_init);
			  v = 0; idx = 0; idx_init = 0; idx_balloon = 0;
			  //memset(balloonx, 0, sizeof balloonx); memset(balloony, 0, sizeof balloony);
			  PaintImage(); 
		  }
		  break;
	  case ID_DISPLAY_SOBEL:
		  Sobel = (Sobel + 1) % 2;
		  if (Sobel == 1) { Original = 0; PaintSobel(); }
		  break;
	  case ID_REGIONGROWING_STEP:
		  Step = (Step + 1) % 2;
		  if (Step == 1) { Auto = Restore = 0; }
		  break;
	  case ID_REGIONGROWING_GROWTHPREDICATES:
		  DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc);
		  break;
	  case ID_FILE_LOAD:
		if (OriginalImage != NULL)
		  {
		  free(OriginalImage);
		  OriginalImage=NULL;
		  }
		memset(&(ofn),0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.lpstrFile=filename;
		filename[0]=0;
		ofn.nMaxFile=MAX_FILENAME_CHARS;
		ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrFilter = "PNM files\0*.pnm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL)
		  {
		  MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt,"%s %d %d %d ",header,&COLS,&ROWS,&BYTES);
		if (strcmp(header,"P6") != 0  ||  BYTES != 255)
		  {
		  MessageBox(NULL,"Not a PNM (P6 greyscale) image",filename,MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		OriginalImage=(unsigned char *)calloc(ROWS*COLS*3, sizeof(unsigned char));
		//header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage,sizeof(unsigned char),ROWS*COLS*3,fpt);
		fclose(fpt);
		SetWindowText(hWnd,filename);
		GreyImage = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
		CalculateGreyScale();
		SobelImage = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
		CalculateSobelImage();
		PaintImage();
		memset(rbx, 0, sizeof rbx); memset(rby, 0, sizeof rby); memset(status, 0, sizeof status);
		memset(rbx_init, 0, sizeof rbx_init); memset(rby_init, 0, sizeof rby_init);
		v = 0; idx = 0; idx_init = 0;
		break;

      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;
      }
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	if (Original == 1) { PaintImage(); }
	else if (Sobel == 1) { PaintSobel(); }
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_PAINT:
	if (Original == 1) { PaintImage(); }
	else if (Sobel == 1) { PaintSobel(); }
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_LBUTTONDOWN:
	  if (Auto == 1)
	  {
		  ThreadCol = LOWORD(lParam);
		  ThreadRow = HIWORD(lParam);
		  ThreadRunning = 1;
		  _beginthread(RegionGrowThread, 0, MainWnd);
	  }
	  if (Step == 1)
	  {
		  ThreadCol = LOWORD(lParam);
		  ThreadRow = HIWORD(lParam);
		  ThreadRunning = 1;
		  _beginthread(RegionGrowThread, 0, MainWnd);
	  }
	  if (shiftFlag == 1)
	  {
		  selected_x = LOWORD(lParam);
		  selected_y = HIWORD(lParam);
		  sprintf(text, "Selected: %d %d", selected_x, selected_y);
		  hDC = GetDC(MainWnd);
		  TextOut(hDC, 200, 0, text, strlen(text));
		  SetPixel(hDC, selected_x, selected_y, RGB(255, 127, 0));
		  ReleaseDC(MainWnd, hDC);
	  }
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_RBUTTONDOWN:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		balloonx = (int*)calloc(ROWS*COLS, sizeof(int));
		balloony = (int*)calloc(ROWS * COLS, sizeof(int));
		if (xPos >= 0 && xPos < COLS && yPos >= 0 && yPos < ROWS)
		{
			hDC = GetDC(MainWnd);
			int m = 0;
			for (double i = 0; i < 2*M_PI; i+=0.01)
			{
				if (m % 3 == 0)
				{	
					balloonx[m/3] = (int)(xPos + 10 * cos(i));
					balloony[m/3] = (int)(yPos + 10 * sin(i));
				}
				m++;
			}
			idx_balloon = m / 3;
			for (int k = 0; k < idx_balloon; k++)
			{
				for (int r = balloony[k] - 2; r <= balloony[k] + 2; r++)
				{
					for (int c = balloonx[k] - 2; c <= balloonx[k] + 2; c++)
					{
						SetPixel(hDC, c, r, RGB(0, 255, 0));
					}
				}
				
			}
			ReleaseDC(MainWnd, hDC);
		}
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;
  case WM_MOUSEMOVE:
	  if (wParam & WM_LBUTTONDOWN)
	  {
		  if (shiftFlag == 0)
		  {
			  xPos = LOWORD(lParam);
			  yPos = HIWORD(lParam);
			  if (xPos >= 0 && xPos < COLS && yPos >= 0 && yPos < ROWS)
			  {
				  hDC = GetDC(MainWnd);
				  rbx_init[idx_init] = xPos;
				  rby_init[idx_init] = yPos;
				  idx_init++;

				  for (int d = 0; d < idx_init; d++)
				  {
					  for (int r = rby_init[d] - 2; r <= rby_init[d] + 2; r++)
					  {
						  for (int c = rbx_init[d] - 2; c <= rbx_init[d] + 2; c++)
						  {
							  SetPixel(hDC, c, r, RGB(255, 0, 0));
						  }
					  }
				  }
				  ReleaseDC(MainWnd, hDC);
			  }
		  }
		 
	  }
      return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	  break;
 
  case WM_KEYDOWN:
	if (wParam == 's'  ||  wParam == 'S')
	  PostMessage(MainWnd,WM_COMMAND,ID_SHOWPIXELCOORDS,0);	  /* send message to self */
	if (wParam == 'j' || wParam == 'J') { stepFlag = 1; }
	if (wParam & VK_SHIFT) { shiftFlag = 1; }
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_LBUTTONUP:
	  if (shiftFlag == 1)
	  {
		  shiftFlag = 0;
		  shifted_x = LOWORD(lParam);
		  shifted_y = HIWORD(lParam);
		  if (RubberBandActive == 1)
		  {
			  for (int s = 0; s < idx; s++)
			  {
				  if ((rbx[s] + 5) > selected_x && (rbx[s] - 5) <= selected_x && (rby[s] + 5) > selected_y && (rby[s] - 5) <= selected_y)
				  {
					  rbx[s] = shifted_x; rby[s] = shifted_y;
					  status[s] = 1;
				  }
			  }
		  }
		  else if (BalloonActive == 1)
		  {
			  for (int s = 0; s < idx_balloon; s++)
			  {
				  if ((balloonx[s] + 5) > selected_x && (balloonx[s] - 5) <= selected_x && (balloony[s] + 5) > selected_y && (balloony[s] - 5) <= selected_y)
				  {
					  balloonx[s] = shifted_x; balloony[s] = shifted_y;
					  status[s] = 1;
				  }
			  }
		  }
		  PaintImage();
		  hDC = GetDC(MainWnd);
		  if (RubberBandActive == 1)
		  {
			  for (int d = 0; d < idx; d++)
			  {
				  for (int r = rby[d] - 2; r <= rby[d] + 2; r++)
				  {
					  for (int c = rbx[d] - 2; c <= rbx[d] + 2; c++)
					  {
						  if (status[d] == 0) { SetPixel(hDC, c, r, RGB(255, 0, 0)); }
						  else if (status[d] == 1) { SetPixel(hDC, c, r, RGB(255, 127, 0)); }
					  }
				  }
			  }
		  }
		  else if (BalloonActive == 1)
		  {
			  for (int d = 0; d < idx_balloon; d++)
			  {
				  for (int r = balloony[d] - 2; r <= balloony[d] + 2; r++)
				  {
					  for (int c = balloonx[d] - 2; c <= balloonx[d] + 2; c++)
					  {
						  if (status[d] == 0) { SetPixel(hDC, c, r, RGB(0, 255, 0)); }
						  else if (status[d] == 1) { SetPixel(hDC, c, r, RGB(255, 127, 0)); }
					  }
				  }
			  }
		  }
	  }
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;
  case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
	hDC=GetDC(MainWnd);
	SetPixel(hDC,TimerCol,TimerRow,RGB(0,0,255));	/* color the animation pixel blue */
	ReleaseDC(MainWnd,hDC);
	TimerRow++;
	TimerCol+=2;
	break;
  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	if (Original == 1) { PaintImage(); }
	else if (Sobel == 1) { PaintSobel(); } /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	if (Original == 1) { PaintImage(); }
	else if (Sobel == 1) { PaintSobel(); }
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }

hMenu=GetMenu(MainWnd);

if (Auto == 1) { CheckMenuItem(hMenu, ID_REGIONGROWING_AUTO, MF_CHECKED); /* you can also call EnableMenuItem() to grey(disable) an option */
}
else { CheckMenuItem(hMenu, ID_REGIONGROWING_AUTO, MF_UNCHECKED); }

if (Step == 1) { CheckMenuItem(hMenu, ID_REGIONGROWING_STEP, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_REGIONGROWING_STEP, MF_UNCHECKED); }

if (Yellow == 1) { CheckMenuItem(hMenu, ID_COLOR_YELLOW, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_COLOR_YELLOW, MF_UNCHECKED); }

if (Orange == 1) { CheckMenuItem(hMenu, ID_COLOR_ORANGE, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_COLOR_ORANGE, MF_UNCHECKED); }

if (Red == 1) { CheckMenuItem(hMenu, ID_COLOR_RED, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_COLOR_RED, MF_UNCHECKED); }

if (Blue == 1) { CheckMenuItem(hMenu, ID_COLOR_BLUE, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_COLOR_BLUE, MF_UNCHECKED); }

if (Green == 1) { CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_COLOR_GREEN, MF_UNCHECKED); }

if (RubberBand == 1) { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_RUBBERBAND, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_RUBBERBAND, MF_UNCHECKED); }

if (Balloon == 1) { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_BALLOON, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_BALLOON, MF_UNCHECKED); }

if (Neutral == 1) { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_NEUTRAL, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_ACTIVECONTOURS_NEUTRAL, MF_UNCHECKED); }

if (Original == 1) { CheckMenuItem(hMenu, ID_DISPLAY_ORIGINAL, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_DISPLAY_ORIGINAL, MF_UNCHECKED); }

if (Sobel == 1) { CheckMenuItem(hMenu, ID_DISPLAY_SOBEL, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_DISPLAY_SOBEL, MF_UNCHECKED); }

DrawMenuBar(hWnd);

return(0L);

}

BOOL fRelative;
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	BOOL *Val1, *Val2;
	switch (Message)
	{
	//case WM_INITDIALOG:
		//return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			predicate1 = GetDlgItemInt(hDlg, IDC_EDIT1, &Val1, fRelative);
			if (!Val1)
			{
				MessageBox(hDlg, "Couldn't retrieve Predicate 1", NULL, MB_OK);
				SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, -1l);
			}
			predicate2 = GetDlgItemInt(hDlg, IDC_EDIT2, &Val2, fRelative);
			if (!Val2)
			{
				MessageBox(hDlg, "Couldn't retrieve Predicate 2", NULL, MB_OK);
				SendDlgItemMessage(hDlg, IDC_EDIT2, EM_SETSEL, 0, -1l);
			}
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, WM_CLOSE);
	default:
		return FALSE;
	}
	return FALSE;
}

void CalculateSobelImage()
{
	int		i, j, b, r, c, r2, c2;
	double* Gx_sobel, * Gy_sobel, * sobel, val, max, min;
	int Gx[9] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
	int Gy[9] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
	Gx_sobel = (double*)calloc(ROWS * COLS, sizeof(double));
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			val = 0.0;
			if (r < 1 || c < 1 || r >= (ROWS - 1) || c >= (COLS - 1))
			{
				Gx_sobel[r * COLS + c] = 0.0;
			}
			else
			{
				for (r2 = -1; r2 <= 1; r2++)
				{
					for (c2 = -1; c2 <= 1; c2++)
					{
						val = val + (GreyImage[(r + r2) * COLS + (c + c2)] * Gx[(r2 + 1) * 3 + (c2 + 1)]);
					}
				}
				Gx_sobel[r * COLS + c] = val;
			}
		}
	}

	Gy_sobel = (double*)calloc(ROWS * COLS, sizeof(double));

	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			val = 0.0;
			if (r < 1 || c < 1 || r >= (ROWS - 1) || c >= (COLS - 1))
			{
				Gy_sobel[r * COLS + c] = 0.0;
			}
			else
			{
				for (r2 = -1; r2 <= 1; r2++)
				{
					for (c2 = -1; c2 <= 1; c2++)
					{
						val = val + (GreyImage[(r + r2) * COLS + (c + c2)] * Gy[(r2 + 1) * 3 + (c2 + 1)]);
					}
				}
				Gy_sobel[r * COLS + c] = val;
			}
		}
	}

	sobel = (double*)calloc(ROWS * COLS, sizeof(double));
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			sobel[r * COLS + c] = sqrt(pow(Gx_sobel[r * COLS + c], 2) + pow(Gy_sobel[r * COLS + c], 2));
		}
	}

	max = 0; min = 99999999;

	for (int j = 0; j < ROWS * COLS; j++)
	{
		if (sobel[j] > max) { max = sobel[j]; }
		if (sobel[j] < min) { min = sobel[j]; }
	}
	float factor = max - min; float feed = 0;
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			if (r < 1 || c < 1 || r >= (ROWS - 1) || c >= (COLS - 1)) { SobelImage[r * COLS + c] = 0; }
			else
			{
				feed = (255 * (sobel[r * COLS + c] - min)) / (factor);
				int f = round(feed);
				SobelImage[r * COLS + c] = f;
			}

		}
	}
}

void CalculateGreyScale()
{
	int r, c;
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			GreyImage[r*COLS+c] = (int)((OriginalImage[(r * COLS + c) * 3] + OriginalImage[(r * COLS + c) * 3 + 1] + OriginalImage[(r * COLS + c) * 3 + 2]) / 3);
		}
	}
}

void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
unsigned char		*DisplayImage;

if (OriginalImage == NULL)
  return;		/* no image to draw */

		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
DISPLAY_ROWS=ROWS;
DISPLAY_COLS=COLS;
if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS, sizeof(unsigned char));
for (r = 0; r < ROWS; r++)
	for (c = 0; c < COLS; c++)
		DisplayImage[r * DISPLAY_COLS + c] = GreyImage[r * COLS + c];

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=DISPLAY_COLS;
bm_info_header.biHeight=-DISPLAY_ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=8; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=256;
bm_info_header.biClrImportant=256;
bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
bm_info->bmiHeader=bm_info_header;
for (i=0; i<256; i++)
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 

SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
			  0, /* first scan line */
			  DISPLAY_ROWS, /* number of scan lines */
			  DisplayImage,bm_info,DIB_RGB_COLORS);
ReleaseDC(MainWnd,hDC);
EndPaint(MainWnd,&Painter);

free(DisplayImage);
free(bm_info);
}

void PaintSobel()

{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO* bm_info;
	int					i, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	unsigned char* SobelDisplay;

	if (GreyImage == NULL)
		return;		/* no image to draw */

			  /* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	SobelDisplay = (unsigned char*)calloc(DISPLAY_ROWS * DISPLAY_COLS, sizeof(unsigned char));
	for (r = 0; r < ROWS; r++)
		for (c = 0; c < COLS; c++)
			SobelDisplay[r * DISPLAY_COLS + c] = SobelImage[r * COLS + c];


	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i < 256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		SobelDisplay, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(SobelDisplay);
	free(bm_info);
}


void RegionGrowThread(HWND AnimationWindowHandle)
{
	HDC		hDC;
	char	text[300];
	int CenterRow, CenterCol, BoxWidth, r, c, RegionSize, *indices, r2, c2;
	CenterRow = ThreadRow;
	CenterCol = ThreadCol;
	unsigned char* labels;
	float avg, var;

	labels = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	indices = (int*)calloc(ROWS * COLS, sizeof(int));
	avg = var = 0.0;	/* compute average and variance in 7x7 window */
	for (r2 = -3; r2 <= 3; r2++)
		for (c2 = -3; c2 <= 3; c2++)
			avg += (double)(OriginalImage[(CenterRow + r2) * COLS + (CenterCol + c2)]);
	avg /= 49.0;
	for (r2 = -3; r2 <= 3; r2++)
		for (c2 = -3; c2 <= 3; c2++)
			var += SQR(avg - (double)OriginalImage[(CenterRow + r2) * COLS + (CenterCol + c2)]);
	var = sqrt(var) / 49.0;
	if (var < 5.0)	/* condition for seeding a new region is low var */
	{
		TotalRegions++;
		RegionGrowing(OriginalImage, labels, ROWS, COLS, CenterRow, CenterCol, 0, TotalRegions,
			indices, &RegionSize);
	}	
}

void RegionGrowing(unsigned char* image,	/* image data */
	unsigned char* labels,	/* segmentation labels */
	int ROWS, int COLS,	/* size of image */
	int r, int c,		/* pixel to paint from */
	int paint_over_label,	/* image label to paint over */
	int new_label,		/* image label for painting */
	int* indices,		/* output:  indices of pixels painted */
	int* count)		/* output:  count of pixels painted */
{
	int	r2, c2;
	int	queue[MAX_QUEUE], qh, qt, distance;
	int	average, total;	/* average and total intensity in growing region */
	HDC hDC;
	*count = 0;
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = new_label;
	average = total = (int)image[r * COLS + c];
	if (indices != NULL)
		indices[0] = r * COLS + c;
	queue[0] = r * COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	while (qt != qh && ThreadRunning == 1)
	{
		if (stepFlag == 1)
		{
			if ((*count) % 50 == 0)	/* recalculate average after each 50 pixels join */
			{
				average = total / (*count);
			}
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					if (r2 == 0 && c2 == 0)
						continue;
					if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
						(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
						continue;
					if (labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] != paint_over_label)
						continue;
					/* test criteria to join region */
					if (abs((int)(image[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2])
						- average) > predicate1)
						continue;
					distance = sqrt(pow((r - (queue[qt] / COLS + r2)),2) + pow((c - (queue[qt] % COLS + c2)), 2));
					if (distance > predicate2)
						continue;
					labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] = new_label;
					hDC = GetDC(MainWnd);
					if (Red == 0 && Yellow == 0 && Orange == 0 && Green == 0 && Blue == 0)
					{
						SetPixel(hDC, queue[qt] % COLS + c2, (queue[qt] / COLS + r2), RGB(255, 0, 0));
					}
					else
					{
						SetPixel(hDC, queue[qt] % COLS + c2, (queue[qt] / COLS + r2), color2);
					}
					ReleaseDC(MainWnd, hDC);
					if (indices != NULL)
						indices[*count] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					total += image[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2];
					(*count)++;
					queue[qh] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					qh = (qh + 1) % MAX_QUEUE;
					if (qh == qt) { exit(0); }
				}
			qt = (qt + 1) % MAX_QUEUE;
			if (Auto == 1) { Sleep(0.1); }
			else if (Step == 1) { stepFlag = 0; }
		}
		}	
}

void RubberBandThread()
{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;

	int n = 0;
	int r, c, next_x, next_y;
	#define win 5
	double ie1[win * win], ie2[win * win], ee[win * win], ie1_norm[win * win], ie2_norm[win * win], ee_norm[win * win], energy_fin[win * win];
	double avg;
	while (n < 80)
	{
		for (int i = 0; i < idx; i++)
		{
			int mm = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx) { ie1[mm] = pow(((r + rby[i]) - rby[0]), 2) + pow(((c + rbx[i]) - rbx[0]), 2); }
					else { ie1[mm] = pow(((r + rby[i]) - rby[i + 1]), 2) + pow(((c + rbx[i]) - rbx[i + 1]), 2); }
					mm++;
				}
			}

			int o = 0; double sum = 0;
			for (int z = 0; z < idx; z++)
			{
				if (z + 1 >= idx) { next_x = rbx[0]; next_y = rby[0]; }
				else { next_x = rbx[z + 1]; next_y = rby[z + 1]; }
				sum += sqrt(pow((rbx[z] - next_x), 2) + pow((rby[z] - next_y), 2));
			}
			avg = sum / idx;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx) { ie2[o] = pow(avg - sqrt(pow(((r + rby[i]) - rby[0]), 2) + pow(((c + rbx[i]) - rbx[0]), 2)), 2); }
					else { ie2[o] = pow(avg - sqrt(pow(((r + rby[i]) - rby[i + 1]), 2) + pow(((c + rbx[i]) - rbx[i + 1]), 2)), 2); }
					o++;
				}
			}

			int p = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ee[p] = pow(SobelImage[(r + rby[i]) * COLS + (c + rbx[i])], 2);
					p++;
				}
			}

			int max_ie1 = 0; int min_ie1 = 99999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie1[j] > max_ie1) { max_ie1 = ie1[j]; }
				if (ie1[j] < min_ie1) { min_ie1 = ie1[j]; }
			}
			float factor_ie1 = max_ie1 - min_ie1;
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie1_norm[r * win + c] = (ie1[r * win + c] - min_ie1) / (factor_ie1);
				}
			}
			float max_ie2 = 0.0; float min_ie2 = 9999999.999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie2[j] > max_ie2) { max_ie2 = ie2[j]; }
				if (ie2[j] < min_ie2) { min_ie2 = ie2[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie2_norm[r * win + c] = (ie2[r * win + c] - min_ie2) / (max_ie2 - min_ie2);
				}
			}
			float max_ee = 0.0; float min_ee = 999999999.9999;
			for (int j = 0; j < win * win; j++)
			{
				if (ee[j] > max_ee) { max_ee = ee[j]; }
				if (ee[j] < min_ee) { min_ee = ee[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ee_norm[r * win + c] = (ee[r * win + c] - min_ee) / (max_ee - min_ee);
				}
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					energy_fin[r * win + c] = (16*ie1_norm[r * win + c]) + (6.25*ie2_norm[r * win + c]) - (10 * ee_norm[r * win + c]);
				}
			}
			float min_val = 9999999.99999; int min_ind = 0;
			for (int j = 0; j < win * win; j++)
			{
				if (energy_fin[j] < min_val) { min_val = energy_fin[j]; min_ind = j; }
			}
			rbx[i] = rbx[i] + (min_ind % win) - (win / 2);
			rby[i] = rby[i] + (min_ind / win) - (win / 2);
		}
		n++;
		hDC = GetDC(MainWnd);
		PaintImage();
		for (int d = 0; d < idx; d++)
		{
			for (int r = rby[d] - 2; r <= rby[d] + 2; r++)
			{
				for (int c = rbx[d] - 2; c <= rbx[d] + 2; c++)
				{
					SetPixel(hDC, c, r, RGB(255, 0, 0));
				}
			}
		}
		ReleaseDC(MainWnd, hDC);
	}
}

void NeutralRBThread()
{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO* bm_info;

	int n = 0;
	int r, c, next_x, next_y;
#define win 5
	double ie1[win * win], ie2[win * win], ee[win * win], ie1_norm[win * win], ie2_norm[win * win], ee_norm[win * win], energy_fin[win * win];
	double avg;
	while (n < 80)
	{
		for (int i = 0; i < idx; i++)
		{
			int mm = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx) { ie1[mm] = pow(((r + rby[i]) - rby[0]), 2) + pow(((c + rbx[i]) - rbx[0]), 2); }
					else { ie1[mm] = pow(((r + rby[i]) - rby[i + 1]), 2) + pow(((c + rbx[i]) - rbx[i + 1]), 2); }
					mm++;
				}
			}

			int o = 0; double sum = 0;
			for (int z = 0; z < idx; z++)
			{
				if (z + 1 >= idx) { next_x = rbx[0]; next_y = rby[0]; }
				else { next_x = rbx[z + 1]; next_y = rby[z + 1]; }
				sum += sqrt(pow((rbx[z] - next_x), 2) + pow((rby[z] - next_y), 2));
			}
			avg = sum / idx;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx) { ie2[o] = pow(avg - sqrt(pow(((r + rby[i]) - rby[0]), 2) + pow(((c + rbx[i]) - rbx[0]), 2)), 2); }
					else { ie2[o] = pow(avg - sqrt(pow(((r + rby[i]) - rby[i + 1]), 2) + pow(((c + rbx[i]) - rbx[i + 1]), 2)), 2); }
					o++;
				}
			}

			int p = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ee[p] = pow(SobelImage[(r + rby[i]) * COLS + (c + rbx[i])], 2);
					p++;
				}
			}

			int max_ie1 = 0; int min_ie1 = 99999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie1[j] > max_ie1) { max_ie1 = ie1[j]; }
				if (ie1[j] < min_ie1) { min_ie1 = ie1[j]; }
			}
			float factor_ie1 = max_ie1 - min_ie1;
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie1_norm[r * win + c] = (ie1[r * win + c] - min_ie1) / (factor_ie1);
				}
			}
			float max_ie2 = 0.0; float min_ie2 = 9999999.999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie2[j] > max_ie2) { max_ie2 = ie2[j]; }
				if (ie2[j] < min_ie2) { min_ie2 = ie2[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie2_norm[r * win + c] = (ie2[r * win + c] - min_ie2) / (max_ie2 - min_ie2);
				}
			}
			float max_ee = 0.0; float min_ee = 999999999.9999;
			for (int j = 0; j < win * win; j++)
			{
				if (ee[j] > max_ee) { max_ee = ee[j]; }
				if (ee[j] < min_ee) { min_ee = ee[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ee_norm[r * win + c] = (ee[r * win + c] - min_ee) / (max_ee - min_ee);
				}
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					energy_fin[r * win + c] = (1 * ie1_norm[r * win + c]) + (6.25 * ie2_norm[r * win + c]) - (10 * ee_norm[r * win + c]);
					//energy_fin[r * win + c] = (50 * ie1_norm[r * win + c]) - (5 * ee_norm[r * win + c]);
				}
			}
			float min_val = 9999999.99999; int min_ind = 0;
			for (int j = 0; j < win * win; j++)
			{
				if (energy_fin[j] < min_val) { min_val = energy_fin[j]; min_ind = j; }
			}
			if (status[i] == 0)
			{
				rbx[i] = rbx[i] + (min_ind % win) - (win / 2);
				rby[i] = rby[i] + (min_ind / win) - (win / 2);
			}
			else if (status[i] == 1)
			{
				rbx[i] = rbx[i];
				rby[i] = rby[i];
			}
		}
		n++;
		hDC = GetDC(MainWnd);
		PaintImage();
		for (int d = 0; d < idx; d++)
		{
			for (int r = rby[d] - 2; r <= rby[d] + 2; r++)
			{
				for (int c = rbx[d] - 2; c <= rbx[d] + 2; c++)
				{
					if (status[d] == 0) { SetPixel(hDC, c, r, RGB(255, 0, 0)); }
					else if (status[d] == 1) { SetPixel(hDC, c, r, RGB(255, 127, 0)); }
				}
			}
		}
		ReleaseDC(MainWnd, hDC);
	}
}

void BalloonThread()
{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			* bm_info;

	int n = 0;
	int r, c, next_x, next_y;
    #define win 7
	double ie1[win * win], ie2[win * win], ee[win * win], ie1_norm[win * win], ie2_norm[win * win], ee_norm[win * win], energy_fin[win * win];
	double avg;
	while (n < 60)
	{
		for (int i = 0; i < idx_balloon; i++)
		{
			int mm = 0; int cx = 0; int cy = 0;
			for (int z = 0; z < idx_balloon; z++)
			{
				cx += balloonx[z];
				cy += balloony[z];
			}
			cx /= idx_balloon;
			cy /= idx_balloon;

			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ie1[mm] = pow(((r + balloony[i]) - cy), 2) + pow(((c + balloonx[i]) - cx), 2);
					mm++;
				}
			}

			int o = 0; double sum = 0;
			for (int z = 0; z < idx_balloon; z++)
			{
				if (z + 1 >= idx_balloon) { next_x = balloonx[0]; next_y = balloony[0]; }
				else { next_x = balloonx[z + 1]; next_y = balloony[z + 1]; }
				sum += sqrt(pow((balloonx[z] - next_x), 2) + pow((balloony[z] - next_y), 2));
			}
			avg = sum / idx_balloon;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx_balloon) { ie2[o] = pow(avg - sqrt(pow(((r + balloony[i]) - balloony[0]), 2) + pow(((c + balloonx[i]) - balloonx[0]), 2)), 2); }
					else { ie2[o] = pow(avg - sqrt(pow(((r + balloony[i]) - balloony[i + 1]), 2) + pow(((c + balloonx[i]) - balloonx[i + 1]), 2)), 2); }
					o++;
				}
			}

			int p = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ee[p] = pow(SobelImage[(r + balloony[i]) * COLS + (c + balloonx[i])], 2);
					p++;
				}
			}

			int max_ie1 = 0; int min_ie1 = 99999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie1[j] > max_ie1) { max_ie1 = ie1[j]; }
				if (ie1[j] < min_ie1) { min_ie1 = ie1[j]; }
			}
			float factor_ie1 = max_ie1 - min_ie1;
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie1_norm[r * win + c] = (ie1[r * win + c] - min_ie1) / (factor_ie1);
				}
			}
			float max_ie2 = 0.0; float min_ie2 = 9999999.999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie2[j] > max_ie2) { max_ie2 = ie2[j]; }
				if (ie2[j] < min_ie2) { min_ie2 = ie2[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie2_norm[r * win + c] = (ie2[r * win + c] - min_ie2) / (max_ie2 - min_ie2);
				}
			}
			float max_ee = 0.0; float min_ee = 999999999.9999;
			for (int j = 0; j < win * win; j++)
			{
				if (ee[j] > max_ee) { max_ee = ee[j]; }
				if (ee[j] < min_ee) { min_ee = ee[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ee_norm[r * win + c] = (ee[r * win + c] - min_ee) / (max_ee - min_ee);
				}
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					if (n <= 10)
					{
						energy_fin[r * win + c] = -(10 * ie1_norm[r * win + c]) + (3 * ie2_norm[r * win + c]) - (3 * ee_norm[r * win + c]);
					}
					else if (n > 10 && n <= 30)
					{
						energy_fin[r * win + c] = -(25 * ie1_norm[r * win + c]) + (30 * ie2_norm[r * win + c]) - (10 * ee_norm[r * win + c]);
					}
					else if (n > 30 && n < 50)
					{
						energy_fin[r * win + c] = -(16 * ie1_norm[r * win + c]) + (9 * ie2_norm[r * win + c]) - (100 * ee_norm[r * win + c]);
					}
					else
					{
						energy_fin[r * win + c] = -(5 * ie1_norm[r * win + c]) + (0 * ie2_norm[r * win + c]) - (80 * ee_norm[r * win + c]);
					}
				}
			}
			float min_val = 9999999.99999; int min_ind = 0;
			for (int j = 0; j < win * win; j++)
			{
				if (energy_fin[j] < min_val) { min_val = energy_fin[j]; min_ind = j; }
			}
			balloonx[i] = balloonx[i] + (min_ind % win) - (win / 2);
			balloony[i] = balloony[i] + (min_ind / win) - (win / 2);
		}
		n++;
		hDC = GetDC(MainWnd);
		PaintImage();
		for (int d = 0; d < idx_balloon; d++)
		{
			for (int r = balloony[d] - 2; r <= balloony[d] + 2; r++)
			{
				for (int c = balloonx[d] - 2; c <= balloonx[d] + 2; c++)
				{
					SetPixel(hDC, c, r, RGB(0, 255, 0));
				}
			}
		}
		ReleaseDC(MainWnd, hDC);
	}
}

void NeutralBalloonThread()
{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			* bm_info;

	int n = 0;
	int r, c, next_x, next_y;
    #define win 7
	double ie1[win * win], ie2[win * win], ee[win * win], ie1_norm[win * win], ie2_norm[win * win], ee_norm[win * win], energy_fin[win * win];
	double avg;
	while (n < 60)
	{
		for (int i = 0; i < idx_balloon; i++)
		{
			int mm = 0; int cx = 0; int cy = 0;
			for (int z = 0; z < idx_balloon; z++)
			{
				cx += balloonx[z];
				cy += balloony[z];
			}
			cx /= idx_balloon;
			cy /= idx_balloon;

			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ie1[mm] = pow(((r + balloony[i]) - cy), 2) + pow(((c + balloonx[i]) - cx), 2);
					mm++;
				}
			}

			int o = 0; double sum = 0;
			for (int z = 0; z < idx_balloon; z++)
			{
				if (z + 1 >= idx_balloon) { next_x = balloonx[0]; next_y = balloony[0]; }
				else { next_x = balloonx[z + 1]; next_y = balloony[z + 1]; }
				sum += sqrt(pow((balloonx[z] - next_x), 2) + pow((balloony[z] - next_y), 2));
			}
			avg = sum / idx_balloon;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					if (i + 1 >= idx_balloon) { ie2[o] = pow(avg - sqrt(pow(((r + balloony[i]) - balloony[0]), 2) + pow(((c + balloonx[i]) - balloonx[0]), 2)), 2); }
					else { ie2[o] = pow(avg - sqrt(pow(((r + balloony[i]) - balloony[i + 1]), 2) + pow(((c + balloonx[i]) - balloonx[i + 1]), 2)), 2); }
					o++;
				}
			}

			int p = 0;
			for (r = -win / 2; r <= win / 2; r++)
			{
				for (c = -win / 2; c <= win / 2; c++)
				{
					ee[p] = pow(SobelImage[(r + balloony[i]) * COLS + (c + balloonx[i])], 2);
					p++;
				}
			}

			int max_ie1 = 0; int min_ie1 = 99999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie1[j] > max_ie1) { max_ie1 = ie1[j]; }
				if (ie1[j] < min_ie1) { min_ie1 = ie1[j]; }
			}
			float factor_ie1 = max_ie1 - min_ie1;
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie1_norm[r * win + c] = (ie1[r * win + c] - min_ie1) / (factor_ie1);
				}
			}
			float max_ie2 = 0.0; float min_ie2 = 9999999.999999;
			for (int j = 0; j < win * win; j++)
			{
				if (ie2[j] > max_ie2) { max_ie2 = ie2[j]; }
				if (ie2[j] < min_ie2) { min_ie2 = ie2[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ie2_norm[r * win + c] = (ie2[r * win + c] - min_ie2) / (max_ie2 - min_ie2);
				}
			}
			float max_ee = 0.0; float min_ee = 999999999.9999;
			for (int j = 0; j < win * win; j++)
			{
				if (ee[j] > max_ee) { max_ee = ee[j]; }
				if (ee[j] < min_ee) { min_ee = ee[j]; }
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					ee_norm[r * win + c] = (ee[r * win + c] - min_ee) / (max_ee - min_ee);
				}
			}
			for (r = 0; r < win; r++)
			{
				for (c = 0; c < win; c++)
				{
					if (n <= 10)
					{
						energy_fin[r * win + c] = -(2 * ie1_norm[r * win + c]) + (40 * ie2_norm[r * win + c]) - (24 * ee_norm[r * win + c]);
					}
					else if (n > 10 && n <= 30)
					{
						energy_fin[r * win + c] = -(2 * ie1_norm[r * win + c]) + (40 * ie2_norm[r * win + c]) - (24 * ee_norm[r * win + c]);
					}
					else if (n > 30 && n < 50)
					{
						energy_fin[r * win + c] = -(2 * ie1_norm[r * win + c]) + (20 * ie2_norm[r * win + c]) - (100 * ee_norm[r * win + c]);
					}
					else
					{
						energy_fin[r * win + c] = -(2 * ie1_norm[r * win + c]) + (0 * ie2_norm[r * win + c]) - (80 * ee_norm[r * win + c]);
					}
				}
			}
			float min_val = 9999999.99999; int min_ind = 0;
			for (int j = 0; j < win * win; j++)
			{
				if (energy_fin[j] < min_val) { min_val = energy_fin[j]; min_ind = j; }
			}
			if (status[i] == 0)
			{
				balloonx[i] = balloonx[i] + (min_ind % win) - (win / 2);
				balloony[i] = balloony[i] + (min_ind / win) - (win / 2);
			}
			else if (status[i] == 1)
			{
				balloonx[i] = balloonx[i];
				balloony[i] = balloony[i];
			}
		}
		n++;
		hDC = GetDC(MainWnd);
		PaintImage();
		for (int d = 0; d < idx_balloon; d++)
		{
			for (int r = balloony[d] - 2; r <= balloony[d] + 2; r++)
			{
				for (int c = balloonx[d] - 2; c <= balloonx[d] + 2; c++)
				{
					if (status[d] == 0) { SetPixel(hDC, c, r, RGB(0, 255, 0)); }
					else if (status[d] == 1) { SetPixel(hDC, c, r, RGB(255, 127, 0)); }
				}
			}
		}
		ReleaseDC(MainWnd, hDC);
	}
}