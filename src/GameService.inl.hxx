
/* Do not include this file. Include GameService.hxx instead */

template <typename ServiceType> struct GameService::require_service_type
{
  static_assert(Types::ContainsType<ServiceType>, "ServiceType must be registered as a Service in GameService::Types");
  using type = ServiceType;
};

template <typename ServiceInner, typename Tag> struct GameService::require_service_type<StrongType<ServiceInner, Tag>>
{
  static_assert(Types::ContainsType<StrongType<ServiceInner, Tag>>,
                "StrongType<ServiceInner, Tag> must be registered as a Service in GameService::Types");
  using type = ServiceInner;
};

template <typename ServiceType> ServiceType &GameService::GetService()
{
  return *std::get<typename require_service_type<ServiceType *>::type>(m_Services);
}
