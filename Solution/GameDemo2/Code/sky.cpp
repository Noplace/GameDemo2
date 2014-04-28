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

namespace demo {



  int Sky::Initialize(ve::Context* context) {
    int hr = RenderObject::Initialize(context);
    return hr;
  }

  int Sky::Deinitialize() {
    RenderObject::Deinitialize();
    return S_OK;
  }

 

  concurrency::task<int> Sky::LoadAsync() {
    return concurrency::create_task([this](){
      auto gfx = (ve::ContextD3D11*)context_;

      static auto filename1 = (ve::GetExePath() + "\\vs_tex.cso");
      auto vs_fd = ve::ReadDataAsync(filename1.c_str()).get();
      static auto filename2 = (ve::GetExePath() + "\\ps_sky.cso");
      auto ps_data = ve::ReadDataAsync(filename2.c_str()).get();

   

	  
        if (vs_fd.data != nullptr) {
          context_->CreateVertexShader(vs_fd.data,vs_fd.length,vs_);
          context_->CreateInputLayout(VertexPositionColorTextureElementDesc,ARRAYSIZE(VertexPositionColorTextureElementDesc),vs_fd,input_layout_);
          SafeDeleteArray(&vs_fd.data);
        }
    



	      if (ps_data.data != nullptr) {
          gfx->CreatePixelShader(ps_data.data,ps_data.length,ps_);
      

		      //CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(LoadingScenePSCB), D3D11_BIND_CONSTANT_BUFFER);
		      //ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc1,nullptr, (void**)&ps_cb0));
          //CD3D11_BUFFER_DESC constantBufferDesc2(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    	    //ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc2,nullptr, (void**)&m_constantBuffer ));

		      CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(SkyPSShaderCB1), D3D11_BIND_CONSTANT_BUFFER);
		      ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc3,nullptr, (void**)&sky_ps_cb1));

          SafeDeleteArray(&ps_data.data);
        }


  /*

      auto createCubeTask = (createVSTask).then([this,gfx] () {
    
        int size_y = 50;
        int size_x = 50;
      

        VertexPositionColor* vertices = new VertexPositionColor[(size_y)*(size_x)];
        uint32_t* indices = new uint32_t[(size_y)*(size_x)*6];
        m_indexCount = (size_y-1)*(size_x-1)*6;
        auto iptr = indices;
        for (int y=0;y<size_y;++y)
          for (int x=0;x<size_x;++x) {
            vertices[x+(y*size_x)].pos = dx::XMFLOAT3(x, -0.5f, y);
            vertices[x+(y*size_x)].color = dx::XMFLOAT3(1.0f, 1.0f, 1.0f);
          
          
         
          }
          int  count = 0;
          for (int y=0;y<size_y-1;++y)
            for (int x=0;x<size_x-1;++x) {
                *iptr++ = (x+0)+((y+0)*(size_x));
                *iptr++ = (x+1)+((y+0)*(size_x));
                *iptr++ = (x+0)+((y+1)*(size_x));
                *iptr++ = (x+0)+((y+1)*(size_x));
                *iptr++ = (x+1)+((y+0)*(size_x));
                *iptr++ = (x+1)+((y+1)*(size_x));
                count+=6;
             }

    


		    D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		    vertexBufferData.pSysMem = vertices;
		    vertexBufferData.SysMemPitch = 0;
		    vertexBufferData.SysMemSlicePitch = 0;
		    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColor)*size_y*size_x, D3D11_BIND_VERTEX_BUFFER);
		    ThrowIfFailed(

			    gfx->device()->CreateBuffer(
				    &vertexBufferDesc,
				    &vertexBufferData,
				    &vb
				    )
			    );


		    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		    indexBufferData.pSysMem = indices;
		    indexBufferData.SysMemPitch = 0;
		    indexBufferData.SysMemSlicePitch = 0;
		    CD3D11_BUFFER_DESC indexBufferDesc(4*m_indexCount, D3D11_BIND_INDEX_BUFFER);
		    ThrowIfFailed(
			    gfx->device()->CreateBuffer(
				    &indexBufferDesc,
				    &indexBufferData,
				    &ib
				    )
			    );

        SafeDeleteArray(&indices);
        SafeDeleteArray(&vertices);
	    });

	    return createCubeTask.then([this] () {
		    status = 1;
      
        //std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        return (int)S_OK;
	    });*/

      world_ = dx::XMMatrixIdentity();
      return (int)S_OK;
    });
  }

  concurrency::task<int> Sky::UnloadAsync() {
    return concurrency::create_task([this](){
      context_->DestoryInputLayout(input_layout_);
//      SafeRelease(&cb);
      SafeRelease(&vb);
      SafeRelease(&ib);
      //context_->DestroyShader(ps_);
      context_->DestroyShader(vs_);
     // context_->DestroyShader(gs_);
     // context_->DestroyShader(ds_);
     // context_->DestroyShader(hs_);
      return (int)S_OK;
    });
  }



  int Sky::Update(float timeTotal, float timeDelta) {
    (void) timeDelta; // Unused parameter.

    return S_OK;
  }
  

  int Sky::Render() {
	  if (status != 1)
	  {
		  return S_FALSE;
	  }

    context_->SetInputLayout(input_layout_);
    //context_->SetShader(hs_);
    //context_->SetShader(ds_);
    context_->SetShader(vs_);

	  UINT stride = sizeof(VertexPositionColor);
	  UINT offset = 0;
    auto gfx = (ve::ContextD3D11*)context_;
	  gfx->device_context()->IASetVertexBuffers(0,1,&vb,&stride,&offset);
	  gfx->device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfx->device_context()->IASetIndexBuffer(ib,DXGI_FORMAT_R32_UINT,0);
	  
	  gfx->device_context()->DrawIndexed(m_indexCount,0,0);
    return S_OK;
  }

}