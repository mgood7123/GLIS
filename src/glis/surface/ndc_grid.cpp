//
// Created by smallville7123 on 20/08/20.
//

#include <glis/surface/ndc_grid.hpp>
#include <cassert>

GLIS_NDC_Tools::Grid::Grid() {}

GLIS_NDC_Tools::Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    x = new float[width+1];
    y = new float[height+1];
    float widthdiv2 = static_cast<float>((width)/2);
    float heightdiv2 = static_cast<float>((height)/2);
    for (int i = 0; i <= width; i++) {
        float pixel = static_cast<float>(i);
        float step_1 = pixel - widthdiv2 - 0.0f;
        float step_2 = step_1 / widthdiv2;
        x[i] = step_2;
    }
    for (int i = 0; i <= height; i++) {
        float pixel = static_cast<float>(i);
        float step_1 = pixel - heightdiv2 - 0.0f;
        float step_2 = step_1 / heightdiv2;
        y[i] = step_2;
    }
}

GLIS_NDC_Tools::Grid::~Grid() {
    delete[] x;
    delete[] y;
}

GLIS_NDC_Tools::GridPixelCentered::GridPixelCentered() {}

GLIS_NDC_Tools::GridPixelCentered::GridPixelCentered(int width, int height) {
    this->width = width;
    this->height = height;
    x = new float[width+1];
    y = new float[height+1];

    // https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Viewport_transform
    // https://wikimedia.org/api/rest_v1/media/math/render/svg/138401cb60a7c667336fb41d47d22f37c8f2a569

    // NDC to Window Space: x: (width/2)*X_NDC + X + (width/2)
    // NDC to Window Space: y: (height/2)*Y_NDC + Y + (height/2)

    // NDC {X: -1, Y: 1}, {X: 1, Y: 1}

    // width: 400
    // height: 400

    // window space: x: ((400/2) * -1) + 0 + (400/2)
    // window space: x: (200 * -1) + 0 + 200
    // window space: x: -200 + 200
    // window space: x: 0

    // window space: y: ((400/2) * 1) + 0 + (400/2)
    // window space: y: (200 * 1) + 0 + 200
    // window space: y: 200 + 200
    // window space: y: 400

    // window space: {X: 0, Y: 400}, {X: 400, Y: 400}

    // https://magcius.github.io/xplain/article/rast1.html

    // determine what side of a pixel the given NDC coordinate will draw to
    // and fix it if it is off screen

    // https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf
    // 14.5 Line Segments, pdf page 493, printed page 471
    // also see section 13.7 in the GL 4.6 spec ("Primitive Clipping")

    // almost all GPU's support line widths of up to 32 pixels,
    // and most GPU's support widths of up to 2048 pixels
    // 10 is the max width on nvidia, 7 is max on intel


    // shift NDC values by half a pixel so NDC is on pixel center instead of pixel edge
    // [13:04] <imirkin> ndc value +/- 1/(2*width)
    // [13:04] <imirkin> + on the left, - on the right
    // [13:05] <imirkin> you can't hard-code it as the width/height may change,
    // but you can take them in as uniforms

    // In OpenGL (ES), a pixel of the framebuffer is defined as being covered by a primitive
    // if the center of the pixel is covered by the primitive as illustrated in the diagram
    // to the right.

    float widthdiv2 = static_cast<float>((width)/2);
    float heightdiv2 = static_cast<float>((height)/2);
    for (int i = 0; i <= width; i++) {
        float pixel = static_cast<float>(i);
        float step_1 = pixel - widthdiv2 - 0.0f;
        float step_2 = step_1 / widthdiv2;
        x[i] = step_2 > 0.0f
               ? step_2 - (1.0f/(2.0f*static_cast<float>(width)))
               : step_2 + (1.0f/(2.0f*static_cast<float>(width)));
    }
    for (int i = 0; i <= height; i++) {
        float pixel = static_cast<float>(i);
        float step_1 = pixel - heightdiv2 - 0.0f;
        float step_2 = step_1 / heightdiv2;
        y[i] = step_2 > 0.0f
               ? step_2 - (1.0f/(2.0f*static_cast<float>(height)))
               : step_2 + (1.0f/(2.0f*static_cast<float>(height)));
    }
}

GLIS_NDC_Tools::GridPixelCentered::~GridPixelCentered() {
    delete[] x;
    delete[] y;
}

void GLIS_NDC_Tools::GridPixelCentered::test() {
    Grid grid(width, height);
    for (int i = 0; i <= width; i++) {
        float val = grid.x[i] > 0.0f
                    ? grid.x[i] - (1.0f/(2.0f*static_cast<float>(width)))
                    : grid.x[i] + (1.0f/(2.0f*static_cast<float>(width)));
        assert(val == x[i]);
    }
    for (int i = 0; i <= height; i++) {
        float val = grid.y[i] > 0.0f
                    ? grid.y[i] - (1.0f/(2.0f*static_cast<float>(height)))
                    : grid.y[i] + (1.0f/(2.0f*static_cast<float>(height)));
        assert(val == y[i]);
    }
}

Magnum::Vector2 GLIS_NDC_Tools::GridPixelCentered::Correct_NDC(const Magnum::Vector2 &xy) {
    return {findClosest(x, width+1, xy[0]), findClosest(y, height+1, xy[1])};
}

Magnum::Vector2 GLIS_NDC_Tools::NDC_to_WindowSpace(const Magnum::Vector2 &xy, int width, int height) {
    // https://www.khronos.org/opengl/wiki/Vertex_Post-Processing#Viewport_transform
    // https://wikimedia.org/api/rest_v1/media/math/render/svg/138401cb60a7c667336fb41d47d22f37c8f2a569

    // NDC to Window Space: x: (width/2)*X_NDC + X + (width/2)
    // NDC to Window Space: y: (height/2)*Y_NDC + Y + (height/2)

    // NDC {X: -1, Y: 1}, {X: 1, Y: 1}

    // width: 400
    // height: 400

    // window space: x: ((400/2) * -1) + 0 + (400/2)
    // window space: x: (200 * -1) + 0 + 200
    // window space: x: -200 + 200
    // window space: x: 0

    // window space: y: ((400/2) * 1) + 0 + (400/2)
    // window space: y: (200 * 1) + 0 + 200
    // window space: y: 200 + 200
    // window space: y: 400

    // window space: {X: 0, Y: 400}, {X: 400, Y: 400}

    // https://magcius.github.io/xplain/article/rast1.html

    // determine what side of a pixel the given NDC coordinate will draw to
    // and fix it if it is off screen

    // https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf
    // 14.5 Line Segments, pdf page 493, printed page 471
    // also see section 13.7 in the GL 4.6 spec ("Primitive Clipping")

    // almost all GPU's support line widths of up to 32 pixels,
    // and most GPU's support widths of up to 2048 pixels
    // 10 is the max width on nvidia, 7 is max on intel


    // shift NDC values by half a pixel so NDC is on pixel center instead of pixel edge
    // [13:04] <imirkin> ndc value +/- 1/(2*width)
    // [13:04] <imirkin> + on the left, - on the right
    // [13:05] <imirkin> you can't hard-code it as the width/height may change, but you can take them in as uniforms

    float widthdiv2 = static_cast<float>(width/2);
    float heightdiv2 = static_cast<float>(height/2);
    float xf = (widthdiv2 * xy[0]) + 0.0f + widthdiv2;
    float yf = (heightdiv2 * xy[1]) + 0.0f + heightdiv2;
    return {xf, yf};
}

Magnum::Vector2i GLIS_NDC_Tools::NDC_to_WindowSpacei(const Magnum::Vector2 & xy, int width, int height) {
    Magnum::Vector2 v = NDC_to_WindowSpace(xy, width, height);
    return {static_cast<int>(v[0]), static_cast<int>(v[1])};
}