#pragma once

#include <Greet.h>

#include "Camera.h"
#include "DebugRenderer.h"
#include "math/Vec3.h"

struct SpiderLeg
{
  Greet::Ref<DebugRenderer> debugRenderer;

  std::vector<Greet::Vec3<float>> jointPositions;

  Greet::Vec3<float> footPos;
  Greet::Vec3<float> footPrevPos;
  Greet::Vec3<float> footTargetPos;
  Greet::Vec3<float> footRestPos;
  Greet::Vec3<float> attractingPos;

  Greet::Timer timer;


  float boneLength = 1.f;

  SpiderLeg(const Greet::Vec3<float>& jointPos, const Greet::Vec3<float>& footPos, const Greet::Vec3<float>& attractingPos, const Greet::Ref<DebugRenderer>& renderer)
    : debugRenderer{renderer}, footTargetPos{footPos}, footPos{footPos}, footPrevPos{footPos}
  {
    for(int i = 0; i < 4; i++)
    {
      jointPositions.push_back(footPos + Greet::Vec3<float>(0, boneLength, 0) * i);
    }
    RecalcInverseKinematic(jointPos, attractingPos);
  }

  void Update()
  {
    if(footTargetPos != footPos)
    {
      const float STEP_TIME = 0.2f;
      float elapsed = timer.Elapsed();
      float portionDone = elapsed / STEP_TIME;

      if(portionDone < 1.0f)
      {
        footPos.x = footPrevPos.x + (footTargetPos.x - footPrevPos.x) * portionDone;
        footPos.y = footPrevPos.y + sin(portionDone * M_PI) * 0.2;
        footPos.z = footPrevPos.z + (footTargetPos.z - footPrevPos.z) * portionDone;
      }
      else
        footPos = footTargetPos;
    }
  }

  void Render()
  {
    for(int i = 0; i < jointPositions.size() - 1; i++)
    {
      Greet::Vec3<float> dir = jointPositions[i + 1] - jointPositions[i];
      debugRenderer->Line(Greet::Line{jointPositions[i], dir}, 0.1, Greet::Color{1, 1, 1});
    }

    for(auto&& joint : jointPositions)
    {
      debugRenderer->Point(joint, 0.05, Greet::Color{1.0f, 1.0f, 1.0f});
    }
  }

  void RenderDebug()
  {
    Greet::Plane plane{};
    debugRenderer->Circle(footRestPos + Greet::Vec3<float>{0, 0.01, 0}, plane, 1.0, Greet::Color{1, 0, 0});
    debugRenderer->Point(attractingPos, 0.1, Greet::Color{0, 0, 1});
  }

  void RecalcInverseKinematic(const Greet::Vec3<float>& targetPos, const Greet::Vec3<float>& attractingPos)
  {
    Greet::Vec3<float> offset = footPos - jointPositions[0];
    for(int i = 0; i < jointPositions.size() - 1; i++)
    {
      jointPositions[i] = jointPositions[i] + offset;
    }

    jointPositions[jointPositions.size()-1] = targetPos;

    for(int i = 0; i < 10; i++)
    {
      for(int j = jointPositions.size() - 2; j > 0; j--)
      {
        jointPositions[j] = jointPositions[j + 1] + (jointPositions[j] - jointPositions[j + 1]).Normalize() * boneLength;
      }

      for(int j = 1; j < jointPositions.size(); j++)
      {
        jointPositions[j] = jointPositions[j - 1] + (jointPositions[j] - jointPositions[j - 1]).Normalize() * boneLength;
      }
    }

    for(int i = 1; i < jointPositions.size() - 1; i++)
    {
      Greet::Plane plane{jointPositions[i + 1] - jointPositions[i - 1], jointPositions[i - 1]};
      Greet::Vec3<float> projectAttraction = plane.ProjectPoint(attractingPos);
      Greet::Vec3<float> projectJoint = plane.ProjectPoint(jointPositions[i]);
      float angle = Greet::Vec::SignedAngle(projectJoint - jointPositions[i - 1], projectAttraction - jointPositions[i - 1], plane.normal);
      jointPositions[i] = Greet::Quaternion::AxisAngle(plane.normal, angle) * (jointPositions[i] - jointPositions[i - 1]) + jointPositions[i - 1];
    }
  }

  void BodyMoved(const Greet::Vec3<float>& jointPos, const Greet::Vec3<float>& footRestPos, const Greet::Vec3<float>& attractingPos)
  {
    this->attractingPos = attractingPos;
    Greet::Vec3<float> dir = footRestPos - this->footRestPos;
    this->footRestPos = footRestPos;
    if((footPos - footRestPos).LengthSQ() > 1.0 && footPos == footTargetPos)
    {
      footPrevPos = footPos;
      footTargetPos = footRestPos + Greet::Vec::Normalize(dir) * 0.99;
      timer.Reset();
    }
    RecalcInverseKinematic(jointPos, attractingPos);
  }
};

class Spider
{
  std::vector<SpiderLeg> legs;

  Greet::Ref<DebugRenderer> renderer;

  Greet::Vec3<float> position;

  public:
    Spider(const Greet::Vec3<float>& position);
    Spider();

    void Render(const Cam& cam);
    void Update(float timeElapsed);
    void UpdateJointPositions();

};
