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

__declspec(align(16))
class Terrain : public ve::RenderObject {

 public:
  void *operator new( size_t stAllocateBlock) {
    return _aligned_malloc(sizeof(Terrain),16);
  }

   void   operator delete (void* p)  {
      return _aligned_free(p);
   }
  int Initialize(ve::Context* context);
  int Deinitialize();
  concurrency::task<int> LoadAsync();
  concurrency::task<int> UnloadAsync();
  int UpdateVerticies() { return S_OK; }
  int UpdateTransform() { return S_OK; }
  int Update(float timeTotal, float timeDelta);
  int Render();
 private:
	int status;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	//ID3D11Buffer* cb;
  ve::InputLayout input_layout_;
	uint32_t m_indexCount;
	ModelViewProjectionConstantBuffer cb_data;
  ve::HullShader hs_;//ps_,ds_,hs_,gs_;
  ve::DomainShader ds_;
  ve::VertexShader vs_;
};

}