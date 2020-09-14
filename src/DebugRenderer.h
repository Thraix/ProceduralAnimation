#pragma once

#include <Greet.h>
#include "Camera.h"

class DebugRenderer
{
  private:
    Greet::Ref<Greet::Shader> shader;
    Greet::Ref<Greet::Mesh> planeMesh;
    Greet::Ref<Greet::Mesh> pointMesh;
    Greet::Ref<Greet::Mesh> lineMesh;
    Greet::Ref<Greet::Mesh> cubeMesh;

  public:
    DebugRenderer()
      : shader{Greet::Shader::FromFile("res/shaders/shader3d.glsl")},
      planeMesh{new Greet::Mesh{Greet::MeshFactory::Quad(0, 0, 0, 1, 1)}},
      pointMesh{new Greet::Mesh{Greet::MeshFactory::Sphere({0, 0, 0}, 1, 20, 20)}},
      lineMesh{new Greet::Mesh{Greet::MeshFactory::Cube2(0, 0.5f, 0, 1, 1, 1)}},
      cubeMesh{new Greet::Mesh{Greet::MeshFactory::Cube2(0, 0, 0, 1, 1, 1)}}
    {
      planeMesh->SetEnableCulling(false);
      shader->Enable();
      shader->SetUniformBoolean("uHasTexture", false);
      shader->Disable();
    }

    void Begin(const Cam& cam) const
    {
      shader->Enable();
      cam.BindShaderMatrices(shader);
    }

    void Point(const Greet::Vec3<float>& pos, float radius, const Greet::Color& color)
    {
      shader->SetUniform3f("uColor", {color.r, color.g, color.b});
      shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::Translate(pos) * Greet::Mat4::Scale(radius));
      pointMesh->Bind();
      pointMesh->Render();
      pointMesh->Unbind();
    }

    void Plane(const Greet::Vec3<float>& pos, const Greet::Plane& plane, float size, const Greet::Color& color)
    {
      shader->SetUniform3f("uColor", {color.r, color.g, color.b});
      shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::AlignAxis(pos, plane.normal, {0, 1, 0}) * Greet::Mat4::Scale(size));
      planeMesh->Bind();
      planeMesh->Render();
      planeMesh->Unbind();
    }

    void Line(const Greet::Line& line, float size, const Greet::Color& color)
    {
      shader->SetUniform3f("uColor", {color.r, color.g, color.b});
      shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::AlignAxis(line.pos, line.dir, {0, 1, 0}) * Greet::Mat4::Scale({size, line.dir.Length(), size}));
      lineMesh->Bind();
      lineMesh->Render();
      lineMesh->Unbind();
    }

    void Cube(const Greet::Vec3<float>& pos, const Greet::Vec3<float>& size, const Greet::Color& color)
    {
      shader->SetUniform3f("uColor", {color.r, color.g, color.b});
      shader->SetUniformMat4("uTransformationMatrix", Greet::Mat4::Translate(pos) * Greet::Mat4::Scale(size));
      cubeMesh->Bind();
      cubeMesh->Render();
      cubeMesh->Unbind();
    }

    void End()
    {
      shader->Disable();
    }
};
