#pragma once

#include <Greet.h>

#include "Camera.h"
#include "math/Vec3.h"

struct SpiderLeg
{
  Greet::Ref<Greet::Mesh> debugSphere;

  Greet::Ref<Greet::Mesh> model;
  Greet::Ref<Greet::Shader> shader;

  std::vector<Greet::Vec3<float>> jointPositions;

  Greet::Vec3<float> footRestPos;
  Greet::Vec3<float> attractingPos;

  float boneLength = 0.75f;

  SpiderLeg(const Greet::Ref<Greet::Mesh>& model, const Greet::Ref<Greet::Shader>& shader, const Greet::Vec3<float>& jointPos, const Greet::Vec3<float>& footPos, const Greet::Vec3<float>& attractingPos)
    : debugSphere{new Greet::Mesh{Greet::MeshFactory::Sphere({0, 0, 0}, 1, 20, 20)}}, model{model}, shader{shader}, footRestPos{footPos}
  {
    for(int i = 0; i < 4; i++)
    {
      jointPositions.push_back(footPos + Greet::Vec3<float>(0, boneLength, 0) * i);
    }
    RecalcInverseKinematic(jointPos, attractingPos);
  }

  void Render()
  {
    model->Bind();
    for(int i = 0; i < jointPositions.size() - 1; i++)
    {
      Greet::Vec3<float> dir = (jointPositions[i + 1] - jointPositions[i]).Normalize();
      shader->SetUniform3f("uColor", Greet::Vec3<float>{1.0f, 1.0f, 1.0f} / i);
      shader->SetUniformMat4("uTransformationMatrix",
          Greet::Mat4::AlignAxis(jointPositions[i], dir, {0, 1, 0}) *
          Greet::Mat4::Scale({0.1, boneLength, 0.1}) *
          Greet::Mat4::Translate({0, 0.5f, 0}));
      model->Render();
    }
    model->Unbind();
  }

  void RenderDebug()
  {
    debugSphere->Bind();
    int i = 0;
    for(auto&& joint : jointPositions)
    {
      shader->SetUniform3f("uColor", Greet::Vec3<float>{0.0f, 0.0f, 0.7f} / i);
      shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::Translate(joint) * Greet::Mat4::Scale(0.1));
      debugSphere->Render();
      i++;
    }
    shader->SetUniform3f("uColor", Greet::Vec3<float>{0.0f, 0.0f, 0.7f} / i);
    shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::Translate(attractingPos) * Greet::Mat4::Scale(0.1));
    debugSphere->Render();
    debugSphere->Unbind();
  }

  void RecalcInverseKinematic(const Greet::Vec3<float>& targetPos, const Greet::Vec3<float>& attractingPos)
  {
    if((targetPos - jointPositions[jointPositions.size() - 1]).LengthSQ() < 0.001)
      return;
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
      float angle = SignedAngle(projectJoint - jointPositions[i - 1], projectAttraction - jointPositions[i - 1], plane.normal);
      Greet::Vec3<float> p = Greet::Quaternion::AxisAngle(plane.normal, angle) * (jointPositions[i] - jointPositions[i - 1]) + jointPositions[i - 1];
      if(std::isnan(p.x))
      {
        Greet::Log::Info("----------------------------------");
        Greet::Log::Info("-1: ", jointPositions[i - 1]);
        Greet::Log::Info(" 0: ", jointPositions[i]);
        Greet::Log::Info("+1: ", jointPositions[i + 1]);
        Greet::Log::Info("ProjJointDiff: ", Greet::Vec::Normalize(projectJoint - jointPositions[i - 1]));
        Greet::Log::Info("ProjAttrDiff: ", Greet::Vec::Normalize(projectAttraction - jointPositions[i - 1]));
        Greet::Log::Info("Dot: ", Greet::Vec::Dot(Greet::Vec::Normalize(projectJoint - jointPositions[i - 1]), Greet::Vec::Normalize(projectAttraction - jointPositions[i - 1])));
        Greet::Log::Info("acos: ", acos(Greet::Vec::Dot(Greet::Vec::Normalize(projectJoint - jointPositions[i - 1]), Greet::Vec::Normalize(projectAttraction - jointPositions[i - 1]))));

        Greet::Log::Info("Angle: ", angle);
        Greet::Log::Info("Plane: ", plane);
        Greet::Log::Info("SinCos: ", sin(angle), " ", cos(angle));
        Greet::Log::Info("Quat: ", Greet::Quaternion::AxisAngle(plane.normal, angle));
        Greet::Log::Info(attractingPos, " ", projectAttraction);
        Greet::Log::Info(projectJoint, plane);
        abort();
      }
      jointPositions[i] = p;
    }
//
  }

  void SetTargetPos(const Greet::Vec3<float>& targetPos, const Greet::Vec3<float>& attractingPos)
  {
    RecalcInverseKinematic(targetPos, attractingPos);
  }

    float SignedAngle(const Greet::Vec3<float>& vec1, const Greet::Vec3<float>& vec2, const Greet::Vec3<float>& normal)
    {
      float angle = acos(Greet::Vec::Dot(Greet::Vec::Normalize(vec1), Greet::Vec::Normalize(vec2)));
      float dot = Greet::Vec::Dot(Greet::Vec::Normalize(vec1), Greet::Vec::Normalize(vec2));
      if(dot < -1.0f || dot > 1.0f)
        angle = 0;
      Greet::Vec3<float> cross = Greet::Vec::Cross(vec1, vec2);
      if (Greet::Vec::Dot(normal, cross) > 0)
        angle = -angle;
      return angle;
    }
};

class Spider
{
  std::vector<SpiderLeg> legs;

  Greet::Ref<Greet::Mesh> body;
  Greet::Ref<Greet::Mesh> legPart;
  Greet::Ref<Greet::Shader> shader;

  Greet::Vec3<float> position;

  public:
    Spider(const Greet::Vec3<float>& position);
    Spider();

    void Render(const Cam& cam);
    void Update(float timeElapsed);
    void UpdateJointPositions();

};
