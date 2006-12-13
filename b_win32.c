/*
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "st_types.h"
#include "betray.h"

#ifdef BETRAY_WIN32_SYSTEM_WRAPPER

static boolean	input_focus = TRUE;
static uint	screen_size_x = 800;
static uint	screen_size_y = 600;
static boolean	mouse_warp = FALSE;
static HDC	hDC;				/* device context */
static HWND	hWnd;

extern void sui_get_input(BInputState *data);

boolean b_win32_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen) 
{  
	return TRUE;
}

void (*sdl_context_func)(void) = NULL;

void betray_set_context_update_func(void (*context_func)(void))
{
	sdl_context_func = context_func;
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	static PAINTSTRUCT ps;
	static int omx, omy, mx, my;
	BInputState *input;
	int i;

	input = betray_get_input_state();
	switch(uMsg)
	{
		case WM_PAINT :
			betray_action(BAM_DRAW);
			glFlush();
			SwapBuffers(hDC);
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		return 0;
		case WM_SIZE :
			screen_size_x = LOWORD(lParam);
			screen_size_y = HIWORD(lParam);
			betray_reshape_view(screen_size_x, screen_size_y);
		return 0;
	    case WM_LBUTTONDOWN :
	    case WM_LBUTTONUP :
	    case WM_RBUTTONDOWN :
	    case WM_RBUTTONUP :
	    case WM_MBUTTONDOWN :
	    case WM_MBUTTONUP :
	    case WM_MOUSEMOVE :
		if(uMsg == WM_LBUTTONDOWN)
			input->mouse_button[0] = TRUE;
		if(uMsg == WM_LBUTTONUP)
			input->mouse_button[0] = FALSE;
		if(uMsg == WM_RBUTTONDOWN)
			input->mouse_button[1] = TRUE;
		if(uMsg == WM_RBUTTONUP)
			input->mouse_button[1] = FALSE;
		if(uMsg == WM_MBUTTONDOWN)
			input->mouse_button[2] = TRUE;
		if(uMsg == WM_MBUTTONUP)
			input->mouse_button[2] = FALSE;

		input->pointer_x = (float)LOWORD(lParam) / (float)screen_size_x * 2.0 - 1.0;
		input->pointer_y = (1.0 - (float)HIWORD(lParam) / (float)screen_size_y * 2.0) * (float)screen_size_y / (float)screen_size_x;
		{
			RECT r;
			int		style;
			r.left   = 0;
			r.top    = 0;
			r.right  = 1;
			r.bottom = 1;

			style = GetWindowLong( hWnd, GWL_STYLE );
			AdjustWindowRect( &r, style, FALSE );
		}
		return 0;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
				{
					static FILE *f = NULL;
					f = fopen("key_output.txt", "a");
					fprintf(f, "sys pressed = %u %u\n", wParam, lParam); 
					fclose(f);
				}
				if(betray_is_type_in())
				{
					if(wParam == 8) /* enter */
					{
					//	betray_move_cursor(1);
						betray_delete_character();
					}
					else if(wParam == 13) /* enter */
						betray_end_type_in_mode(FALSE);
					else if(wParam == 27) /* ESC */
						betray_end_type_in_mode(TRUE);
					else if(wParam == 39)
						betray_move_cursor(1);
					else if(wParam == 37)
						betray_move_cursor(-1);
					else if(wParam == 46)
					{
						betray_move_cursor(1);
						betray_delete_character();
					}
					else if(wParam == 35) /* END */
						betray_move_cursor(32000);
					else if(wParam == 36) /* HOME */
						betray_move_cursor(-32000);
				}
			break;
		case WM_CHAR :
			{
				uint key;
			/*	{
					static FILE *f = NULL;
					f = fopen("key_output.txt", "a");
					fprintf(f, "key pressed = %u\n", wParam); 
					fclose(f);
				}*/
				key = wParam;
				if(betray_is_type_in())
				{
					if(key > 32 && key < 128)
					{
						betray_insert_character(key);
					}
				}
				else
				{
					if(input->event_count < MAX_EVENT_COUNT)
					{
						input->event[input->event_count].state = TRUE;
						input->event[input->event_count++].button = key;
					}
				}
			}
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
				if(!betray_is_type_in() && input->event_count < MAX_EVENT_COUNT)
				{
					input->event[input->event_count].state = FALSE;
					input->event[input->event_count++].button = lParam;
				}
			}
			break;
	    case WM_CLOSE:
			exit(0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
}


HWND  hWnd;				/* window */

boolean b_win32_init_display(uint size_x, uint size_y, boolean full_screen, const char *caption) 
{
	int n, pf;
	HGLRC hRC;				/* opengl context */
	WNDCLASS    wc;
	LOGPALETTE* lpPal;
	PIXELFORMATDESCRIPTOR pfd;
	static HINSTANCE hInstance = 0;

	/* only register the window class once - use hInstance as a flag. */
	if(!hInstance)
	{
		hInstance = GetModuleHandle(NULL);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "OpenGL";

		if(!RegisterClass(&wc))
		{
			printf("Betray: RegisterClass() failed: Cannot register window class.\n");
			return FALSE;
		}
	}

	hWnd = CreateWindow("OpenGL", caption, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 10, 10, size_x, size_y, NULL, NULL, hInstance, NULL);

	if(hWnd == NULL)
	{
		printf("Betray: CreateWindow() failed:  Cannot create a window.\n");
		return FALSE;
	}

	hDC = GetDC(hWnd);

	/* there is no guarantee that the contents of the stack that become
	the pfd are zeroed, therefore _make sure_ to clear these bits. */
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cStencilBits = 8;
	pfd.cDepthBits = 24;
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat(hDC, &pfd);
	if(pf == 0)
	{
		printf("Betray: ChoosePixelFormat() failed: Cannot find a suitable pixel format.\n"); 
		return FALSE;
	} 
	if(SetPixelFormat(hDC, pf, &pfd) == FALSE)
	{
		printf("Betray: SetPixelFormat() failed: Cannot set format specified.\n");
		return FALSE;
	} 

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	ReleaseDC(hDC, hWnd);

	hDC = GetDC(hWnd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	return TRUE;
}    

void system_wrapper_endframe(void)
{ 
}

void system_wrapper_lose_focus(void)
{
	input_focus = FALSE;
}

void betray_set_mouse_warp(boolean warp)
{
	mouse_warp = warp;
}

//PROC  ( WINAPI * bglGetProcAddress)(LPCSTR);

void *betray_get_gl_proc_address()
{
	HINSTANCE hinstOpenGL;
//	return &SDL_GL_GetProcAddress;
//	hinstOpenGL = LoadLibrary("opengl32");
//	bglGetProcAddress = GetProcAddress(hinstOpenGL, "wglGetProcAddress");
	return &wglGetProcAddress;
}

extern void betray_time_update(void);

int my_nCmdShow;

void betray_launch_main_loop(void)
{
	MSG   msg;				/* message */
	BInputState *input;

	input = betray_get_input_state();
	ShowWindow(hWnd, my_nCmdShow);

/*	while(GetMessage(&msg, hWnd, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/
	while(TRUE)
	{
		float x, y;
		input->delta_pointer_x = -input->pointer_x;
		input->delta_pointer_y = -input->pointer_y;
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(!GetMessage(&msg, NULL, 0, 0))
				;
			TranslateMessage(&msg);
   			DispatchMessage(&msg);
		}
		input->delta_pointer_x += input->pointer_x;
		input->delta_pointer_y += input->pointer_y;

		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
		{
			input->click_pointer_x = input->pointer_x;
			input->click_pointer_y = input->pointer_y;
		}
		betray_time_update();
		betray_action(BAM_EVENT);	
		betray_action(BAM_DRAW);
		input->delta_pointer_x = 0;
		input->delta_pointer_y = 0;	
		input->event_count = 0;
		glFlush();
		SwapBuffers(hDC);
		input->time++;
		input->last_mouse_button[0] = input->mouse_button[0];
		input->last_mouse_button[1] = input->mouse_button[1];
		input->last_mouse_button[2] = input->mouse_button[2];
		betray_action(BAM_MAIN);
	}
}

extern int main(int argc, char **argv);

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
	char *argv[] = { "file.exe", NULL };

	my_nCmdShow = nCmdShow;
	main(1, &argument);

	return TRUE;
}

#endif
