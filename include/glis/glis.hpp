#define GLIS_COMMON_WIDTH 1080
#define GLIS_COMMON_HEIGHT 2032
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Platform/GLContext.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/ImGuiIntegration/Widgets.h>

#define GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(name) void (*name)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &)
#define GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(functionName, GLIS_name, GLIS_CLASS_name, GLIS_FONT_name, GLIS_FPS_name) void functionName (GLIS & GLIS_name, GLIS_CLASS & GLIS_CLASS_name, GLIS_FONT & GLIS_FONT_name, GLIS_FPS & GLIS_FPS_name)

#include <glis/ipc/ipc.hpp>
#include <glis/font/font.hpp>
#include <glis/internal/internal.hpp>
#include <glis/backup/backup.hpp>
#include <glis/compositor/compositor.hpp>