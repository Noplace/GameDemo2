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
#include <VisualEssence/Code/util/WICTextureLoader.h>

namespace demo {



class SpriteBatch : public ve::RenderObject {
  std::vector<ve::Sprite*> render_list_;
  int max_sprite_count;
  ID3D11Buffer* vb;
  int Initialize(ve::Context* context) {
    max_sprite_count = 1000;
    ve::RenderObject::Initialize(context);
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = nullptr;//verticies;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(ve::VertexPositionColorTexture)*4*max_sprite_count, D3D11_BIND_VERTEX_BUFFER);
		context->CreateBuffer(&vertexBufferDesc,nullptr,(void**)&vb);
    world_ = dx::XMMatrixIdentity();
    dirty_ = 0xffffffff;
    UpdateTransform();
  }

  int Render() {
	  UINT stride = sizeof(ve::VertexPositionColorTexture);
	  UINT offset = 0;
    context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context_->SetVertexBuffers(0,1,(const void**)&vb,&stride,&offset);
    context_->Draw(4,0);
    return S_OK;
  }
};



__declspec(align(16))
struct LoadingScenePSCB
{
	float totalTime;
  float deltaTime;
  float opacity;
};


class LoadingScene : public ve::Scene {

 public:
  ve::ContextD3D11* gfx;
  ve::Sprite sprite_;
  float aspectRatio;
  //required for the dx::XMMatrix in the camera
  void *operator new( size_t stAllocateBlock) {
    return _aligned_malloc(sizeof(LoadingScene),16);
  }

  void   operator delete (void* p)  {
    return _aligned_free(p);
  }

  int Initialize(ve::Context* context) {
    int hr = Scene::Initialize(context);
    gfx = (ve::ContextD3D11*)context;
    camera_.Initialize(gfx);
    
    aspectRatio = float(gfx->width()) / float(gfx->height());
    camera_.BuildProjectionMatrix(aspectRatio);
  	dx::XMStoreFloat4x4(&m_constantBufferData.projection,camera_.projection_transposed());
    _1st_person_cam1.Initialize(context);
    

    sprite_.Initialize(context);
    sprite_.set_opacity(0.5);
    AddRenderObject(&sprite_);
    return hr;
  }

  int Deinitialize() {
    Scene::Deinitialize();
    sprite_.Deinitialize();
    return S_OK;
  }

  concurrency::task<int> LoadAsync() {

   
    //static auto filename1 = (ve::GetExePath() + "\\vs_tex.cso");
    //auto loadVSTask = ve::ReadDataAsync(filename1.c_str());

    static auto filename2 = (ve::GetExePath() + "\\ps_sky.cso");
    auto loadPSTask = ve::ReadDataAsync(filename2.c_str());

    auto vs_result = context_->shader_manager().RequestVertexShader("vs_tex.cso",ve::VertexPositionColorTextureElementDesc,ARRAYSIZE(ve::VertexPositionColorTextureElementDesc));
    vs_ = vs_result.vs;
    m_inputLayout = (ID3D11InputLayout*)vs_result.il.pointer();

	  /*auto createVSTask = loadVSTask.then([this](ve::FileData fd){
      if (fd.data != nullptr) {
        gfx->CreateVertexShader(fd.data,fd.length,vs_);
      
    


		    ThrowIfFailed(
			    gfx->device()->CreateInputLayout(
				    ve::VertexPositionColorTextureElementDesc,
				    ARRAYSIZE(ve::VertexPositionColorTextureElementDesc),
				    fd.data,
				    fd.length,
				    &m_inputLayout
				    )
			    );
          SafeDeleteArray(&fd.data);
      }
    });*/

 

	  auto createPSTask = loadPSTask.then([this](ve::FileData fd){
      if (fd.data != nullptr) {
        gfx->CreatePixelShader(fd.data,fd.length,ps_);
      

		    CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(LoadingScenePSCB), D3D11_BIND_CONSTANT_BUFFER);
		    ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc1,nullptr, (void**)&ps_cb0));
        CD3D11_BUFFER_DESC constantBufferDesc2(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    	  ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc2,nullptr, (void**)&m_constantBuffer ));

		    CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(SkyPSShaderCB1), D3D11_BIND_CONSTANT_BUFFER);
		    ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc3,nullptr, (void**)&ps_cb1));

        SafeDeleteArray(&fd.data);
      }
    });




    auto createCubeTask = (createPSTask ).then([this] () {
      
      //gfx->CreateTexture(256,256,DXGI_FORMAT_R8G8B8A8_UNORM,0,texture_);
      //gfx->CreateResourceView(texture_,trv);
      //DirectX::CreateWICTextureFromFile(gfx->device(),L"D:\\Personal\\Projects\\GameDemo2\\Solution\\GameDemo2\\Resources\\cog.png",(ID3D11Resource**)&texture_.data_pointer,(ID3D11ShaderResourceView**)&trv.data_pointer);
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory( &sampDesc, sizeof(sampDesc) );
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        int hr = gfx->device()->CreateSamplerState( &sampDesc, &sampler_state );

        //gfx->device_context()->PSSetSamplers( 0, 1, &sampler_state );

		  ve::VertexPositionColorTexture cubeVertices[] = 
		  {
			  {dx::XMFLOAT3( 0.0f,  0.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(0,0)},
			  {dx::XMFLOAT3( aspectRatio,  0.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(1,0)},
			  {dx::XMFLOAT3( 0.0f,  1.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(0,1)},
			  {dx::XMFLOAT3( aspectRatio,  1.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(1,1)},

		  };

		  D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		  vertexBufferData.pSysMem = cubeVertices;
		  vertexBufferData.SysMemPitch = 0;
		  vertexBufferData.SysMemSlicePitch = 0;
		  CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		  ThrowIfFailed(
			  gfx->device()->CreateBuffer(
				  &vertexBufferDesc,
				  &vertexBufferData,
				  &m_vertexBuffer
				  )
			  );

		  unsigned short cubeIndices[] = 
		  {
			  0,1,2, // -x
			  1,2,3,

		
		  };

		  m_indexCount = ARRAYSIZE(cubeIndices);

		  D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		  indexBufferData.pSysMem = cubeIndices;
		  indexBufferData.SysMemPitch = 0;
		  indexBufferData.SysMemSlicePitch = 0;
		  CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		  ThrowIfFailed(
			  gfx->device()->CreateBuffer(
				  &indexBufferDesc,
				  &indexBufferData,
				  &m_indexBuffer
				  )
			  );
	  });

	  return createCubeTask.then([this] () {
		  m_loadingComplete = true;
      input_layout_.set_pointer(m_inputLayout);
      dx::XMStoreFloat4x4(&m_constantBufferData.model, dx::XMMatrixTranspose(dx::XMMatrixIdentity()));
      return (int)S_OK;
	  });
  }

  concurrency::task<int> UnloadAsync() {
    return concurrency::create_task([this](){
      gfx->DestoryInputLayout(input_layout_);
      gfx->DestroyTexture(texture_);
      gfx->DestroyResourceView(trv);
      SafeRelease(&sampler_state);

      SafeRelease(&m_constantBuffer);
      SafeRelease(&ps_cb1);
      SafeRelease(&ps_cb0);
      SafeRelease(&m_vertexBuffer);
      SafeRelease(&m_indexBuffer);
      gfx->DestroyShader(ps_);
      gfx->DestroyShader(vs_);
      return (int)S_OK;
    });
  }

  POINT mouse_last,mouse_current;
  int Set() {
    gfx->SetInputLayout(input_layout_);
    //gfx->device_context()->IASetInputLayout(m_inputLayout);
    gfx->SetShader(vs_);
    gfx->SetShader(ps_);
    GetCursorPos(&mouse_last);
    return S_OK;
  }

  int Unset() {

    return S_OK;
  }

  int Update(float timeTotal, float timeDelta) {
    (void) timeDelta; // Unused parameter.


    _1st_person_cam1.Update(timeDelta);
    camera_.BuildViewMatrix();
	  dx::XMStoreFloat4x4(&m_constantBufferData.view, camera_.view_transposed());
    ps_cb1_data.cameraLookAt = _1st_person_cam1.camTarget;
    ps_cb1_data.cameraPosition = _1st_person_cam1.camPosition;
    ps_cb1_data.cameraUp = _1st_person_cam1.camUp;
    ps_cb1_data.cameraForward = _1st_person_cam1.camForward;
    ps_cb1_data.cameraRight = _1st_person_cam1.camRight;
    ps_cb0_data.deltaTime = timeDelta;
    ps_cb0_data.totalTime = timeTotal;

	  /*dx::XMStoreFloat4x4(&m_constantBufferData.model, 
    dx::XMMatrixTranspose(dx::XMMatrixAffineTransformation2D(
dx::XMVectorSet(1,1,1,0),
dx::XMVectorSet(0.5f,0.5f,0,0),
timeTotal,
dx::XMVectorSet(0,0,0,0))));
*/
    //sprite_.set_position(sin(timeTotal),0);

 
    for (auto i=render_list_.begin(); i != render_list_.end(); ++i) {
      (*i)->Update(timeTotal,timeDelta);
    }

    return S_OK;
  }


  int Render() {
	  if (!m_loadingComplete)
	  {
		  return S_FALSE;
	  }
    ps_cb0_data.opacity = 1.0f;
	  gfx->device_context()->UpdateSubresource(m_constantBuffer,0,NULL,&m_constantBufferData,0,0);
    gfx->device_context()->UpdateSubresource(ps_cb0,0,NULL,&ps_cb0_data,0,0);
    gfx->device_context()->UpdateSubresource(ps_cb1,0,NULL,&ps_cb1_data,0,0);
	  UINT stride = sizeof(ve::VertexPositionColorTexture);
	  UINT offset = 0;
    gfx->device_context()->IASetIndexBuffer(m_indexBuffer,DXGI_FORMAT_R16_UINT,0);
	  gfx->device_context()->IASetVertexBuffers(0,1,&m_vertexBuffer,&stride,&offset);
    
	  gfx->device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	  gfx->device_context()->VSSetConstantBuffers(0,1,&m_constantBuffer);
    gfx->device_context()->PSSetConstantBuffers(0,1,&ps_cb0);
    gfx->device_context()->PSSetConstantBuffers(1,1,&ps_cb1);
    //gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,0,1,&trv.data_pointer);
	  //gfx->device_context()->DrawIndexed(m_indexCount,0,0);

    for (auto i=render_list_.begin(); i != render_list_.end(); ++i) {
      dx::XMStoreFloat4x4(&m_constantBufferData.model, dx::XMMatrixTranspose((*i)->world()));
      gfx->device_context()->UpdateSubresource(m_constantBuffer,0,NULL,&m_constantBufferData,0,0);
      ps_cb0_data.opacity = (*i)->opacity();
      gfx->device_context()->UpdateSubresource(ps_cb0,0,NULL,&ps_cb0_data,0,0);
      (*i)->Render();
    }

    return S_OK;
  }

 private:
	bool m_loadingComplete;
  ve::OrthoCamera camera_;
  ve::FirstPersonCamera _1st_person_cam1;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_constantBuffer,*ps_cb0,*ps_cb1;
  ID3D11SamplerState* sampler_state;
  ve::InputLayout input_layout_;
  ve::Texture texture_;
  ve::ResourceView trv;
	uint32_t m_indexCount;
  
	ModelViewProjectionConstantBuffer m_constantBufferData;
  LoadingScenePSCB ps_cb0_data;
  SkyPSShaderCB1 ps_cb1_data;

};

}