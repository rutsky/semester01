/* task03.c
 * Task 03.
 * Bob Rustky, SPbSTU, 1057/2.
 * 24.09.2006
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

/* Board size */
#define BOARD_X 8
#define BOARD_Y 8
/* Maximum absolute offset value */
#define MAX_OFFSET (BOARD_X > BOARD_Y ? BOARD_X : BOARD_Y)
/* Maximum number of turns queue */
#define MAX_PATH_QUEUE ((BOARD_X + 2 * MAX_OFFSET) * (BOARD_Y + 2 * MAX_OFFSET) + 1)

/* Axes width and height */
#define AXIS_WIDTH 20
#define AXIS_HEIGHT 20
/* Caption height */
#define CAPTION_HEIGHT 20
/* Figure name width */
#define FIGURE_WIDTH 100
/* Path length string width */
#define PATH_WIDTH 200

/* Global Variables */
HINSTANCE hInst;                       /* Current instance */
const char *szWindowClass = "MyClass"; /* Main class name */

/* Integer point structure definition */
typedef struct
{
  int x, y;
} Point;

typedef struct
{
  char *name; /* Figure name */
  int n;      /* Number of various turns */
  int *offX;  /* X offsets of turns */
  int *offY;  /* Y offsets of turns */
} ChessFigure;

/* Searching path function.
 * ARGUMENTS:
 *   - pointer to arrat with result path:
 *       Point *path;
 *   - maximum length of result path:
 *       int maxPath;
 *   - start point:
 *       Point *from;
 *   - end point:
 *       Point *to;
 *   - chess figure:
 *       ChessFigure *figure;
 * RETURNS:
 *   (int) length of founded path, negative if there is no path.
 */
int SearchTurns( Point *path, int maxPath,
                 Point *from, Point *to, ChessFigure *figure )
{
  Point board[BOARD_X + 2 * MAX_OFFSET][BOARD_Y + 2 * MAX_OFFSET];
  int queueStart = 0, queueEnd = 0,
    i, x, y,
    queuePointsX[MAX_PATH_QUEUE],
    queuePointsY[MAX_PATH_QUEUE],
    found;

  /* Clearing return path board */
  for (y = 0; y < BOARD_X + 2 * MAX_OFFSET; y++)
    for (x = 0; x < BOARD_Y + 2 * MAX_OFFSET; x++)
    {
      if (y >= MAX_OFFSET && y < BOARD_X + MAX_OFFSET &&
          x >= MAX_OFFSET && x < BOARD_Y + MAX_OFFSET)
      {
        /* 'empty' */
        board[y][x].x = 0;
        board[y][x].y = 0;
      }
      else
      {
        /* 'was' */
        board[y][x].x = -1;
        board[y][x].y = -1;
      }
    }

  /* Breadth-First search */
  found = 0;
  /* Starting search from 'to' point */
  queuePointsX[queueEnd]   = MAX_OFFSET + to->x;
  queuePointsY[queueEnd++] = MAX_OFFSET + to->y;
  board[MAX_OFFSET + to->x][MAX_OFFSET + to->y].x = -1; /* Marking as 'was' */
  /* While there is point to explore */
  while (queueStart != queueEnd && !found)
  {
    /* Getting point */
    x = queuePointsX[queueStart];
    y = queuePointsY[queueStart++];
    /* Checking neighbours */
    for (i = 0; i < figure->n; i++)
    {
      if (board[x + figure->offX[i]][y + figure->offY[i]].x == 0 &&
          board[x + figure->offX[i]][y + figure->offY[i]].y == 0)
      {
        /* Marking neighbour */
        /* Neighbour will reference on current point */
        board[x + figure->offX[i]][y + figure->offY[i]].x = -figure->offX[i];
        board[x + figure->offX[i]][y + figure->offY[i]].y = -figure->offY[i];
        /* Adding neghbour to queue */
        queuePointsX[queueEnd]   = x + figure->offX[i];
        queuePointsY[queueEnd++] = y + figure->offY[i];

        /* Checking if neighbour is finish cell */
        if (x + figure->offX[i] == MAX_OFFSET + from->x &&
            y + figure->offY[i] == MAX_OFFSET + from->y)
        {
          found = 1;
          break;
        }
      }
    }
  }

  /* Writing path */
  if (board[MAX_OFFSET + from->x][MAX_OFFSET + from->y].x != 0 ||
      board[MAX_OFFSET + from->x][MAX_OFFSET + from->y].y != 0)
  {
    x = from->x;
    y = from->y;
    for (i = 0; i < maxPath && (x != to->x || y != to->y); i++)
    {
      path[i].x = x + board[MAX_OFFSET + x][MAX_OFFSET + y].x;
      path[i].y = y + board[MAX_OFFSET + x][MAX_OFFSET + y].y;
      x = path[i].x;
      y = path[i].y;
    }

    return i;
  }
  else
    return -1;
} /* End of 'SearchTurns' function */

/* Saves instance handle and creates main window function */
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
  HWND hWnd;

  /* Store instance handle in our global variable */
  hInst = hInstance;

  hWnd = CreateWindow(szWindowClass, 
    "Task 03. Bob Rutsky, SPbSTU, 1057/2. 23.09.2006.", WS_OVERLAPPEDWINDOW,
    50, 20, 600, 640, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
} /* End of 'InitInstance' function */

/* Checking is point in rectangle function.
 * ARGUMENTS:
 *   - pointer to rectangle:
 *       RECT *rt;
 *   - point coordinates:
 *       int x, y;
 * RETURNS:
 *   (int) non-zero if point in rectangle, zero otherwise.
 */
int _isInRect( RECT *rt, int x, int y )
{
  return x >= rt->left && x < rt->right && y >= rt->top && y < rt->bottom;
} /* End of '_isInRect' function */

/* Calculating X coordinate of 'part' block of 'partsTotal' blocks
 * of rectangle.
 * ARGUMENTS:
 *   - rectangle:
 *       RECT *rt;
 *   - total parts in rectangle:
 *       int partsTotal;
 *   - needed part:
 *       double part;
 * RETURNS:
 *   (int) X coordinate of 'part' block of 'partsTotal' blocks
 * of rectangle.
 */
int _getXPartOfRect( RECT *rt, int partsTotal, double part )
{
  return 
    (int)(rt->left + (double)(rt->right - rt->left) / partsTotal * part);
} /* End of '_getXPartOfRect' function */

/* Calculating Y coordinate of 'part' block of 'partsTotal' blocks
 * of rectangle.
 * ARGUMENTS:
 *   - rectangle:
 *       RECT *rt;
 *   - total parts in rectangle:
 *       int partsTotal;
 *   - needed part:
 *       double part;
 * RETURNS:
 *   (int) Y coordinate of 'partI' block of 'partsTotal' blocks
 * of rectangle.
 */
int _getYPartOfRect( RECT *rt, int partsTotal, double part )
{
  return 
    (int)(rt->top + (double)(rt->bottom - rt->top) / partsTotal * part);
} /* End of '_getYPartOfRect' function */

/* Calculating on which part of rectangle falls 'x' coordinate function.
 * AREGUMENTS:
 *   - rectangle:
 *       RECT *rt;
 *   - total parts in rectangle:
 *       int partsTotal;
 *   - x coordinate of point:
 *       double x;
 * RETUNS:
 *   (int) on which part of rectangle falls 'x' coordinate.
 */
int _getXRectPart( RECT *rt, int partsTotal, double x )
{
  return (int)((x - rt->left) / ((rt->right - rt->left) / partsTotal));
} /* End of '_getXRectPart' function */

/* Calculating on which part of rectangle falls 'y' coordinate function.
 * AREGUMENTS:
 *   - rectangle:
 *       RECT *rt;
 *   - total parts in rectangle:
 *       int partsTotal;
 *   - y coordinate of point:
 *       double y;
 * RETUNS:
 *   (int) on which part of rectangle falls 'y' coordinate.
 */
int _getYRectPart( RECT *rt, int partsTotal, double y )
{
  return (int)((y - rt->top) / ((rt->bottom - rt->top) / partsTotal));
} /* End of '_getYRectPart' function */

/* Drawing chessboard function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to board rectangle:
 *       RECT *boardRect;
 *   - black square brush:
 *       HBRUSH hBlackBrush;
 *   - white square brush:
 *       HBRUSH hWhiteBrush;
 * RETURNS: None.
 */
void DrawChessboard( HDC hdc, RECT *boardRect, HBRUSH hBlackBrush, HBRUSH hWhiteBrush )
{
  int x, y;

  for (y = 0; y < BOARD_Y; y++)
    for (x = 0; x < BOARD_X; x++)
    {
      if ((x + y) % 2 == 0)
      {
        SelectObject(hdc, hWhiteBrush);
      }
      else
      {
        SelectObject(hdc, hBlackBrush);
      }

      Rectangle(hdc,
        _getXPartOfRect(boardRect, BOARD_X, x),
        _getYPartOfRect(boardRect, BOARD_Y, y),
        _getXPartOfRect(boardRect, BOARD_X, x + 1),
        _getYPartOfRect(boardRect, BOARD_Y, y + 1));
    }
} /* End of 'DrawChessboard' function */

/* Drawing figure function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to board rectangle:
 *       RECT *boardRect;
 *   - pointer to figure coordinates:
 *       Point *fig;
 *   - figure name:
 *       char *name;
 * RETURNS: None.
 */
void DrawFigureName( HDC hdc, RECT *boardRect, Point *fig, char *name )
{
  RECT rt;

  rt.left   = _getXPartOfRect(boardRect, BOARD_X, fig->x);
  rt.top    = _getYPartOfRect(boardRect, BOARD_Y, fig->y);
  rt.right  = _getXPartOfRect(boardRect, BOARD_X, fig->x + 1);
  rt.bottom = _getYPartOfRect(boardRect, BOARD_Y, fig->y + 1);

  DrawText(hdc, name, strlen(name), &rt,
    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
} /* End of 'DrawFigureName' function */

/* Drawing figure caption function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to board rectangle:
 *       RECT *figureRect;
 *   - figure name:
 *       char *name;
 * RETURNS: None.
 */
void DrawFigureCaption( HDC hdc, RECT *figureRect, char *name )
{
  DrawText(hdc, name, strlen(name), figureRect,
    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
} /* End of 'DrawFigureCaption' function */

/* Drawing path length caption function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to board rectangle:
 *       RECT *pathLenRect;
 *   - path length:
 *       int pathLen;
 * RETURNS: None.
 */
void DrawPathLenCaption( HDC hdc, RECT *pathLenRect, int pathLen )
{
  char buf[100];

  if (pathLen >= 0)
  {
    sprintf(buf, "Path length: %d.", pathLen);
  }
  else
  {
    sprintf(buf, "There is no path.");
  }

  DrawText(hdc, buf, strlen(buf), pathLenRect,
    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
} /* End of 'DrawPathLenCaption' function */

/* Drawing 'X' axis function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to axis rectangle:
 *       RECT *xAxisRect;
 * RETURNS: None.
 */
void DrawXAxis( HDC hdc, RECT *xAxisRect )
{
  RECT rt;
  int i;
  char buf[10];

  rt.top    = xAxisRect->top;
  rt.bottom = xAxisRect->bottom;
  for (i = 0; i < BOARD_X; i++)
  {
    rt.left  = _getXPartOfRect(xAxisRect, BOARD_X, i);
    rt.right = _getXPartOfRect(xAxisRect, BOARD_X, i + 1);

    sprintf(buf, "%c", 'a' + i);

    DrawText(hdc, buf, strlen(buf), &rt,
      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }
} /* End of 'DrawXAxis' function */

/* Drawing 'Y' axis function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to axis rectangle:
 *       RECT *yAxisRect;
 * RETURNS: None.
 */
void DrawYAxis( HDC hdc, RECT *yAxisRect )
{
  RECT rt;
  int i;
  char buf[10];

  rt.left  = yAxisRect->left;
  rt.right = yAxisRect->right;
  for (i = 0; i < BOARD_Y; i++)
  {
    rt.top    = _getYPartOfRect(yAxisRect, BOARD_Y, i);
    rt.bottom = _getYPartOfRect(yAxisRect, BOARD_Y, i + 1);

    sprintf(buf, "%c", '0' + BOARD_Y - i - 1);

    DrawText(hdc, buf, strlen(buf), &rt,
      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }
} /* End of 'DrawYAxis' function */

/* Drawing arrow from one cell to another function.
 * ARGUMENTS:
 *   - drawing device descriptor:
 *       HDC hdc;
 *   - pointer to board rectangle:
 *       RECT *boardRect;
 *   - pointer to start point:
 *       Point *start;
 *   - founded path:
 *       Point *path;
 *   - founded path length:
 *       int pathLen;
 *   - pen for lines:
 *       HPEN hPen;
 *   - brush for circles:
 *       HPEN hBrush;
 * RETURNS: None.
 */
void DrawPath( HDC hdc, RECT *boardRect, Point *start, Point *path,
               int pathLen,
               HPEN hPen, HBRUSH hBrush )
{
  int i;

  SelectObject(hdc, hPen);
  SelectObject(hdc, hBrush);

  Ellipse(hdc,
    _getXPartOfRect(boardRect, BOARD_X, start->x + 0.25),
    _getYPartOfRect(boardRect, BOARD_Y, start->y + 0.25),
    _getXPartOfRect(boardRect, BOARD_X, start->x + 0.75),
    _getYPartOfRect(boardRect, BOARD_Y, start->y + 0.75));

  MoveToEx(hdc,
    _getXPartOfRect(boardRect, BOARD_X, start->x + 0.5),
    _getYPartOfRect(boardRect, BOARD_Y, start->y + 0.5),
    NULL);

  for (i = 0; i < pathLen; i++)
  {
    Ellipse(hdc,
      _getXPartOfRect(boardRect, BOARD_X, path[i].x + 0.25),
      _getYPartOfRect(boardRect, BOARD_Y, path[i].y + 0.25),
      _getXPartOfRect(boardRect, BOARD_X, path[i].x + 0.75),
      _getYPartOfRect(boardRect, BOARD_Y, path[i].y + 0.75));

    LineTo(hdc,
      _getXPartOfRect(boardRect, BOARD_X, path[i].x + 0.5),
      _getYPartOfRect(boardRect, BOARD_Y, path[i].y + 0.5));
  }
} /* End of 'DrawArrow' function */

/* Creating chess figures function.
 * ARGUMENTS:
 *   - pointer to array of characters:
 *       ChessFigure *chessFigures[];
 * RETURNS:
 *   (int) number of created figures.
 */
int CreateChessFigures( ChessFigure *chessFigures[] )
{
  int i, n;
  char *figures[] = { "Knight", "Bishop", "Rook", "Queen", "King" };
  
  *chessFigures = malloc(sizeof(ChessFigure) *
    sizeof(figures) / sizeof(figures[0]));

  n = 0;

  /* Knight */
  (*chessFigures)[n].name = figures[n];
  (*chessFigures)[n].n = 8;
  (*chessFigures)[n].offX = malloc(sizeof(int) * (*chessFigures)[n].n);
  (*chessFigures)[n].offY = malloc(sizeof(int) * (*chessFigures)[n].n);

  (*chessFigures)[n].offX[0] =  2;
  (*chessFigures)[n].offX[1] =  2;
  (*chessFigures)[n].offX[2] =  1;
  (*chessFigures)[n].offX[3] = -1;
  (*chessFigures)[n].offX[4] = -2;
  (*chessFigures)[n].offX[5] = -2;
  (*chessFigures)[n].offX[6] = -1;
  (*chessFigures)[n].offX[7] =  1;

  (*chessFigures)[n].offY[0] =  1;
  (*chessFigures)[n].offY[1] = -1;
  (*chessFigures)[n].offY[2] = -2;
  (*chessFigures)[n].offY[3] = -2;
  (*chessFigures)[n].offY[4] = -1;
  (*chessFigures)[n].offY[5] =  1;
  (*chessFigures)[n].offY[6] =  2;
  (*chessFigures)[n].offY[7] =  2;

  n++;

  /* Bishop */
  (*chessFigures)[n].name = "Bishop";
  (*chessFigures)[n].n = MAX_OFFSET * 4;
  (*chessFigures)[n].offX = malloc(sizeof(int) * (*chessFigures)[n].n);
  (*chessFigures)[n].offY = malloc(sizeof(int) * (*chessFigures)[n].n);

  for (i = 0; i < MAX_OFFSET * 4; i += 4)
  {
    (*chessFigures)[n].offX[i]     =  i / 4 + 1;
    (*chessFigures)[n].offY[i]     =  i / 4 + 1;
    (*chessFigures)[n].offX[i + 1] = -i / 4 - 1;
    (*chessFigures)[n].offY[i + 1] =  i / 4 + 1;
    (*chessFigures)[n].offX[i + 2] = -i / 4 - 1;
    (*chessFigures)[n].offY[i + 2] = -i / 4 - 1;
    (*chessFigures)[n].offX[i + 3] =  i / 4 + 1;
    (*chessFigures)[n].offY[i + 3] = -i / 4 - 1;
  }

  n++;

  /* Rook */
  (*chessFigures)[n].name = "Rook";
  (*chessFigures)[n].n = MAX_OFFSET * 4;
  (*chessFigures)[n].offX = malloc(sizeof(int) * (*chessFigures)[n].n);
  (*chessFigures)[n].offY = malloc(sizeof(int) * (*chessFigures)[n].n);

  for (i = 0; i < MAX_OFFSET * 4; i += 4)
  {
    (*chessFigures)[n].offX[i]     =  i / 4 + 1;
    (*chessFigures)[n].offY[i]     =  0;
    (*chessFigures)[n].offX[i + 1] = -i / 4 - 1;
    (*chessFigures)[n].offY[i + 1] =  0;
    (*chessFigures)[n].offX[i + 2] =  0;
    (*chessFigures)[n].offY[i + 2] =  i / 4 + 1;
    (*chessFigures)[n].offX[i + 3] =  0;
    (*chessFigures)[n].offY[i + 3] = -i / 4 - 1;
  }

  n++;

  /* Queen */
  (*chessFigures)[n].name = "Queen";
  (*chessFigures)[n].n = MAX_OFFSET * 8;
  (*chessFigures)[n].offX = malloc(sizeof(int) * (*chessFigures)[n].n);
  (*chessFigures)[n].offY = malloc(sizeof(int) * (*chessFigures)[n].n);

  for (i = 0; i < MAX_OFFSET * 8; i += 8)
  {
    (*chessFigures)[n].offX[i]     =  i / 8 + 1;
    (*chessFigures)[n].offY[i]     =  i / 8 + 1;
    (*chessFigures)[n].offX[i + 1] = -i / 8 - 1;
    (*chessFigures)[n].offY[i + 1] =  i / 8 + 1;
    (*chessFigures)[n].offX[i + 2] = -i / 8 - 1;
    (*chessFigures)[n].offY[i + 2] = -i / 8 - 1;
    (*chessFigures)[n].offX[i + 3] =  i / 8 + 1;
    (*chessFigures)[n].offY[i + 3] = -i / 8 - 1;
    (*chessFigures)[n].offX[i + 4] =  i / 8 + 1;
    (*chessFigures)[n].offY[i + 4] =  0;
    (*chessFigures)[n].offX[i + 5] = -i / 8 - 1;
    (*chessFigures)[n].offY[i + 5] =  0;
    (*chessFigures)[n].offX[i + 6] =  0;
    (*chessFigures)[n].offY[i + 6] =  i / 8 + 1;
    (*chessFigures)[n].offX[i + 7] =  0;
    (*chessFigures)[n].offY[i + 7] = -i / 8 - 1;
  }

  n++;

  /* King */
  (*chessFigures)[n].name = "King";
  (*chessFigures)[n].n = 8;
  (*chessFigures)[n].offX = malloc(sizeof(int) * (*chessFigures)[n].n);
  (*chessFigures)[n].offY = malloc(sizeof(int) * (*chessFigures)[n].n);

  (*chessFigures)[n].offX[0] =  1;
  (*chessFigures)[n].offX[1] =  1;
  (*chessFigures)[n].offX[2] =  0;
  (*chessFigures)[n].offX[3] = -1;
  (*chessFigures)[n].offX[4] = -1;
  (*chessFigures)[n].offX[5] = -1;
  (*chessFigures)[n].offX[6] =  0;
  (*chessFigures)[n].offX[7] =  1;

  (*chessFigures)[n].offY[0] =  0;
  (*chessFigures)[n].offY[1] =  0;
  (*chessFigures)[n].offY[2] = -1;
  (*chessFigures)[n].offY[3] = -1;
  (*chessFigures)[n].offY[4] =  0;
  (*chessFigures)[n].offY[5] =  1;
  (*chessFigures)[n].offY[6] =  1;
  (*chessFigures)[n].offY[7] =  1;

  n++;

  return n;
} /* End of 'CreateChessFigures' function */

/* Destroying chess figures function.
 * ARGUMENTS:
 *   - array of chess figures:
 *       ChessFigure *chessFigure;
 *   - number of chess figures:
 *       int nChessFigures;
 * RETURNS: None.
 */
void DestroyChessFigures( ChessFigure *chessFigures, int nChessFigures )
{
  int i;

  for (i = 0; i < nChessFigures; i++)
  {
    free(chessFigures[i].offX);
    free(chessFigures[i].offY);
  }

  free(chessFigures);
} /* End of 'DestroyChessFigures' function */

/* Main window process messages callback function */
LRESULT CALLBACK WndProc( HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam )
{
  PAINTSTRUCT ps;
  HDC hdc;
  static RECT rt, boardRect, xAxisRect, yAxisRect, figureRect, pathLenRect;
  static Point fig1, fig2;
  static Point path[MAX_PATH_QUEUE];
  static HBRUSH hBlackBrush, hWhiteBrush, hRedBrush;
  static HPEN hPen;
  static ChessFigure *chessFigures;
  static int nChessFigures, currentChessFigure = 0 , pathLen = -1;
  int mX, mY;
  
  switch (message)
  {
  case WM_CREATE:
    /* Initializing chess figures */
    nChessFigures = CreateChessFigures(&chessFigures);
    /* Initializing board data */
    srand((unsigned)time(NULL));
    fig1.x = (int)((double)rand() / RAND_MAX * BOARD_X);
    fig1.y = (int)((double)rand() / RAND_MAX * BOARD_Y);
    fig2.x = (int)((double)rand() / RAND_MAX * BOARD_X);
    fig2.y = (int)((double)rand() / RAND_MAX * BOARD_Y);
    pathLen = SearchTurns(path, MAX_PATH_QUEUE, &fig1, &fig2,
      &chessFigures[currentChessFigure]);
    /* Creating instruments */
    hBlackBrush = CreateSolidBrush(RGB(0x00, 0x00, 0x00));
    hWhiteBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
    hRedBrush = CreateSolidBrush(RGB(0xFF, 0x00, 0x00));
    hPen = CreatePen(PS_SOLID, 4, RGB(0xFF, 0x10, 0x10));
    break;
  case WM_SIZE:
    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);

    /* Calculating render rectangles */
    GetClientRect(hWnd, &rt);

    /* Board rectangle */
    boardRect.left = rt.left + AXIS_WIDTH;
    boardRect.right = rt.right;
    boardRect.top = rt.top + CAPTION_HEIGHT;
    boardRect.bottom = rt.bottom - AXIS_HEIGHT;

    /* Correcting aspect */
    if ((double)(boardRect.right - boardRect.left) / BOARD_X >
        (double)(boardRect.bottom - boardRect.top) / BOARD_Y)
    {
      boardRect.right = (long)(boardRect.left +
        (double)(boardRect.bottom - boardRect.top) / BOARD_Y * BOARD_X);
    }
    else
    {
      boardRect.bottom = (long)(boardRect.top +
        (double)(boardRect.right - boardRect.left) / BOARD_X * BOARD_Y);
    }
    
    /* 'X' axis rectangle */
    xAxisRect.left   = boardRect.left;
    xAxisRect.right  = boardRect.right;
    xAxisRect.top    = boardRect.bottom;
    xAxisRect.bottom = boardRect.bottom + AXIS_HEIGHT;

    /* 'Y' axis rectangle */
    yAxisRect.left   = rt.left;
    yAxisRect.right  = rt.left + AXIS_WIDTH;
    yAxisRect.top    = boardRect.top;
    yAxisRect.bottom = boardRect.bottom;

    /* Figure name rectangle */
    figureRect.left   = rt.left + AXIS_WIDTH;
    figureRect.right  = rt.left + AXIS_WIDTH + FIGURE_WIDTH;
    figureRect.top    = rt.top;
    figureRect.bottom = rt.top + CAPTION_HEIGHT;

    /* Path length string rectangle */
    pathLenRect.left   = rt.left + FIGURE_WIDTH + AXIS_WIDTH;
    pathLenRect.right  = rt.left + FIGURE_WIDTH + AXIS_WIDTH + PATH_WIDTH;
    pathLenRect.top    = rt.top;
    pathLenRect.bottom = rt.top + CAPTION_HEIGHT;

    /* Drawing figure caption */
    DrawFigureCaption(hdc, &figureRect,
      chessFigures[currentChessFigure].name);

    /* Drawing path length caption */
    DrawPathLenCaption(hdc, &pathLenRect, pathLen);

    /* Drawing chessboard */
    DrawChessboard(hdc, &boardRect, hBlackBrush, hWhiteBrush);

    /* Drawing axes */
    DrawXAxis(hdc, &xAxisRect);
    DrawYAxis(hdc, &yAxisRect);

    /* Drawing path */
    DrawPath(hdc, &boardRect, &fig1, path, pathLen, hPen, hRedBrush);

    /* Drawing figures labels */
    DrawFigureName(hdc, &boardRect, &fig1, "Fig.1");
    DrawFigureName(hdc, &boardRect, &fig2, "Fig.2");
    
    EndPaint(hWnd, &ps);
    break;
  case WM_CHAR:
    /* Quiting on escape */
    if (wParam == 27)
      SendMessage(hWnd, WM_DESTROY, 0, 0);
    break;
  case WM_LBUTTONUP:
    mX = LOWORD(lParam);
    mY = HIWORD(lParam);

    /* Changing first figure position */
    if (_isInRect(&boardRect, mX, mY))
    {
      fig1.x = _getXRectPart(&boardRect, BOARD_X, mX);
      fig1.y = _getYRectPart(&boardRect, BOARD_Y, mY);
      pathLen = SearchTurns(path, MAX_PATH_QUEUE, &fig1, &fig2,
        &chessFigures[currentChessFigure]);

      InvalidateRect(hWnd, &rt, TRUE);
    }
    /* Changing figure type (next) */
    else if (_isInRect(&figureRect, mX, mY))
    {
      currentChessFigure = (currentChessFigure + 1) % nChessFigures;
      pathLen = SearchTurns(path, MAX_PATH_QUEUE, &fig1, &fig2,
        &chessFigures[currentChessFigure]);

      InvalidateRect(hWnd, &rt, TRUE);
    }
    break;
  case WM_RBUTTONUP:
    mX = LOWORD(lParam);
    mY = HIWORD(lParam);

    if (_isInRect(&boardRect, mX, mY))
    {
      /* Changing second figure position */
      fig2.x = _getXRectPart(&boardRect, BOARD_X, mX);
      fig2.y = _getYRectPart(&boardRect, BOARD_Y, mY);
      pathLen = SearchTurns(path, MAX_PATH_QUEUE, &fig1, &fig2,
        &chessFigures[currentChessFigure]);

      InvalidateRect(hWnd, &rt, TRUE);
    }
    /* Changing figure type (previous) */
    else if (_isInRect(&figureRect, mX, mY))
    {
      currentChessFigure =
        (currentChessFigure + nChessFigures - 1) % nChessFigures;
      pathLen = SearchTurns(path, MAX_PATH_QUEUE, &fig1, &fig2,
        &chessFigures[currentChessFigure]);

      InvalidateRect(hWnd, &rt, TRUE);
    }
    break;
  case WM_DESTROY:
    /* Destroying created chess figures */
    DestroyChessFigures(chessFigures, nChessFigures);
    /* Destroying created instruments */
    DeleteObject(hBlackBrush);
    DeleteObject(hWhiteBrush);
    DeleteObject(hRedBrush);
    DeleteObject(hPen);
    /* Quiting */
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
} /* End of 'WndProc' function */

/* Registering class function */
ATOM MyRegisterClass( HINSTANCE hInstance )
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = (WNDPROC)WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = NULL;
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm       = NULL;

  return RegisterClassEx(&wcex);
} /* End of 'MyRegisterClass' function */

/* Main program function */
int APIENTRY WinMain( HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPSTR     lpCmdLine,
                      int       nCmdShow)
{
  MSG msg;

  /* Register main class */
  MyRegisterClass(hInstance);

  /* Perform application initialization */
  if (!InitInstance(hInstance, nCmdShow)) 
  {
    return FALSE;
  }

  /* Main message loop */
  while (GetMessage(&msg, NULL, 0, 0)) 
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
} /* End of 'WinMain' function */
