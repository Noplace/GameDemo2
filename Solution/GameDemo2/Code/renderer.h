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
#pragma once

namespace demo {

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch Win32 API errors.
		throw;
	}
}

__declspec(align(16))
struct ModelViewProjectionConstantBuffer
{
	dx::XMFLOAT4X4 model;
	dx::XMFLOAT4X4 view;
  dx::XMFLOAT4X4 viewInv;
	dx::XMFLOAT4X4 projection;
};

__declspec(align(16))
struct TimeConstantBuffer {
	float totalTime;
  float deltaTime;
};

__declspec(align(16))
struct ViewInverseConstantBuffer
{
  dx::XMFLOAT4X4 viewInv;
};

class Renderer {
 public:
  Renderer(){}
  ~Renderer(){}
  void Init(HWND handle_);
  void Deinit();
  void Update(float , float);
  void Render();
  void OnWindowSizeChanged();
  ve::ContextD3D11* gfx;
  ve::Scene* current_scene;

//ve::PixelShader ps;
//ve::VertexShader vs;


};

}