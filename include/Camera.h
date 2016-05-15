#pragma once

// GL Includes
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
// Std. Includes
#include <vector>

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SENSITIVITY =  0.25f;
const GLfloat ZOOM       =  90.0f;

class Camera{
    private:
        //Camera attributes
        glm::vec3 mFront;
        glm::vec3 mUp, mWorldUp;
        glm::vec3 mRight;
        //Eular angles
        GLfloat mYaw, mPitch, mSensitivity;
    public:
        GLfloat Zoom;
        glm::vec3 mPosition;
        //Camera Constructor
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)): mYaw(YAW), mPitch(PITCH), Zoom(ZOOM), mSensitivity(SENSITIVITY){
            this->mPosition = position;
            this->mWorldUp = up;
            this->updateCameraVectors();
        }

        glm::mat4 getViewMatrix(){
             return glm::lookAt(this->mPosition, this->mPosition + this->mFront, this->mUp);
        }

        //MOuse input processing function
        void processMouseMovement(GLfloat xOffset, GLfloat yOffset){
            xOffset *= mSensitivity;
            yOffset *= mSensitivity;

            this->mYaw += xOffset;
            this->mPitch += yOffset;

            if (this->mPitch > 89.0f)
                this->mPitch = 89.0f;
            if (this->mPitch < -89.0f)
                this->mPitch = -89.0f;

            this->updateCameraVectors();
        }
        //Mouse scroll processing function
        void processMouseScroll(GLfloat yOffset){
            if (this->Zoom >= 1.0f && this->Zoom <= 90.0f)
                this->Zoom -= yOffset;
            if (this->Zoom <= 1.0f)
                this->Zoom = 1.0f;
            if (this->Zoom >= 90.0f)
                this->Zoom = 90.0f;
        }
        void updateCameraVectors(){
            glm::vec3 front;
            front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
            front.y = sin(glm::radians(mPitch));
            front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
            this->mFront = glm::normalize(front);
            //recalculate the right and up vector
            this->mRight = glm::normalize(glm::cross(this->mFront, this->mWorldUp));
            this->mUp = glm::normalize(glm::cross(this->mRight, this->mFront));
        }
    };
