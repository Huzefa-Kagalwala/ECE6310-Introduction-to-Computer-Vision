
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

#define MAX_QUEUE  10000

#define SQR(x) ((x)*(x))

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd, ret;

HINSTANCE hInstance;

int Restore, stepFlag, predicate1, predicate2;
		// Display flags
int		RubberBand, Balloon, Auto, Step, Original, Sobel, Neutral;
		// Image data
unsigned char	*OriginalImage, *DisplayImage, *SobelImage, *GreyImage, *SobelDisplay;
int				ROWS,COLS;
		// Storage for contour points
int rbx[1000000], rby[1000000], *balloonx, *balloony;
int rbx_init[1000000]; rby_init[1000000], status[1000000];
		// Flags for storage
int v = 0; int idx = 0; int idx_init = 0; int idx_balloon;
int shiftFlag = 0;
int selected_x, selected_y, shifted_x, shifted_y;
int RubberBandActive, BalloonActive;

#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;
int		Yellow, Orange, Red, Blue, Green;
COLORREF color2, color;

		// Region Growing variables
int TotalRegions;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
void PaintImage();
void PaintSobel();
void CalculateSobelImage();
void CalculateGreyScale();
void AnimationThread(void *);		/* passes address of window */
void RegionGrowThread(void*);
void RegionGrowing(void *);
void RubberBandThread();
void BalloonThread();
void NeutralRBThread();
void NeutralBalloonThread();