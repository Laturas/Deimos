#include <windows.h>
#include <windowsx.h>
#include "guifuncs.c"
#include "resources.c"

const char g_szClassName[] = "myWindowClass";

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;
static bool mouse_is_pressed = false;

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
        KILL:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        case WM_PAINT: {
            static PAINTSTRUCT paint;
            static HDC device_context;
            device_context = BeginPaint(hwnd, &paint);
            BitBlt(device_context,
                   0, 0,
                   paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                   frame_device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   SRCCOPY);
            //RECT item = {0,500,500,0};
            //LPCSTR hel = "Hello";
            //DrawText(frame_device_context,hel,5,&item,DT_BOTTOM);

            //HDC dc = GetDc(hwnd);
            RECT rc;
            SetTextColor(device_context,0xFFFFFF);
            SetBkColor(device_context,(0x222222));

            GetClientRect(hwnd, &rc);
            DrawText(device_context, "Hello!", -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            ReleaseDC(hwnd, device_context);

            EndPaint(hwnd, &paint);
        } break;
        case WM_SIZING: {
            frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if(frame_bitmap) DeleteObject(frame_bitmap);
            frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
            SelectObject(frame_device_context, frame_bitmap);

            frame.width =  LOWORD(lParam);
            frame.height = HIWORD(lParam);
        } break;
        case WM_SIZE: {
            frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if(frame_bitmap) DeleteObject(frame_bitmap);
            frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
            SelectObject(frame_device_context, frame_bitmap);

            frame.width =  LOWORD(lParam);
            frame.height = HIWORD(lParam);
        } break;
        case WM_LBUTTONDOWN: {
            
            mouse_is_pressed = true;
            int xPos = GET_X_LPARAM(lParam); 
            int yPos = GET_Y_LPARAM(lParam);
            //printf("Position: (%d, %d)\n",xPos,yPos); fflush(stdout);
        } break;
        case WM_LBUTTONUP: {
            mouse_is_pressed = false;
        } break;
        case WM_RBUTTONDOWN: {
            
            rclick = true;
            m_x_pos = GET_X_LPARAM(lParam);
            m_y_pos = WINHEIGHT - GET_Y_LPARAM(lParam);
        } break;
        case WM_RBUTTONUP: {
            rclick = false;
        } break;
        case WM_MOUSEMOVE: {
            m_x_pos = GET_X_LPARAM(lParam);
            m_y_pos = WINHEIGHT - GET_Y_LPARAM(lParam);
            int xPos = GET_X_LPARAM(lParam); 
            int yPos = GET_Y_LPARAM(lParam);
            if (mouse_is_pressed) {
                cam_x_pos -= prev_mouse_x - xPos;
                cam_y_pos += prev_mouse_y - yPos;
            }
            prev_mouse_x = xPos;
            prev_mouse_y = yPos;
        }
        case WM_MOUSEWHEEL: {
            int zDelt = GET_WHEEL_DELTA_WPARAM(wParam);

            if (zDelt > 0) {scale = (scale == 1) ? 5 : scale + 5;}
            if (zDelt < 0) {scale = (scale <= 5) ? 1 : scale - 5;}

            
        }
        case WM_KEYDOWN: {
            //printf("Something pressed!\n"); fflush(stdout);
            if (wParam == MK_CONTROL)
            {
                goto KILL;
            }
        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_HIDE );
    int offset = LookupIconIdFromDirectory(icon_buffer, TRUE);
    HICON ic = CreateIconFromResource(icon_buffer + offset, 4286 - offset, TRUE, 0x00030000);

    //HICON ic = CreateIconFromResource(icon_array,4551,1,0x00030000);
    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = ic;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = ic;

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Wacky Desmos Clone",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        static unsigned int p = 0;
        
        //frame.pixels[(p++)%(frame.width*frame.height)] = 0x88888888;
        //frame.pixels[18%(frame.width*frame.height)] = 0x88888888;

        REDRAW_ALL(0x22222222, 0xCCCCCCCC);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_INTERNALPAINT);

        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}