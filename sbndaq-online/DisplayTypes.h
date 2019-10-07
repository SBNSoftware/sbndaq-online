#ifndef _sbndaq_online_DisplayTypes_h_
#define _sbndaq_online_DisplayTypes_h_

// Macro to get the name of a binary type for bookkeeping in database entries
//
// Usage: for a binary type, binary_t, where binary_t is one of the
//        REGISTER'd types below, call:
//
//          DisplayType<binary_t>::name
//
//        This will return a const char * (null-terminated C-string) of the
//        binary type name

template<typename T>
struct DisplayType;
#define REGISTER_DISPLAY_TYPE(type) template <> struct DisplayType<type> \
    { static const char *name; }; \
    const char *DisplayType<type>::name = #type;
REGISTER_DISPLAY_TYPE(double);
REGISTER_DISPLAY_TYPE(float);
REGISTER_DISPLAY_TYPE(uint8_t);
REGISTER_DISPLAY_TYPE(uint16_t);
REGISTER_DISPLAY_TYPE(uint32_t);
REGISTER_DISPLAY_TYPE(uint64_t);
REGISTER_DISPLAY_TYPE(int8_t);
REGISTER_DISPLAY_TYPE(int16_t);
REGISTER_DISPLAY_TYPE(int32_t);
REGISTER_DISPLAY_TYPE(int64_t);

#undef REGISTER_DISPLAY_TYPE

#endif
