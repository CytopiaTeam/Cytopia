
template <typename ResourceID>
struct ResourceManager::resource_type
{
};

template<>
struct ResourceManager::resource_type<SoundtrackID>
{
  using type = SoundtrackUPtr &;
};

template<typename ResourceID>
typename ResourceManager::resource_type<ResourceID>::type ResourceManager::get(const ResourceID & id)
{
  fetch(id);
  if constexpr(std::is_same_v<ResourceID, SoundtrackID>)
    return m_soundtracks.at(id).resource;
}
