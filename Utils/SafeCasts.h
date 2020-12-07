#ifndef SAFECASTS_H
#define SAFECASTS_H

#define RGM_BEGIN_SAFE_CAST(SafeCast, BaseType_)                   \
    template<typename T> class SafeCast {                          \
     public:                                                       \
      static constexpr bool kCastSupported = false;                \
      typedef BaseType_ BaseType;                                  \
    };                                                             \
    RGM_DECLARE_SAFE_CAST(SafeCast, BaseType_);                    \
    BaseType_* SafeCast<BaseType_>::Cast(BaseType_ *x) { return x; }

#define RGM_DECLARE_SAFE_CAST(SafeCast, To)         \
    template<> class SafeCast<To> {                 \
     public:                                        \
      static constexpr bool kCastSupported = true;  \
      static inline To* Cast(SafeCast<void>::BaseType *x); \
    }

#define RGM_IMPLEMENT_SAFE_CAST(SafeCast, To) \
    To* SafeCast<To>::Cast(SafeCast<void>::BaseType *x) { return x->As ## To(); }

#endif // SAFECASTS_H
