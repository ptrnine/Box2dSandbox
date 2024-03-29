#pragma once

#define SINGLETON_IMPL(CLASS) \
public: \
CLASS(const CLASS&) = delete; \
CLASS& operator=(const CLASS&) = delete; \
 \
static CLASS& instance() { \
    static CLASS inst; \
    return inst; \
}

#define DECLARE_GET(FIELD) \
auto& FIELD() const { return _##FIELD; }

#define DECLARE_SET(FIELD) \
void FIELD(const decltype(_##FIELD)& value) { _##FIELD = value; }

#define DECLARE_GET_SET(FIELD) \
DECLARE_GET(FIELD) \
DECLARE_SET(FIELD)

#define DECLARE_SMART_POINTERS_T(CLASS) \
using SharedPtr = std::shared_ptr<CLASS>; \
using UniquePtr = std::unique_ptr<CLASS>; \
using WeakPtr   = std::weak_ptr<CLASS>

#define DECLARE_SELF_FABRICS(CLASS)  \
template<typename... Args>          \
static auto createUnique(Args&&... args) { \
    return std::make_unique<CLASS>(args...); \
} \
template<typename... Args>          \
static auto createShared(Args&&... args) { \
    return std::make_shared<CLASS>(args...); \
}                                          \
DECLARE_SMART_POINTERS_T(CLASS)


#define DEBUG_VAL_LMAO(value) \
std::cout << #value ": " << (value) << std::endl
