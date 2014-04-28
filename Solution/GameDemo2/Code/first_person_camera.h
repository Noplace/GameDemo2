#pragma once

namespace demo {

class FirstPersonCamera : public ve::PrespectiveCamera {
 public:

  dx::XMVECTOR DefaultForward = dx::XMVectorSet(0.0f,0.0f,-1.0f, 0.0f);
  dx::XMVECTOR DefaultRight = dx::XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
  dx::XMVECTOR DefaultUp = dx::XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
  dx::XMVECTOR camForward;
  dx::XMVECTOR camRight = dx::XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
  dx::XMVECTOR camUp,camTarget,camPosition ;
  dx::XMMATRIX camRotationMatrix;
  dx::XMMATRIX groundWorld;

  float moveLeftRight = 0.0f;
  float moveBackForward = 0.0f;
  float moveBottomTop = 0.0f;
  float camYaw = 0.0f;
  float camPitch = 0.0f;

  FirstPersonCamera() : ve::PrespectiveCamera() {
    camUp =  dx::XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
   
    view_ = dx::XMMatrixIdentity();
  }


  void Update(float timeDelta) {
     
    //testing code
    {
      float speed = 6.0f * timeDelta;
        if (GetAsyncKeyState('W'))
          moveBackForward += speed;
        if (GetAsyncKeyState('S'))
          moveBackForward -= speed;

        if (GetAsyncKeyState('A'))
          moveLeftRight -= speed;
        if (GetAsyncKeyState('D'))
          moveLeftRight += speed;

        if (GetAsyncKeyState(VK_UP))
          camPitch += speed;
        if (GetAsyncKeyState(VK_DOWN))
          camPitch -= speed;

        if (GetAsyncKeyState(VK_LEFT))
          camYaw += speed;
        if (GetAsyncKeyState(VK_RIGHT))
          camYaw -= speed;
        /*GetCursorPos(&mouse_current);
        if ((mouse_current.x != mouse_last.x) || (mouse_current.y != mouse_last.y)) {
          camera_.camYaw  += -(mouse_current.x- mouse_last.x)   * speed * 0.01f;
          camera_.camPitch += (mouse_current.y- mouse_last.y)  * speed * 0.01f;
          mouse_last = mouse_current;
        }*/
    }

    camRotationMatrix = dx::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	  camTarget = dx::XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	  camTarget = dx::XMVector3Normalize(camTarget);

	  /*dx::XMMATRIX RotateYTempMatrix;
	  RotateYTempMatrix = dx::XMMatrixRotationY(camPitch);
    dx::XMMATRIX RotateXTempMatrix;
	  RotateYTempMatrix = dx::XMMatrixRotationY(camPitch);*/

	  camRight = dx::XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	  camUp = dx::XMVector3TransformCoord(DefaultUp, camRotationMatrix);
	  camForward = dx::XMVector3TransformCoord(DefaultForward, camRotationMatrix);

    camPosition = dx::XMVectorAdd(camPosition,dx::XMVectorMultiply(dx::XMVectorSet(moveBottomTop,moveBottomTop,moveBottomTop,moveBottomTop),camUp));
	  camPosition = dx::XMVectorAdd(camPosition,dx::XMVectorMultiply(dx::XMVectorSet(moveLeftRight,moveLeftRight,moveLeftRight,moveLeftRight),camRight));
	  camPosition = dx::XMVectorAdd(camPosition,dx::XMVectorMultiply(dx::XMVectorSet(moveBackForward,moveBackForward,moveBackForward,moveBackForward),camForward));

  	moveLeftRight = 0.0f;
  	moveBackForward = 0.0f;
    moveBottomTop  = 0.0f;
	  camTarget = dx::XMVectorAdd(camPosition,camTarget);	

	  
    BuildViewMatrix(camPosition,camTarget,camUp);
  }

};

}