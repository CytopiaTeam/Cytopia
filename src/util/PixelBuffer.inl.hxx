
template <typename Range>
PixelBuffer::PixelBuffer(const Rectangle & r, const Range & range) :
  m_Bounds(r),
  m_Pixels(range.begin(), range.end())
{
  if(static_cast<std::size_t>(r.width()) * r.height() != m_Pixels.size())
    throw CytopiaError{
        TRACE_INFO "Tried to create a PixelBuffer with non matching Rectangle and Range. Width: " + 
        std::to_string(r.width()) + "px, Height: " + std::to_string(r.height()) + "px but buffer was " +
        std::to_string(m_Pixels.size()) + "px"
    };
}

