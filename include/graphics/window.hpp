
namespace flex {
class Window {
private:
  unsigned int mWidth;
  unsigned int mHeight;
  bool mFullscreen;

public:
  Window();
  Window(const unsigned int &width, const unsigned int &height,
         const bool &fullscreen);
  ~Window();
};
} // namespace flex