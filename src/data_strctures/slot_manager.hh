#ifndef GM_SLOT_MANAGER_HH
#define GM_SLOT_MANAGER_HH


#include <cstdint>
#include <iomanip>
#include <optional>
#include <vector>

#include "settings/debug/debug_settings.hh"

#include "debug/logger/logger.hh"


namespace tgm
{



using CompleteId = uint64_t;
using SlotId = uint32_t;


class SlotManager
{
    public:
        using size_type = uint32_t;
        static size_type constexpr max_capacity = 0x7FFFFFFF;

        SlotManager(size_type const capacity);
        SlotManager(SlotManager const&) = delete;
        SlotManager(SlotManager &&) = default;
        
        auto create_id() -> std::tuple<bool, CompleteId, SlotId>;
        
        auto validate_id(CompleteId const id) -> std::optional<SlotId>;

        auto destroy_id(CompleteId const id) -> std::optional<SlotId>;


        auto first_nonFreeId() -> CompleteId
        {
            for (auto id : m_ids)
            {
                if (!is_free(id))
                    return id;
            }

            return 0;
        }


        auto debug_idsVecSize() { return m_ids.size(); }
        auto debug_freeListSize() { return m_free.size(); }

        void debug_setVersion(CompleteId & id, size_type const new_version)
        {
            auto& current_id = m_ids[slot(id)];

            CompleteId vn = new_version; //conversion from uint32_t to uint64_t

            current_id = (id & UINT64_C(0x8000000000000000)) | (vn << 31) | (id & UINT64_C(0x000000007FFFFFFF));

            m_ids[slot(id)] = current_id;

            id = current_id;
        }
        auto const& debug_idsVecRef() const { return m_ids; }
        auto debug_idsVecCopy() { return m_ids; }

        static auto debug_isFree(CompleteId const id) -> bool { return is_free(id); }
        static auto debug_version(CompleteId const id) -> size_type { return version(id); }
        static auto debug_slot(CompleteId const id) -> size_type { return slot(id); }

    private:
        size_type m_capacity = 0;							//maximum number of slots allowed
        size_type m_max_used = 0;							//number of slots ever used (including freed slots)
        size_type m_count = 0;								//number of non-free slots

        std::vector<CompleteId> m_ids;						//complete id (free | version | slot) of each sprite slot
        std::vector<size_type> m_free;						//index of free slots



        static auto is_free(CompleteId id) -> bool      { return                         id & UINT64_C(0x8000000000000000);         }
        static auto version(CompleteId id) -> size_type { return static_cast<size_type>((id & UINT64_C(0x7FFFFFFF80000000)) >> 31); }
        static auto slot(CompleteId id) -> size_type	  { return static_cast<size_type>( id & UINT64_C(0x000000007FFFFFFF)       ); }
        
        static auto setFree_and_increaseVersion(CompleteId id) -> CompleteId;

        friend auto operator<<(Logger & lgr, SlotManager const& sm) -> Logger &;
};



} // namespace tgm


#endif //GM_SLOT_MANAGER_HH