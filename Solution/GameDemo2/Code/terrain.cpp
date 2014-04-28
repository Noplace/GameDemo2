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



  int Terrain::Initialize(ve::Context* context) {
    int hr = RenderObject::Initialize(context);
    return hr;
  }

  int Terrain::Deinitialize() {
    RenderObject::Deinitialize();
    return S_OK;
  }

 

  concurrency::task<int> Terrain::LoadAsync() {

    auto gfx = (ve::ContextD3D11*)context_;

    static auto filename1 = (ve::GetExePath() + "\\vs_terrain.cso");
    auto loadVSTask = ve::ReadDataAsync(filename1.c_str());
    //static auto filename2 = (ve::GetExePath() + "\\ps.cso");
    //auto loadPSTask = ve::ReadDataAsync(filename2.c_str());

    //static auto filename3 = (ve::GetExePath() + "\\test_hull.cso");
    //auto loadHSTask = ve::ReadDataAsync(filename3.c_str());
    //static auto filename4 = (ve::GetExePath() + "\\test_domain.cso");
    //auto loadDSTask = ve::ReadDataAsync(filename4.c_str());
    //static auto filename5 = (ve::GetExePath() + "\\test_geom.cso");
    //auto loadGSTask = ve::ReadDataAsync(filename5.c_str());


    /*auto createHSTask = loadHSTask.then([this,gfx](ve::FileData fd){
      ID3D11HullShader* ptr;
      gfx->device()->CreateHullShader(fd.data,fd.length,0,&ptr);
      hs_.set_internal_pointer(ptr);
    });

    auto createDSTask = loadDSTask.then([this,gfx](ve::FileData fd){
      ID3D11DomainShader* ptr;
      gfx->device()->CreateDomainShader(fd.data,fd.length,0,&ptr);
      ds_.set_internal_pointer(ptr);
    });

    auto createGSTask = loadGSTask.then([this](ve::FileData fd){
      ID3D11GeometryShader* ptr;
      gfx->device()->CreateGeometryShader(fd.data,fd.length,0,&ptr);
      gs_.set_internal_pointer(ptr);
    });*/



	  auto createVSTask = loadVSTask.then([this](ve::FileData fd){
      if (fd.data != nullptr) {
    
        context_->CreateVertexShader(fd.data,fd.length,vs_);
    

        const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
		    {
			    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			    { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    };

        context_->CreateInputLayout(vertexDesc,ARRAYSIZE(vertexDesc),fd,input_layout_);

		   
        SafeDeleteArray(&fd.data);
      }
    });

/*
    const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
		    {
			    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			    { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    };
    auto sm = new ve::ShaderManager();
    sm->Initialize(context_);
    auto createVSTask = sm->RequestVertexShaderAsync(filename1,(const void*)vertexDesc,2).then([this](ve::RequestVertexShaderResult pair){
      vs_ = pair.vs;
      input_layout_ = pair.il;
        
        

    });
    delete sm;*/
    

 /*

	  auto createPSTask = loadPSTask.then([this](ve::FileData fd){
      if (fd.data != nullptr) {
        gfx->CreatePixelShader(fd.data,fd.length,ps_);
		    CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		    ThrowIfFailed(
			    gfx->device()->CreateBuffer(
				    &constantBufferDesc,
				    nullptr,
				    &m_constantBuffer
				    )
			    );
        SafeDeleteArray(&fd.data);
      }
    });
*/



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
      world_ = dx::XMMatrixIdentity();
      //std::this_thread::sleep_for(std::chrono::milliseconds(4000));
      return (int)S_OK;
	  });

  }

  concurrency::task<int> Terrain::UnloadAsync() {
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



  int Terrain::Update(float timeTotal, float timeDelta) {
    (void) timeDelta; // Unused parameter.

    return S_OK;
  }
  

  int Terrain::Render() {
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