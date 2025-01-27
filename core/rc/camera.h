#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"
#include "../rc/transformations.h"

namespace rc {

    struct Camera {
        ew::Vec3 position;
        ew::Vec3 target;
        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        bool orthographic;
        float orthoSize;
        ew::Mat4 ViewMatrix() {
            return rc::LookAt(position, target, ew::Vec3(0, 1, 0)); //World->View
        };
        ew::Mat4 ProjectionMatrix() {
            if (orthographic)
                return Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
            else
                return Perspective(ew::Radians(fov), aspectRatio, nearPlane, farPlane);
            
        } 
    };
    struct CameraControls {
        double prevMouseX, prevMouseY;
        float yaw = 0, pitch = 0;
        float mouseSensitivity = 0.1f;
        bool firstMouse = true;
        float moveSpeed = 5.0f;
    };

}













