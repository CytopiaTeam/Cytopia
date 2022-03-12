
template <typename ResourceID>
struct ResourceManager::resource_type
{
};

#ifdef USE_AUDIO
template<>
struct ResourceManager::resource_type<SoundtrackID>
{
  using type = SoundtrackUPtr &;
};
#endif // USE_AUDIO

template<typename ResourceID>
typename ResourceManager::resource_type<ResourceID>::type ResourceManager::get(const ResourceID & id)
{
  fetch(id);
#ifdef USE_AUDIO
  if constexpr(std::is_same_v<ResourceID, SoundtrackID>)
    return m_soundtracks.at(id).resource;
#endif // USE_AUDIO
}
