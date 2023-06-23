#include "Camera.h"

// TODO: fill up the following function properly
void Camera::set_rotation(const glm::quat &_q)
{
  glm::quat rotation = _q;
  // 새로운 rotation에 맞춰서 카메라의 front, up, right 방향 업뎃
  front_dir_ = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
  up_dir_ = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
  right_dir_ = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

// TODO: re-write the following function properly
const glm::quat Camera::get_rotation() const
{
  glm::vec3 zAxis = -front_dir_; // !!!Negative front direction
  glm::vec3 xAxis = glm::normalize(glm::cross(up_dir_, zAxis));
  glm::vec3 yAxis = glm::cross(zAxis, xAxis);
  glm::mat3 rotationMatrix(
      xAxis.x, xAxis.y, xAxis.z,
      yAxis.x, yAxis.y, yAxis.z,
      zAxis.x, zAxis.y, zAxis.z);

  return glm::quat_cast(rotationMatrix);
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::quat &_q, const glm::vec3 &_t)
{
  set_rotation(_q);
  position_ = _t;
}

// TODO: fill up the following function properly
void Camera::get_pose(glm::quat &_q, glm::vec3 &_t) const
{
  _q = get_rotation();
  _t = position_;
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_pose() const
{
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), position_);
  glm::mat4 rotation = glm::mat4_cast(get_rotation());
  return translation * rotation;
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::mat4 &_frame)
{
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  // right_dir_ = ..... ; // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)
  // up_dir_    = ..... ;    // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  // front_dir_ = ..... ;    // -z
  // pos_cam: position of the camera
  // position_  = ..... ;    // pos
  //_frame의 상단 좌측 3x3 부분을 추출하여 3x3 회전 행렬로 변환
  glm::mat3 rotationMatrix = glm::mat3(_frame);
  // rotation matrix를 기반으로 카메라 방향 업뎃
  right_dir_ = glm::normalize(rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f));
  up_dir_ = glm::normalize(rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f));
  front_dir_ = glm::normalize(rotationMatrix * glm::vec3(0.0f, 0.0f, -1.0f));

  // position_에 _frame 행렬의 마지막 열에 위치한 카메라의 위치 벡터 할당
  position_ = glm::vec3(_frame[3]);
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::vec3 &_pos, const glm::vec3 &_at, const glm::vec3 &_up_dir)
{
  // up_dir_ = _up_dir;
  // front_dir_ = ; // -z_cam direction
  //  right_dir_ = ..... ;    // +x_cam direction
  //  up_dir_    = ..... ;    // +y_cam direction

  // position_  = ..... ;      // pos
  front_dir_ = glm::normalize(_at - _pos);
  right_dir_ = glm::normalize(glm::cross(_up_dir, front_dir_));
  up_dir_ = glm::cross(front_dir_, right_dir_);
  position_ = _pos;
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_view_matrix() const
{
  return glm::lookAt(position_, position_ + front_dir_, up_dir_);
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_projection_matrix() const
{
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho,
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_
  if (mode() == kOrtho)
  {
    float left = -ortho_scale_ * aspect_ * 0.5f;
    float right = ortho_scale_ * aspect_ * 0.5f;
    float bottom = -ortho_scale_ * 0.5f;
    float top = ortho_scale_ * 0.5f;
    return glm::ortho(left, right, bottom, top, near_, far_);
  }
  if (mode() == kPerspective)
  {
    return glm::perspective(glm::radians(fovy_), aspect_, near_, far_);
  }
  else
  {
    // Invalid camera mode
    return glm::mat4(1.0f);
  }
}

// TODO: fill up the following functions properly
void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
}

// TODO: fill up the following functions properly
void Camera::move_backward(float delta)
{
  position_ -= delta * front_dir_;
}

// TODO: fill up the following functions properly
void Camera::move_left(float delta)
{
  position_ -= glm::normalize(glm::cross(front_dir_, up_dir_)) * delta;
}

// TODO: fill up the following functions properly
void Camera::move_right(float delta)
{
  position_ += glm::normalize(glm::cross(front_dir_, up_dir_)) * delta;
}

// TODO: fill up the following functions properly
void Camera::move_up(float delta)
{
  position_ += delta * up_dir_;
}

// TODO: fill up the following functions properly
void Camera::move_down(float delta)
{
  position_ -= delta * up_dir_;
}
