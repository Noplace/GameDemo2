/*****************************************************************************************************************
* Copyright (c) 2014 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#include "global.h"
#include <Windowsx.h>

namespace demo {

ClientMainWindow::ClientMainWindow() {
  dispmode = 0;
  class_ = 0;
  memset(display,0,sizeof(display));
  display[0].style = WS_VISIBLE | WS_BORDER|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  display[1].style = WS_POPUP | WS_VISIBLE | WS_MAXIMIZE;


  memset(&display[0].mode, 0, sizeof(display[0].mode));
	display[0].mode.dmSize       = sizeof(display[0].mode);
	display[0].mode.dmPelsWidth  = (unsigned long)640;
	display[0].mode.dmPelsHeight = (unsigned long)480;
	display[0].mode.dmBitsPerPel = 32;			
	display[0].mode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
 
  memset(&display[1].mode, 0, sizeof(display[1].mode));
	display[1].mode.dmSize       = sizeof(display[1].mode);
	display[1].mode.dmPelsWidth  = (unsigned long)640;
	display[1].mode.dmPelsHeight = (unsigned long)480;
	display[1].mode.dmBitsPerPel = 32;			
	display[1].mode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

 

  sprintf_s(title_,"Game Client");
  sprintf_s(class_name_,"%s_class",title_);
  memset(&window_class_,0,sizeof(window_class_));
  window_class_.cbSize = sizeof(window_class_);
  window_class_.cbClsExtra = 0;
  window_class_.cbWndExtra = 0;
  window_class_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class_.lpfnWndProc = WindowProc;
  window_class_.lpszClassName = class_name_;
  window_class_.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
  window_class_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); 
  void test_json();
  //test_json();
}

ClientMainWindow::~ClientMainWindow() {
  
}

void ClientMainWindow::Create() {
  class_ = RegisterClassEx(&window_class_);
  CreateWindow(class_name_,title_,display[dispmode].style,0,0,100,100,NULL,NULL,NULL,this);
  SetWindowText(handle_,title_);
  dc_ = GetDC(handle_);
  /*PIXELFORMATDESCRIPTOR pfd = {
    0,1,PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0
  };
  SetPixelFormat(dc_, ChoosePixelFormat(dc_, &pfd) , &pfd);
  */

  
}

void ClientMainWindow::Start() {
  SetClientSize(640,480);
  Center();
  Show();
  //gl.Initialize(handle_,640,480,1,0,true);
  timer =  new ve::Timer();
  renderer.Init(handle_);

   

}

void ClientMainWindow::Destroy() {
 // gl.Deinitialize(handle_);
  renderer.Deinit();
  SafeDelete(&timer);
  if (class_!=0) {
    UnregisterClass(class_name_,0);
    class_ = 0;
  }
 
}



void ClientMainWindow::SetClientSize(int width,int height) {
  RECT rectangle;
  SetRect(&rectangle,0,0,width,height);
  AdjustWindowRectEx(&rectangle,display[dispmode].style,0,0);
  int actual_width  = static_cast<int>(rectangle.right  - rectangle.left);
  int actual_height = static_cast<int>(rectangle.bottom - rectangle.top);
  SetWindowPos(handle_,0,0,0,actual_width,actual_height,SWP_NOACTIVATE|SWP_NOMOVE);  
}

void ClientMainWindow::Center() {
  RECT rectangle;
  GetWindowRect(handle_,&rectangle);
  
  int screen_width  = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);
  
  int left   = (screen_width - rectangle.right)/2;
  int top    = (screen_height - rectangle.bottom)/2;
  
  SetWindowPos(handle_,0,left,top,0,0,SWP_NOACTIVATE|SWP_NOSIZE);  
  
}

void ClientMainWindow::Show() {
  ShowWindow(handle_, SW_SHOW); 
  UpdateWindow(handle_); 
}

void ClientMainWindow::Hide() {
  ShowWindow(handle_, SW_HIDE); 
}

void ClientMainWindow::Fullscreen() {
  dispmode = 1;
  display[dispmode].mode.dmPelsWidth = 1024;
  display[dispmode].mode.dmPelsHeight = 768;

  SetWindowPos(handle_,0,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE); 
  SetWindowLongPtr(handle_,GWL_STYLE,display[dispmode].style);
  SetClientSize(display[dispmode].mode.dmPelsWidth,display[dispmode].mode.dmPelsHeight);

  auto r = ChangeDisplaySettings(&display[dispmode].mode,CDS_FULLSCREEN);
  ShowCursor(0);
  
    //HDC hDC = GetDC(CreateWindow("edit",0,WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
  //wglMakeCurrent(hDC, wglCreateContext(hDC));
  //HDC hDC = GetDC(CreateWindow("edit",0,WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
  //SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd) , &pfd);
  //wglMakeCurrent(hDC, wglCreateContext(hDC));
  //
}

void ClientMainWindow::Windowed() {
  dispmode = 0;
  display[dispmode].mode.dmPelsWidth = 640;
  display[dispmode].mode.dmPelsHeight = 480;

  ChangeDisplaySettings(0,0);
  ShowCursor(1);

  SetWindowLongPtr(handle_,GWL_STYLE,display[dispmode].style);
  SetClientSize(display[dispmode].mode.dmPelsWidth,display[dispmode].mode.dmPelsHeight);
  Center();


}

void ClientMainWindow::Step() {
  //gl.BeginScene(1,1,0,1);
  timer->Update();
  static auto update_span_accumulator = 0.0f;
  static auto render_span_accumulator = 0.0f;
  static auto update_total_time = 0.0f;
  auto time_span = timer->delta();

  if (time_span > 500.0) //clamping time
    time_span = 500.0;

  update_span_accumulator += time_span;
  render_span_accumulator += time_span;
  const float update_step_dt = 0.016667f;
  while (update_span_accumulator >= update_step_dt) {
    
    renderer.Update(update_total_time,update_step_dt);
    update_span_accumulator -= update_step_dt;
    update_total_time += update_step_dt;
  }

  const float render_step_dt = 0.016667f;
  if (render_span_accumulator >= render_step_dt) {
    renderer.Render();
    render_span_accumulator -= render_step_dt;
  }
}

LRESULT CALLBACK ClientMainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  static ClientMainWindow* win = nullptr;
 
  switch( uMsg ) {
    case WM_NCCREATE: {
      CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
      win = reinterpret_cast<ClientMainWindow*>(create_struct->lpCreateParams);
      win->handle_ = hwnd;
      SetWindowLongPtr(hwnd,GWLP_USERDATA,(ULONG_PTR)(win));
      return TRUE;
    }  
  }
  
  //ClientMainWindow* current_window = reinterpret_cast<ClientMainWindow*>(static_cast<LONG_PTR>(GetWindowLongPtr(hwnd,GWLP_USERDATA)));
  
  if (win != nullptr) {
    switch( uMsg ) {
      case WM_KEYDOWN:
        if (wParam == VK_F11) {
          //if (win->dispmode == 0)
            //win->Fullscreen();
          //else
            //win->Windowed();
          
        }
        return 0;
      case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        return 0;
      case WM_MOUSEMOVE:
        {
          char str[25];
          sprintf_s(str,"x:%04d y:%04d",GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
          SetWindowText(hwnd,str);
        }
        return 0;
      case WM_LBUTTONUP:
        ReleaseCapture();
        return 0;
      case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
          return 0;
      break;
      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
  }
  return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

}