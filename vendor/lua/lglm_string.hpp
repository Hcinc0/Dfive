/*
** $Id: lglm_string.h $
** Stack-based glm/gtx/string_cast.inl implementation
** See Copyright Notice in lua.h
*/

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

/* Buffer size of format header */
#define GLM_FORMAT_BUFFER 128

/* Simplify template casting */
#define GLM_STRING_CAST(X) static_cast<typename cast<T>::value_type>((X))

/* Common format_lua_string header */
#define GLM_STRING_HEADER                                                              \
  char const *PrefixStr = prefix<T>::value();                                          \
  char const *LiteralStr = lua_literal<T, std::numeric_limits<T>::is_iec559>::value(); \
  char format_text[GLM_FORMAT_BUFFER];

/* Forward declare functor for pushing Lua strings without intermediate std::string */
namespace glm {
namespace detail {
  static int _vsnprintf(char *buff, size_t buff_len, const char *msg, ...) {
    int length = 0;

    va_list list;
    va_start(list, msg);
#if (GLM_COMPILER & GLM_COMPILER_VC)
    length = vsprintf_s(buff, buff_len, msg, list);
#else
    length = std::vsnprintf(buff, buff_len, msg, list);
#endif
    va_end(list);

    assert(length > 0);
    return length;
  }

  /* Use the predefined literal floating-point format defined in luaconf.h if possible */
  template<typename T, bool isFloat = false>
  struct lua_literal {
    static GLM_FUNC_QUALIFIER char const *value() {
      return literal<T, std::numeric_limits<T>::is_iec559>::value();
    }
  };

  template<typename T>
  struct lua_literal<T, true> {
    static GLM_FUNC_QUALIFIER char const *value() {
//#if defined(LUA_NUMBER_FMT)
//      return LUA_NUMBER_FMT;
//#else
      return literal<T, true>::value();
//#endif
    }
  };

  template<typename matType>
  struct format_lua_string { };

  template<typename T, qualifier Q>
  struct format_lua_string<vec<1, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<1, glm_Float> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%svec1(%s)", PrefixStr, LiteralStr);
      return _vsnprintf(buff, buff_len, format_text, GLM_STRING_CAST(x[0]));
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<vec<2, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<2, glm_Float> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%svec2(%s, %s)", PrefixStr, LiteralStr, LiteralStr);
      return _vsnprintf(buff, buff_len, format_text, GLM_STRING_CAST(x[0]), GLM_STRING_CAST(x[1]));
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<vec<3, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<3, glm_Float> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%svec3(%s, %s, %s)", PrefixStr, LiteralStr, LiteralStr, LiteralStr);
      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0]),
        GLM_STRING_CAST(x[1]),
        GLM_STRING_CAST(x[2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<vec<4, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, vec<4, glm_Float> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%svec4(%s, %s, %s, %s)", PrefixStr, LiteralStr, LiteralStr, LiteralStr, LiteralStr);
      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0]),
        GLM_STRING_CAST(x[1]),
        GLM_STRING_CAST(x[2]),
        GLM_STRING_CAST(x[3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<qua<T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, qua<glm_Float> const &q) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%squat(%s, {%s, %s, %s})", PrefixStr, LiteralStr, LiteralStr, LiteralStr, LiteralStr);
      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(q.w),
        GLM_STRING_CAST(q.x),
        GLM_STRING_CAST(q.y),
        GLM_STRING_CAST(q.z)
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<2, 2, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 2, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat2x2((%s, %s), (%s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<2, 3, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat2x3((%s, %s, %s), (%s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<2, 4, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<2, 4, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat2x4((%s, %s, %s, %s), (%s, %s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]), GLM_STRING_CAST(x[0][3]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2]), GLM_STRING_CAST(x[1][3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<3, 2, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 2, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat3x2((%s, %s), (%s, %s), (%s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<3, 3, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 3, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat3x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1]), GLM_STRING_CAST(x[2][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<3, 4, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<3, 4, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat3x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]), GLM_STRING_CAST(x[0][3]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2]), GLM_STRING_CAST(x[1][3]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1]), GLM_STRING_CAST(x[2][2]), GLM_STRING_CAST(x[2][3])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<4, 2, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 2, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat4x2((%s, %s), (%s, %s), (%s, %s), (%s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr,
        LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1]),
        GLM_STRING_CAST(x[3][0]), GLM_STRING_CAST(x[3][1])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<4, 3, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 3, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat4x3((%s, %s, %s), (%s, %s, %s), (%s, %s, %s), (%s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1]), GLM_STRING_CAST(x[2][2]),
        GLM_STRING_CAST(x[3][0]), GLM_STRING_CAST(x[3][1]), GLM_STRING_CAST(x[3][2])
      );
    }
  };

  template<typename T, qualifier Q>
  struct format_lua_string<mat<4, 4, T, Q>> {
    static GLM_FUNC_QUALIFIER int call(char *buff, size_t buff_len, mat<4, 4, T, Q> const &x) {
      GLM_STRING_HEADER
      _vsnprintf(format_text, GLM_FORMAT_BUFFER, "%smat4x4((%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s), (%s, %s, %s, %s))",
        PrefixStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr,
        LiteralStr, LiteralStr, LiteralStr, LiteralStr
      );

      return _vsnprintf(buff, buff_len, format_text,
        GLM_STRING_CAST(x[0][0]), GLM_STRING_CAST(x[0][1]), GLM_STRING_CAST(x[0][2]), GLM_STRING_CAST(x[0][3]),
        GLM_STRING_CAST(x[1][0]), GLM_STRING_CAST(x[1][1]), GLM_STRING_CAST(x[1][2]), GLM_STRING_CAST(x[1][3]),
        GLM_STRING_CAST(x[2][0]), GLM_STRING_CAST(x[2][1]), GLM_STRING_CAST(x[2][2]), GLM_STRING_CAST(x[2][3]),
        GLM_STRING_CAST(x[3][0]), GLM_STRING_CAST(x[3][1]), GLM_STRING_CAST(x[3][2]), GLM_STRING_CAST(x[3][3])
      );
    }
  };

  /// <summary>
  /// Return: the number of characters written if successful or negative value
  /// if an error occurred.
  /// </summary>
  template<class matType>
  static GLM_FUNC_QUALIFIER int format_type(char *buff, size_t buff_len, matType const &x) {
    return detail::format_lua_string<matType>::call(buff, buff_len, x);
  }
}
}

/*
** {==================================================================
** glm::hash implementation without std::hash dependency.
** ===================================================================
*/

namespace glm {
namespace hash {
#include <cmath>

#if !defined(l_hashfloat)
  /// <summary>
  /// ltable.c: l_hashfloat
  /// </summary>
  static int l_hashfloat(lua_Number n) {
    int i;
    lua_Integer ni;
    n = std::frexp(n, &i) * -cast_num(INT_MIN);
    if (!lua_numbertointeger(n, &ni)) { /* is 'n' inf/-inf/NaN? */
      lua_assert(luai_numisnan(n) || l_mathop(fabs)(n) == cast_num(HUGE_VAL));
      return 0;
    }
    else { /* normal case */
      unsigned int u = cast_uint(i) + cast_uint(ni);
      return cast_int(u <= cast_uint(INT_MAX) ? u : ~u);
    }
  }
#endif

  static GLM_INLINE void hash_combine(size_t &seed, size_t hash) {
    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<2, T, Q> const &v) {
    size_t seed = 0;
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.x)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.y)));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<3, T, Q> const &v) {
    size_t seed = 0;
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.x)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.y)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.z)));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::vec<4, T, Q> const &v) {
    size_t seed = 0;
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.x)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.y)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.z)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.w)));
    return seed;
  }

  template<typename T, glm::qualifier Q>
  GLM_FUNC_QUALIFIER typename std::enable_if<std::is_floating_point<T>::value, size_t>::type hash(glm::qua<T, Q> const &v) {
    size_t seed = 0;
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.x)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.y)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.z)));
    glm::hash::hash_combine(seed, l_hashfloat(static_cast<lua_Number>(v.w)));
    return seed;
  }
}
}

/* }================================================================== */

#undef GLM_STRING_HEADER
