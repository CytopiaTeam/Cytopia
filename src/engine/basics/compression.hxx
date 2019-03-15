#include <zlib.h>

#include <string>

// set buffer chunksize to 65kb
constexpr int CHUNK_SIZE = 65536;

/**
  * @brief Compress a given string with zlib
  * Compress the given string. Returns an empty string if something went wrong
  * @param stringToCompress String that should be compressed
  * @return std::string compressed data
  */
std::string compressString(const std::string &stringToCompress)
{
  z_stream zstream;
  std::string compressedString;
  int deflateResult = Z_OK;
  char writeBuffer[CHUNK_SIZE];

  // initialize zstream struct with zeroes, to prevent memory access violation during Initialization.
  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;

  if (deflateInit(&zstream, Z_BEST_COMPRESSION) != Z_OK)
  {
    LOG(LOG_ERROR) << "Failed to initialize zlib stream.";
    return "";
  }

  zstream.avail_in = static_cast<unsigned int>(stringToCompress.size());
  zstream.next_in = (Bytef *)stringToCompress.data();

  // run decompression until deflate stream ends
  do
  {
    zstream.next_out = reinterpret_cast<Bytef *>(writeBuffer);
    zstream.avail_out = sizeof(writeBuffer);

    deflateResult = deflate(&zstream, Z_FINISH);

    if (compressedString.size() < zstream.total_out)
    {
      compressedString.append(writeBuffer, zstream.total_out - compressedString.size());
    }
  } while (zstream.avail_out == 0);

  deflateEnd(&zstream);

  if (deflateResult != Z_STREAM_END)
  {
    LOG(LOG_ERROR) << "Error (" << deflateResult << ") while compressing file. " << zstream.msg;
    return "";
  }

  return compressedString;
}

/**
 * @brief decompresses given string
 * Decompresses a given string.
 * @param compressedString The String that should be decompressed
 * @return std::string Uncompressed string
 */
std::string decompressString(const std::string &compressedString)
{
  z_stream zstream;
  std::string uncompressedString;
  int inflateResult;
  char readBuffer[CHUNK_SIZE];

  // initialize zstream struct with zeroes, to prevent memory access violation during Initialization.
  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;

  if (inflateInit(&zstream) != Z_OK)
  {
    LOG(LOG_ERROR) << "Failed to initialize zlib stream.";
    return "";
  }

  zstream.next_in = (Bytef *)compressedString.data();
  zstream.avail_in = static_cast<unsigned int>(compressedString.size());

  // run decompression until buffer is empty
  do
  {
    zstream.next_out = reinterpret_cast<Bytef *>(readBuffer);
    zstream.avail_out = sizeof(readBuffer);

    inflateResult = inflate(&zstream, 0);

    if (uncompressedString.size() < zstream.total_out)
    {
      uncompressedString.append(readBuffer, zstream.total_out - uncompressedString.size());
    }

  } while (inflateResult == Z_OK);

  inflateEnd(&zstream);

  if (inflateResult != Z_STREAM_END)
  {
    LOG(LOG_ERROR) << "Error (" << inflateResult << ") while uncompressing file. " << zstream.msg;
    return "";
  }

  return uncompressedString;
}
