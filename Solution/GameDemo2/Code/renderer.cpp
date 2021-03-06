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
#include "scenes/loading_scene.h"

namespace demo {



static LoadingScene* loading_scene;

void Renderer::Init(HWND handle_) {
  gfx = new ve::ContextD3D11();
  gfx->Initialize();
  gfx->CreateDisplay(handle_);

  gfx->shader_manager().set_resource_path(ve::GetExePath()+"\\");
   

  loading_scene = new LoadingScene();
  loading_scene->Initialize(gfx);
  loading_scene->Load();
  current_scene = loading_scene;
  current_scene->Set();

return;

	auto new_scene = new WorldScene();
  new_scene->Initialize(gfx);
  /*new_scene->LoadAsync().then([this,new_scene](int result){
    current_scene = new_scene;
    current_scene->Set();
  });*/
  new_scene->Load();
  //current_scene->Unset();
  current_scene = new_scene;
  current_scene->Set();
  

}

void Renderer::Deinit() {
  current_scene->Unset();
  current_scene->Unload();
  current_scene->Deinitialize();
  //loading_scene->Unload();
  //loading_scene->Deinitialize();
  gfx->Deinitialize();
  SafeDelete(&current_scene);
  SafeDelete(&gfx);
}

void Renderer::Update(float timeTotal, float timeDelta) {
  gfx->action_manager().Update(timeDelta);
  current_scene->Update(timeTotal,timeDelta);
}

void Renderer::Render() {
  gfx->ClearTarget();
  gfx->SetDefaultTargets();
  current_scene->Render();
  gfx->Render();
}

void Renderer::OnWindowSizeChange(uint32_t width, uint32_t height) {
  gfx->OnWindowSizeChange(width,height);
  current_scene->OnWindowSizeChange();
}

}