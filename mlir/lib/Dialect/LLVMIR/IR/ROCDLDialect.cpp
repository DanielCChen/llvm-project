//===- ROCDLDialect.cpp - ROCDL IR Ops and Dialect registration -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the types and operation details for the ROCDL IR dialect in
// MLIR, and the LLVM IR dialect.  It also registers the dialect.
//
// The ROCDL dialect only contains GPU specific additions on top of the general
// LLVM dialect.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/LLVMIR/ROCDLDialect.h"

#include "mlir/Dialect/GPU/IR/CompilationInterfaces.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Operation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace ROCDL;

#include "mlir/Dialect/LLVMIR/ROCDLOpsDialect.cpp.inc"

//===----------------------------------------------------------------------===//
// Parsing for ROCDL ops
//===----------------------------------------------------------------------===//

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.load.* %rsrc, %offset, %soffset, %aux
//     : result_type`
ParseResult RawBufferLoadOp::parse(OpAsmParser &parser,
                                   OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 4> ops;
  Type type;
  if (parser.parseOperandList(ops, 4) || parser.parseColonType(type) ||
      parser.addTypeToList(type, result.types))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);
  return parser.resolveOperands(ops, {i32x4Ty, int32Ty, int32Ty, int32Ty},
                                parser.getNameLoc(), result.operands);
}

void RawBufferLoadOp::print(OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getRes().getType();
}

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.store.* %vdata, %rsrc,  %offset,
//     %soffset, %aux : result_type`
ParseResult RawBufferStoreOp::parse(OpAsmParser &parser,
                                    OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 5> ops;
  Type type;
  if (parser.parseOperandList(ops, 5) || parser.parseColonType(type))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);

  if (parser.resolveOperands(ops, {type, i32x4Ty, int32Ty, int32Ty, int32Ty},
                             parser.getNameLoc(), result.operands))
    return failure();
  return success();
}

void RawBufferStoreOp::print(OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getVdata().getType();
}

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.atomic.fadd.* %vdata, %rsrc,  %offset,
//     %soffset, %aux : result_type`
ParseResult RawBufferAtomicFAddOp::parse(OpAsmParser &parser,
                                         OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 5> ops;
  Type type;
  if (parser.parseOperandList(ops, 5) || parser.parseColonType(type))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);

  if (parser.resolveOperands(ops, {type, i32x4Ty, int32Ty, int32Ty, int32Ty},
                             parser.getNameLoc(), result.operands))
    return failure();
  return success();
}

void RawBufferAtomicFAddOp::print(mlir::OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getVdata().getType();
}

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.atomic.fmax.* %vdata, %rsrc,  %offset,
//     %soffset, %aux : result_type`
ParseResult RawBufferAtomicFMaxOp::parse(OpAsmParser &parser,
                                         OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 5> ops;
  Type type;
  if (parser.parseOperandList(ops, 5) || parser.parseColonType(type))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);

  if (parser.resolveOperands(ops, {type, i32x4Ty, int32Ty, int32Ty, int32Ty},
                             parser.getNameLoc(), result.operands))
    return failure();
  return success();
}

void RawBufferAtomicFMaxOp::print(mlir::OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getVdata().getType();
}

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.atomic.smax.* %vdata, %rsrc,  %offset,
//     %soffset, %aux : result_type`
ParseResult RawBufferAtomicSMaxOp::parse(OpAsmParser &parser,
                                         OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 5> ops;
  Type type;
  if (parser.parseOperandList(ops, 5) || parser.parseColonType(type))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);

  if (parser.resolveOperands(ops, {type, i32x4Ty, int32Ty, int32Ty, int32Ty},
                             parser.getNameLoc(), result.operands))
    return failure();
  return success();
}

void RawBufferAtomicSMaxOp::print(mlir::OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getVdata().getType();
}

// <operation> ::=
//     `llvm.amdgcn.raw.buffer.atomic.umin.* %vdata, %rsrc,  %offset,
//     %soffset, %aux : result_type`
ParseResult RawBufferAtomicUMinOp::parse(OpAsmParser &parser,
                                         OperationState &result) {
  SmallVector<OpAsmParser::UnresolvedOperand, 5> ops;
  Type type;
  if (parser.parseOperandList(ops, 5) || parser.parseColonType(type))
    return failure();

  auto bldr = parser.getBuilder();
  auto int32Ty = bldr.getI32Type();
  auto i32x4Ty = VectorType::get({4}, int32Ty);

  if (parser.resolveOperands(ops, {type, i32x4Ty, int32Ty, int32Ty, int32Ty},
                             parser.getNameLoc(), result.operands))
    return failure();
  return success();
}

void RawBufferAtomicUMinOp::print(mlir::OpAsmPrinter &p) {
  p << " " << getOperands() << " : " << getVdata().getType();
}

//===----------------------------------------------------------------------===//
// ROCDLDialect initialization, type parsing, and registration.
//===----------------------------------------------------------------------===//

// TODO: This should be the llvm.rocdl dialect once this is supported.
void ROCDLDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "mlir/Dialect/LLVMIR/ROCDLOps.cpp.inc"
      >();

  addAttributes<
#define GET_ATTRDEF_LIST
#include "mlir/Dialect/LLVMIR/ROCDLOpsAttributes.cpp.inc"
      >();

  // Support unknown operations because not all ROCDL operations are registered.
  allowUnknownOperations();
  declarePromisedInterface<gpu::TargetAttrInterface, ROCDLTargetAttr>();
}

LogicalResult ROCDLDialect::verifyOperationAttribute(Operation *op,
                                                     NamedAttribute attr) {
  // Kernel function attribute should be attached to functions.
  if (kernelAttrName.getName() == attr.getName()) {
    if (!isa<LLVM::LLVMFuncOp>(op)) {
      return op->emitError() << "'" << kernelAttrName.getName()
                             << "' attribute attached to unexpected op";
    }
  }
  return success();
}

//===----------------------------------------------------------------------===//
// ROCDL target attribute.
//===----------------------------------------------------------------------===//
LogicalResult
ROCDLTargetAttr::verify(function_ref<InFlightDiagnostic()> emitError,
                        int optLevel, StringRef triple, StringRef chip,
                        StringRef features, StringRef abiVersion,
                        DictionaryAttr flags, ArrayAttr files) {
  if (optLevel < 0 || optLevel > 3) {
    emitError() << "The optimization level must be a number between 0 and 3.";
    return failure();
  }
  if (triple.empty()) {
    emitError() << "The target triple cannot be empty.";
    return failure();
  }
  if (chip.empty()) {
    emitError() << "The target chip cannot be empty.";
    return failure();
  }
  if (abiVersion != "400" && abiVersion != "500" && abiVersion != "600") {
    emitError() << "Invalid ABI version, it must be `400`, `500` or '600'.";
    return failure();
  }
  if (files && !llvm::all_of(files, [](::mlir::Attribute attr) {
        return attr && mlir::isa<StringAttr>(attr);
      })) {
    emitError() << "All the elements in the `link` array must be strings.";
    return failure();
  }
  return success();
}

#define GET_OP_CLASSES
#include "mlir/Dialect/LLVMIR/ROCDLOps.cpp.inc"

#define GET_ATTRDEF_CLASSES
#include "mlir/Dialect/LLVMIR/ROCDLOpsAttributes.cpp.inc"
