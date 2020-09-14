#pragma once

#include <Greet.h>

class Cam
{
  private:
    Greet::Vec3<float> position;
    Greet::Vec3<float> rotation;
    Greet::Mat4 viewMatrix;
    Greet::Mat4 projectionMatrix;
    Greet::Mat4 invPVMatrix;

  public:
    Cam(const Greet::Mat4& projectionMatrix)
      : position{0}, rotation{0}, viewMatrix{Greet::Mat4::Identity()}, projectionMatrix{projectionMatrix}, invPVMatrix{Greet::Mat4::Identity()}
    {
      RecalcViewMatrix();
    }

    Cam()
      : Cam{Greet::Mat4{1}}
    {
      RecalcViewMatrix();
    }

    void BindShaderMatrices(const Greet::Ref<Greet::Shader>& shader) const
    {
      shader->SetUniformMat4("uViewMatrix", viewMatrix);
      shader->SetUniformMat4("uProjectionMatrix", projectionMatrix);
      shader->SetUniformMat4("uPVInvMatrix", invPVMatrix);
    }

    void SetProjectionMatrix(const Greet::Mat4& _projectionMatrix)
    {
      projectionMatrix = _projectionMatrix;
      RecalcViewMatrix();
    }

    const Greet::Mat4& GetInvPVMatrix() const
    {
      return invPVMatrix;
    }

    const Greet::Mat4& GetProjectionMatrix() const
    {
      return projectionMatrix;
    }

    const Greet::Mat4& GetViewMatrix() const
    {
      return viewMatrix;
    }

    void SetPosition(const Greet::Vec3<float>& _position)
    {
      position = _position;
      RecalcViewMatrix();
    }

    const Greet::Vec3<float>& GetPosition() const
    {
      return position;
    }

    void SetRotation(const Greet::Vec3<float>& _rotation)
    {
      rotation = _rotation;
      RecalcViewMatrix();
    }

    const Greet::Vec3<float>& GetRotation() const
    {
      return rotation;
    }

    inline void RecalcViewMatrix()
    {
      viewMatrix = Greet::Mat4::RotateX(-rotation.x) * Greet::Mat4::RotateY(-rotation.y) * Greet::Mat4::Translate(-position);
      RecalcInvPVMatrix();
    }

    inline void RecalcInvPVMatrix()
    {
      invPVMatrix = ~(projectionMatrix * viewMatrix);
    }

    Greet::Line GetScreenToWorldCoordinate(const Greet::Vec2& screenPos) const
    {
      Greet::Line line;
      line.pos = invPVMatrix * Greet::Vec3<float>(screenPos.x, screenPos.y, -1.0);
      Greet::Vec3<float> far = invPVMatrix * Greet::Vec3<float>(screenPos.x, screenPos.y, 1.0);
      line.dir = (far - line.pos).Normalize();
      return line;
    }
};

class CamController
{
  private:
    Cam& cam;

  public:
    CamController(Cam& cam)
      : cam{cam} {}

    void Update(float timeElapsed)
    {
      Greet::Vec3<float> rot = cam.GetRotation();
      Greet::Vec3<float> lastRot = rot;
      float rotationSpeed = 180 * timeElapsed;
      if (Greet::Input::IsKeyDown(GREET_KEY_UP))
        rot.x += rotationSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_DOWN))
        rot.x -= rotationSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_LEFT))
        rot.y += rotationSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_RIGHT))
        rot.y -= rotationSpeed;

      Greet::Vec2 posDelta{0};
      float zDelta = 0;
      float moveSpeed = 5 * timeElapsed;
      if (Greet::Input::IsKeyDown(GREET_KEY_W))
        posDelta.y -= moveSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_S))
        posDelta.y += moveSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_A))
        posDelta.x -= moveSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_D))
        posDelta.x += moveSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_LEFT_SHIFT))
        zDelta -= moveSpeed;
      if (Greet::Input::IsKeyDown(GREET_KEY_SPACE))
        zDelta += moveSpeed;

      posDelta.Rotate(-rot.y);

      if(posDelta != Greet::Vec2{0} || zDelta != 0)
        cam.SetPosition(cam.GetPosition() + Greet::Vec3<float>{posDelta.x, zDelta, posDelta.y});
      if(rot != lastRot)
        cam.SetRotation(rot);
    }
};
