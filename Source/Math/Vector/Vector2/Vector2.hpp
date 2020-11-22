#pragma once

namespace cbl {
template <typename T> struct Vector2 {
public:
  T x;
  T y;

  Vector2() = default;
  Vector2<T>(Vector2<T> &other);
  Vector2<T>(Vector2<T> const &other);
  Vector2<T>(T x, T y);

  Vector2<T> &operator=(Vector2<T> other);
  Vector2<T> operator+(Vector2<T> other);
  Vector2<T> operator-(Vector2<T> other);
  Vector2<T> operator*(Vector2<T> other);
  Vector2<T> operator*(T scalar);
  Vector2<T> operator/(Vector2<T> other);
  Vector2<T> operator/(T scalar);
  Vector2<T> &operator++();
  Vector2<T> &operator--();
  Vector2<T> &operator+=(Vector2<T> other);
  Vector2<T> &operator-=(Vector2<T> other);
  Vector2<T> &operator*=(Vector2<T> other);
  Vector2<T> &operator*=(T scalar);
  Vector2<T> &operator/=(Vector2<T> other);
  Vector2<T> &operator/=(T scalar);
  bool operator==(Vector2<T> other);
  bool operator!=(Vector2<T> other);

  template <typename U> explicit operator Vector2<U>();
};

template <typename T> Vector2<T>::Vector2(Vector2<T> &other) {
  x = other.x;
  y = other.y;
}

template <typename T> Vector2<T>::Vector2(const Vector2<T> &other) {
  x = other.x;
  y = other.y;
}

template <typename T> Vector2<T>::Vector2(T x, T y) {
  this->x = x;
  this->y = y;
}

template <typename T> Vector2<T> &Vector2<T>::operator=(Vector2<T> other) {
  x = other.x;
  y = other.y;
  return *this;
}

template <typename T> Vector2<T> Vector2<T>::operator+(Vector2<T> other) {
  return Vector2<T>{x + other.x, y + other.y};
}

template <typename T> Vector2<T> Vector2<T>::operator-(Vector2<T> other) {
  return Vector2<T>{x - other.x, y - other.y};
}

template <typename T> Vector2<T> Vector2<T>::operator*(Vector2<T> other) {
  return Vector2<T>{x * other.x, y * other.y};
}

template <typename T> Vector2<T> Vector2<T>::operator*(T scalar) {
  return Vector2<T>{x * scalar, y * scalar};
}
template <typename T> Vector2<T> Vector2<T>::operator/(Vector2<T> other) {
  return Vector2<T>{x / other.x, y / other.y};
}
template <typename T> Vector2<T> Vector2<T>::operator/(T scalar) {
  return Vector2<T>{x / scalar, y / scalar};
}
template <typename T> Vector2<T> &Vector2<T>::operator++() {
  x++;
  y++;
  return *this;
}
template <typename T> Vector2<T> &Vector2<T>::operator--() {
  x--;
  y--;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator+=(Vector2<T> other) {
  x += other.x;
  y += other.y;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator-=(Vector2<T> other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator*=(Vector2<T> other) {
  x *= other.x;
  y * -other.y;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator*=(T scalar) {
  x *= scalar;
  y *= scalar;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator/=(Vector2<T> other) {
  x /= other.x;
  y /= other.y;
  return *this;
}

template <typename T> Vector2<T> &Vector2<T>::operator/=(T scalar) {
  x /= scalar;
  y /= scalar;
  return *this;
}

template <typename T> bool Vector2<T>::operator==(Vector2<T> other) {
  return x == other.x && y == other.y;
}

template <typename T> bool Vector2<T>::operator!=(Vector2<T> other) {
  return x != other.x || y != other.y;
}

template <typename T> template <typename U> Vector2<T>::operator Vector2<U>() {
  return Vector2<U>{static_cast<U>(x), static_cast<U>(y)};
}
} // namespace cbl::math