#ifndef RESOURCE_MANAGER_HXX_
#define RESOURCE_MANAGER_HXX_

#include <unordered_map>
#include <string>

#ifdef USE_AUDIO
#include "audio/AudioConfig.hxx"
#include "audio/Soundtrack.hxx"
#endif // USE_AUDIO
#include "../GameService.hxx"
#include "../util/Meta.hxx"

template <typename Key, typename Value>
using Mapping = std::unordered_map<Key, Value>;

class ResourceManager : public GameService
{

  template <typename ResourceType>
  struct ResourceItem
  {
    ResourceType resource;
    uint32_t age;
  };

#ifdef USE_AUDIO
  using SoundtrackResource = ResourceItem<SoundtrackUPtr>;

  AudioConfig m_audioConfig;
  Mapping<SoundtrackID, SoundtrackResource> m_soundtracks;
#endif // USE_AUDIO

  uint32_t m_Age;
  uint32_t m_CacheSize;

  /* Maximum of 150MB */
  using MAX_RESOURCE_BYTES = Constant<150000000>;

  template <typename Iterator>
  struct AgeIterator
  {
    using value_type = decltype(ResourceItem<nullptr_t>().age);
    using difference_type = size_t;
    using pointer = value_type *;
    using reference = const value_type &;
    using iterator_category = typename Iterator::iterator_category;
    AgeIterator(Iterator it) : it(it) { }
    reference operator*() const { return (it->second).age; }
    AgeIterator & operator++() { ++it; return *this; }
    bool operator!=(const AgeIterator& other) const { return other.it != it; }
    bool operator==(const AgeIterator& other) const { return other.it == it; }
  private:
    Iterator it;
  };

public:
  
  template <typename ResourceID>
  struct resource_type;

  /**
   * @brief Creates the ResourceManager 
   *  @throws ConfigurationError when loading configuration results in an error
   */
  ResourceManager(GameService::ServiceTuple &);

#ifdef USE_AUDIO
  /**
   *  @brief Fetches a soundtrack
   *  @param SoundtrackPath the path to the Soundtrack
   *  @throws AudioError when loading the file results in an error
   */
  void fetch(SoundtrackID);
#endif // USE_AUDIO

  /**
   *  @brief Fetches and return a Soundtrack
   *  @param ResourceID the id of the resource
   *  @throws AudioError when loading the file results in an error
   */
  template<typename ResourceID>
  typename resource_type<ResourceID>::type get(const ResourceID &);

  /**
   * @brief Cleans up the cache of unused resources
   */
  void prune();

};

#include "ResourceManager.inl.hxx"

#endif
