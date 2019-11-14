
#ifndef MOUSE_CAMERA_H
#define MOUSE_CAMERA_H

#include <Vector2.h>
#include "Camera.h"

enum MouseButtonState { LB_DOWN, LB_UP, MB_DOWN, MB_UP, RB_DOWN, RB_UP };

template <typename Real>
class MouseCamera : public Camera<Real> {
public:
    MouseCamera(Real radius = Real(1));
    ~MouseCamera();

    void onMouseMove(int x, int y);
    void onMouseButton(MouseButtonState buttonState, int x, int y);

    bool isMoving() const;
    bool isZooming() const;

protected:
    Vector2i mouseCur;
    Vector2i mouseLast;

    float rotateSensitivity;
    float zoomSensitivity;

    bool mouseMove;
    bool mouseZoom;
};

const static float DEFAULT_ROTATE_SENSITIVITY = 0.01f;
const static float DEFAULT_ZOOM_SENSITIVITY = 0.1f;

template <typename Real>
MouseCamera<Real>::MouseCamera(Real radius = Real(1)) : Camera(radius) {
    this->rotateSensitivity = DEFAULT_ROTATE_SENSITIVITY;
    this->zoomSensitivity = DEFAULT_ZOOM_SENSITIVITY;
    this->mouseMove = false;
    this->mouseZoom = false;
}

template <typename Real>
MouseCamera<Real>::~MouseCamera() {}

template <typename Real>
void MouseCamera<Real>::onMouseMove(int x, int y) {
    this->mouseCur.set(x, y);

	int x_diff = this->mouseLast.x() - this->mouseCur.x();
	int y_diff = this->mouseLast.y() - this->mouseCur.y();

    if ( mouseMove ) {
        float dTheta = this->rotateSensitivity * (float)x_diff;
	    float dPhi = this->rotateSensitivity * (float)y_diff;
        this->rotate(-dTheta, dPhi);
    }

    if ( mouseZoom ) {
        float dr = this->zoomSensitivity * (float)y_diff;
        this->addRadius(dr);
    }

	this->mouseLast = this->mouseCur;
}

template <typename Real>
void MouseCamera<Real>::onMouseButton(MouseButtonState buttonState, int x, int y) {
    if ( buttonState == LB_DOWN ) {
        mouseLast.set(x, y);
        mouseMove = true;
    }

    if ( buttonState == RB_DOWN ) {
        mouseLast.set(x, y);
        mouseZoom = true;
    }

    if ( buttonState == LB_UP ) mouseMove = false;
    if ( buttonState == RB_UP ) mouseZoom = false;
}

template <typename Real>
bool MouseCamera<Real>::isMoving() const {
    return this->mouseMove;
}

template <typename Real>
bool MouseCamera<Real>::isZooming() const {
    return this->mouseZoom;
}

typedef MouseCamera<float> MouseCameraf;
typedef MouseCamera<double> MouseCamerad;

#endif
