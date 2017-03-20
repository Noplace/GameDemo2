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
#include <Solar/Code/util/WICTextureLoader.h>



namespace demo {


__declspec(align(16))
struct LoadingScenePSCB
{
	float totalTime;
  float deltaTime;
  float opacity;
};

struct SpriteFrame {
  float u0,v0,u1,v1;
};

struct SpriteAnimation {
  SpriteFrame* frames;
  uint16_t current;
  uint16_t count;
  uint16_t default;
};

struct GameObject {
  ve::SpriteBatchSprite* sprite;
  float x,y,z;
  std::vector<SpriteAnimation> animations;
  int current_animation_;
  float frame_time = 0.25f;
  float cur_time = 0;
  void Update(float dt) {
    auto animation = &animations[current_animation_];
    auto frame = &animation->frames[animation->current];
    sprite->set_uv(frame->u0,frame->v0,frame->u1,frame->v1);
    if (cur_time > frame_time) {
      animation->current = (animation->current + 1) % animation->count;
      cur_time = 0;
    }
    cur_time += dt;
  }
};


class FollowSpriteCamera : public ve::Camera {
 public:
  ve::SpriteBatchSprite* game_object_;
/*
  void BuildProjectionMatrix(float aspect_ratio) {
		projection_ = dx::XMMatrixOrthographicOffCenterRH(0.0f,aspect_ratio,1.0f,0.0f,0.0f,1.0f);
  }

  void BuildProjectionMatrix(float width, float height) {
		projection_ = dx::XMMatrixOrthographicOffCenterRH(0.0f,width,height,0.0f,0.0f,1.0f);
  }
*/

  void set_size(float width, float height) {
    width_ = width;
    height_ = height;
    constraint_tl = dx::XMFLOAT2(0,0);
  }


  int Update(float timeDelta) {
  //testing code
    {
    float speed = 100.0f * timeDelta;

      /*if (GetAsyncKeyState(VK_UP))
        cam_top += speed;
      if (GetAsyncKeyState(VK_DOWN))
        cam_top -= speed;

      if (GetAsyncKeyState(VK_LEFT))
        cam_left += speed;
      if (GetAsyncKeyState(VK_RIGHT))
        cam_left -= speed;*/
    }
    
    zoom_ = min(max(zoom_,0.1f),2.0f);
    float hw = width_/2;
    float hh = height_/2;
    float dx = game_object_->center().x/zoom_ - cam_left;
    float dy = game_object_->center().y/zoom_ - cam_top;
    float vel = 0.023f/zoom_;
    float x = cam_left + (dx*vel-dx*vel*vel);
    float y = cam_top +  (dy*vel-dy*vel*vel);
    

    if (enable_restraints == true) {
    //if ((x-hw)<constraint_tl.x) x = constraint_tl.x+hw;
    //if ((y-hh)<constraint_tl.y) y = constraint_tl.y+hh;
   // if ((x+hw)>constraint_br.x) x = constraint_br.x-hw;
   // if ((y+hh)>constraint_br.y) y = constraint_br.y-hh;
    }

    view_ = dx::XMMatrixIdentity();//dx::XMMatrixScaling(zoom_,zoom_,1.0f);//
    projection_ = dx::XMMatrixOrthographicOffCenterRH((x-hw)*zoom_,(x+hw)*zoom_,(y+hh)*zoom_,(y-hh)*zoom_,-100.0f,100.0f);
    cam_left = x;
    cam_top = y;
    return S_OK;
  }
  
  float zoom_=1.0f;
 protected:
    float width_,height_;
    float cam_left,cam_top; 
    dx::XMFLOAT2 constraint_tl;
    dx::XMFLOAT2 constraint_br;
    bool enable_restraints;
};

struct {
  void* target;
  ve::Texture tex;
  ve::ResourceView rv; 
  ve::VertexShader vs;
  ve::PixelShader ps;
  ve::InputLayout il;
  void* vb;
} rt;

class LoadingScene : public ve::Scene {

 public:
  ve::ContextD3D11* gfx;
  ve::Sprite sprite_;
  
  GameObject player1;

  float aspectRatio;
  //required for the dx::XMMatrix in the camera
  void *operator new( size_t stAllocateBlock) {
    return _aligned_malloc(sizeof(LoadingScene),16);
  }

  void   operator delete (void* p)  {
    return _aligned_free(p);
  }


  int OnInput(int msg, int data1) {
    

    return S_OK;
  }

  int Initialize(ve::Context* context) {
    int hr = Scene::Initialize(context);
    gfx = (ve::ContextD3D11*)context;
    camera1_.Initialize(gfx);
    
    camera2_.Initialize(gfx);
    camera2_.BuildProjectionMatrix(640,480);
    camera2_.BuildViewMatrix();

    camera1_node_.set_camera(&camera1_);
    camera1_node_.Initialize(context);
    
    camera2_node_.set_camera(&camera2_);
    camera2_node_.Initialize(context);


    //custom render target
    {
      gfx->CreateRenderTarget(640,480,&rt.target,&rt.tex,&rt.rv);

      ve::VertexPositionTexture vertices[] = 
		  {
			  {dx::XMFLOAT3( 0.0f,  0.0f, 0.0f), dx::XMFLOAT2(0,0)},
			  {dx::XMFLOAT3( 640,  0.0f, 0.0f),dx::XMFLOAT2(1,0)},
			  {dx::XMFLOAT3( 0.0f,  480, 0.0f),dx::XMFLOAT2(0,1)},
			  {dx::XMFLOAT3( 640,  480, 0.0f), dx::XMFLOAT2(1,1)},

		  };

      CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);


		  D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		  vertexBufferData.pSysMem = vertices;
		  vertexBufferData.SysMemPitch = 0;
		  vertexBufferData.SysMemSlicePitch = 0;
	    context_->CreateBuffer(&vertexBufferDesc,(void*)&vertexBufferData,(void**)&rt.vb);


      auto vs_result = context_->shader_manager().RequestVertexShader("rt_vs.cso",ve::VertexPositionTextureElementDesc,ARRAYSIZE(ve::VertexPositionTextureElementDesc));
      rt.vs = vs_result.vs;
      rt.il = vs_result.il;
      auto ps_result = context_->shader_manager().RequestPixelShader("rt_ps.cso");
      rt.ps = ps_result.ps;

    }   

    _1st_person_cam1.Initialize(context);
    

    
    sprite_.Initialize(context);
    sprite_.set_opacity(0.5);
    sprite_.set_size(640,480);
    //AddRenderObject(&sprite_);

    AddRenderObject(&camera1_node_);
    sprite_batch_.Initialize(context_);
    AddRenderObject(&sprite_batch_);
    sprite_batch_.set_auto_garbage_cleanup(true);
    srand(time(0));
    for (int i=0;i<10;++i)     {
      int x1 = rand() % 100;
      int y1 = rand() % 100;
      sprite_list[i] = new ve::SpriteBatchSprite();
      sprite_list[i]->Initialize(context);
      sprite_batch_.AddSprite(sprite_list[i]);
      sprite_list[i]->set_size(50.0f,50.0f);
      sprite_list[i]->set_rotation_origin(dx::XMFLOAT2(25.0f,25.0f));
      sprite_list[i]->set_position(x1,y1);
      sprite_list[i]->set_opacity(0.8f);
      sprite_list[i]->set_zorder(i);
  
    }
  
    sprite_list[0]->set_opacity(1.0f);
    sprite_list[0]->set_color(dx::XMFLOAT3(1,1,0));
    sprite_list[0]->set_zorder(0);
    sprite_list[0]->set_size(24.0f,50.0f);
    float w = (24.0f/256.0f);
    float h = (50.0f/256.0f);
    //sprite_list[0]->set_uv(w,h*0,w*2,h);
    SpriteAnimation animation = {0};
    animation.frames = new SpriteFrame[3];
    animation.count = 2;
    animation.frames[0] = {0,0,w,h};
    animation.frames[1] = {w*2,0,w*3,h};
    animation.default = 0;
    animation.current  = animation.default;

    //sprite_list[0]->set_rotation_origin(dx::XMFLOAT2(50.0f,50.0f));

    player1.sprite = sprite_list[0];
    player1.x = 0;
    player1.y = 0;
    player1.z = 0;
    player1.current_animation_ = 0;
    player1.cur_time = 0;
    player1.animations.push_back(animation);
    player1.Update(0);

    camera1_.game_object_ =  player1.sprite;
    
    

    AddRenderObject(&camera2_node_);
    acGraphics::FontLoaderBinaryFormat font_loader(L"D:\\Personal\\Projects\\GameDemo\\Content\\Fonts\\Arial\\16.fnt");
    font_loader.Initialize(context_);
    font_loader.LoadFont(&font_);
    font_loader.Deinitialize();
    font_writer_.Initialize(context_);
    font_writer_.set_font(&font_);
    font_writer_.PrepareWrite(512);
  
    AddRenderObject(&font_writer_);
    
    ve::EventManager em;
    em.AddEventListener(1,[] (void* source, void* param){
      int test;
      test = 1+2;
      return test;
    });

    em.TriggerEvent(1,(void*)this);
    return hr;
  }

  int Deinitialize() {
    
    camera1_node_.Deinitialize();
    camera2_node_.Deinitialize();
    font_writer_.Deinitialize();
    Scene::Deinitialize();
    sprite_.Deinitialize();
    return S_OK;
  }

   int OnWindowSizeChange() { 
    aspectRatio = float(gfx->width()) / float(gfx->height());
    camera2_.BuildProjectionMatrix(float(gfx->width()) , float(gfx->height()));
    camera2_.BuildViewMatrix();

    camera1_.set_size(float(gfx->width()) , float(gfx->height()));
    //UpdateCamera(0,0);
    for (auto i : render_list_)
      i->OnWindowSizeChange();
    return S_OK; 
   }

  concurrency::task<int> LoadAsync() {


    auto vs_result = context_->shader_manager().RequestVertexShader("vs_tex.cso",ve::VertexPositionColorTextureElementDesc,ARRAYSIZE(ve::VertexPositionColorTextureElementDesc));
    vs_ = vs_result.vs;
    input_layout_ = vs_result.il;

 

    auto createPSTask = context_->shader_manager().RequestPixelShaderAsync("ps_fractal.cso").then([this](ve::RequestPixelShaderResult result){
      if (result.hr == S_FALSE)
        throw;
      ps_ = result.ps;
		  CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(LoadingScenePSCB), D3D11_BIND_CONSTANT_BUFFER);
		  ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc1,nullptr, (void**)&ps_cb0));

		  CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(SkyPSShaderCB1), D3D11_BIND_CONSTANT_BUFFER);
		  ThrowIfFailed( gfx->CreateBuffer(&constantBufferDesc3,nullptr, (void**)&ps_cb1));
    });


    auto createCubeTask = (createPSTask ).then([this] () {
      
      

      //gfx->CreateTexture(256,256,DXGI_FORMAT_R8G8B8A8_UNORM,0,texture_);
      //gfx->CreateResourceView(texture_,trv);
      DirectX::CreateWICTextureFromFile(gfx->device(),L"D:\\Personal\\Projects\\GameDemo2\\Solution\\GameDemo2\\Resources\\Graphics\\Sprites\\PlayStation - Suikoden II - Alex.png",(ID3D11Resource**)&texture_.data_pointer,(ID3D11ShaderResourceView**)&trv.data_pointer);

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

        gfx->device_context()->PSSetSamplers( 0, 1, &sampler_state );

		  ve::VertexPositionColorTexture cubeVertices[] = 
		  {
			  {dx::XMFLOAT3( 0.0f,  0.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(0,0)},
			  {dx::XMFLOAT3( 640,  0.0f, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(1,0)},
			  {dx::XMFLOAT3( 0.0f,  480, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(0,1)},
			  {dx::XMFLOAT3( 640,  480, 0.0f), dx::XMFLOAT3(1, 1, 1),dx::XMFLOAT2(1,1)},

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
     
      return (int)S_OK;
	  });
  }

  concurrency::task<int> UnloadAsync() {
    return concurrency::create_task([this](){
      

      gfx->DestoryInputLayout(input_layout_);
      gfx->DestroyTexture(texture_);
      gfx->DestroyResourceView(trv);
      SafeRelease(&sampler_state);
      sprite_batch_.Deinitialize();

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
  ID3D11DepthStencilState* depth_state_;
  int Set() {

    //temp    
    CD3D11_DEFAULT d;
    CD3D11_RASTERIZER_DESC rDesc(d);
    rDesc.FillMode = D3D11_FILL_WIREFRAME ; // change the ONE setting
    ID3D11RasterizerState* rs_state;
    gfx->device()->CreateRasterizerState( &rDesc, &rs_state ) ;
    //gfx->device_context()->RSSetState(rs_state);
    


    auto gfx = (ve::ContextD3D11*)context_;
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
    depthDisabledStencilDesc.DepthEnable = false;
	  depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	  depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	  depthDisabledStencilDesc.StencilEnable = false;
	  auto hr = gfx->device()->CreateDepthStencilState(&depthDisabledStencilDesc, &depth_state_);
    

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


  /*int UpdateCamera(float timeTotal, float timeDelta) {
    camera_.Update(timeDelta);
	  dx::XMStoreFloat4x4(&m_constantBufferData.view, camera_.view_transposed());
    dx::XMStoreFloat4x4(&m_constantBufferData.projection, camera_.projection_transposed());
    return S_OK;
  }*/

  int Update(float timeTotal, float timeDelta) {

    float speed = 200.0f * timeDelta;
    

    if (GetAsyncKeyState('Z')) {
      camera1_.game_object_ =  sprite_list[rand()%5];
    }

      if (GetAsyncKeyState(VK_UP))
        player1.y -= speed;
      if (GetAsyncKeyState(VK_DOWN))
        player1.y += speed;

      if (GetAsyncKeyState(VK_LEFT))
        player1.x -= speed;
      if (GetAsyncKeyState(VK_RIGHT))
        player1.x += speed;

      if (GetAsyncKeyState('Q'))
        camera1_.zoom_ += timeDelta;
      if (GetAsyncKeyState('A'))
        camera1_.zoom_ -= timeDelta;
    
    player1.Update(timeDelta);
    sprite_list[0]->set_position(player1.x,player1.y);
    sprite_list[0]->set_zorder(player1.z);
    
    _1st_person_cam1.Update(timeDelta);
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
    
    char status1[225];
    sprintf_s(status1,"Test 123 Test Est let it go");

    font_writer_.WriteML(status1,strlen(status1),0);
    

    
    for (int i=0;i<10;++i) {
      //sprite_list[i]->set_color(dx::XMFLOAT3(1,0,1));
      float phase = (1+i*2)/100.0f;
      float a = sin((2*dx::XM_PI*timeTotal)+phase);
      //sprite_list[i]->set_opacity((a+1)/2);
      //sprite_list[i]->set_angle(a);
    }

    for (auto i=render_list_.begin(); i != render_list_.end(); ++i) {
      (*i)->Update(timeTotal,timeDelta);
    }


    //UpdateCamera(timeTotal,timeDelta);

    return S_OK;
  }


  int Render() {
	  if (!m_loadingComplete)
	  {
		  return S_FALSE;
	  }

    //gfx->SetRenderTargets(rt.target);


    ps_cb0_data.opacity = 1.0f;
    gfx->device_context()->UpdateSubresource(ps_cb0,0,NULL,&ps_cb0_data,0,0);
    gfx->device_context()->UpdateSubresource(ps_cb1,0,NULL,&ps_cb1_data,0,0);
    gfx->device_context()->PSSetConstantBuffers(0,1,&ps_cb0);
    gfx->device_context()->PSSetConstantBuffers(1,1,&ps_cb1);
    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,0,1,&trv.data_pointer);

    context_->SetInputLayout(input_layout_);
	  camera2_node_.SetWorldMatrix(dx::XMMatrixIdentity());
    camera2_node_.Render();//user camera2 node to set the vertex constant buffers
    gfx->PushVertexShader(&vs_);
    gfx->PushPixelShader(&ps_);
	  UINT stride = sizeof(ve::VertexPositionColorTexture);
	  UINT offset = 0;
    gfx->device_context()->IASetIndexBuffer(m_indexBuffer,DXGI_FORMAT_R16_UINT,0);
	  gfx->device_context()->IASetVertexBuffers(0,1,&m_vertexBuffer,&stride,&offset);
	  context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    gfx->device_context()->DrawIndexed(m_indexCount,0,0);
    //sprite_.Render();
    gfx->PopPixelShader();
    gfx->PopVertexShader();

    for (auto i=render_list_.begin(); i != render_list_.end(); ++i) {

      camera1_node_.SetWorldMatrix((*i)->world());
      camera2_node_.SetWorldMatrix((*i)->world());

      ps_cb0_data.opacity = (*i)->opacity();
      gfx->device_context()->UpdateSubresource(ps_cb0,0,NULL,&ps_cb0_data,0,0);
      (*i)->Render();
    }

/*
    context_->PushDepthState(depth_state_);
    context_->ClearTarget();
    
    camera2_node_.SetWorldMatrix(dx::XMMatrixIdentity());
    camera2_node_.Render();//user camera2 node to set the vertex constant buffers
    gfx->device_context()->IASetIndexBuffer(nullptr,DXGI_FORMAT_UNKNOWN,0);
    gfx->SetDefaultTargets();
    gfx->SetInputLayout(rt.il);
    gfx->PushVertexShader(&rt.vs);
    gfx->PushPixelShader(&rt.ps);
	  stride = sizeof(ve::VertexPositionTexture);
	  offset = 0;
    gfx->SetVertexBuffers(0,1,(const void**)&rt.vb,&stride,&offset);
    gfx->device_context()->PSSetConstantBuffers(0,1,&ps_cb0);
    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,0,1,&rt.rv.data_pointer);
    context_->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    gfx->Draw(4,0);
    gfx->PopPixelShader();
    gfx->PopVertexShader();
    void* nullt = nullptr;
    gfx->SetShaderResources(ve::ShaderType::kShaderTypePixel,0,1,&nullt);
    context_->PopDepthState();
*/

    return S_OK;
  }
  ve::Camera* camera() { return &camera1_; }
 private:
	bool m_loadingComplete;
  
  FollowSpriteCamera camera1_;
  ve::CameraNode camera1_node_;
  ve::OrthoCamera camera2_;
  ve::CameraNode camera2_node_;


  ve::FirstPersonCamera _1st_person_cam1;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer *ps_cb0,*ps_cb1;
  ID3D11SamplerState* sampler_state;
  ve::InputLayout input_layout_;
  ve::Texture texture_;
  ve::ResourceView trv;
	uint32_t m_indexCount;
  
	
  LoadingScenePSCB ps_cb0_data;
  SkyPSShaderCB1 ps_cb1_data;
  ve::Sprite background_sprite_;
  ve::SpriteBatch sprite_batch_;
  ve::SpriteBatchSprite* sprite_list[1000];
  ve::font::Writer font_writer_;
  acGraphics::Font font_;
};

}