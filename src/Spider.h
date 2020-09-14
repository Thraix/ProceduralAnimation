#pragma once

#include <Greet.h>

#include "Camera.h"
#include "DebugRenderer.h"
#include "math/Vec3.h"

struct SpiderLeg
{
  Greet::Ref<DebugRenderer> debugRenderer;

  std::vector<Greet::Vec3<float>> jointPositions;

  Greet::Vec3<float> footRestPos;
  Greet::Vec3<float> attractingPos;

  float boneLength = 0.75f;

  SpiderLeg(const Greet::Vec3<float>& jointPos, const Greet::Vec3<float>& footPos, const Greet::Vec3<float>& attractingPos, const Greet::Ref<DebugRenderer>& renderer)
    : footRestPos{footPos}, debugRenderer{renderer}
  {
    for(int i = 0; i < 4; i++)
    {
      jointPositions.push_back(footPos + Greet::Vec3<float>(0, boneLength, 0) * i);
    }
    RecalcInverseKinematic(jointPos, attractingPos);
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
      debugRenderer->Point(joint, 0.1, Greet::Color{1.0f, 1.0f, 1.0f});
    }
  }

  void RenderDebug()
  {
  }

  void RecalcInverseKinematic(const Greet::Vec3<float>& targetPos, const Greet::Vec3<float>& attractingPos)
  {
    this->attractingPos = attractingPos;
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
    //
  }

  void SetTargetPos(const Greet::Vec3<float>& targetPos, const Greet::Vec3<float>& attractingPos)
  {
    RecalcInverseKinematic(targetPos, attractingPos);
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
