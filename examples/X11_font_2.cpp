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

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    /* White background */
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    font.render_text("This is Vera font at size 24", "Vera", 24, 0, 24);
    font.render_text("This is Fireflysung font at size 24", "Fireflysung", 24, 0, 48);

    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
    font.set_max_width_height(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 640, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glis.GLIS_Viewport(screen);
    font.set_max_width_height(screen);

    font.add_font("Vera", "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf");
    font.add_font_size("Vera", 24);
    font.add_font("Fireflysung", "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/fireflysung.ttf");
    font.add_font_size("Fireflysung", 24);

    font.set_color(font.colors.white);

    glis.GLIS_SwapBuffers(screen);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}