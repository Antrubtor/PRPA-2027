#include "Otsu.hpp"
#include "logo.h"

// #include <chrono>
// #include <thread>
#include <vector>
#include <tbb/tbb.h>

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
    std::vector<int> grey_hist = std::vector<int>(256, 0);
    for (int y = 0; y < in.height; ++y)
    {
        rgb8* lineptr = (rgb8*)((std::byte*)in.buffer + y * in.stride);
        for (int x = 0; x < in.width; ++x)
        {
            rgb8 pixel = lineptr[x];
            uint8_t grey_value = 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
            grey_hist[grey_value]++;
            lineptr[x] = { grey_value, grey_value, grey_value };
        }
    }
    int sum1 = 0;
    int sumB = 0;
    float wB = 0.0f;
    float wF = 0.0f;
    float mF = 0.0f;
    float max_var = 0.0f;
    int N = in.height * in.width;
    float inter_var = 0.0f;
    unsigned char threshold = 0;
    for (int i = 0; i < 256; i++)
        sum1 += i * grey_hist[i];
    for (int i = 0; i < 256; i++)
    {
        wB += grey_hist[i];
        wF = N - wB;
        if (wB == 0 || wF == 0)
            continue;
        sumB += i * grey_hist[i];
        mF = (sum1 - sumB) / wF;
        inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
        if (inter_var >= max_var)
        {
            threshold = i;
            max_var = inter_var;
        }
    }
    for (int y = 0; y < in.height; ++y)
    {
        rgb8* lineptr = (rgb8*)((std::byte*)in.buffer + y * in.stride);
        for (int x = 0; x < in.width; ++x)
        {
            rgb8 pixel = lineptr[x];
            if (pixel.r < threshold)
                lineptr[x] = { 0, 0, 0 };
            else
                lineptr[x] = { 255, 255, 255 };
        }
    }
}



// Single threaded - Optimized version of the Method
void otsu_st(ImageView<rgb8> in)
{
    // TODO
    std::vector<uint32_t> grey_hist = std::vector<uint32_t>(256, 0);
    std::vector<uint8_t> grey_img(in.width * in.height); // vecteur pour save tous les calculs de gris: lourd en mémoire, plus rapide

    for (int y = 0; y < in.height; ++y)
    {
        rgb8* lineptr = (rgb8*)((std::byte*)in.buffer + y * in.stride);
        for (int x = 0; x < in.width; ++x)
        {
            rgb8 pixel = lineptr[x];
            // 0.299 * 256 = 77 / 0.587 * 256 = 150 / 0.114 * 256 = 29;
            uint8_t grey_value = (77 * pixel.r + 150 * pixel.g + 29 * pixel.b) >> 8;
            grey_img[y * in.width + x] = grey_value;
            grey_hist[grey_value]++;
        }
    }
    unsigned int sum1 = 0;
    unsigned int sumB = 0;
    float wB = 0.0f;
    float wF = 0.0f;
    float mF = 0.0f;
    float max_var = 0.0f;
    unsigned int N = in.height * in.width;
    float inter_var = 0.0f;
    uint8_t threshold = 0;
    for (size_t i = 0; i < 256; i++)
        sum1 += i * grey_hist[i];
    for (size_t i = 0; i < 256; i++)
    {
        wB += grey_hist[i];
        if (wB == 0)
            continue;
        wF = N - wB;
        if (wF == 0)
            continue;
        sumB += i * grey_hist[i];
        mF = (sum1 - sumB) / wF;
        inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
        if (inter_var >= max_var)
        {
            threshold = i;
            max_var = inter_var;
        }
    }
    for (int y = 0; y < in.height; ++y)
    {
        rgb8* lineptr = (rgb8*)((std::byte*)in.buffer + y * in.stride);
        for (int x = 0; x < in.width; ++x)
        {
            uint8_t grey = grey_img[y * in.width + x]; // réutilisation des calculs du gris
            uint8_t val = (grey < threshold) ? 0 : 255;
            lineptr[x] = { val, val, val };
        }
    }
}

void render_base_tile(std::byte* buffer, std::ptrdiff_t stride, const tbb::blocked_range2d<int>& tile, int* histogram)
{
    int x0 = tile.cols().begin();
    int x1 = tile.cols().end();
    int y0 = tile.rows().begin();
    int y1 = tile.rows().end();

    for (int y = y0; y < y1; ++y)
    {
        rgb8* lineptr = reinterpret_cast<rgb8*>(buffer + y * stride);
        for (int x = x0; x < x1; ++x)
        {
            rgb8 pixel = lineptr[x];
            uint8_t grey_value = (77 * pixel.r + 150 * pixel.g + 29 * pixel.b) >> 8;
            histogram[grey_value]++;
        }
    }
}

struct HistogramComputer
{
    std::byte* buffer;
    std::ptrdiff_t stride;

    std::unique_ptr<int[]> histogram;

    HistogramComputer(std::byte* buffer, std::ptrdiff_t stride)
        : buffer(buffer), stride(stride)
    {
        histogram = std::make_unique<int[]>(256 + 1);
        std::fill(histogram.get(), histogram.get() + 256, 0);
    }

    HistogramComputer(HistogramComputer& other, tbb::split)
        : buffer(other.buffer), stride(other.stride)
    {
        histogram = std::make_unique<int[]>(256 + 1);
        std::fill(histogram.get(), histogram.get() + 256, 0);
    }

    void operator()(const tbb::blocked_range2d<int>& tile) {
        render_base_tile(buffer, stride, tile, histogram.get());
    }

    void join(const HistogramComputer& other)
    {
        for (int k = 0; k < 256; ++k)
            histogram[k] += other.histogram[k];
    }
};



// Multi threaded version of the Method
void otsu_mt(ImageView<rgb8> in)
{
    // TODO
    auto full_tile = tbb::blocked_range2d<int>(0, in.height, 32, 0, in.width, 32);
    HistogramComputer histComp = HistogramComputer(reinterpret_cast<std::byte*>(in.buffer), in.stride);
    tbb::parallel_reduce(full_tile, histComp);

    auto& grey_hist = histComp.histogram;
    unsigned int sum1 = 0;
    unsigned int sumB = 0;
    float wB = 0.0f;
    float wF = 0.0f;
    float mF = 0.0f;
    float max_var = 0.0f;
    unsigned int N = in.height * in.width;
    float inter_var = 0.0f;
    uint8_t threshold = 0;
    for (size_t i = 0; i < 256; i++) // bcp trop useless et overkill de paraléliser 256 actions
        sum1 += i * grey_hist[i];
    for (size_t i = 0; i < 256; i++)
    {
        wB += grey_hist[i];
        if (wB == 0)
            continue;
        wF = N - wB;
        if (wF == 0)
            continue;
        sumB += i * grey_hist[i];
        mF = (sum1 - sumB) / wF;
        inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
        if (inter_var >= max_var)
        {
            threshold = i;
            max_var = inter_var;
        }
    }
    tbb::parallel_for(full_tile, [&](const tbb::blocked_range2d<int>& tile) {
        int y0 = tile.rows().begin();
        int y1 = tile.rows().end();
        int x0 = tile.cols().begin();
        int x1 = tile.cols().end();
        for (int y = y0; y < y1; ++y)
        {
            rgb8* lineptr = reinterpret_cast<rgb8*>(reinterpret_cast<std::byte*>(in.buffer) + y * in.stride);
            for (int x = x0; x < x1; ++x)
            {
                uint8_t grey_value = (77 * lineptr[x].r + 150 * lineptr[x].g + 29 * lineptr[x].b) >> 8;
                uint8_t val = (grey_value < threshold) ? 0 : 255;
                lineptr[x] = { val, val, val };
            }
        }
    });
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
