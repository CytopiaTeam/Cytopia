
template <class Object> LOG &LOG::operator<<(const Object &msg)
{
  m_Logger << msg;
  return *this;
}
