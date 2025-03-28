//===-- include/flang/Runtime/io-api.h --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Defines the API of the I/O runtime support library for lowering.

#ifndef FORTRAN_RUNTIME_IO_API_H_
#define FORTRAN_RUNTIME_IO_API_H_

#include "flang/Common/uint128.h"
#include "flang/Runtime/entry-names.h"
#include "flang/Runtime/iostat-consts.h"
#include "flang/Runtime/magic-numbers.h"
#include <cinttypes>
#include <cstddef>

namespace Fortran::runtime {
class Descriptor;
} // namespace Fortran::runtime

namespace Fortran::runtime::io {

struct NonTbpDefinedIoTable;
class NamelistGroup;
class IoStatementState;
using Cookie = IoStatementState *;
using ExternalUnit = int;
using AsynchronousId = int;

static constexpr ExternalUnit DefaultOutputUnit{FORTRAN_DEFAULT_OUTPUT_UNIT};
static constexpr ExternalUnit DefaultInputUnit{FORTRAN_DEFAULT_INPUT_UNIT};

// INQUIRE specifiers are encoded as simple base-26 packings of
// the spellings of their keywords.
using InquiryKeywordHash = std::uint64_t;
constexpr InquiryKeywordHash HashInquiryKeyword(const char *p) {
  InquiryKeywordHash hash{1};
  while (char ch{*p++}) {
    std::uint64_t letter{0};
    if (ch >= 'a' && ch <= 'z') {
      letter = ch - 'a';
    } else {
      letter = ch - 'A';
    }
    hash = 26 * hash + letter;
  }
  return hash;
}

extern "C" {

#define IONAME(name) RTNAME(io##name)

#ifndef IODECL
#define IODECL(name) RT_API_ATTRS IONAME(name)
#endif

#ifndef IODEF
#define IODEF(name) RT_API_ATTRS IONAME(name)
#endif

// These functions initiate data transfer statements (READ, WRITE, PRINT).
// Example: PRINT *, 666 is implemented as the series of calls:
//   Cookie cookie{BeginExternalListOutput(DefaultOutputUnit,
//                                         __FILE__, __LINE__)};
//   OutputInteger32(cookie, 666);
//   EndIoStatement(cookie);
// Formatted I/O with explicit formats can supply the format as a
// const char * pointer with a length, or with a descriptor.

// Internal I/O initiation
// Internal I/O can loan the runtime library an optional block of memory
// in which the library can maintain state across the calls that implement
// the internal transfer; use of these blocks can reduce the need for dynamic
// memory allocation &/or thread-local storage.  The block must be sufficiently
// aligned to hold a pointer.
constexpr std::size_t RecommendedInternalIoScratchAreaBytes(
    int maxFormatParenthesesNestingDepth) {
  return 32 + 8 * maxFormatParenthesesNestingDepth;
}

// For NAMELIST I/O, use the API for the appropriate form of list-directed
// I/O initiation and configuration, then call OutputNamelist/InputNamelist
// below.

// Internal I/O to/from character arrays &/or non-default-kind character
// requires a descriptor, which is copied.
Cookie IODECL(BeginInternalArrayListOutput)(const Descriptor &,
    void **scratchArea = nullptr, std::size_t scratchBytes = 0,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginInternalArrayListInput)(const Descriptor &,
    void **scratchArea = nullptr, std::size_t scratchBytes = 0,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginInternalArrayFormattedOutput)(const Descriptor &,
    const char *format, std::size_t formatLength,
    const Descriptor *formatDescriptor = nullptr, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginInternalArrayFormattedInput)(const Descriptor &,
    const char *format, std::size_t formatLength,
    const Descriptor *formatDescriptor = nullptr, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);

// Internal I/O to/from a default-kind character scalar can avoid a
// descriptor.
Cookie IODECL(BeginInternalListOutput)(char *internal,
    std::size_t internalLength, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginInternalListInput)(const char *internal,
    std::size_t internalLength, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginInternalFormattedOutput)(char *internal,
    std::size_t internalLength, const char *format, std::size_t formatLength,
    const Descriptor *formatDescriptor = nullptr, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginInternalFormattedInput)(const char *internal,
    std::size_t internalLength, const char *format, std::size_t formatLength,
    const Descriptor *formatDescriptor = nullptr, void **scratchArea = nullptr,
    std::size_t scratchBytes = 0, const char *sourceFile = nullptr,
    int sourceLine = 0);

// External unit numbers must fit in default integers. When the integer
// provided as UNIT is of a wider type than the default integer, it could
// overflow when converted to a default integer.
// CheckUnitNumberInRange should be called to verify that a unit number of a
// wide integer type can fit in a default integer. Since it should be called
// before the BeginXXX(unit, ...) call, it has its own error handling interface.
// If handleError is false, and the unit number is out of range, the program
// will be terminated. Otherwise, if unit is out of range, a nonzero Iostat
// code is returned and ioMsg is set if it is not a nullptr.
enum Iostat IODECL(CheckUnitNumberInRange64)(std::int64_t unit,
    bool handleError, char *ioMsg = nullptr, std::size_t ioMsgLength = 0,
    const char *sourceFile = nullptr, int sourceLine = 0);
enum Iostat IODECL(CheckUnitNumberInRange128)(common::int128_t unit,
    bool handleError, char *ioMsg = nullptr, std::size_t ioMsgLength = 0,
    const char *sourceFile = nullptr, int sourceLine = 0);

// External synchronous I/O initiation
Cookie IODECL(BeginExternalListOutput)(ExternalUnit = DefaultOutputUnit,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginExternalListInput)(ExternalUnit = DefaultInputUnit,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginExternalFormattedOutput)(const char *format, std::size_t,
    const Descriptor *formatDescriptor = nullptr,
    ExternalUnit = DefaultOutputUnit, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginExternalFormattedInput)(const char *format, std::size_t,
    const Descriptor *formatDescriptor = nullptr,
    ExternalUnit = DefaultInputUnit, const char *sourceFile = nullptr,
    int sourceLine = 0);
Cookie IODECL(BeginUnformattedOutput)(ExternalUnit = DefaultOutputUnit,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginUnformattedInput)(ExternalUnit = DefaultInputUnit,
    const char *sourceFile = nullptr, int sourceLine = 0);

// WAIT(ID=)
Cookie IODECL(BeginWait)(ExternalUnit, AsynchronousId,
    const char *sourceFile = nullptr, int sourceLine = 0);
// WAIT(no ID=)
Cookie IODECL(BeginWaitAll)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);

// Other I/O statements
Cookie IODECL(BeginClose)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginFlush)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginBackspace)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginEndfile)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginRewind)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);

// OPEN(UNIT=) and OPEN(NEWUNIT=) have distinct interfaces.
Cookie IODECL(BeginOpenUnit)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginOpenNewUnit)(
    const char *sourceFile = nullptr, int sourceLine = 0);

// The variant forms of INQUIRE() statements have distinct interfaces.
// BeginInquireIoLength() is basically a no-op output statement.
Cookie IODECL(BeginInquireUnit)(
    ExternalUnit, const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginInquireFile)(const char *, std::size_t,
    const char *sourceFile = nullptr, int sourceLine = 0);
Cookie IODECL(BeginInquireIoLength)(
    const char *sourceFile = nullptr, int sourceLine = 0);

// If an I/O statement has any IOSTAT=, ERR=, END=, or EOR= specifiers,
// call EnableHandlers() immediately after the Begin...() call.
// An output or OPEN statement may not enable HasEnd or HasEor.
// This call makes the runtime library defer those particular error/end
// conditions to the EndIoStatement() call rather than terminating
// the image.  E.g., for READ(*,*,END=666) A, B, (C(J),J=1,N)
//   Cookie cookie{BeginExternalListInput(DefaultInputUnit,__FILE__,__LINE__)};
//   EnableHandlers(cookie, false, false, true /*END=*/, false);
//   if (InputReal64(cookie, &A)) {
//     if (InputReal64(cookie, &B)) {
//       for (int J{1}; J<=N; ++J) {
//         if (!InputReal64(cookie, &C[J])) break;
//       }
//     }
//   }
//   if (EndIoStatement(cookie) == FORTRAN_RUTIME_IOSTAT_END) goto label666;
void IODECL(EnableHandlers)(Cookie, bool hasIoStat = false, bool hasErr = false,
    bool hasEnd = false, bool hasEor = false, bool hasIoMsg = false);

// ASYNCHRONOUS='YES' or 'NO' on READ/WRITE/OPEN
// Use GetAsynchronousId() to handle ID=.
bool IODECL(SetAsynchronous)(Cookie, const char *, std::size_t);

// Control list options.  These return false on a error that the
// Begin...() call has specified will be handled by the caller.
// The interfaces that pass a default-kind CHARACTER argument
// are limited to passing specific case-insensitive keyword values.
// ADVANCE=YES, NO
bool IODECL(SetAdvance)(Cookie, const char *, std::size_t);
// BLANK=NULL, ZERO
bool IODECL(SetBlank)(Cookie, const char *, std::size_t);
// DECIMAL=COMMA, POINT
bool IODECL(SetDecimal)(Cookie, const char *, std::size_t);
// DELIM=APOSTROPHE, QUOTE, NONE
bool IODECL(SetDelim)(Cookie, const char *, std::size_t);
// PAD=YES, NO
bool IODECL(SetPad)(Cookie, const char *, std::size_t);
bool IODECL(SetPos)(Cookie, std::int64_t);
bool IODECL(SetRec)(Cookie, std::int64_t);
// ROUND=UP, DOWN, ZERO, NEAREST, COMPATIBLE, PROCESSOR_DEFINED
bool IODECL(SetRound)(Cookie, const char *, std::size_t);
// SIGN=PLUS, SUPPRESS, PROCESSOR_DEFINED
bool IODECL(SetSign)(Cookie, const char *, std::size_t);

// Data item transfer for modes other than NAMELIST:
// Any data object that can be passed as an actual argument without the
// use of a temporary can be transferred by means of a descriptor;
// vector-valued subscripts and coindexing will require elementwise
// transfers &/or data copies.  Unformatted transfers to/from contiguous
// blocks of local image memory can avoid the descriptor, and there
// are specializations for the most common scalar types.
//
// These functions return false when the I/O statement has encountered an
// error or end-of-file/record condition that the caller has indicated
// should not cause termination of the image by the runtime library.
// Once the statement has encountered an error, all following items will be
// ignored and also return false; but compiled code should check for errors
// and avoid the following items when they might crash.
bool IODECL(OutputDescriptor)(Cookie, const Descriptor &);
bool IODECL(InputDescriptor)(Cookie, const Descriptor &);
// Formatted (including list directed) I/O data items
bool IODECL(OutputInteger8)(Cookie, std::int8_t);
bool IODECL(OutputInteger16)(Cookie, std::int16_t);
bool IODECL(OutputInteger32)(Cookie, std::int32_t);
bool IODECL(OutputInteger64)(Cookie, std::int64_t);
bool IODECL(OutputInteger128)(Cookie, common::int128_t);
bool IODECL(InputInteger)(Cookie, std::int64_t &, int kind = 8);
bool IODECL(OutputReal32)(Cookie, float);
bool IODECL(InputReal32)(Cookie, float &);
bool IODECL(OutputReal64)(Cookie, double);
bool IODECL(InputReal64)(Cookie, double &);
bool IODECL(OutputComplex32)(Cookie, float, float);
bool IODECL(InputComplex32)(Cookie, float[2]);
bool IODECL(OutputComplex64)(Cookie, double, double);
bool IODECL(InputComplex64)(Cookie, double[2]);
bool IODECL(OutputCharacter)(Cookie, const char *, std::size_t, int kind = 1);
bool IODECL(OutputAscii)(Cookie, const char *, std::size_t);
bool IODECL(InputCharacter)(Cookie, char *, std::size_t, int kind = 1);
bool IODECL(InputAscii)(Cookie, char *, std::size_t);
bool IODECL(OutputLogical)(Cookie, bool);
bool IODECL(InputLogical)(Cookie, bool &);

// NAMELIST I/O must be the only data item in an (otherwise)
// list-directed I/O statement.
bool IODECL(OutputNamelist)(Cookie, const NamelistGroup &);
bool IODECL(InputNamelist)(Cookie, const NamelistGroup &);

// When an I/O list item has a derived type with a specific defined
// I/O subroutine of the appropriate generic kind for the active
// I/O data transfer statement (read/write, formatted/unformatted)
// that pertains to the type or its components, and those subroutines
// are dynamic or neither type-bound nor defined with interfaces
// in the same scope as the derived type (or an IMPORT statement has
// made such a generic interface inaccessible), these data item transfer
// APIs enable the I/O runtime to make the right calls to defined I/O
// subroutines.
bool IODECL(OutputDerivedType)(
    Cookie, const Descriptor &, const NonTbpDefinedIoTable *);
bool IODECL(InputDerivedType)(
    Cookie, const Descriptor &, const NonTbpDefinedIoTable *);

// Additional specifier interfaces for the connection-list of
// on OPEN statement (only).  SetBlank(), SetDecimal(),
// SetDelim(), GetIoMsg(), SetPad(), SetRound(), SetSign(),
// & SetAsynchronous() are also acceptable for OPEN.
// ACCESS=SEQUENTIAL, DIRECT, STREAM
bool IODECL(SetAccess)(Cookie, const char *, std::size_t);
// ACTION=READ, WRITE, or READWRITE
bool IODECL(SetAction)(Cookie, const char *, std::size_t);
// CARRIAGECONTROL=LIST, FORTRAN, NONE
bool IODECL(SetCarriagecontrol)(Cookie, const char *, std::size_t);
// CONVERT=NATIVE, LITTLE_ENDIAN, BIG_ENDIAN, or SWAP
bool IODECL(SetConvert)(Cookie, const char *, std::size_t);
// ENCODING=UTF-8, DEFAULT
bool IODECL(SetEncoding)(Cookie, const char *, std::size_t);
// FORM=FORMATTED, UNFORMATTED
bool IODECL(SetForm)(Cookie, const char *, std::size_t);
// POSITION=ASIS, REWIND, APPEND
bool IODECL(SetPosition)(Cookie, const char *, std::size_t);
bool IODECL(SetRecl)(Cookie, std::size_t); // RECL=

// STATUS can be set during an OPEN or CLOSE statement.
// For OPEN: STATUS=OLD, NEW, SCRATCH, REPLACE, UNKNOWN
// For CLOSE: STATUS=KEEP, DELETE
bool IODECL(SetStatus)(Cookie, const char *, std::size_t);

bool IODECL(SetFile)(Cookie, const char *, std::size_t chars);

// Acquires the runtime-created unit number for OPEN(NEWUNIT=)
bool IODECL(GetNewUnit)(Cookie, int &, int kind = 4);

// READ(SIZE=), after all input items
std::size_t IODECL(GetSize)(Cookie);

// INQUIRE(IOLENGTH=), after all output items
std::size_t IODECL(GetIoLength)(Cookie);

// GetIoMsg() does not modify its argument unless an error or
// end-of-record/file condition is present.
void IODECL(GetIoMsg)(Cookie, char *, std::size_t); // IOMSG=

// Defines ID= on READ/WRITE(ASYNCHRONOUS='YES')
AsynchronousId IODECL(GetAsynchronousId)(Cookie);

// INQUIRE() specifiers are mostly identified by their NUL-terminated
// case-insensitive names.
// ACCESS, ACTION, ASYNCHRONOUS, BLANK, CONVERT, DECIMAL, DELIM, DIRECT,
// ENCODING, FORM, FORMATTED, NAME, PAD, POSITION, READ, READWRITE, ROUND,
// SEQUENTIAL, SIGN, STREAM, UNFORMATTED, WRITE:
bool IODECL(InquireCharacter)(Cookie, InquiryKeywordHash, char *, std::size_t);
// EXIST, NAMED, OPENED, and PENDING (without ID):
bool IODECL(InquireLogical)(Cookie, InquiryKeywordHash, bool &);
// PENDING with ID
bool IODECL(InquirePendingId)(Cookie, AsynchronousId, bool &);
// NEXTREC, NUMBER, POS, RECL, SIZE
bool IODECL(InquireInteger64)(
    Cookie, InquiryKeywordHash, std::int64_t &, int kind = 8);

// This function must be called to end an I/O statement, and its
// cookie value may not be used afterwards unless it is recycled
// by the runtime library to serve a later I/O statement.
// The return value can be used to implement IOSTAT=, ERR=, END=, & EOR=;
// store it into the IOSTAT= variable if there is one, and test
// it to implement the various branches.  The error condition
// returned is guaranteed to only be one of the problems that the
// EnableHandlers() call has indicated should be handled in compiled code
// rather than by terminating the image.
enum Iostat IODECL(EndIoStatement)(Cookie);

} // extern "C"
} // namespace Fortran::runtime::io

#endif /* FORTRAN_RUNTIME_IO_API_H_ */
