#ifndef GM_GUI_EV_HH
#define GM_GUI_EV_HH


#include "base_event.hh"
#include "event_queues_impl.hh"

#include "map/map_forward_decl.hh"
#include "map/city_block.hh"


namespace tgm
{



struct SaveWorldEv : public BaseEvent 
{ 
    SaveWorldEv(std::string const& a_filename) : filename{ a_filename } {}

    std::string const filename;
};


struct LoadWorldEv : public BaseEvent 
{
    LoadWorldEv(std::string const& a_filename) : filename{ a_filename } {}

    std::string const filename;
};

struct ExitEv : public BaseEvent { };


struct MainLoopAnalyzerEv : public BaseEvent { };
struct MovementAnalyzerEv : public BaseEvent { };
struct ControlPanelEv : public BaseEvent { };


struct RetrieveCityBlockEv : public BaseEvent
{
    RetrieveCityBlockEv(bool const a_user_request, CityBlockId const a_cbid) :
        user_request{ a_user_request }, cbid{ a_cbid } {}
        
    bool const user_request;
    CityBlockId const cbid;
};


struct OpenCityBlockGuiEv : public BaseEvent
{
    OpenCityBlockGuiEv(bool const a_user_request, CityBlockId const a_cbid, CityBlock const*const a_city_block) :
        user_request{ a_user_request }, cbid{ a_cbid }, city_block { a_city_block } {}
        
    bool const user_request;
    CityBlockId const cbid;
    CityBlock const*const city_block;
};


using GuiEventQueues = EventQueuesImpl< SaveWorldEv, LoadWorldEv, ExitEv, 
                                        MainLoopAnalyzerEv, MovementAnalyzerEv, ControlPanelEv,
                                        RetrieveCityBlockEv, OpenCityBlockGuiEv >;



}


#endif //GM_GUI_EV_HH