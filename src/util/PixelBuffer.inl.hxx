
template <typename Range>
PixelBuffer::PixelBuffer(const Rectangle &r, const Range &range) : m_Bounds(r), m_Pixels(range.begin(), range.end())
{
  if (r.width() * r.height() != m_Pixels.size())
    throw CytopiaError{TRACE_INFO "Tried to create a PixelBuffer with non matching Rectangle and Range"};
}
