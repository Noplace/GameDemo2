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



class WorldScene : public ve::Scene {

 public:
  ve::ContextD3D11* gfx;

  //required for the dx::XMMatrix in the camera
  void *operator new( size_t stAllocateBlock) {
    return _aligned_malloc(sizeof(CubeScene),16);
  }

   void   operator delete (void* p)  {
      return _aligned_free(p);
   }

  int Initialize(ve::Context* context);
  int Deinitialize();

  concurrency::task<int> LoadAsync();
  concurrency::task<int> UnloadAsync();
  int Set();
  int Unset();
  int Update(float timeTotal, float timeDelta);
  int Render();
  int UpdateWorldMatrix(const dx::XMMATRIX& world);
  ve::Camera* camera() { return &camera_; }

   int OnWindowSizeChange() { 
    float aspectRatio = float(gfx->width()) / float(gfx->height());
	  float fovAngleY = 70.0f * dx::XM_PI / 180.0f;
    camera_.BuildProjectionMatrix(fovAngleY,aspectRatio,0.01f,100.0f);
    dx::XMStoreFloat4x4(&vs_cb0_data.projection,camera_.projection_transposed());
    
    for (auto i : render_list_)
      i->OnWindowSizeChange();
    return S_OK; 
   }

 private:
	bool m_loadingComplete;
  //ThirdPersonCamera camera_;
  ve::FirstPersonCamera camera_;
  ID3D11RasterizerState * scene_rasterizer_state_ ;
	//ID3D11Buffer* m_vertexBuffer;
	//ID3D11Buffer* m_indexBuffer;
  ID3D11Buffer* ps_cb;
	ID3D11Buffer* vs_cb_list[3];
  ve::InputLayout input_layout_;
	//uint32_t m_indexCount;
	ModelViewProjectionConstantBuffer vs_cb0_data;
  TimeConstantBuffer vs_cb1_data;
  ViewInverseConstantBuffer vs_cb2_data;
  //ve::Shader ds_,hs_,gs_;
  Terrain* terrain_;
  Sky* sky_;
  ve::SkyBox* skybox;
};
}