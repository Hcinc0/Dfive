/*
** $Id: lgrit_lib.h $
** API definitions for gritLua.
**
** All functions defined in this header are expected to have default linkage and
** interface with other Lua libraries. For simplicity, this header also
** maintains all LuaGLM API functions under the same constraints.
**
** See Copyright Notice in lua.h
*/

#ifndef lgritlib_h
#define lgritlib_h

#include <math.h>

#include "luaconf.h"
#include "lua.h"
#include "lauxlib.h"

/* Avoid old-style-cast for C++ libraries */
#if defined(__cplusplus)
  #define cast_vec(i) static_cast<lua_VecF>((i))
#else
  #define cast_vec(i) ((lua_VecF)(i))
#endif

#if !defined(LABEL_VECTOR3)
  #define LABEL_INTEGER "integer"
  #define LABEL_NUMBER "number"
  #define LABEL_VECTOR "vector"
  #define LABEL_VECTOR1 "vector1"
  #define LABEL_VECTOR2 "vector2"
  #define LABEL_VECTOR3 "vector3"
  #define LABEL_VECTOR4 "vector4"
  #define LABEL_QUATERN "quat"
  #define LABEL_MATRIX "matrix"
#endif

/*
** {==================================================================
** LuaGLM C-API
** ===================================================================
*/

/* Constructors */

LUA_API int glmVec_vec (lua_State *L);
LUA_API int glmVec_vec1 (lua_State *L);
LUA_API int glmVec_vec2 (lua_State *L);
LUA_API int glmVec_vec3 (lua_State *L);
LUA_API int glmVec_vec4 (lua_State *L);

LUA_API int glmVec_ivec (lua_State *L);
LUA_API int glmVec_ivec1 (lua_State *L);
LUA_API int glmVec_ivec2 (lua_State *L);
LUA_API int glmVec_ivec3 (lua_State *L);
LUA_API int glmVec_ivec4 (lua_State *L);

LUA_API int glmVec_bvec (lua_State *L);
LUA_API int glmVec_bvec1 (lua_State *L);
LUA_API int glmVec_bvec2 (lua_State *L);
LUA_API int glmVec_bvec3 (lua_State *L);
LUA_API int glmVec_bvec4 (lua_State *L);

LUA_API int glmMat_mat (lua_State *L);
LUA_API int glmMat_mat2x2 (lua_State *L);
LUA_API int glmMat_mat2x3 (lua_State *L);
LUA_API int glmMat_mat2x4 (lua_State *L);
LUA_API int glmMat_mat3x2 (lua_State *L);
LUA_API int glmMat_mat3x3 (lua_State *L);
LUA_API int glmMat_mat3x4 (lua_State *L);
LUA_API int glmMat_mat4x2 (lua_State *L);
LUA_API int glmMat_mat4x3 (lua_State *L);
LUA_API int glmMat_mat4x4 (lua_State *L);

LUA_API int glmVec_qua (lua_State *L);

/* Returns the name of the type encoded by the vector variant */
LUA_API const char *glm_typename (lua_State *L, int idx);

/* Push a string representing the vector/matrix object on top of the stack. */
LUA_API const char *glm_pushstring (lua_State *L, int idx);

/*
** Unpack an individual vector and place its contents to onto the Lua stack,
** returning the number of elements (i.e., dimensions of vector).
*/
LUA_API int glm_unpack_vector (lua_State *L, int idx);

/*
** Unpack an the individual column-vectors of the given Matrix and place them
** onto the Lua stack, returning the number of elements, dimensions of vector,
** placed on the stack.
*/
LUA_API int glm_unpack_matrix (lua_State *L, int idx);

/*
** Jenkins-hash the object at the provided index. String values are hashed,
** boolean and numeric values are casted to lua_Integer; otherwise, zero is
** returned.
**
** @PARAM ignore_case: A string value is hashed as-is. Otherwise, the lowercase
**  of each string character is computed then hashed.
**
** @TODO: Possibly consider allow the potentially destructive lua_tolstring.
*/
LUA_API lua_Integer glm_tohash (lua_State *L, int idx, int ignore_case);

/* }================================================================== */

/*
** {==================================================================
** @DEPRECATED gritLua base API
** ===================================================================
*/

#define V_NOTABLE    0x0  /* Only explicit vector types can be parsed */
#define V_PARSETABLE 0x1  /* Attempt to parse table values as vectors. */
#define V_NONUMBER   0x2  /* Ignore lua_Number being the implicit VECTOR1; i.e.
                             the dimensions of the returned vector >= 2. */

/* Number of dimensions associated with the vector object */
LUA_API int luaVec_dimensions (int rtt);

/* Returns the variant of the vector if it is indeed a vector, zero otherwise */
LUA_API int lua_isvector (lua_State *L, int idx, int flags);
LUA_API int lua_tovector (lua_State *L, int idx, int flags, lua_Float4 *vector);
LUA_API void lua_pushvector (lua_State *L, lua_Float4 f4, int variant);

/* Returns true if the object at the given index is a matrix, storing its
** dimensions in size & secondary. These are extensions to the grit-lua API */
LUA_API int lua_ismatrix (lua_State *L, int idx, int *size, int *secondary);
LUA_API int lua_tomatrix (lua_State *L, int idx, lua_Mat4 *matrix);
LUA_API int lua_pushmatrix (lua_State *L, lua_Mat4 *matrix);

/*
** Jenkins-hash the object at the provided index. String values are hashed,
** boolean and numeric values are casted to lua_Integer; otherwise, zero is
** returned.
*/
LUA_API lua_Integer lua_ToHash (lua_State *L, int idx, int ignore_case);

/*
** Compatibility API: check if the object at the given index is a vector of a
** specific variant.
*/
#define lua_isvector1(L, I, F) (lua_isvector((L), (I), (F)) == LUA_VVECTOR1)
#define lua_isvector2(L, I, F) (lua_isvector((L), (I), (F)) == LUA_VVECTOR2)
#define lua_isvector3(L, I, F) (lua_isvector((L), (I), (F)) == LUA_VVECTOR3)
#define lua_isvector4(L, I, F) (lua_isvector((L), (I), (F)) == LUA_VVECTOR4)
#define lua_isquat(L, I, F) (lua_isvector((L), (I), V_NOTABLE) == LUA_VQUAT \
                             || (((F) & V_PARSETABLE) != 0 && lua_isvector((L), (I), (F)) == LUA_VVECTOR4))

/*
** Compatibility API: Populate a Float4 of an explicit type, throwing an error
** otherwise.
*/
#define _lua_checkv(L, I, F, V, T, ERR)          \
  do {                                           \
    if (lua_tovector((L), (I), (F), (V)) != (T)) \
      luaL_typeerror((L), (I), (ERR));           \
  } while (0)

#define lua_checkv1(L, I, F, V) _lua_checkv(L, I, F, V, LUA_VVECTOR1, LABEL_VECTOR1)
#define lua_checkv2(L, I, F, V) _lua_checkv(L, I, F, V, LUA_VVECTOR2, LABEL_VECTOR2)
#define lua_checkv3(L, I, F, V) _lua_checkv(L, I, F, V, LUA_VVECTOR3, LABEL_VECTOR3)
#define lua_checkv4(L, I, F, V) _lua_checkv(L, I, F, V, LUA_VVECTOR4, LABEL_VECTOR4)
#define lua_checkquat(L, I, F, V) _lua_checkv(L, I, F, V, LUA_VQUAT, LABEL_QUATERN)

/*
** Compatibility API, replaced by: a single lua_pushvector in lua.h. This macro
** avoids the use of designated initializers for C++ (e.g., MSVC C7555)
*/
#define _lua_pushvector(L, T, X, Y, Z, W)              \
  do {                                                 \
    /* Avoid use of designated initializers for C++ */ \
    lua_Float4 f4 = { (X), (Y), (Z), (W) };            \
    lua_pushvector((L), f4, (T));                      \
  } while (0)

#define lua_pushvector2(L, X, Y) _lua_pushvector(L, LUA_VVECTOR2, (X), (Y), cast_vec(0.0), cast_vec(0.0))
#define lua_pushvector3(L, X, Y, Z) _lua_pushvector(L, LUA_VVECTOR3, (X), (Y), (Z), cast_vec(0.0))
#define lua_pushvector4(L, X, Y, Z, W) _lua_pushvector(L, LUA_VVECTOR4, (X), (Y), (Z), (W))
#define lua_pushquat(L, W, X, Y, Z) _lua_pushvector(L, LUA_VQUAT, (X), (Y), (Z), (W))

/* }================================================================== */

/*
** {==================================================================
** @DEPRECATED gritLua base library compatibility
** ===================================================================
*/

/*
** Returns the dot product of x and y, i.e., result = x * y.
**   T glm::dot(qua<T, Q> const &x, qua<T, Q> const &y)
**   T glm::dot(vec<L, T, Q> const &x, vec<L, T, Q> const &y)
*/
LUA_API int glmVec_dot (lua_State *L);

/*
** Returns the cross product of x and y.
**   qua<T, Q> glm::cross(qua<T, Q> const &x, qua<T, Q> const &y)
**   vec<3, T, Q> glm::cross(qua<T, Q> const &x, vec<3, T, Q> const &y)
**   vec<3, T, Q> glm::cross(vec<3, T, Q> const &x, vec<3, T, Q> const &y)
**   vec<3, T, Q> glm::cross(vec<3, T, Q> const &x, qua<T, Q> const &y)
**   T glm::cross(vec<2, T, Q> const &x, vec<2, T, Q> const &y)
*/
LUA_API int glmVec_cross (lua_State *L);

/*
** Returns the m/q inverse.
**   qua<T, Q> glm::inverse(qua<T, Q> const &q)
**   mat<C, R, T, Q> glm::inverse(mat<C, R, T, Q> const &m)
*/
LUA_API int glmVec_inverse (lua_State *L);

/*
** Returns a vector in the same direction as x but with length of 1.
**   qua<T, Q> glm::normalize(qua<T, Q> const &x)
**   vec<L, T, Q> glm::normalize(vec<L, T, Q> const &x)
*/
LUA_API int glmVec_normalize (lua_State *L);

/*
** Spherical linear interpolation of two vectors/quaternions.
**   qua<T, Q> glm::slerp(qua<T, Q> const &x, qua<T, Q> const &y, T a)
**   vec<3, T, Q> glm::slerp(vec<3, T, Q> const &x, vec<3, T, Q> const &y, T const &a)
*/
LUA_API int glmVec_slerp (lua_State *L);

/* grit-lua math library extension */
LUA_API int glmVec_clamp (lua_State *L);

/* }================================================================== */

#endif
