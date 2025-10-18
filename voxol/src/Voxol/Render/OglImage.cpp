#include "OglImage.h"
#include <vector>
#include <filesystem>
#include <fstream>
#include <png.h>

namespace Voxol::Render
{

void OglImage ::test()
{
}

RawData::Image2DBytesData OglImage::loadPNGFromAssets(const std::string& filename)
{
    auto texPath = std::filesystem::path(SRC_DIR) / "assets/";
    return loadPNG(texPath.string() + filename);
}

RawData::Image2DBytesData OglImage::loadPNG(const std::string& filePath)
{
    FILE* fp = fopen(filePath.data(), "rb");
    if (!fp)
    {
        printf("Failed to open PNG file.\n");
        return {};
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
    {
        printf("png_create_read_struct failed.\n");
        return {};
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        printf("png_create_info_struct failed.\n");
        return {};
    }

    if (setjmp(png_jmpbuf(png)))
    {
        printf("Error during png init_io.");
        return {};
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    int      width  = png_get_image_width(png, info);
    int      height = png_get_image_height(png, info);
    png_byte color  = png_get_color_type(png, info);
    png_byte depth  = png_get_bit_depth(png, info);

    if (depth == 16) png_set_strip_16(png);
    if (color == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color == PNG_COLOR_TYPE_GRAY && depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

    if (color == PNG_COLOR_TYPE_RGB || color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    std::vector<unsigned char> pixels(width * height * 4);
    std::vector<png_bytep>     rows(height);
    for (int y = 0; y < height; ++y)
        rows[y] = pixels.data() + y * width * 4;

    png_read_image(png, rows.data());

    fclose(fp);
    png_destroy_read_struct(&png, &info, nullptr);

    return {width, height, GL_RGBA, GL_RGBA, std::move(pixels)};
}
} // namespace Voxol::Test