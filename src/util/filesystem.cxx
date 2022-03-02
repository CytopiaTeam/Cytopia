#include "Filesystem.hxx"
#include "../util/Exception.hxx"
#include <cstdio>
#include <png.h>

PixelBuffer fs::readPNGFile(const std::string & fileName) {
  /**
   * @todo  There is a security concern here. This function should only ever
   *        be used for files in the resource directory. There is a code snippet here
   *        that would fix this issue but requires the fs::path library
   */

  /** 
  
  Fix using the fs::path library

  fs::path fpath(BASE_PATH / "resources");
  error_code errc;
  fpath = boost::filesystem::canonical(fs::path(rpath), fpath, errc);
  if(errc) {
    throw AssetError{TRACE_INFO "Cannot fetch assets which are not in the /resources directory: " + rpath};
  }
  **/

  FILE* fp = fopen(fileName.c_str(), "rb");
  if(!fp)
    throw AssetError{TRACE_INFO "Could not load asset: " + fileName};
  
  /* Check for PNG */
  unsigned char header[8];
  fread(header, 1, 8, fp);
  if(png_sig_cmp(header, 0, 8)) {
    fclose(fp);
    throw AssetError{TRACE_INFO "Tried to load a non-PNG asset: " + fileName};
  }

  /* Create png object */
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
      nullptr, nullptr, nullptr);
  if(!png) {
    fclose(fp);
    throw AssetError{TRACE_INFO "Could not allocate PNG object: " + fileName};
  }

  png_infop info = png_create_info_struct(png);
  if(!info) {
    fclose(fp);
    png_destroy_read_struct(&png, nullptr, nullptr);
    throw AssetError{TRACE_INFO "Could not allocate PNG info object: " + fileName};
  }

  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  int depth = png_get_bit_depth(png, info);
  int channels = png_get_channels(png, info);
  if(depth != 8 or channels != 4) {
    fclose(fp);
    png_destroy_read_struct(&png, &info, nullptr);
    throw AssetError{TRACE_INFO "Unsupported PNG asset with depth = " 
      + std::to_string(depth) + " and channels = " + std::to_string(channels)};
  }
  std::vector<uint32_t> pixels(static_cast<std::size_t>(width) * height, 0);
  unsigned char ** rows = png_get_rows(png, info);
  for(int i = 0; i < height * width; ++i) {
    pixels[i] |= rows[i / width][channels * (i % width) + 0] << 24;
    pixels[i] |= rows[i / width][channels * (i % width) + 1] << 16;
    pixels[i] |= rows[i / width][channels * (i % width) + 2] <<  8;
    pixels[i] |= rows[i / width][channels * (i % width) + 3] <<  0;
  }
  fclose(fp);
  png_destroy_read_struct(&png, &info, nullptr);
  return { Rectangle{ 0, 0, width-1, height-1}, std::move(pixels) };
}
