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

//int Terrain::SetParameters()

concurrency::task<int> Terrain::LoadAsync() {

  auto gfx = (ve::ContextD3D11*)context_;

  static auto filename1 = (ve::GetExePath() + "\\vs_terrain.cso");
  auto loadVSTask = ve::ReadDataAsync(filename1.c_str());
  //static auto filename2 = (ve::GetExePath() + "\\ps.cso");
  //auto loadPSTask = ve::ReadDataAsync(filename2.c_str());
	 


    
  auto pair = context_->shader_manager().RequestVertexShader("vs_terrain.cso",ve::VertexPositionColorElementDesc,ARRAYSIZE(ve::VertexPositionColorElementDesc));
  vs_ = pair.vs;
  input_layout_ = pair.il;

 
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


  auto createCubeTask = concurrency::create_task([this,gfx](){ 
    
      
    ve::VertexPositionColor* vertices = new ve::VertexPositionColor[(terrain_width_)*(terrain_depth_)];
    uint32_t* indices = new uint32_t[(terrain_depth_)*(terrain_width_)*6];
    m_indexCount = (terrain_depth_-1)*(terrain_width_-1)*6;
    auto iptr = indices;
   
    for (int y=0;y<terrain_depth_;++y)
      for (int x=0;x<terrain_width_;++x) {
        vertices[x+(y*terrain_width_)].pos = dx::XMFLOAT3((x-(terrain_width_*0.5f))*terrain_quad_size_, 0.0f, (y-(terrain_depth_*0.5f))*terrain_quad_size_);
        vertices[x+(y*terrain_width_)].color = dx::XMFLOAT3(1.0f, 1.0f, 1.0f);
      }

      int  count = 0;
      for (int y=0;y<terrain_depth_-1;++y)
        for (int x=0;x<terrain_width_-1;++x) {
            *iptr++ = (x+0)+((y+0)*(terrain_width_));
            *iptr++ = (x+1)+((y+0)*(terrain_width_));
            *iptr++ = (x+0)+((y+1)*(terrain_width_));
            *iptr++ = (x+0)+((y+1)*(terrain_width_));
            *iptr++ = (x+1)+((y+0)*(terrain_width_));
            *iptr++ = (x+1)+((y+1)*(terrain_width_));
            count+=6;
          }

    


		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(ve::VertexPositionColor)*terrain_depth_*terrain_width_, D3D11_BIND_VERTEX_BUFFER);
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

    CD3D11_DEFAULT d;
    CD3D11_RASTERIZER_DESC rDesc(d);

    rDesc.FillMode = D3D11_FILL_WIREFRAME ; // change the ONE setting
    //rDesc.CullMode = D3D11_CULL_BACK;

    gfx->device()->CreateRasterizerState( &rDesc, &terrain_rasterizer_state_ ) ;
      
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
  //context_->PushRasterizerState(terrain_rasterizer_state_);
  context_->SetInputLayout(input_layout_);
  context_->PushVertexShader(&vs_);

	UINT stride = sizeof(ve::VertexPositionColor);
	UINT offset = 0;
  auto gfx = (ve::ContextD3D11*)context_;
  context_->SetVertexBuffers(0,1,(const void**)&vb,&stride,&offset);
	context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  gfx->device_context()->IASetIndexBuffer(ib,DXGI_FORMAT_R32_UINT,0);
	context_->DrawIndexed(m_indexCount,0,0);
  context_->PopVertexShader();
  //context_->PopRasterizerState();
  return S_OK;
}

}