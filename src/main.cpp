#include <Greet.h>

#include "Camera.h"
#include "Spider.h"
#include "math/MathFunc.h"
#include "math/Vec3.h"

using namespace Greet;

class Application : public App
{
  Ref<Shader> skyboxShader;
  Ref<Mesh> skyplane;

  Ref<Mesh> plane;
  Ref<Mesh> sphere;
  Ref<Shader> shader;
  Ref<Texture> texture;
  Ref<Spider> spider;
  Cam cam;
  CamController camController = CamController{cam};

  public:
    Application()
      : App{"", 720, 480}, cam{Mat4::Perspective(Window::GetAspect(), 90, 0.001, 1000.0)}
    {
      SetFrameCap(60);
    }

    void Init() override
    {
      texture = TextureManager::LoadTexture2D("res/textures/default.meta");
      cam.SetPosition({0, 5, 0});
      skyboxShader = Shader::FromFile("res/shaders/skybox.glsl");
      skyplane = Ref<Mesh>(new Mesh{{{-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0}}, {0, 1, 2, 0, 2, 3}});
      MeshData data = MeshFactory::Quad(0, 0, 0, 1, 1);
      data.AddAttribute(BufferAttribute{1, BufferAttributeType::VEC2, false}, Pointer<Vec2>{{0, 0}, {1, 0}, {1, 1}, {0, 1}});
      plane = Ref<Mesh>(new Mesh{data});
      sphere = Ref<Mesh>(new Mesh{MeshFactory::Sphere({0, 0, 0}, 1, 20, 20)});
      shader = Shader::FromFile("res/shaders/shader3d.glsl");
      spider = Ref<Spider>(new Spider{{0, 1, 0}});
    }

    void Render() override
    {
      ////////////////
      // Skybox
      RenderCommand::EnableDepthTest(false);
      skyboxShader->Enable();
      skyboxShader->SetUniformMat4("uProjectionMatrix", cam.GetProjectionMatrix());
      skyboxShader->SetUniformMat4("uViewMatrix", cam.GetViewMatrix());
      skyboxShader->SetUniformMat4("uPVInvMatrix", cam.GetInvPVMatrix());
      skyplane->Bind();
      skyplane->Render();
      skyplane->Unbind();
      skyboxShader->Disable();
      RenderCommand::EnableDepthTest(true);

      ////////////////
      // Flooring
      shader->Enable();

      shader->SetUniform3f("uColor", Greet::Vec3<float>{1.0f, 1.0f, 1.0f});
      shader->SetUniformMat4("uTransformationMatrix", Mat4::Scale(100));
      shader->SetUniformMat4("uProjectionMatrix", cam.GetProjectionMatrix());
      shader->SetUniformMat4("uViewMatrix", cam.GetViewMatrix());
      shader->SetUniform1i("uTextureRepeatCount", 100);
      shader->SetUniformBoolean("uHasTexture", true);
      texture->Enable(0);
      plane->Bind();
      plane->Render();
      plane->Unbind();
      texture->Disable();

      spider->Render(cam);
    }

    void Tick() override
    {
    }

    void Update(float timeElapsed) override
    {
      camController.Update(timeElapsed);
      spider->Update(timeElapsed);
    }

    void OnEvent(Event& event) override
    {
      if(EVENT_IS_TYPE(event ,EventType::VIEWPORT_RESIZE))
      {
        cam.SetProjectionMatrix(Mat4::Perspective(Window::GetAspect(), 90, 0.001, 1000.0));
      }
    }
};

int main()
{
  Application app;
  app.Start();
}
