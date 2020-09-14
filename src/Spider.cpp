#include "Spider.h"

using namespace Greet;

Spider::Spider(const Vec3<float>& position)
  : position{position}, renderer{new DebugRenderer{}}
{
  for(int i = 0; i < 8; i++)
  {
    float xPos = (i / 4) - 0.5f;
    float zPos = (i % 4) / 3.0f - 0.5f;
    legs.push_back(SpiderLeg(position + Vec3<float>{xPos, 0, zPos}, position + Vec3<float>{xPos * 3, -1.0, zPos * 3}, position + Vec3<float>{xPos * 4, 1.0, zPos * 4}, renderer));
  }
}

Spider::Spider()
  : Spider{Vec3<float>{0.0f, 0.0f, 0.0f}}
{
}

void Spider::Render(const Cam& cam)
{
  renderer->Begin(cam);

  Mat4 transform = Mat4::Translate(position);
  for(auto&& leg : legs)
  {
    leg.Render();
    leg.RenderDebug();
  }
  renderer->Cube(position, {1, 0.4f, 1.0f}, Color{1.0f, 1.0f, 1.0f});
  renderer->Cube(position + Vec3<float>{0, 0.2f, 0.5f}, {0.4f, 0.4f, 0.4f}, Color{1.0f, 1.0f, 1.0f});
  renderer->Point(position + Vec3<float>{-0.1f, 0.3f, 0.7f}, 0.05f, Color{1.0f, 1.0f, 1.0f});
  renderer->Point(position + Vec3<float>{ 0.1f, 0.3f, 0.7f}, 0.05f, Color{1.0f, 1.0f, 1.0f});
  renderer->Line(Line{position + Vec3<float>{ 0.1f, 0.2f, 0.7f}, Vec3<float>{-0.05f, -0.05f, 0.1f}}, 0.05f, Color{1.0f, 1.0f, 1.0f});
  renderer->Line(Line{position + Vec3<float>{-0.1f, 0.2f, 0.7f}, Vec3<float>{0.05f, -0.05f, 0.1f}}, 0.05f, Color{1.0f, 1.0f, 1.0f});
  renderer->End();
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
