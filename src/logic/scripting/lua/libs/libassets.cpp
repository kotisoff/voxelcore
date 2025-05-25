#include "api_lua.hpp"

#include "assets/Assets.hpp"
#include "coders/png.hpp"
#include "debug/Logger.hpp"
#include "engine/Engine.hpp"
#include "graphics/core/Texture.hpp"
#include "util/Buffer.hpp"

using namespace scripting;

static void load_texture(
    const ubyte* bytes, size_t size, const std::string& destname
) {
    try {
        engine->getAssets()->store(png::load_texture(bytes, size), destname);
    } catch (const std::runtime_error& err) {
        debug::Logger logger("lua.assetslib");
        logger.error() << err.what();
    }
}

static int l_load_texture(lua::State* L) {
    if (lua::istable(L, 1)) {
        lua::pushvalue(L, 1);
        size_t size = lua::objlen(L, 1);
        util::Buffer<ubyte> buffer(size);
        for (size_t i = 0; i < size; i++) {
            lua::rawgeti(L, i + 1);
            buffer[i] = lua::tointeger(L, -1);
            lua::pop(L);
        }
        lua::pop(L);
        load_texture(buffer.data(), buffer.size(), lua::require_string(L, 2));
    } else {
        auto string = lua::bytearray_as_string(L, 1);
        load_texture(
            reinterpret_cast<const ubyte*>(string.data()),
            string.size(),
            lua::require_string(L, 2)
        );
        lua::pop(L);
    }
    return 0;
}

const luaL_Reg assetslib[] = {
    {"load_texture", lua::wrap<l_load_texture>},
    {NULL, NULL}
};
