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

class ThirdPersonCamera : public ve::PrespectiveCamera {
 public:
  dx::XMVECTOR at_vector;
  dx::XMVECTOR pos_vector;
  const dx::XMVECTOR up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  ThirdPersonCamera() : ve::PrespectiveCamera() {
    at_vector = dx::XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
    pos_vector = dx::XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
    view_ = dx::XMMatrixIdentity();
  }
  void RotateCamera(float angle, float x, float y, float z) {
    auto temp = dx::XMQuaternionRotationNormal(dx::XMVectorSet(x,y,z,0),angle);

   
    auto temp2 = dx::XMQuaternionMultiply(dx::XMQuaternionConjugate(temp),dx::XMQuaternionMultiply(temp,at_vector));
    at_vector  = temp2;

/*
    auto axis = dx::XMVector3Cross(dx::XMVectorSubtract(at_vector,pos_vector), up);
    axis = dx::XMVector3Normalize(axis);

    view_ = dx::XMMatrixRotationAxis(axis,angle);*/
    BuildViewMatrix(pos_vector,at_vector,up);
  }

  void Update() {
    
  }


};

}