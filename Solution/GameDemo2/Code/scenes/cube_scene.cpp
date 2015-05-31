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
#include "../global.h"



namespace demo {



int CubeScene::Initialize(ve::Context* context) {
  int hr = Scene::Initialize(context);
  gfx = (ve::ContextD3D11*)context;
  
  camera_.Initialize(gfx);
    
  float aspectRatio = float(gfx->width()) / float(gfx->height());
	float fovAngleY = 70.0f * dx::XM_PI / 180.0f;
  camera_.BuildProjectionMatrix(fovAngleY,aspectRatio,0.01f,100.0f);
  dx::XMStoreFloat4x4(&vs_cb0_data.projection,camera_.projection_transposed());
  camera_.camPosition = dx::XMVectorSet(0,0,9,0);
  

  terrain_ = new Terrain();
  terrain_->set_terrain_quad_size(0.5f);
  terrain_->set_terrain_depth(100);
  terrain_->set_terrain_width(100);
  terrain_->Initialize(context);
  sky_ = new Sky();
  sky_->Initialize(context);

  AddRenderObject(sky_);
  AddRenderObject(terrain_);

    
  return hr;
}

int CubeScene::Deinitialize() {
  sky_->Deinitialize();
  terrain_->Deinitialize();
  Scene::Deinitialize();
  return S_OK;
}

 

concurrency::task<int> CubeScene::LoadAsync() {

    

  //static auto filename1 = (ve::GetExePath() + "\\vs.cso");
  //auto loadVSTask = ve::ReadDataAsync(filename1.c_str());
  static auto filename2 = (ve::GetExePath() + "\\ps.cso");
  auto loadPSTask = ve::ReadDataAsync(filename2.c_str());
/*
  static auto filename3 = (ve::GetExePath() + "\\test_hull.cso");
  auto loadHSTask = ve::ReadDataAsync(filename3.c_str());
  static auto filename4 = (ve::GetExePath() + "\\test_domain.cso");
  auto loadDSTask = ve::ReadDataAsync(filename4.c_str());
  static auto filename5 = (ve::GetExePath() + "\\test_geom.cso");
  auto loadGSTask = ve::ReadDataAsync(filename5.c_str());

  auto createHSTask = loadHSTask.then([this](ve::FileData fd){
    ID3D11HullShader* ptr;
    gfx->device()->CreateHullShader(fd.data,fd.length,0,&ptr);
    hs_.set_internal_pointer(ptr);
  });

  auto createDSTask = loadDSTask.then([this](ve::FileData fd){
    ID3D11DomainShader* ptr;
    gfx->device()->CreateDomainShader(fd.data,fd.length,0,&ptr);
    ds_.set_internal_pointer(ptr);
  });

  auto createGSTask = loadDSTask.then([this](ve::FileData fd){
    ID3D11GeometryShader* ptr;
    gfx->device()->CreateGeometryShader(fd.data,fd.length,0,&ptr);
    gs_.set_internal_pointer(ptr);
  });*/

   

  auto sm_result = context_->shader_manager().RequestVertexShader("vs.cso",ve::VertexPositionColorElementDesc,ARRAYSIZE(ve::VertexPositionColorElementDesc));
  vs_ = sm_result.vs;
  input_layout_ = sm_result.il;


	/*auto createVSTask = loadVSTask.then([this](ve::FileData fd){
    if (fd.data != nullptr) {
      gfx->CreateVertexShader(fd.data,fd.length,vs_);
    

     

      gfx->CreateInputLayout(ve::VertexPositionColorElementDesc,ARRAYSIZE(ve::VertexPositionColorElementDesc),fd,input_layout_);

		  /*ThrowIfFailed(
			  gfx->device()->CreateInputLayout(
				  vertexDesc,
				  ARRAYSIZE(vertexDesc),
				  fd.data,
				  fd.length,
				  &m_inputLayout
				  )
			  );*/
      /* SafeDeleteArray(&fd.data);
    }
  });*/

 

	auto createPSTask = loadPSTask.then([this](ve::FileData fd){
    if (fd.data != nullptr) {
      gfx->CreatePixelShader(fd.data,fd.length,ps_);
		  CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ve::ModelViewProjectionConstantBuffer),D3D11_BIND_CONSTANT_BUFFER);
      constantBufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE;
      constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		  ThrowIfFailed(gfx->device()->CreateBuffer(&constantBufferDesc,nullptr,&vs_cb_list[0]));


		  CD3D11_BUFFER_DESC constantBufferDesc2(sizeof(TimeConstantBuffer),D3D11_BIND_CONSTANT_BUFFER);
		  ThrowIfFailed(gfx->device()->CreateBuffer(&constantBufferDesc2,nullptr,&vs_cb_list[1]));
      ThrowIfFailed(gfx->device()->CreateBuffer(&constantBufferDesc2,nullptr,&ps_cb));

		  CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(ve::ModelViewProjectionConstantBuffer),D3D11_BIND_CONSTANT_BUFFER);
      constantBufferDesc3.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE;
      constantBufferDesc3.Usage = D3D11_USAGE_DYNAMIC;
		  ThrowIfFailed(gfx->device()->CreateBuffer(&constantBufferDesc3,nullptr,&vs_cb_list[2]));


		  



      SafeDeleteArray(&fd.data);
    }
  });



/*
  auto createCubeTask = (createPSTask ).then([this] () {
		ve::VertexPositionColor cubeVertices[] = 
		{
			{dx::XMFLOAT3(-0.5f, -0.5f, -0.5f), dx::XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{dx::XMFLOAT3(-0.5f, -0.5f,  0.5f), dx::XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{dx::XMFLOAT3(-0.5f,  0.5f, -0.5f), dx::XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{dx::XMFLOAT3(-0.5f,  0.5f,  0.5f), dx::XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{dx::XMFLOAT3( 0.5f, -0.5f, -0.5f), dx::XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{dx::XMFLOAT3( 0.5f, -0.5f,  0.5f), dx::XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{dx::XMFLOAT3( 0.5f,  0.5f, -0.5f), dx::XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{dx::XMFLOAT3( 0.5f,  0.5f,  0.5f), dx::XMFLOAT3(1.0f, 1.0f, 1.0f)},
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
	});*/

	return createPSTask.then([this] () {

    terrain_->LoadAsync().get();
    sky_->LoadAsync().get();

    sky_->world() = dx::XMMatrixTranspose(dx::XMMatrixTranspose(dx::XMMatrixTranslation(0,20,0)));

    CD3D11_DEFAULT d;
    CD3D11_RASTERIZER_DESC rDesc(d);

    // cd3d is the ID3D11DeviceContext  
    //gfx->device_context()->RSGetState( &rState ) ; // retrieve the current state
    //rState->GetDesc( &rDesc ) ;    // get the desc of the state
    //rDesc.FillMode = D3D11_FILL_WIREFRAME ; // change the ONE setting
    //rDesc.CullMode = D3D11_CULL_BACK;
    // create a whole new rasterizer state
    // d3d is the ID3D11Device
      gfx->device()->CreateRasterizerState( &rDesc, &scene_rasterizer_state_ ) ;
      
		m_loadingComplete = true;
      
    auto action1 =  new ve::CustomTimeBoundAction([this](float dt){
      static float timeTotal = 0;
      sky_->world() = dx::XMMatrixTranspose(dx::XMMatrixRotationY(timeTotal * dx::XM_PIDIV4));
      timeTotal += dt;
    },3);

    auto action2 = new ve::DelayAction(5);

    auto action3 =  new ve::CustomTimeBoundAction([this](float dt){
      static float timeTotal = 0;
      terrain_->world() = dx::XMMatrixTranspose(dx::XMMatrixRotationY(timeTotal * dx::XM_PIDIV4));
      timeTotal += dt;
    },10);

    //gfx->action_manager().Run(new ve::ParallelAction({ action1 , action3}));

    //std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    return (int)S_OK;
	});
}

concurrency::task<int> CubeScene::UnloadAsync() {
  return concurrency::create_task([this](){
    SafeRelease(&scene_rasterizer_state_);
    sky_->UnloadAsync().get();
    terrain_->UnloadAsync().get();
    gfx->DestoryInputLayout(input_layout_);
    SafeRelease(&vs_cb_list[2]);
    SafeRelease(&vs_cb_list[1]);
    SafeRelease(&vs_cb_list[0]);
    SafeRelease(&ps_cb);
    //SafeRelease(&m_vertexBuffer);
    //SafeRelease(&m_indexBuffer);
    gfx->DestroyShader(ps_);
    gfx->DestroyShader(vs_);
    //gfx->DestroyShader(gs_);
    //gfx->DestroyShader(ds_);
    //gfx->DestroyShader(hs_);
    return (int)S_OK;
  });
}

int CubeScene::Set() {
  gfx->SetShader(vs_); //default vertex shader
  gfx->PushRasterizerState(scene_rasterizer_state_);
  gfx->PushPixelShader(&ps_);
  return S_OK;
}

int CubeScene::Unset() {
  gfx->PopPixelShader();
  gfx->PopRasterizerState();
  return S_OK;
}

POINT mouse_current,mouse_last;
int CubeScene::Update(float timeTotal, float timeDelta) {

   
  camera_.Update(timeDelta);
	dx::XMVECTOR eye = dx::XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
	dx::XMVECTOR at = dx::XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
	dx::XMVECTOR up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  //camera_.BuildViewMatrix(eye,at,up);


  //camera_.RotateCamera(timeTotal * dx::XM_PIDIV4,0,1.0f,0);
  vs_cb1_data.deltaTime = timeDelta;
  vs_cb1_data.totalTime = timeTotal;
	dx::XMStoreFloat4x4(&vs_cb0_data.view, camera_.view_transposed());
  auto viewInv = dx::XMMatrixTranspose(dx::XMMatrixInverse(nullptr,camera_.view()));

  dx::XMStoreFloat4x4(&vs_cb0_data.viewInv, viewInv);
  dx::XMStoreFloat4x4(&vs_cb2_data.viewInv, viewInv);

	dx::XMStoreFloat4x4(&vs_cb0_data.model, dx::XMMatrixTranspose(dx::XMMatrixRotationY(timeTotal * dx::XM_PIDIV4)));
  dx::XMStoreFloat4x4(&vs_cb0_data.model, dx::XMMatrixTranspose(dx::XMMatrixIdentity()));

  gfx->CopyBufferFast(vs_cb_list[0],&vs_cb0_data,sizeof(vs_cb0_data));
  //gfx->device_context()->UpdateSubresource1(vs_cb_list[0],0,NULL,&vs_cb0_data,0,0,D3D11_COPY_DISCARD);
  gfx->device_context()->UpdateSubresource(vs_cb_list[1],0,NULL,&vs_cb1_data,0,0);

  gfx->CopyBufferFast(vs_cb_list[2],&vs_cb2_data,sizeof(vs_cb2_data));

  
   
  for (auto i : render_list_) {
    i->Update(timeTotal,timeDelta);
  }

  

  return S_OK;
}
  


int CubeScene::Render() {
	if (!m_loadingComplete)
	{
		return S_FALSE;
	}
  /*
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	gfx->device_context()->IASetVertexBuffers(0,1,&m_vertexBuffer,&stride,&offset);
	gfx->device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  gfx->device_context()->IASetIndexBuffer(m_indexBuffer,DXGI_FORMAT_R16_UINT,0);
	  
	gfx->device_context()->DrawIndexed(m_indexCount,0,0);*/

    
  for (auto i : render_list_) {
    UpdateWorldMatrix(dx::XMMatrixTranspose(i->world()));
    //dx::XMStoreFloat4x4(&m_constantBufferData.model, dx::XMMatrixTranspose(i->world()));
    //gfx->device_context()->UpdateSubresource(m_constantBuffer,0,NULL,&m_constantBufferData,0,0);
  //gfx->SetConstantBuffers()
    gfx->device_context()->VSSetConstantBuffers(0,3,vs_cb_list);
    //gfx->device_context()->VSSetConstantBuffers(1,1,&vs_cb1);
    gfx->device_context()->PSSetConstantBuffers(0,1,&vs_cb_list[1]); 
    i->Render();
  }
  
    
  return S_OK;
}

int CubeScene::UpdateWorldMatrix(const dx::XMMATRIX& world) {
  dx::XMStoreFloat4x4(&vs_cb0_data.model,world);
  //gfx->device_context()->UpdateSubresource(vs_cb_list[0],0,NULL,&vs_cb0_data,0,0);
  gfx->CopyBufferFast(vs_cb_list[0],&vs_cb0_data,sizeof(vs_cb0_data));
  return S_OK;
}


}