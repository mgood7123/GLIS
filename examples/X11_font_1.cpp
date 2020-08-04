//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

const char *vertexSource = R"glsl( #version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = vec4(aColor, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char *fragmentSource = R"glsl( #version 300 es
out highp vec4 FragColor;

in highp vec4 ourColor;
in highp vec2 TexCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture1, TexCoord);
/*
    FragColor = mix(
        texture(texture1, TexCoord), // texture 1
        texture(texture2, TexCoord), // texture 2
        0.2 // interpolation,
        // If the third value is 0.0 it returns the first input
        // If it's 1.0 it returns the second input value.
        // A value of 0.2 will return 80% of the first input color and 20% of the second input color
        // resulting in a mixture of both our textures.
    );
*/
}
)glsl";

GLuint texture;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLIS_FONT::ATLAS atlas;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    font.A.display(font);
//    for (int i = 1; i < 6001; ++i) {
//        wrap ? font.GLIS_font_RenderText(
//                GLIS_val_to_string(i),
//                glis.GLIS_wrap_to_range(GLIS::GLIS_WRAP_MODE::PING_PONG, 6*(i-1), 0, screen.width, 0, 1),
//                glis.GLIS_wrap_to_range(GLIS::GLIS_WRAP_MODE::PING_PONG, font.height*i, 0, screen.height, 0, 1),
//                font.GLIS_font_color_white
//        ) : font.GLIS_font_RenderText(
//                GLIS_val_to_string(i),
//                6*(i-1),
//                font.height*i,
//                font.GLIS_font_color_white
//        );
//    }
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glViewport(0, 0, renderer.width, renderer.height);
    font.GLIS_font_set_RenderText_w_h(renderer.width, renderer.height);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    font.A.free_resources();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 640, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glViewport(0, 0, screen.width, screen.height);
    atlas.set_width_height(screen.width, screen.height);
    atlas.load_font("vera", "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf");
    atlas.generate_font_size(12);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}