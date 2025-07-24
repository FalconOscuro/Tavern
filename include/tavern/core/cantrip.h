#ifndef TAVERN_CANTRIP_H
#define TAVERN_CANTRIP_H


namespace tavern::core {

class cantrip
{
    // need to pass directory, scan for cantrip.yml & read in module info
    bool load_module();
    void unload_module();

    void get_module();

    bool is_module_loaded() const;


};

} /* namespace tavern::core */

#endif /* end of include guard: TAVERN_CANTRIP_H */
