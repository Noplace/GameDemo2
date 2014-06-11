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
#include <VisualEssence/Code/util/DDSTextureLoader.h>

namespace demo {



int permutation[256];
// gradients for 3d noise
  const float gradients[16][3] =  
  {
    {1,1,0},
    {-1,1,0},
    {1,-1,0},
    {-1,-1,0},
    {1,0,1},
    {-1,0,1},
    {1,0,-1},
    {-1,0,-1}, 
    {0,1,1},
    {0,-1,1},
    {0,1,-1},
    {0,-1,-1},
    {1,1,0},
    {0,-1,1},
    {-1,1,0},
    {0,-1,-1}
  };

class PerlinNoiseHelper : ve::Component {
 public:

  

  ve::ResourceView rv_perm_tex;
  ve::ResourceView rv_grad_tex;

   void InitNoiseFunctions(int seed) {


      srand(seed);

      // Reset
      for (int i = 0; i < 256; i++)
      {
          permutation[i] = -1;
      }

      // Generate random numbers
      for (int i = 0; i < 256; i++)
      {
          while (true)
          {
              int iP = rand() % 256;
              if (permutation[iP] == -1)
              {
                  permutation[iP] = i;
                  break;
              }
          }
      }
  }

  int Initialize(ve::Context* context) {
    HRESULT hr;
    Component::Initialize(context);
    auto gfx = (ve::ContextD3D11*)context;
    ve::Texture tex;
    {
      CD3D11_TEXTURE1D_DESC desc((DXGI_FORMAT)DXGI_FORMAT_R8_UNORM,256,1,1,D3D11_BIND_SHADER_RESOURCE,D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE);
      UINT s;
      hr = gfx->device()->CheckFormatSupport(DXGI_FORMAT_R8_UNORM,&s);
      bool support = (D3D11_FORMAT_SUPPORT_TEXTURE1D & s) != 0;
      
      hr = gfx->device()->CreateTexture1D(&desc,nullptr,(ID3D11Texture1D**)&tex.data_pointer);
      
      ID3D11Texture1D* texptr = (ID3D11Texture1D*)tex.data_pointer;
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      gfx->device_context()->Map(texptr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
      BYTE* mappedData = reinterpret_cast<BYTE*>(mappedResource.pData);
      InitNoiseFunctions(time(nullptr));
      for(UINT i = 0; i < mappedResource.RowPitch; ++i)
      {
          mappedData[i] = permutation[i];
      }
      gfx->device_context()->Unmap(texptr, 0);
    
      //gfx->CreateTexture(256,256,DXGI_FORMAT_R8G8B8A8_UNORM,1,clouds_tex_);
      context_->CreateResourceView(tex,rv_perm_tex);
      context_->DestroyTexture(tex);
    }

    {
      CD3D11_TEXTURE1D_DESC desc2((DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_SNORM,16,1,1,D3D11_BIND_SHADER_RESOURCE,D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE);
      hr = gfx->device()->CreateTexture1D(&desc2,nullptr,(ID3D11Texture1D**)&tex.data_pointer);

      ID3D11Texture1D* texptr = (ID3D11Texture1D*)tex.data_pointer;
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      gfx->device_context()->Map(texptr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
      auto mappedData = reinterpret_cast<dx::PackedVector::XMBYTE4*>(mappedResource.pData);

      for(UINT i = 0; i < 16; ++i)
      {
          mappedData[i].x = ((gradients[i][0])*127);
          mappedData[i].y = ((gradients[i][1])*127);
          mappedData[i].z = ((gradients[i][2])*127);
          mappedData[i].w = 127;
      }
      gfx->device_context()->Unmap(texptr, 0);

      context_->CreateResourceView(tex,rv_grad_tex);
      context_->DestroyTexture(tex);
    }

    return hr;
  }

  int Deinitialize() {
    context_->DestroyResourceView(rv_perm_tex);
    context_->DestroyResourceView(rv_grad_tex);
    return S_OK;
  }
};

PerlinNoiseHelper perlin;

int Sky::Initialize(ve::Context* context) {
  int hr = RenderObject::Initialize(context);
  perlin.Initialize(context);
  return hr;
}

int Sky::Deinitialize() {
  perlin.Deinitialize();
  RenderObject::Deinitialize();
  return S_OK;
}

 ve::VertexPositionColorTexture* InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat) {
	float quadSize, radius, constant, textureDelta;
	int i, j, index;
	float positionX, positionY, positionZ, tu, tv;


	// Create the array to hold the sky plane coordinates.
	auto m_skyPlane = new ve::VertexPositionColorTexture[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	if(!m_skyPlane)
	{
		return nullptr;
	}

	// Determine the size of each quad on the sky plane.
	quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// Calculate the radius of the sky plane based on the width.
	radius = skyPlaneWidth / 2.0f;

	// Calculate the height constant to increment by.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// Calculate the texture coordinate increment value.
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	// Loop through the sky plane and build the coordinates based on the increment values given.
	for(j=0; j<=skyPlaneResolution; j++)
	{
		for(i=0; i<=skyPlaneResolution; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// Calculate the texture coordinates.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// Calculate the index into the sky plane array to add this coordinate.
			index = j * (skyPlaneResolution + 1) + i;

			// Add the coordinates to the sky plane array.
			m_skyPlane[index].pos = dx::XMFLOAT3(positionX,positionY,positionZ);
      m_skyPlane[index].color = dx::XMFLOAT3(1,1,1);
      m_skyPlane[index].uv = dx::XMFLOAT2(tu,tv);
		}
	}

	return m_skyPlane;
}

concurrency::task<int> Sky::LoadAsync() {
  return concurrency::create_task([this](){
    auto gfx = (ve::ContextD3D11*)context_;

    

    auto sky_vs_result = context_->shader_manager().RequestVertexShader("vs_sky.cso",ve::VertexPositionColorTextureElementDesc,ARRAYSIZE(ve::VertexPositionColorTextureElementDesc));
    sky_vs_ = sky_vs_result.vs;
    sky_il_ = sky_vs_result.il;
    auto sky_ps_result = context_->shader_manager().RequestPixelShader("ps_sky.cso");
    sky_ps_ = sky_ps_result.ps;

	  CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(SkyPSShaderCB1), D3D11_BIND_CONSTANT_BUFFER);
    ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc3,nullptr, (void**)&sky_ps_cb1));

   
    auto clouds_vs_result = context_->shader_manager().RequestVertexShader("vs_clouds.cso",ve::VertexPositionTexture3DElementDesc,ARRAYSIZE(ve::VertexPositionTexture3DElementDesc));
    clouds_vs_ = clouds_vs_result.vs;
    clouds_il_ = clouds_vs_result.il;
      
    auto clouds_ps_result = context_->shader_manager().RequestPixelShader("ps_clouds.cso");
    clouds_ps_ = clouds_ps_result.ps;
   	  
    
    //cloud data
    {

      //CD3D11_TEXTURE3D_DESC desc((DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_UNORM,256,256,256);
      //int hr = gfx->device()->CreateTexture3D(&desc,nullptr,(ID3D11Texture3D**)&clouds_tex_.data_pointer);
      //clouds_tex_.format = DXGI_FORMAT_R8G8B8A8_UNORM;
  
      //gfx->CreateTexture(256,256,DXGI_FORMAT_R8G8B8A8_UNORM,1,clouds_tex_);
      //gfx->CreateResourceView(clouds_tex_,clouds_trv_);

      //DirectX::CreateDDSTextureFromFile(gfx->device(),L"C:\\Users\\U11111\\Downloads\\flame.dds",(ID3D11Resource**)&clouds_tex_.data_pointer,(ID3D11ShaderResourceView**)&clouds_trv_.data_pointer);



		  ve::VertexPositionColor cubeVertices[] = 
		  {
			  {dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3(0.0f, 0.0f, 0.0f)},
			  {dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), dx::XMFLOAT3(0.0f, 0.0f, 1.0f)},
			  {dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), dx::XMFLOAT3(0.0f, 1.0f, 0.0f)},
			  {dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3(0.0f, 1.0f, 1.0f)},
			  {dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), dx::XMFLOAT3(1.0f, 0.0f, 0.0f)},
			  {dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), dx::XMFLOAT3(1.0f, 0.0f, 1.0f)},
			  {dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3(1.0f, 1.0f, 0.0f)},
			  {dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3(1.0f, 1.0f, 1.0f)},
		  };
      /*for (auto i = 0; i < 8; ++i) {
        cubeVertices[i].pos.x *= 2.0f; 
        cubeVertices[i].pos.y *= 2.0f; 
        cubeVertices[i].pos.z *= 2.0f; 
      }*/
      unsigned short cubeIndices[] = 
		  {
			  0,2,1, // -x
			  1,2,3,

			  4,5,6, // +x
			  5,7,6,

			  0,1,5, // -y
			  0,5,4,

			  2,6,7, // +y
			  2,7,3,

			  0,4,6, // -z
			  0,6,2,

			  1,3,7, // +z
			  1,7,5,
		  };


        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		    vertexBufferData.pSysMem = cubeVertices;
		    vertexBufferData.SysMemPitch = 0;
		    vertexBufferData.SysMemSlicePitch = 0;
		    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(ve::VertexPositionTexture3D)*8, D3D11_BIND_VERTEX_BUFFER);
		    ThrowIfFailed(gfx->device()->CreateBuffer(	 &vertexBufferDesc,		 &vertexBufferData,	  &clouds_vb_	 )    );
        D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		    indexBufferData.pSysMem = cubeIndices;
		    indexBufferData.SysMemPitch = 0;
		    indexBufferData.SysMemSlicePitch = 0;
		    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		    ThrowIfFailed(gfx->device()->CreateBuffer( &indexBufferDesc, &indexBufferData,	  &clouds_ib_		 )
			  );
    }


      auto skyPlaneResolution = 10;
      auto sourceVertices = InitializeSkyPlane(skyPlaneResolution,100.0f,5.5f,5.0f,4);
	    int m_vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;
	    sky_index_count = m_vertexCount;
      auto vertices = new ve::VertexPositionColorTexture[m_vertexCount];
      unsigned long index =0;
      auto indices = new unsigned long[sky_index_count];
      for(auto j=0; j<skyPlaneResolution; j++)
	      {
		      for(auto i=0; i<skyPlaneResolution; i++)
		      {
			      int index1 = j * (skyPlaneResolution + 1) + i;
			      int index2 = j * (skyPlaneResolution + 1) + (i+1);
			      int index3 = (j+1) * (skyPlaneResolution + 1) + i;
			      int index4 = (j+1) * (skyPlaneResolution + 1) + (i+1);
            auto color = dx::XMFLOAT3(0.5,0.5,1);
			      // Triangle 1 - Upper Left
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index1].pos.x, sourceVertices[index1].pos.y, sourceVertices[index1].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index1].uv.x, sourceVertices[index1].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;

			      // Triangle 1 - Upper Right
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index2].pos.x, sourceVertices[index2].pos.y, sourceVertices[index2].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index2].uv.x, sourceVertices[index2].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;

			      // Triangle 1 - Bottom Left
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index3].pos.x, sourceVertices[index3].pos.y, sourceVertices[index3].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index3].uv.x, sourceVertices[index3].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;

			      // Triangle 2 - Bottom Left
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index3].pos.x, sourceVertices[index3].pos.y, sourceVertices[index3].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index3].uv.x, sourceVertices[index3].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;

			      // Triangle 2 - Upper Right
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index2].pos.x, sourceVertices[index2].pos.y, sourceVertices[index2].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index2].uv.x, sourceVertices[index2].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;

			      // Triangle 2 - Bottom Right
			      vertices[index].pos = dx::XMFLOAT3(sourceVertices[index4].pos.x, sourceVertices[index4].pos.y, sourceVertices[index4].pos.z);
			      vertices[index].uv = dx::XMFLOAT2(sourceVertices[index4].uv.x, sourceVertices[index4].uv.y);
            vertices[index].color = color;
			      indices[index] = index;
			      index++;
		      }
	      }

      D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		  vertexBufferData.pSysMem = vertices;
		  vertexBufferData.SysMemPitch = 0;
		  vertexBufferData.SysMemSlicePitch = 0;
		  CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(ve::VertexPositionColorTexture)*m_vertexCount, D3D11_BIND_VERTEX_BUFFER);
		  ThrowIfFailed( gfx->device()->CreateBuffer(	 &vertexBufferDesc,		 &vertexBufferData,	  &sky_vb_	 )    );
      
      sky_index_count = index;
      D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		  indexBufferData.pSysMem = indices;
		  indexBufferData.SysMemPitch = 0;
		  indexBufferData.SysMemSlicePitch = 0;
		  CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned long)*sky_index_count, D3D11_BIND_INDEX_BUFFER);
		  ThrowIfFailed(
			  gfx->device()->CreateBuffer(
				  &indexBufferDesc,
				  &indexBufferData,
				  &sky_ib_
				  )
			  );

      SafeDeleteArray(&vertices);
      SafeDeleteArray(&indices);

      D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
      ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
      depthDisabledStencilDesc.DepthEnable = false;
	    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	    depthDisabledStencilDesc.StencilEnable = false;
	    /*depthDisabledStencilDesc.StencilReadMask = 0xFF;
	    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;*/
	    auto hr = gfx->device()->CreateDepthStencilState(&depthDisabledStencilDesc, &sky_depth_state_);

      D3D11_RASTERIZER_DESC rasterDesc;
	    rasterDesc.AntialiasedLineEnable = false;
	    rasterDesc.CullMode = D3D11_CULL_NONE;
	    rasterDesc.DepthBias = 0;
	    rasterDesc.DepthBiasClamp = 0.0f;
	    rasterDesc.DepthClipEnable = false;
	    rasterDesc.FillMode = D3D11_FILL_SOLID;
	    rasterDesc.FrontCounterClockwise = false;
	    rasterDesc.MultisampleEnable = true;
	    rasterDesc.ScissorEnable = false;
	    rasterDesc.SlopeScaledDepthBias = 0.0f;

	    // Create the no culling rasterizer state.
	    hr = gfx->device()->CreateRasterizerState(&rasterDesc, &sky_rasterizer_state_);
	   


      world_ = dx::XMMatrixIdentity();
      status = 1;
      return (int)S_OK;
    });
  }

  concurrency::task<int> Sky::UnloadAsync() {
    return concurrency::create_task([this](){
      

      context_->DestoryInputLayout(sky_il_);
      SafeRelease(&sky_vb_);
      SafeRelease(&sky_ib_);
      SafeRelease(&sky_depth_state_);
      SafeRelease(&sky_rasterizer_state_);
      context_->DestroyResourceView(clouds_trv_);
      context_->DestroyShader(sky_ps_);
      context_->DestroyShader(sky_vs_);


      context_->DestoryInputLayout(clouds_il_);
      SafeRelease(&clouds_vb_);
      SafeRelease(&clouds_ib_);
      context_->DestroyShader(clouds_ps_);
      context_->DestroyShader(clouds_vs_);

      return (int)S_OK;
    });
  }



  int Sky::Update(float timeTotal, float timeDelta) {
    (void) timeDelta; // Unused parameter.

    //

    return S_OK;
  }

  int Sky::Render() {
	  if (status != 1)
	  {
		  return S_FALSE;
	  }
    
    context_->SetInputLayout(sky_il_);
    context_->PushVertexShader(&sky_vs_);
    //context_->PushPixelShader(&sky_ps_);
	  UINT stride = sizeof(ve::VertexPositionColorTexture);
	  UINT offset = 0;
    auto gfx = (ve::ContextD3D11*)context_;
    context_->PushDepthState(sky_depth_state_);
    context_->PushRasterizerState(sky_rasterizer_state_);
    context_->SetVertexBuffers(0,1,(const void**)&sky_vb_,&stride,&offset);
    context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfx->device_context()->IASetIndexBuffer(sky_ib_,DXGI_FORMAT_R32_UINT,0);
	  gfx->device_context()->DrawIndexed(sky_index_count,0,0);
    context_->PopRasterizerState();
    context_->PopDepthState();
    //context_->PopPixelShader();
    context_->PopVertexShader();


    //scene_->UpdateWorldMatrix(dx::XMMatrixTranspose(dx::XMMatrixIdentity()));
    auto cloud_world = dx::XMMatrixScaling(10,1,10)*dx::XMMatrixTranslation(0,12,0);
    scene_->UpdateWorldMatrix(dx::XMMatrixTranspose(cloud_world));
    
    context_->SetInputLayout(clouds_il_);
    context_->PushVertexShader(&clouds_vs_);
    context_->PushPixelShader(&clouds_ps_);

    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,0,1,&clouds_trv_.data_pointer);
    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,1,1,&perlin.rv_perm_tex.data_pointer);
    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,2,1,&perlin.rv_grad_tex.data_pointer);
    context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    uint32_t strides[] = { sizeof(ve::VertexPositionTexture3D)};
    context_->SetVertexBuffers(0,1,(const void**)&clouds_vb_,strides,&offset);
    gfx->device_context()->IASetIndexBuffer(clouds_ib_,DXGI_FORMAT_R16_UINT,0);
    gfx->DrawIndexed(36,0,0);
    context_->PopVertexShader();
    context_->PopPixelShader();
  

    return S_OK;
  }

}