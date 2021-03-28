
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

hWnd=CreateWindow("PLUS","Huzefa_Kagalwala_Lab4",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,700,650,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

ShowPixelCoords=0;
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
	  case ID_REGIONGROWING_RESTORE40011:
		  Restore = (Restore + 1) % 2;
		  if (Restore == 1) {
			  ThreadRunning = 0;
			  PaintImage();
			  Auto = Step = 0;
		  }
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
		ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL)
		  {
		  MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
		if (strcmp(header,"P5") != 0  ||  BYTES != 255)
		  {
		  MessageBox(NULL,"Not a PPM (P5 greyscale) image",filename,MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
		header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage,sizeof(unsigned char),ROWS*COLS,fpt);
		fclose(fpt);
		SetWindowText(hWnd,filename);
		PaintImage();
		break;

      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;
      }
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_PAINT:
	PaintImage();
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
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_RBUTTONDOWN:
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;
  case WM_MOUSEMOVE:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
 
  case WM_KEYDOWN:
	if (wParam == 's'  ||  wParam == 'S')
	  PostMessage(MainWnd,WM_COMMAND,ID_SHOWPIXELCOORDS,0);	  /* send message to self */
	if (wParam == 'j' || wParam == 'J')
	{
		stepFlag = 1;
	}
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
	hDC=GetDC(MainWnd);
	SetPixel(hDC,TimerCol,TimerRow,RGB(0,0,255));	/* color the animation pixel blue */
	ReleaseDC(MainWnd,hDC);
	TimerRow++;
	TimerCol+=2;
	break;
  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;
  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	PaintImage();
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
if (ShowPixelCoords == 1) { CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */ }
else  { CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_UNCHECKED); }

if (Auto == 1) { CheckMenuItem(hMenu, ID_REGIONGROWING_AUTO, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_REGIONGROWING_AUTO, MF_UNCHECKED); }

if (Step == 1) { CheckMenuItem(hMenu, ID_REGIONGROWING_STEP, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_REGIONGROWING_STEP, MF_UNCHECKED); }

if (Restore == 1) { CheckMenuItem(hMenu, ID_REGIONGROWING_RESTORE40011, MF_CHECKED); }
else { CheckMenuItem(hMenu, ID_REGIONGROWING_RESTORE40011, MF_UNCHECKED); }

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
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
	DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];

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


void RegionGrowThread(HWND AnimationWindowHandle)
{
	HDC		hDC;
	char	text[300];
	int CenterRow, CenterCol, BoxWidth, r, c, RegionSize, * indices, r2, c2;
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
			if (Auto == 1) { Sleep(1); }
			else if (Step == 1) { stepFlag = 0; }
		}
		}	
}