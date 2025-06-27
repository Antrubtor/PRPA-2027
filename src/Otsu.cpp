#include "Otsu.hpp"
#include "logo.h"

#include <chrono>
#include <thread>

// Single threaded version of the Method
static void otsu(ImageView<rgb8> in)
{
  for (int y = 0; y < in.height; ++y)
  {
    rgb8* lineptr = (rgb8*)((std::byte*)in.buffer + y * in.stride);
    for (int x = 0; x < in.width; ++x)
    {
      lineptr[x].r = 0; // Back out red component

      if (x < logo_width && y < logo_height)
      {
        float alpha  = logo_data[y * logo_width + x] / 255.f;
        lineptr[x].g = uint8_t(alpha * lineptr[x].g + (1 - alpha) * 255);
        lineptr[x].b = uint8_t(alpha * lineptr[x].b + (1 - alpha) * 255);
      }
    }
  }
}

void otsu_baseline(ImageView<rgb8> in)
{
  // TODO
  otsu(in);
  // You can fake a long-time process with sleep
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }
}



// Single threaded - Optimized version of the Method
void otsu_st(ImageView<rgb8> in)
{
  // TODO
  otsu(in);
  // You can fake a long-time process with sleep
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(50ms);
  }
}


// Multi threaded version of the Method
void otsu_mt(ImageView<rgb8> in)
{
  // TODO
  otsu(in);
  // You can fake a long-time process with sleep
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);
  }
}

extern "C" {

static Method m_method = BASELINE;

void set_otsu_method(Method m)
{
  m_method = m;
}


void otsu(uint8_t* buffer, int width, int height, int stride)
{
  if (buffer)
  {
    switch (m_method)
    {
    case BASELINE:
      otsu_baseline({(rgb8*)buffer, width, height, stride});
      break;
    case ST:
      otsu_st({(rgb8*)buffer, width, height, stride});
      break;
    case MT:
      otsu_mt({(rgb8*)buffer, width, height, stride});
      break;
    }
  }
}


}