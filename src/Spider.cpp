#include "Spider.h"

using namespace Greet;

Spider::Spider(const Vec3<float>& position)
  : position{position}
{
  body = Ref<Mesh>{new Mesh{MeshFactory::Cube(0, 0, 0, 1.0f, 1.0f, 1.0f)}};
  legPart = Ref<Mesh>{new Mesh{MeshFactory::Cube(0, 0, 0, 1.0f, 1.0f, 1.0f)}};
  shader = Shader::FromFile("res/shaders/shader3d.glsl");

  for(int i = 0; i < 8; i++)
  {
    float xPos = (i / 4) - 0.5f;
    float zPos = (i % 4) / 3.0f - 0.5f;
    legs.push_back(SpiderLeg(legPart, shader, position + Vec3<float>{xPos, 0, zPos}, position + Vec3<float>{xPos * 3, -1.0, zPos * 3}, position + Vec3<float>{xPos * 2, 1.0, zPos * 2}));
  }
}

Spider::Spider()
  : Spider{Vec3<float>{0.0f, 0.0f, 0.0f}}
{
}

void Spider::Render(const Cam& cam)
{
  shader->Enable();
  cam.BindShaderMatrices(shader);
  shader->SetUniformBoolean("uHasTexture", false);

  Mat4 transform = Mat4::Translate(position);
  for(auto&& leg : legs)
  {
    leg.Render();
    leg.RenderDebug();
  }
  shader->SetUniformMat4("uTransformationMatrix",  transform * Mat4::Scale({1, 0.4f, 1}));
  shader->SetUniform3f("uColor", Greet::Vec3<float>{1.0f, 1.0f, 1.0f});
  body->Bind();
  body->Render();
  body->Unbind();
  shader->Disable();
}

void Spider::Update(float timeElapsed)
{
  float dist = 4.0f * timeElapsed;
  if(Input::IsKeyDown(GREET_KEY_H))
    position.x += dist;
  if(Input::IsKeyDown(GREET_KEY_L))
    position.x -= dist;
  if(Input::IsKeyDown(GREET_KEY_K))
    position.z += dist;
  if(Input::IsKeyDown(GREET_KEY_J))
    position.z -= dist;
  UpdateJointPositions();
}

void Spider::UpdateJointPositions()
{
  for(int i = 0; i < 8; i++)
  {
    float xPos = (i / 4) - 0.5f;
    float zPos = (i % 4) / 3.0f - 0.5f;
    legs[i].SetTargetPos(position + Vec3<float>{xPos, 0, zPos}, position + Vec3<float>{xPos * 3, 1.0, zPos * 3});
  }
}
