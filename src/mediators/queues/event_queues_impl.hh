#ifndef GM_EVENT_CONTAINER_IMPL_HH
#define GM_EVENT_CONTAINER_IMPL_HH


#include <memory>
#include <queue>

#include "base_event.hh"


template <typename B, typename ...Ts>
bool constexpr all_base_of()
{
    return (std::conjunction_v<std::is_base_of<B, Ts>...>);
}

template <typename ...Ts>
class EventQueuesImpl
{
	static_assert(all_base_of<BaseEvent, Ts...>(), "EventQueues has been designed only for events.");
	//TODO: Make an assert to force all types to be different (unique)
	static_assert(true, "Types must be all different."); 


	public:
		EventQueuesImpl() = default;
		EventQueuesImpl(EventQueuesImpl const&) = delete;
		auto operator=(EventQueuesImpl const&) -> EventQueuesImpl = delete;

		template<typename T, typename ...Us>
		void push(Us... args)
		{
			std::get<std::queue<T>>(queues).emplace(std::forward<Us>(args)...);
		}


		template<typename T>
		auto get() noexcept -> std::queue<T> &
		{
			return std::get<std::queue<T>>(queues);
		}
	

	private:
		std::tuple<std::queue<Ts>...> queues;
};


#endif //GM_EVENT_CONTAINER_IMPL_HH
