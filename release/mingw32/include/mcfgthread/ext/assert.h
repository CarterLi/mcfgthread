// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef __MCFCRT_EXT_ASSERT_H_
#define __MCFCRT_EXT_ASSERT_H_

#include "../env/_crtdef.h"

_MCFCRT_EXTERN_C_BEGIN

extern __attribute__((__noreturn__))
int __MCFCRT_OnAssertionFailure(const wchar_t *__pwszExpression, const wchar_t *__pwszFile, unsigned long __ulLine, const wchar_t *__pwszMessage) _MCFCRT_NOEXCEPT;

_MCFCRT_EXTERN_C_END

#define __MCFCRT_ASSERT_WIDEN_X(__s_)           L ## __s_
#define __MCFCRT_ASSERT_WIDEN(__s_)             __MCFCRT_ASSERT_WIDEN_X(__s_)

#endif

#undef _MCFCRT_ASSERT
#undef _MCFCRT_ASSERT_MSG

#ifdef NDEBUG
#	define _MCFCRT_ASSERT(__expr_)              ((void)(sizeof(!(__expr_))))
#	define _MCFCRT_ASSERT_MSG(__expr_, __msg_)  ((void)(sizeof(!(__expr_))))
#else
#	define _MCFCRT_ASSERT(__expr_)              ((void)(!(__expr_) && __MCFCRT_OnAssertionFailure(__MCFCRT_ASSERT_WIDEN(#__expr_), __MCFCRT_ASSERT_WIDEN(__FILE__), __LINE__, L"")))
#	define _MCFCRT_ASSERT_MSG(__expr_, __msg_)  ((void)(!(__expr_) && __MCFCRT_OnAssertionFailure(__MCFCRT_ASSERT_WIDEN(#__expr_), __MCFCRT_ASSERT_WIDEN(__FILE__), __LINE__, (__msg_))))
#endif