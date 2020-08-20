//
// Created by smallville7123 on 20/08/20.
//

#ifndef ANDROIDCOMPOSITOR_NDC_GRID_HPP
#define ANDROIDCOMPOSITOR_NDC_GRID_HPP

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

#include <magnum/src/Magnum/Magnum.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Matrix3.h>

class GLIS_NDC_Tools {
public:
    class Grid {
    public:
        int width = 0;
        int height = 0;
        float * x = nullptr;
        float * y = nullptr;

        Grid();

        Grid(int width, int height);

        ~Grid();
    };

    class GridPixelCentered {
    public:
        int width = 0;
        int height = 0;
        float * x = nullptr;
        float * y = nullptr;

        GridPixelCentered();;

        GridPixelCentered(int width, int height);

        ~GridPixelCentered();

        void test();

        // Method to compare which one is the more close.
        // We find the closest by taking the difference
        // between the target and both values. It assumes
        // that val2 is greater than val1 and target lies
        // between these two.
        template <typename T>
        T & getClosest(T & val1, T & val2, T target) {
            return (target - val1 >= val2 - target) ? val2 : val1;
        }

        // Returns element closest to target in arr[]
        template <typename T>
        T findClosest(T arr[], int n, T target) {
            // Corner cases
            if (target <= arr[0])
                return arr[0];
            if (target >= arr[n - 1])
                return arr[n - 1];

            // Doing binary search
            int i = 0, j = n, mid = 0;
            while (i < j) {
                mid = (i + j) / 2;

                if (arr[mid] == target)
                    return arr[mid];

                /* If target is less than array element,
                    then search in left */
                if (target < arr[mid]) {

                    // If target is greater than previous
                    // to mid, return closest of two
                    if (mid > 0 && target > arr[mid - 1])
                        return getClosest(arr[mid - 1], arr[mid], target);

                    /* Repeat for left half */
                    j = mid;
                }

                    // If target is greater than mid
                else {
                    if (mid < n - 1 && target < arr[mid + 1])
                        return getClosest(arr[mid], arr[mid + 1], target);
                    // update i
                    i = mid + 1;
                }
            }

            // Only single element left after search
            return arr[mid];
        }

        // In OpenGL (ES), a pixel of the framebuffer is defined as being covered by a primitive
        // if the center of the pixel is covered by the primitive as illustrated in the diagram
        // to the right.
        Magnum::Vector2 Correct_NDC(const Magnum::Vector2 &xy);
    };

    static Magnum::Vector2 NDC_to_WindowSpace(const Magnum::Vector2 & xy, int width, int height);

    static Magnum::Vector2i NDC_to_WindowSpacei(const Magnum::Vector2 & xy, int width, int height);
};

#endif //ANDROIDCOMPOSITOR_NDC_GRID_HPP
