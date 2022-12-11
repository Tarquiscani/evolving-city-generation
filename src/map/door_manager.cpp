#include "door_manager.hh"


#include <chrono>

#include "utilities.hh"


namespace tgm
{



void DoorManager::update()
{
    //Close all the doors.
    //TODO: PERFORMANCE: Close only previously opened doors to shorten this loop.
    /*for (auto & d : m_doors)
    {
        if (d.is_open())
            try_close_door(d);
    }*/


    auto & tod_queue = m_door_events->get<TryOpenDoorEv>();
    while (!tod_queue.empty())
    {
        auto& e = tod_queue.front();

        auto d = m_doors.weak_get(e.door_id);
        if (d && !d->is_open()) // The door could not exist anymore.
        {
            open_door(*d);
        }

        tod_queue.pop();
    }

    
    auto & diwad_queue = m_door_events->get<DebugInteractWithAllDoorsEv>();
    while (!diwad_queue.empty())
    {
        detail::DataArrayEl<Door>(0, Vector3i{}, false);//TODO: MSVC: This fix is necessary to use custom structured binding. Remove this fix when the bug of MSVC will be fixed.

        for (auto & [did, d] : m_doors)
        {
            if (d.is_open())
                try_close_door(d);
            else
                open_door(d);
        }


        diwad_queue.pop();
    }
}

auto DoorManager::create_door(Vector3i const& pos, bool const vertical) -> DoorId
{
    auto& el = m_doors.create(pos, vertical);

    auto spid = m_dynamic_manager.create(compute_volume(pos, vertical, false), vertical ? verticalClosed_subimage : horizontalClosed_subimage);
    el.value.set_spriteId(spid);

    return el.id();
}

void DoorManager::destroy_door(DoorId const did)
{
    auto const& d = m_doors.get_or_throw(did);

    #if PLAYERMOVEMENT_VISUALDEBUG
        if(d.is_open()) { PMdeb.add_impassableTile(d.position()); }
    #endif

    m_dynamic_manager.destroy(d.sprite_id());

    m_doors.destroy(did);
        
}

void DoorManager::open_door(Door & d)
{
    d.do_open();

    auto const pos = d.position();
    auto const vert = d.vertical();
    
    m_dynamic_manager.modify(d.sprite_id(), compute_volume(pos, vert, true), vert ? verticalOpen_subimage : horizontalOpen_subimage);

    // Reproduce a random sound
    if (Utilities::rand(100) > 80)
    {
        m_audio_manager.reproduce_sound("media/audio/open_door.wav");
    }
    else
    {
        m_audio_manager.reproduce_sound("media/audio/open_door_2.wav");
    }

    m_tiles->open_door(pos);


    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.begin_chapter("Opening door", pos);
        PMdeb.remove_impassableTile(pos);
        PMdeb.end_chapter();
    #endif
}

void DoorManager::try_close_door(Door & d)
{
    auto const pos = d.position();
    auto const& tile = m_tiles->get_existent(pos);

    if (!tile.does_host_mobiles())
    {
        d.close();
        
        auto const vert = d.vertical();

        m_dynamic_manager.modify(d.sprite_id(), compute_volume(pos, vert, false), vert ? verticalClosed_subimage : horizontalClosed_subimage);

        m_tiles->close_door(pos);
        
        
        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.begin_chapter("Closing door", pos);
            PMdeb.add_impassableTile(pos);
            PMdeb.end_chapter();
        #endif
    }
}


auto DoorManager::compute_volume(Vector3i const& pos, bool const vertical, bool const open) noexcept -> FloatParallelepiped
{
    FloatParallelepiped vol{pos.x * GSet::upt, pos.y * GSet::upt, pos.z * GSet::upt, 0.f, 0.f ,1.f * GSet::upt};

    float door_thickness = GSet::tiles_to_units(6.f / 50.f);
    float door_span = GSet::tiles_to_units(1.f);
    
    if (vertical)
    {
        if (open)
        {
            vol.left -= GSet::tiles_to_units(28.f/50.f);
            vol.length = door_thickness;
            vol.width = door_span;
        }
        else
        {
            vol.left += GSet::tiles_to_units(22.f/50.f);
            vol.length = door_span;
            vol.width = door_thickness;
        }
    }
    else
    {
        if (open)
        {
            vol.behind -= GSet::tiles_to_units(1.f/4.f);
            vol.length = door_span;
            vol.width = door_thickness;
        }
        else
        {
            vol.behind += GSet::tiles_to_units(3.f/4.f);
            vol.length = door_thickness;
            vol.width = door_span;
        }
    }

    return vol;
}


bool DoorManager::is_vertical(DoorId const did)
{
    return m_doors.get_or_throw(did).vertical();
}



} //namespace tgm