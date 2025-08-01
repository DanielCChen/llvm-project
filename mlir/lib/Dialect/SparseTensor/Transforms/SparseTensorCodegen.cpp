//===- SparseTensorCodegen.cpp - Sparse tensor primitives conversion ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A pass that converts sparse tensor types and primitives to actual compiler
// visible buffers and actual compiler IR that implements these primitives on
// the selected sparse tensor storage schemes. This pass provides an alternative
// to the SparseTensorConversion pass, eliminating the dependence on a runtime
// support library (other than for file I/O), and providing many more
// opportunities for subsequent compiler optimization of the generated code.
//
//===----------------------------------------------------------------------===//

#include "Utils/CodegenUtils.h"
#include "Utils/SparseTensorDescriptor.h"

#include "mlir/Dialect/Arith/Utils/Utils.h"
#include "mlir/Dialect/Bufferization/IR/Bufferization.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/Utils/Utils.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SparseTensor/IR/Enums.h"
#include "mlir/Dialect/SparseTensor/IR/SparseTensor.h"
#include "mlir/Dialect/SparseTensor/IR/SparseTensorType.h"
#include "mlir/Dialect/SparseTensor/Transforms/Passes.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Transforms/DialectConversion.h"

#include <optional>

using namespace mlir;
using namespace mlir::sparse_tensor;

//===----------------------------------------------------------------------===//
// Helper methods.
//===----------------------------------------------------------------------===//

/// Flatten the given value ranges into a single vector of values.
static SmallVector<Value> flattenValues(ArrayRef<ValueRange> values) {
  SmallVector<Value> result;
  for (const auto &vals : values)
    llvm::append_range(result, vals);
  return result;
}

/// Generates a load with proper `index` typing.
static Value genLoad(OpBuilder &builder, Location loc, Value mem, Value idx) {
  idx = genCast(builder, loc, idx, builder.getIndexType());
  return memref::LoadOp::create(builder, loc, mem, idx);
}

/// Generates a store with proper `index` typing and proper value.
static void genStore(OpBuilder &builder, Location loc, Value val, Value mem,
                     Value idx) {
  idx = genCast(builder, loc, idx, builder.getIndexType());
  val = genCast(builder, loc, val,
                cast<ShapedType>(mem.getType()).getElementType());
  memref::StoreOp::create(builder, loc, val, mem, idx);
}

/// Creates a straightforward counting for-loop.
static scf::ForOp createFor(OpBuilder &builder, Location loc, Value upper,
                            MutableArrayRef<Value> fields,
                            Value lower = Value()) {
  Type indexType = builder.getIndexType();
  if (!lower)
    lower = constantZero(builder, loc, indexType);
  Value one = constantOne(builder, loc, indexType);
  scf::ForOp forOp =
      scf::ForOp::create(builder, loc, lower, upper, one, fields);
  for (unsigned i = 0, e = fields.size(); i < e; i++)
    fields[i] = forOp.getRegionIterArg(i);
  builder.setInsertionPointToStart(forOp.getBody());
  return forOp;
}

/// Creates a push back operation.
static void createPushback(OpBuilder &builder, Location loc,
                           MutSparseTensorDescriptor desc,
                           SparseTensorFieldKind kind, std::optional<Level> lvl,
                           Value value, Value repeat = Value()) {
  Type etp = desc.getMemRefElementType(kind, lvl);
  Value field = desc.getMemRefField(kind, lvl);
  StorageSpecifierKind specFieldKind = toSpecifierKind(kind);

  auto pushBackOp = PushBackOp::create(
      builder, loc, desc.getSpecifierField(builder, loc, specFieldKind, lvl),
      field, genCast(builder, loc, value, etp), repeat);

  desc.setMemRefField(kind, lvl, pushBackOp.getOutBuffer());
  desc.setSpecifierField(builder, loc, specFieldKind, lvl,
                         pushBackOp.getNewSize());
}

/// Generates code that allocates a sparse storage scheme for given rank.
static void allocSchemeForRank(OpBuilder &builder, Location loc,
                               MutSparseTensorDescriptor desc, Level startLvl) {
  const SparseTensorType stt(desc.getRankedTensorType());
  Value linear = constantIndex(builder, loc, 1);
  const Level lvlRank = stt.getLvlRank();
  for (Level lvl = startLvl; lvl < lvlRank; lvl++) {
    const auto lt = stt.getLvlType(lvl);
    if (isCompressedLT(lt) || isLooseCompressedLT(lt)) {
      // Append linear x positions, initialized to zero. Since each compressed
      // dimension initially already has a single zero entry, this maintains
      // the desired "linear + 1" length property at all times. For loose
      // compression, we multiply linear by two in order to append both the
      // lo/hi positions.
      Value posZero = constantZero(builder, loc, stt.getPosType());
      if (isLooseCompressedLT(lt)) {
        Value two = constantIndex(builder, loc, 2);
        linear = arith::MulIOp::create(builder, loc, linear, two);
      }
      createPushback(builder, loc, desc, SparseTensorFieldKind::PosMemRef, lvl,
                     /*value=*/posZero, /*repeat=*/linear);
      return;
    } else if (isSingletonLT(lt) || isNOutOfMLT(lt)) {
      return; // nothing to do
    }
    // Keep compounding the size, but nothing needs to be initialized
    // at this level. We will eventually reach a compressed level or
    // otherwise the values array for the from-here "all-dense" case.
    assert(isDenseLT(lt));
    Value size = desc.getLvlSize(builder, loc, lvl);
    linear = arith::MulIOp::create(builder, loc, linear, size);
  }
  // Reached values array so prepare for an insertion.
  Value valZero = constantZero(builder, loc, stt.getElementType());
  createPushback(builder, loc, desc, SparseTensorFieldKind::ValMemRef,
                 std::nullopt, /*value=*/valZero, /*repeat=*/linear);
}

/// Creates allocation operation.
static Value createAllocation(OpBuilder &builder, Location loc,
                              MemRefType memRefType, Value sz,
                              bool enableInit) {
  Value buffer = memref::AllocOp::create(builder, loc, memRefType, sz);
  Type elemType = memRefType.getElementType();
  if (enableInit) {
    Value fillValue = constantZero(builder, loc, elemType);
    linalg::FillOp::create(builder, loc, fillValue, buffer);
  }
  return buffer;
}

/// Creates the dim sizes array, filling in from dynamic sizes.
static void createDimSizes(OpBuilder &builder, Location loc,
                           SparseTensorType stt, ValueRange dynSizes,
                           /*out*/ SmallVectorImpl<Value> &dimSizesValues) {
  const Dimension dimRank = stt.getDimRank();
  dimSizesValues.clear();
  dimSizesValues.reserve(dimRank);
  unsigned i = 0;
  for (const Size sz : stt.getDimShape())
    dimSizesValues.push_back(ShapedType::isDynamic(sz)
                                 ? dynSizes[i++]
                                 : constantIndex(builder, loc, sz));
}

/// Creates allocation for each field in sparse tensor type. Note that
/// for all dynamic memrefs in the sparse tensor stroage layout, the
/// memory size is really the capacity of the "vector", while the actual
/// size resides in the sizes array.
static void createAllocFields(OpBuilder &builder, Location loc,
                              SparseTensorType stt, bool enableInit,
                              Value sizeHint,
                              SmallVectorImpl<Value> &lvlSizesValues,
                              /*out*/ SmallVectorImpl<Value> &fields) {
  Level lvlRank = stt.getLvlRank();
  // Set up some heuristic sizes. We try to set the initial
  // size based on available information. Otherwise we just
  // initialize a few elements to start the reallocation chain.
  // TODO: refine this
  Value posHeuristic, crdHeuristic, valHeuristic;
  if (stt.isAllDense()) {
    valHeuristic = lvlSizesValues[0];
    for (Level lvl = 1; lvl < lvlRank; lvl++)
      valHeuristic = arith::MulIOp::create(builder, loc, valHeuristic,
                                           lvlSizesValues[lvl]);
  } else if (sizeHint) {
    if (stt.getAoSCOOStart() == 0) {
      posHeuristic = constantIndex(builder, loc, 2);
      crdHeuristic = arith::MulIOp::create(
          builder, loc, constantIndex(builder, loc, lvlRank), sizeHint); // AOS
    } else if (lvlRank == 2 && stt.isDenseLvl(0) && stt.isCompressedLvl(1)) {
      posHeuristic = arith::AddIOp::create(builder, loc, sizeHint,
                                           constantIndex(builder, loc, 1));
      crdHeuristic = sizeHint;
    } else {
      posHeuristic = crdHeuristic = constantIndex(builder, loc, 16);
    }
    valHeuristic = sizeHint;
  } else {
    posHeuristic = crdHeuristic = valHeuristic =
        constantIndex(builder, loc, 16);
  }
  // Initializes all fields. An initial storage specifier and allocated
  // positions/coordinates/values memrefs (with heuristic capacity).
  foreachFieldAndTypeInSparseTensor(
      stt,
      [&builder, &fields, stt, loc, posHeuristic, crdHeuristic, valHeuristic,
       enableInit](Type fType, FieldIndex fIdx, SparseTensorFieldKind fKind,
                   Level /*lvl*/, LevelType /*lt*/) -> bool {
        assert(fields.size() == fIdx);
        Value field;
        switch (fKind) {
        case SparseTensorFieldKind::StorageSpec:
          field = SparseTensorSpecifier::getInitValue(builder, loc, stt);
          break;
        case SparseTensorFieldKind::PosMemRef:
          field = createAllocation(builder, loc, cast<MemRefType>(fType),
                                   posHeuristic, enableInit);
          break;
        case SparseTensorFieldKind::CrdMemRef:
          field = createAllocation(builder, loc, cast<MemRefType>(fType),
                                   crdHeuristic, enableInit);
          break;
        case SparseTensorFieldKind::ValMemRef:
          field = createAllocation(builder, loc, cast<MemRefType>(fType),
                                   valHeuristic, enableInit);
          break;
        }
        assert(field);
        fields.push_back(field);
        // Returns true to continue the iteration.
        return true;
      });
  // Initialize the storage scheme to an empty tensor. Sets the lvlSizes
  // and gives all position fields an initial zero entry, so that it is
  // easier to maintain the "linear + 1" length property.
  MutSparseTensorDescriptor desc(stt, fields);
  Value posZero = constantZero(builder, loc, stt.getPosType());
  for (Level lvl = 0, lvlRank = stt.getLvlRank(); lvl < lvlRank; lvl++) {
    desc.setLvlSize(builder, loc, lvl, lvlSizesValues[lvl]);
    const auto lt = stt.getLvlType(lvl);
    if (isCompressedLT(lt) || isLooseCompressedLT(lt))
      createPushback(builder, loc, desc, SparseTensorFieldKind::PosMemRef, lvl,
                     /*value=*/posZero);
  }
  allocSchemeForRank(builder, loc, desc, /*rank=*/0);
}

/// Helper method that generates block specific to compressed case:
///
///  // given: parentPos = posCursor[lvl-1]
///  pstart = desc.positions[lvl][parentPos]
///  pstop = desc.positions[lvl][parentPos+1]
///  plast = pstop - 1
///  msz = desc.coordinates[lvl].size()
///  if (pstart < pstop) {
///    isPresent = (desc.coordinates[lvl][plast] == lvlCoords[lvl])
///  } else { // first insertion
///    isPresent = false
///    desc.positions[lvl][parentPos] = msz
///  }
///  if (isPresent) { // coordinate is already present
///    pnext = plast
///  } else {
///    desc.coordinates[lvl].push_back(lvlCoords[lvl])
///    desc.positions[lvl][parentPos+1] = msz+1
///    pnext = msz
///    <prepare level lvl+1>
///  }
///  posCursor[lvl] = pnext
static Value genCompressed(OpBuilder &builder, Location loc,
                           MutSparseTensorDescriptor desc, ValueRange lvlCoords,
                           Value /*unused*/, Value parentPos, Level lvl) {
  const SparseTensorType stt(desc.getRankedTensorType());
  const Level lvlRank = stt.getLvlRank();
  assert(lvl < lvlRank && "Level is out of bounds");
  assert(lvlCoords.size() == static_cast<size_t>(lvlRank) &&
         "Level-rank mismatch");
  SmallVector<Type> types;
  Type indexType = builder.getIndexType();
  Type boolType = builder.getIntegerType(1);
  unsigned crdFidx;
  unsigned crdStride;
  std::tie(crdFidx, crdStride) = desc.getCrdMemRefIndexAndStride(lvl);
  const Value one = constantIndex(builder, loc, 1);
  const Value pp1 = arith::AddIOp::create(builder, loc, parentPos, one);
  const Value positionsAtLvl = desc.getPosMemRef(lvl);
  const Value pstart = genLoad(builder, loc, positionsAtLvl, parentPos);
  const Value pstop = genLoad(builder, loc, positionsAtLvl, pp1);
  const Value crdMsz = desc.getCrdMemSize(builder, loc, lvl);
  const Value crdStrideC =
      crdStride > 1 ? constantIndex(builder, loc, crdStride) : Value();
  const Value msz =
      crdStrideC ? arith::DivUIOp::create(builder, loc, crdMsz, crdStrideC)
                 : crdMsz;
  const Value plast = arith::SubIOp::create(
      builder, loc, genCast(builder, loc, pstop, indexType), one);
  // Conditional expression.
  Value lt = arith::CmpIOp::create(builder, loc, arith::CmpIPredicate::ult,
                                   pstart, pstop);
  types.push_back(boolType);
  scf::IfOp ifOp1 = scf::IfOp::create(builder, loc, types, lt, /*else*/ true);
  types.pop_back();
  builder.setInsertionPointToStart(&ifOp1.getThenRegion().front());
  Value crd = genLoad(
      builder, loc, desc.getMemRefField(crdFidx),
      crdStrideC ? arith::MulIOp::create(builder, loc, plast, crdStrideC)
                 : plast);
  Value eq = arith::CmpIOp::create(builder, loc, arith::CmpIPredicate::eq,
                                   genCast(builder, loc, crd, indexType),
                                   lvlCoords[lvl]);
  scf::YieldOp::create(builder, loc, eq);
  builder.setInsertionPointToStart(&ifOp1.getElseRegion().front());
  if (lvl > 0)
    genStore(builder, loc, msz, positionsAtLvl, parentPos);
  scf::YieldOp::create(builder, loc, constantI1(builder, loc, false));
  builder.setInsertionPointAfter(ifOp1);
  // If present construct. Note that for a non-unique dimension level, we
  // simply set the condition to false and rely on CSE/DCE to clean up the IR.
  //
  // TODO: generate less temporary IR?
  //
  for (unsigned i = 0, e = desc.getNumFields(); i < e; i++)
    types.push_back(desc.getField(i).getType());
  types.push_back(indexType);
  const Value p = stt.isUniqueLvl(lvl) ? ifOp1.getResult(0)
                                       : constantI1(builder, loc, false);
  scf::IfOp ifOp2 = scf::IfOp::create(builder, loc, types, p, /*else*/ true);
  // If present (fields unaffected, update pnext to plast).
  builder.setInsertionPointToStart(&ifOp2.getThenRegion().front());

  // FIXME: This does not looks like a clean way, but probably the most
  // efficient way.
  desc.getFields().push_back(plast);
  scf::YieldOp::create(builder, loc, desc.getFields());
  desc.getFields().pop_back();

  // If !present (changes fields, update pnext).
  builder.setInsertionPointToStart(&ifOp2.getElseRegion().front());
  Value mszp1 = arith::AddIOp::create(builder, loc, msz, one);
  genStore(builder, loc, mszp1, positionsAtLvl, pp1);
  createPushback(builder, loc, desc, SparseTensorFieldKind::CrdMemRef, lvl,
                 /*value=*/lvlCoords[lvl]);
  // Prepare the next level "as needed".
  if ((lvl + 1) < lvlRank)
    allocSchemeForRank(builder, loc, desc, lvl + 1);

  desc.getFields().push_back(msz);
  scf::YieldOp::create(builder, loc, desc.getFields());
  desc.getFields().pop_back();

  // Update fields and return next pos.
  builder.setInsertionPointAfter(ifOp2);
  unsigned o = 0;
  for (unsigned i = 0, e = desc.getNumFields(); i < e; i++)
    desc.setField(i, ifOp2.getResult(o++));
  return ifOp2.getResult(o);
}

/// Generates insertion finalization code.
static void genEndInsert(OpBuilder &builder, Location loc,
                         SparseTensorDescriptor desc) {
  const SparseTensorType stt(desc.getRankedTensorType());
  const Level lvlRank = stt.getLvlRank();
  for (Level lvl = 0; lvl < lvlRank; lvl++) {
    const auto lt = stt.getLvlType(lvl);
    if (isCompressedLT(lt)) {
      // Compressed dimensions need a position cleanup for all entries
      // that were not visited during the insertion pass.
      //
      // TODO: avoid cleanup and keep compressed scheme consistent at all
      // times?
      //
      if (lvl > 0) {
        Type posType = stt.getPosType();
        Value posMemRef = desc.getPosMemRef(lvl);
        Value hi = desc.getPosMemSize(builder, loc, lvl);
        Value zero = constantIndex(builder, loc, 0);
        Value one = constantIndex(builder, loc, 1);
        // Vector of only one, but needed by createFor's prototype.
        SmallVector<Value, 1> inits{genLoad(builder, loc, posMemRef, zero)};
        scf::ForOp loop = createFor(builder, loc, hi, inits, one);
        Value i = loop.getInductionVar();
        Value oldv = loop.getRegionIterArg(0);
        Value newv = genLoad(builder, loc, posMemRef, i);
        Value posZero = constantZero(builder, loc, posType);
        Value cond = arith::CmpIOp::create(
            builder, loc, arith::CmpIPredicate::eq, newv, posZero);
        scf::IfOp ifOp = scf::IfOp::create(builder, loc, TypeRange(posType),
                                           cond, /*else*/ true);
        builder.setInsertionPointToStart(&ifOp.getThenRegion().front());
        genStore(builder, loc, oldv, posMemRef, i);
        scf::YieldOp::create(builder, loc, oldv);
        builder.setInsertionPointToStart(&ifOp.getElseRegion().front());
        scf::YieldOp::create(builder, loc, newv);
        builder.setInsertionPointAfter(ifOp);
        scf::YieldOp::create(builder, loc, ifOp.getResult(0));
        builder.setInsertionPointAfter(loop);
      }
    } else {
      assert(isDenseLT(lt) || isLooseCompressedLT(lt) || isSingletonLT(lt) ||
             isNOutOfMLT(lt));
    }
  }
}

/// Generates a subview into the sizes.
static Value genSliceToSize(OpBuilder &builder, Location loc, Value mem,
                            Value sz) {
  auto memTp = llvm::cast<MemRefType>(mem.getType());
  // For higher-dimensional memrefs, we assume that the innermost
  // dimension is always of the right size.
  // TODO: generate complex truncating view here too?
  if (memTp.getRank() > 1)
    return mem;
  // Truncate linear memrefs to given size.
  return memref::SubViewOp::create(
             builder, loc,
             MemRefType::get({ShapedType::kDynamic}, memTp.getElementType()),
             mem, ValueRange{}, ValueRange{sz}, ValueRange{},
             ArrayRef<int64_t>{0},                    // static offset
             ArrayRef<int64_t>{ShapedType::kDynamic}, // dynamic size
             ArrayRef<int64_t>{1})                    // static stride
      .getResult();
}

/// Creates the reassociation array.
static SmallVector<ReassociationIndices>
getReassociationForFlattening(ShapedType srcTp, unsigned batchLvls) {
  SmallVector<ReassociationIndices> ret(batchLvls + 1, {});
  // Create reassociation in the form:
  // {0}, {1}, ..., {batchLvl - 1}, {batchLvl, ..., rank}
  for (unsigned i = 0; i < batchLvls; i++)
    ret[i].push_back(i);

  for (int i = batchLvls, e = srcTp.getRank(); i < e; i++)
    ret.back().push_back(i);

  return ret;
}

//===----------------------------------------------------------------------===//
// Codegen rules.
//===----------------------------------------------------------------------===//

namespace {

/// Helper class to help lowering sparse_tensor.insert operation.
class SparseInsertGenerator
    : public FuncCallOrInlineGenerator<SparseInsertGenerator> {
public:
  SparseInsertGenerator(TensorType rtp, TypeRange retTypes, ValueRange params,
                        bool genCall)
      : FuncCallOrInlineGenerator(retTypes, params, genCall), rtp(rtp) {};

  /// Generates code along an insertion path without the need for a "cursor".
  /// This current insertion strategy comes at the expense of some testing
  /// overhead for each insertion. The strategy will be optimized later for
  /// common insertion patterns. The current insertion strategy also assumes
  /// insertions occur in "a reasonable order" that enables building the
  /// storage scheme in an appending/inserting kind of fashion (i.e. no
  /// in-between insertions that need data movement). The implementation
  /// relies on CSE/DCE to clean up all bookkeeping that is not needed.
  ///
  /// TODO: better unord/not-unique; also generalize, optimize, specialize!
  SmallVector<Value> genImplementation(TypeRange retTypes, ValueRange args,
                                       OpBuilder &builder, Location loc) {
    const SparseTensorType stt(llvm::cast<RankedTensorType>(rtp));
    const Level lvlRank = stt.getLvlRank();
    // Extract fields and coordinates from args.
    SmallVector<Value> fields = llvm::to_vector(args.drop_back(lvlRank + 1));
    MutSparseTensorDescriptor desc(stt, fields);
    const SmallVector<Value> coords =
        llvm::to_vector(args.take_back(lvlRank + 1).drop_back());
    Value value = args.back();
    Value parentPos = constantZero(builder, loc, builder.getIndexType());
    // Generate code for every level.
    for (Level lvl = 0; lvl < lvlRank; lvl++) {
      const auto lt = stt.getLvlType(lvl);
      if (isCompressedLT(lt) || isLooseCompressedLT(lt)) {
        // Create:
        //   if (!present) {
        //     coordinates[lvl].push_back(coords[lvl])
        //     <update positions and prepare level lvl + 1>
        //   }
        //   positions[lvl] = coordinates.size() - 1
        //   <insert @ positions[lvl] at next level lvl + 1>
        if (isLooseCompressedLT(lt)) {
          Value two = constantIndex(builder, loc, 2);
          parentPos = arith::MulIOp::create(builder, loc, parentPos, two);
        }
        parentPos =
            genCompressed(builder, loc, desc, coords, value, parentPos, lvl);
      } else if (isSingletonLT(lt) || isNOutOfMLT(lt)) {
        // Create:
        //   coordinates[lvl].push_back(coords[lvl])
        //   positions[lvl] = positions[lvl-1]
        //   <insert @ positions[lvl] at next level lvl + 1>
        createPushback(builder, loc, desc, SparseTensorFieldKind::CrdMemRef,
                       lvl, /*value=*/coords[lvl]);
      } else {
        assert(isDenseLT(lt));
        // Construct the new position as:
        //   positions[lvl] = size * positions[lvl-1] + coords[lvl]
        //   <insert @ positions[lvl] at next level lvl + 1>
        Value size = desc.getLvlSize(builder, loc, lvl);
        Value mult = arith::MulIOp::create(builder, loc, size, parentPos);
        parentPos = arith::AddIOp::create(builder, loc, mult, coords[lvl]);
      }
    }
    // Reached the actual value append/insert.
    if (!stt.isDenseLvl(lvlRank - 1))
      createPushback(builder, loc, desc, SparseTensorFieldKind::ValMemRef,
                     std::nullopt, value);
    else
      genStore(builder, loc, value, desc.getValMemRef(), parentPos);
    return fields;
  }

  std::string getMangledFuncName() {
    // The mangled name of the function has this format:
    //   <namePrefix>_<LT>_<shape>_<ordering>_<eltType>_<crdWidth>_<posWidth>
    constexpr const char kInsertFuncNamePrefix[] = "_insert_";
    const SparseTensorType stt(llvm::cast<RankedTensorType>(rtp));
    SmallString<32> nameBuffer;
    llvm::raw_svector_ostream nameOstream(nameBuffer);
    nameOstream << kInsertFuncNamePrefix;
    const Level lvlRank = stt.getLvlRank();
    for (Level l = 0; l < lvlRank; l++) {
      std::string lvlType = toMLIRString(stt.getLvlType(l));
      // Replace/remove punctuations in level properties.
      std::replace_if(
          lvlType.begin(), lvlType.end(),
          [](char c) { return c == '(' || c == ','; }, '_');
      llvm::erase_if(lvlType, [](char c) { return c == ')' || c == ' '; });
      nameOstream << lvlType << "_";
    }
    // Static dim sizes are used in the generated code while dynamic sizes are
    // loaded from the dimSizes buffer. This is the reason for adding the shape
    // to the function name.
    for (const auto sz : stt.getDimShape())
      nameOstream << sz << "_";
    // Permutation information is also used in generating insertion.
    if (!stt.isIdentity())
      nameOstream << stt.getDimToLvl() << "_";
    nameOstream << stt.getElementType() << "_";
    nameOstream << stt.getCrdWidth() << "_" << stt.getPosWidth();
    return nameOstream.str().str();
  }

private:
  TensorType rtp;
};

/// Sparse tensor storage conversion rule for returns.
class SparseReturnConverter : public OpConversionPattern<func::ReturnOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(func::ReturnOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Create a return with the flattened value extracted from sparse tensors.
    rewriter.replaceOpWithNewOp<func::ReturnOp>(
        op, flattenValues(adaptor.getOperands()));
    return success();
  }
};

/// Sparse tensor storage conversion rule for calls.
class SparseCallConverter : public OpConversionPattern<func::CallOp> {
public:
  // The default CallOp converter can not handle 1:N type conversion.
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(func::CallOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    // In case of:
    //  sparse_tensor, f, sparse_tensor = call @foo(...)
    // ==>
    //  memref..., f, memref = call @foo(...) replace with
    //  cast(memref...)->sparse_tensor, f, cast(memref...)->sparse_tensor
    SmallVector<Type> finalRetTy;
    if (failed(typeConverter->convertTypes(op.getResultTypes(), finalRetTy)))
      return failure();

    // (1) Generates new call with flattened return value.
    auto newCall =
        func::CallOp::create(rewriter, loc, op.getCallee(), finalRetTy,
                             flattenValues(adaptor.getOperands()));
    // (2) Gather sparse tensor returns.
    SmallVector<SmallVector<Value>> packedResultVals;
    // Tracks the offset of current return value (of the original call)
    // relative to the new call (after sparse tensor flattening);
    unsigned retOffset = 0;
    // Temporal buffer to hold the flattened list of type for
    // a sparse tensor.
    SmallVector<Type> sparseFlat;
    for (auto ret : op.getResults()) {
      assert(retOffset < newCall.getNumResults());
      auto retType = ret.getType();
      if (failed(typeConverter->convertType(retType, sparseFlat)))
        llvm_unreachable("Failed to convert type in sparse tensor codegen");

      // Converted types can not be empty when the type conversion succeed.
      assert(!sparseFlat.empty());
      if (sparseFlat.size() > 1) {
        auto flatSize = sparseFlat.size();
        packedResultVals.emplace_back();
        llvm::append_range(packedResultVals.back(),
                           newCall.getResults().slice(retOffset, flatSize));
        retOffset += flatSize;
      } else {
        // If this is an 1:1 conversion, no need for casting.
        packedResultVals.emplace_back();
        packedResultVals.back().push_back(newCall.getResult(retOffset));
        retOffset++;
      }
      sparseFlat.clear();
    }

    assert(packedResultVals.size() == op.getNumResults());
    rewriter.replaceOpWithMultiple(op, std::move(packedResultVals));
    return success();
  }
};

/// Sparse codegen rule for level accesses.
class SparseLvlOpConverter : public OpConversionPattern<LvlOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(LvlOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    std::optional<int64_t> lvl = op.getConstantLvlIndex();
    RankedTensorType srcType = op.getSource().getType();
    if (!lvl || !getSparseTensorEncoding(srcType))
      return failure();

    auto desc = getDescriptorFromTensorTuple(adaptor.getSource(), srcType);
    auto sz = desc.getLvlSize(rewriter, op.getLoc(), *lvl);

    rewriter.replaceOp(op, sz);
    return success();
  }
};

// TODO: use a new SortCOO operation here instead of reusing convert op.
struct SparseReorderCOOConverter : public OpConversionPattern<ReorderCOOOp> {
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ReorderCOOOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    MLIRContext *ctx = op.getContext();

    SparseTensorType srcStt = getSparseTensorType(op.getInputCoo());
    SparseTensorType dstStt = getSparseTensorType(op.getResultCoo());

    // Should have been verified.
    assert(dstStt.isAllOrdered() && !srcStt.isAllOrdered() &&
           dstStt.isCOOType() && srcStt.isCOOType());
    assert(dstStt.hasSameDimToLvl(srcStt));

    // We don't need a mutable descriptor here as we perform sorting in-place.
    auto desc = getDescriptorFromTensorTuple(adaptor.getInputCoo(),
                                             op.getInputCoo().getType());
    auto nnz = desc.getValMemSize(rewriter, op.getLoc());
    auto crd = desc.getAOSMemRef();
    auto val = desc.getValMemRef();

    // Otherwise we need another data shuffle and a non-identity map.
    assert(dstStt.hasSameDimToLvl(srcStt));
    (void)dstStt; // to silence warning when assertion is disabled

    auto id = AffineMap::getMultiDimIdentityMap(srcStt.getLvlRank(), ctx);

    SortOp::create(rewriter, loc, nnz, crd, ValueRange{val}, id,
                   rewriter.getIndexAttr(0), op.getAlgorithm());

    // Since we do in-place sorting, the destinate tensor will have the same set
    // of memrefs as the source tensor.
    rewriter.replaceOpWithMultiple(op, {adaptor.getInputCoo()});
    return success();
  }
};

template <typename Op, StorageSpecifierKind kind>
class SparseSliceGetterOpConverter : public OpConversionPattern<Op> {
public:
  using OpConversionPattern<Op>::OpConversionPattern;
  using typename OpConversionPattern<Op>::OneToNOpAdaptor;

  LogicalResult
  matchAndRewrite(Op op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Simply lowers to specifer.get <field> operation.
    auto desc = getDescriptorFromTensorTuple(adaptor.getSlice(),
                                             op.getSlice().getType());
    auto v = desc.getSpecifierField(rewriter, op.getLoc(), kind,
                                    op.getDim().getZExtValue());

    rewriter.replaceOp(op, v);
    return success();
  }
};

/// Sparse codegen rule for trivial tensor casts.
class SparseCastConverter : public OpConversionPattern<tensor::CastOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(tensor::CastOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Only rewrite identically annotated source/dest.
    auto encDst = getSparseTensorEncoding(op.getType());
    auto encSrc = getSparseTensorEncoding(op.getSource().getType());
    if (!encDst || encDst != encSrc)
      return failure();
    rewriter.replaceOpWithMultiple(op, {adaptor.getSource()});
    return success();
  }
};

class SparseReMapConverter : public OpConversionPattern<ReinterpretMapOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ReinterpretMapOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Simply fold the operation.
    rewriter.replaceOpWithMultiple(op, {adaptor.getSource()});
    return success();
  }
};

/// Sparse codegen rule for the alloc operator.
class SparseTensorAllocConverter
    : public OpConversionPattern<bufferization::AllocTensorOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  SparseTensorAllocConverter(const TypeConverter &typeConverter,
                             MLIRContext *context, bool enableInit)
      : OpConversionPattern(typeConverter, context),
        enableBufferInitialization(enableInit) {}

  LogicalResult
  matchAndRewrite(bufferization::AllocTensorOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    const auto resType = getSparseTensorType(op);
    if (!resType.hasEncoding())
      return failure();

    Location loc = op.getLoc();
    // Deal with copy.
    if (op.getCopy()) {
      auto desc = getDescriptorFromTensorTuple(
          adaptor.getCopy(), cast<RankedTensorType>(op.getCopy().getType()));
      SmallVector<Value> fields;
      fields.reserve(desc.getNumFields());
      // Memcpy on memref fields.
      for (auto field : desc.getMemRefFields()) {
        auto memrefTp = cast<MemRefType>(field.getType());
        auto size = memref::DimOp::create(rewriter, loc, field, 0);
        auto copied =
            memref::AllocOp::create(rewriter, loc, memrefTp, ValueRange{size});
        memref::CopyOp::create(rewriter, loc, field, copied);
        fields.push_back(copied);
      }
      // Reuses specifier.
      fields.push_back(desc.getSpecifier());
      assert(fields.size() == desc.getNumFields());
      rewriter.replaceOpWithMultiple(op, {fields});
      return success();
    }

    if (!resType.isIdentity()) {
      return rewriter.notifyMatchFailure(
          op, "try run --sparse-reinterpret-map before codegen");
    }
    // Level size equals to dimension size since lvl2dim map is an identity map.
    SmallVector<Value> lvlSizesValues;
    createDimSizes(rewriter, loc, resType,
                   flattenValues(adaptor.getDynamicSizes()),
                   /*dimSizesValues=*/lvlSizesValues);

    // Construct allocation for each field.
    Value sizeHint = op.getSizeHint();
    SmallVector<Value> fields;
    createAllocFields(rewriter, loc, resType, enableBufferInitialization,
                      sizeHint, lvlSizesValues, fields);

    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {fields});
    return success();
  }

private:
  bool enableBufferInitialization;
};

/// Sparse codegen rule for the empty tensor operator.
class SparseTensorEmptyConverter : public OpConversionPattern<tensor::EmptyOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  SparseTensorEmptyConverter(const TypeConverter &typeConverter,
                             MLIRContext *context, bool enableInit)
      : OpConversionPattern(typeConverter, context),
        enableBufferInitialization(enableInit) {}

  LogicalResult
  matchAndRewrite(tensor::EmptyOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    const auto resType = getSparseTensorType(op);
    if (!resType.hasEncoding())
      return failure();

    if (!resType.isIdentity()) {
      return rewriter.notifyMatchFailure(
          op, "try run --sparse-reinterpret-map before codegen");
    }

    Location loc = op.getLoc();
    // Level size equals to dimension size since lvl2dim map is an identity map.
    SmallVector<Value> lvlSizesValues;
    createDimSizes(rewriter, loc, resType, adaptor.getDynamicSizes(),
                   /*dimSizesValues=*/lvlSizesValues);
    // Construct allocation for each field.
    Value sizeHint; // none
    SmallVector<Value> fields;
    createAllocFields(rewriter, loc, resType, enableBufferInitialization,
                      sizeHint, lvlSizesValues, fields);

    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {fields});
    return success();
  }

private:
  bool enableBufferInitialization;
};

/// Sparse codegen rule for the dealloc operator.
class SparseTensorDeallocConverter
    : public OpConversionPattern<bufferization::DeallocTensorOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  SparseTensorDeallocConverter(const TypeConverter &typeConverter,
                               MLIRContext *context, bool createDeallocs)
      : OpConversionPattern(typeConverter, context),
        createDeallocs(createDeallocs) {}

  LogicalResult
  matchAndRewrite(bufferization::DeallocTensorOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto enc = getSparseTensorEncoding(op.getTensor().getType());
    if (!enc)
      return failure();

    // If user requests not to deallocate sparse tensors, simply erase the
    // operation.
    if (createDeallocs) {
      // Replace the sparse tensor deallocation with field deallocations.
      Location loc = op.getLoc();
      auto desc = getDescriptorFromTensorTuple(
          adaptor.getTensor(),
          cast<RankedTensorType>(op.getTensor().getType()));
      for (auto input : desc.getMemRefFields())
        // Deallocate every buffer used to store the sparse tensor handler.
        memref::DeallocOp::create(rewriter, loc, input);
    }
    rewriter.eraseOp(op);
    return success();
  }

private:
  const bool createDeallocs;
};

/// Sparse codegen rule for tensor rematerialization.
class SparseTensorLoadConverter : public OpConversionPattern<LoadOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(LoadOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Prepare descriptor.
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    // Generate optional insertion finalization code.
    if (op.getHasInserts())
      genEndInsert(rewriter, op.getLoc(), desc);
    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {desc.getFields()});
    return success();
  }
};

/// Sparse codegen rule for the expand op.
class SparseExpandConverter : public OpConversionPattern<ExpandOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ExpandOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    if (!getSparseTensorEncoding(op.getTensor().getType()))
      return failure();
    Location loc = op->getLoc();
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    const auto srcType = getSparseTensorType(op.getTensor());
    Type eltType = srcType.getElementType();
    Type boolType = rewriter.getIntegerType(1);
    Type idxType = rewriter.getIndexType();
    // All initialization should be done on entry of the loop nest.
    rewriter.setInsertionPointAfter(op.getTensor().getDefiningOp());

    // Determine the size for access expansion (always the innermost stored
    // level size).
    const auto sz = desc.getLvlSize(rewriter, loc, srcType.getLvlRank() - 1);
    // Generate a memref for `sz` elements of type `t`.
    const auto genAlloc = [&](Type t) {
      const auto memTp = MemRefType::get({ShapedType::kDynamic}, t);
      return memref::AllocOp::create(rewriter, loc, memTp, ValueRange{sz});
    };
    // Allocate temporary buffers for values/filled-switch and added.
    // We do not use stack buffers for this, since the expanded size may
    // be rather large (as it envelops a single expanded dense dimension).
    Value values = genAlloc(eltType);
    Value filled = genAlloc(boolType);
    Value added = genAlloc(idxType);
    Value zero = constantZero(rewriter, loc, idxType);
    // Reset the values/filled-switch to all-zero/false. Note that this
    // introduces an O(N) operation into the computation, but this reset
    // operation is amortized over the innermost loops for the access
    // pattern expansion. As noted in the operation doc, we would like
    // to amortize this setup cost even between kernels.
    linalg::FillOp::create(rewriter, loc,
                           ValueRange{constantZero(rewriter, loc, eltType)},
                           ValueRange{values});
    linalg::FillOp::create(rewriter, loc,
                           ValueRange{constantZero(rewriter, loc, boolType)},
                           ValueRange{filled});
    // Replace expansion op with these buffers and initial coordinate.
    assert(op.getNumResults() == 4);
    rewriter.replaceOp(op, {values, filled, added, zero});
    return success();
  }
};

/// Sparse codegen rule for the compress operator.
class SparseCompressConverter : public OpConversionPattern<CompressOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(CompressOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op->getLoc();
    SmallVector<Value> fields;
    auto desc = getMutDescriptorFromTensorTuple(adaptor.getTensor(), fields,
                                                op.getTensor().getType());
    Value values = llvm::getSingleElement(adaptor.getValues());
    Value filled = llvm::getSingleElement(adaptor.getFilled());
    Value added = llvm::getSingleElement(adaptor.getAdded());
    Value count = llvm::getSingleElement(adaptor.getCount());
    const SparseTensorType dstType(desc.getRankedTensorType());
    Type eltType = dstType.getElementType();

    // If the innermost level is ordered, we need to sort the coordinates
    // in the "added" array prior to applying the compression.
    if (dstType.isOrderedLvl(dstType.getLvlRank() - 1))
      SortOp::create(rewriter, loc, count, added, ValueRange{},
                     rewriter.getMultiDimIdentityMap(1),
                     rewriter.getIndexAttr(0),
                     SparseTensorSortKind::HybridQuickSort);
    // While performing the insertions, we also need to reset the elements
    // of the values/filled-switch by only iterating over the set elements,
    // to ensure that the runtime complexity remains proportional to the
    // sparsity of the expanded access pattern.
    //
    // Generate
    //    out_memrefs = for (i = 0; i < count; i++)(in_memrefs) {
    //      crd = added[i];
    //      value = values[crd];
    //      insert({lvlCoords, crd}, value);
    //      new_memrefs = insert(in_memrefs, {lvlCoords, crd}, value);
    //      values[crd] = 0;
    //      filled[crd] = false;
    //      yield new_memrefs
    //    }
    scf::ForOp loop = createFor(rewriter, loc, count, desc.getFields());
    Value i = loop.getInductionVar();

    Value crd = genLoad(rewriter, loc, added, i);
    Value value = genLoad(rewriter, loc, values, crd);
    SmallVector<Value> params(desc.getFields().begin(), desc.getFields().end());
    SmallVector<Type> flatSpTensorTps = llvm::to_vector(
        llvm::map_range(desc.getFields(), [](Value v) { return v.getType(); }));
    SmallVector<Value> flatLvlCoords = flattenValues(adaptor.getLvlCoords());
    params.append(flatLvlCoords.begin(), flatLvlCoords.end());
    params.push_back(crd);
    params.push_back(value);
    SparseInsertGenerator insertGen(op.getTensor().getType(), flatSpTensorTps,
                                    params, /*genCall=*/true);
    SmallVector<Value> insertRet = insertGen.genCallOrInline(rewriter, loc);
    genStore(rewriter, loc, constantZero(rewriter, loc, eltType), values, crd);
    genStore(rewriter, loc, constantI1(rewriter, loc, false), filled, crd);
    scf::YieldOp::create(rewriter, loc, insertRet);

    rewriter.setInsertionPointAfter(loop);
    // Deallocate the buffers on exit of the full loop nest.
    Operation *parent = getTop(op);
    rewriter.setInsertionPointAfter(parent);
    memref::DeallocOp::create(rewriter, loc, values);
    memref::DeallocOp::create(rewriter, loc, filled);
    memref::DeallocOp::create(rewriter, loc, added);
    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {loop->getResults()});
    return success();
  }
};

/// Sparse codegen rule for the insert operator.
class SparseInsertConverter : public OpConversionPattern<tensor::InsertOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(tensor::InsertOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto stt = getSparseTensorType(op.getDest());
    if (!stt.hasEncoding())
      return failure();
    assert(stt.isIdentity() && "Run reinterpret-map before conversion.");

    Location loc = op.getLoc();
    auto desc =
        getDescriptorFromTensorTuple(adaptor.getDest(), op.getDest().getType());
    TypeRange flatSpTensorTps = desc.getFields().getTypes();
    SmallVector<Value> params = llvm::to_vector(desc.getFields());
    SmallVector<Value> flatIndices = flattenValues(adaptor.getIndices());
    params.append(flatIndices.begin(), flatIndices.end());
    params.push_back(llvm::getSingleElement(adaptor.getScalar()));
    SparseInsertGenerator insertGen(op.getDest().getType(), flatSpTensorTps,
                                    params, /*genCall=*/true);
    SmallVector<Value> ret = insertGen.genCallOrInline(rewriter, loc);
    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {ret});
    return success();
  }
};

/// Sparse codegen rule for position accesses.
class SparseToPositionsConverter : public OpConversionPattern<ToPositionsOp> {
public:
  using OpAdaptor = typename ToPositionsOp::Adaptor;
  using OpConversionPattern<ToPositionsOp>::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ToPositionsOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Replace the requested position access with corresponding field.
    // The view is restricted to the actual size to ensure clients
    // of this operation truly observe size, not capacity!
    Location loc = op.getLoc();
    Level lvl = op.getLevel();
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    auto mem = desc.getPosMemRef(lvl);
    auto size = desc.getPosMemSize(rewriter, loc, lvl);
    rewriter.replaceOp(op, genSliceToSize(rewriter, loc, mem, size));
    return success();
  }
};

/// Sparse codegen rule for accessing the coordinates arrays.
class SparseToCoordinatesConverter
    : public OpConversionPattern<ToCoordinatesOp> {
public:
  using OpAdaptor = typename ToCoordinatesOp::Adaptor;
  using OpConversionPattern<ToCoordinatesOp>::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ToCoordinatesOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Replace the requested coordinates access with corresponding field.
    // The view is restricted to the actual size to ensure clients
    // of this operation truly observe size, not capacity!
    Location loc = op.getLoc();
    Level lvl = op.getLevel();
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    auto mem = desc.getCrdMemRefOrView(rewriter, loc, lvl);
    if (lvl < getSparseTensorType(op.getTensor()).getAoSCOOStart()) {
      auto size = desc.getCrdMemSize(rewriter, loc, lvl);
      mem = genSliceToSize(rewriter, loc, mem, size);
    }
    rewriter.replaceOp(op, mem);
    return success();
  }
};

/// Sparse codegen rule for accessing the linear coordinates buffer.
class SparseToCoordinatesBufferConverter
    : public OpConversionPattern<ToCoordinatesBufferOp> {
public:
  using OpAdaptor = typename ToCoordinatesBufferOp::Adaptor;
  using OpConversionPattern<ToCoordinatesBufferOp>::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ToCoordinatesBufferOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Replace the requested coordinates access with corresponding field.
    // The view is restricted to the actual size to ensure clients
    // of this operation truly observe size, not capacity!
    Location loc = op.getLoc();
    Level lvl = getSparseTensorType(op.getTensor()).getAoSCOOStart();
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    auto mem = desc.getAOSMemRef();
    auto size = desc.getCrdMemSize(rewriter, loc, lvl);
    rewriter.replaceOp(op, genSliceToSize(rewriter, loc, mem, size));
    return success();
  }
};

/// Sparse codegen rule for value accesses.
class SparseToValuesConverter : public OpConversionPattern<ToValuesOp> {
public:
  using OpAdaptor = typename ToValuesOp::Adaptor;
  using OpConversionPattern<ToValuesOp>::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ToValuesOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Replace the requested values access with corresponding field.
    // The view is restricted to the actual size to ensure clients
    // of this operation truly observe size, not capacity!
    Location loc = op.getLoc();
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    auto mem = desc.getValMemRef();
    auto size = desc.getValMemSize(rewriter, loc);
    rewriter.replaceOp(op, genSliceToSize(rewriter, loc, mem, size));
    return success();
  }
};

/// Sparse codegen rule for the convert operator.
class SparseConvertConverter : public OpConversionPattern<ConvertOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(ConvertOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    SparseTensorEncodingAttr encDst = getSparseTensorEncoding(op.getType());
    SparseTensorEncodingAttr encSrc =
        getSparseTensorEncoding(op.getSource().getType());
    // The output tensor can not be a slice and those cases should have been
    // rejected by ConvertOp::verify() already.
    assert(!encDst.isSlice() && "Cannot convert to a sparse tensor slices.");
    // Different encoding (except for different bitwidth) should be handled by
    // rewriting.
    // We need further rewrites if the input tensor is a slice too.
    if (encDst.withoutBitWidths() != encSrc.withoutBitWidths() ||
        encSrc.isSlice()) {
      return failure();
    }

    Type retElemTp = op.getResult().getType().getElementType();
    Type srcElemTp = op.getSource().getType().getElementType();
    // Fold the trivial cases.
    if (retElemTp == srcElemTp && encDst == encSrc) {
      rewriter.replaceOpWithMultiple(op, {adaptor.getSource()});
      return success();
    }
    //
    // Do element-wise type conversion without using InsertOp.
    //
    // for each memref in srcTensor:
    //   dst = memref.alloc
    //   if srcMemRefType != dstMemRefType:
    //     for every dst[i] = cast(src[i])
    //   else:
    //     dst = memref.copy(src)
    Location loc = op.getLoc();
    auto srcDesc = getDescriptorFromTensorTuple(adaptor.getSource(),
                                                op.getSource().getType());
    SmallVector<Value> fields;
    foreachFieldAndTypeInSparseTensor(
        SparseTensorType(cast<RankedTensorType>(op.getResult().getType())),
        [&rewriter, &fields, srcDesc,
         loc](Type fTp, FieldIndex fIdx, SparseTensorFieldKind fKind, Level lvl,
              LevelType /*lt*/) -> bool {
          // Simply reuses the storage specifier as it is an SSA value.
          if (fKind == SparseTensorFieldKind::StorageSpec) {
            fields.push_back(srcDesc.getSpecifier());
          } else {
            // Allocates new memrefs
            Value srcMem = srcDesc.getMemRefField(fIdx);
            // TODO: We can instead use the actual memSize in specifier, that
            // would require a subViewOp to avoid overflow when copying
            // values.
            Value sz = linalg::createOrFoldDimOp(rewriter, loc, srcMem, 0);
            auto dstMem = memref::AllocOp::create(rewriter, loc,
                                                  cast<MemRefType>(fTp), sz);
            if (fTp != srcMem.getType()) {
              // Converts elements type.
              scf::buildLoopNest(
                  rewriter, loc, constantIndex(rewriter, loc, 0), sz,
                  constantIndex(rewriter, loc, 1),
                  [srcMem, &dstMem](OpBuilder &builder, Location loc,
                                    ValueRange ivs) {
                    Value v = memref::LoadOp::create(builder, loc, srcMem, ivs);
                    Value casted = genCast(builder, loc, v,
                                           dstMem.getType().getElementType());
                    memref::StoreOp::create(builder, loc, casted, dstMem, ivs);
                  });
            } else {
              // TODO: We can even reuse the same memref for the new tensor,
              // but that requires a `ref-counting` based memory management
              // for shared memrefs between multiple sparse tensors.
              memref::CopyOp::create(rewriter, loc, srcMem, dstMem);
            }
            fields.push_back(dstMem);
          }
          return true;
        });

    rewriter.replaceOpWithMultiple(op, {fields});
    return success();
  }
};

class SparseExtractSliceConverter
    : public OpConversionPattern<tensor::ExtractSliceOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(tensor::ExtractSliceOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    MLIRContext *ctx = op.getContext();
    auto srcEnc = getSparseTensorEncoding(op.getSourceType());
    auto dstEnc = getSparseTensorEncoding(op.getResult().getType());
    // TODO: We should check these in ExtractSliceOp::verify.
    if (!srcEnc || !dstEnc || !dstEnc.isSlice())
      return failure();
    assert(srcEnc.withoutDimSlices() == dstEnc.withoutDimSlices());

    SmallVector<Value> fields;
    auto desc = getMutDescriptorFromTensorTuple(adaptor.getSource(), fields,
                                                op.getSource().getType());

    auto newSpec = StorageSpecifierInitOp::create(
        rewriter, loc, StorageSpecifierType::get(ctx, dstEnc),
        desc.getSpecifier());
    desc.setSpecifier(newSpec);

    // Fills in slice information.
    for (auto [idx, offset, size, stride] : llvm::enumerate(
             op.getMixedOffsets(), op.getMixedSizes(), op.getMixedStrides())) {
      Dimension dim = idx;

      Value offsetV = getValueOrCreateConstantIndexOp(rewriter, loc, offset);
      Value sizeV = getValueOrCreateConstantIndexOp(rewriter, loc, size);
      Value strideV = getValueOrCreateConstantIndexOp(rewriter, loc, stride);
      // TODO: We could probably only set dynamic value here. But it would
      // requires us to fill the hole when casting a static slice to dynamic
      // slice.
      desc.setSpecifierField(rewriter, loc, StorageSpecifierKind::DimOffset,
                             dim, offsetV);

      // FIXME: we need to distinguish level sizes and dimension size for slices
      // here. Maybe we should store slice level sizes in a different array
      // instead of reusing it.
      assert(srcEnc.isIdentity());
      desc.setSpecifierField(rewriter, loc, StorageSpecifierKind::LvlSize, dim,
                             sizeV);
      desc.setSpecifierField(rewriter, loc, StorageSpecifierKind::DimStride,
                             dim, strideV);
    }

    // NOTE: we can not generate tuples directly from descriptor here, as the
    // descriptor is holding the original type, yet we want the slice type
    // here (they shared every memref but with an updated specifier).
    rewriter.replaceOpWithMultiple(op, {desc.getFields()});
    return success();
  }
};

/// Sparse codegen rule for number of entries operator.
class SparseNumberOfEntriesConverter
    : public OpConversionPattern<NumberOfEntriesOp> {
public:
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(NumberOfEntriesOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // Query memSizes for the actually stored values.
    // FIXME: the nse value computed in this way might be wrong when there is
    // any "loose_compressed" level.
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    rewriter.replaceOp(op, desc.getValMemSize(rewriter, op.getLoc()));
    return success();
  }
};

struct SparseAssembleOpConverter : public OpConversionPattern<AssembleOp> {
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(AssembleOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    const auto stt = getSparseTensorType(op.getResult());

    SmallVector<Value> fields;

    foreachFieldAndTypeInSparseTensor(
        stt,
        [&rewriter, &fields, &op, &stt,
         loc](Type fType, FieldIndex fIdx, SparseTensorFieldKind fKind,
              Level /*lvl*/, LevelType lt) -> bool {
          assert(fields.size() == fIdx);
          if (fKind == SparseTensorFieldKind::StorageSpec) {
            fields.push_back(
                SparseTensorSpecifier::getInitValue(rewriter, loc, stt));
          } else {
            // Else simply takes the inputs.
            Value tensor = fKind == SparseTensorFieldKind::ValMemRef
                               ? op.getValues()
                               : op.getLevels()[fIdx];
            // TODO: handle batch.
            TypedValue<BaseMemRefType> mem = genToMemref(rewriter, loc, tensor);
            if (mem.getType().getRank() > stt.getBatchLvlRank() + 1) {
              // Flattens the buffer to batchLvlRank.
              auto reassoc = getReassociationForFlattening(
                  mem.getType(), stt.getBatchLvlRank());
              mem = memref::CastOp::create(
                  rewriter, loc, fType,
                  memref::CollapseShapeOp::create(rewriter, loc, mem, reassoc));
            } else {
              mem = memref::CastOp::create(rewriter, loc, fType, mem);
            }
            fields.push_back(mem);
          }
          return true;
        });

    MutSparseTensorDescriptor desc(stt, fields);
    Value c0 = constantIndex(rewriter, loc, 0);
    Value c1 = constantIndex(rewriter, loc, 1);
    Value c2 = constantIndex(rewriter, loc, 2);
    Value posBack = c0; // index to the last value in the position array
    Value memSize = c1; // memory size for current array

    Level trailCOOStart = stt.getAoSCOOStart();
    Level trailCOORank = stt.getLvlRank() - trailCOOStart;
    // Sets up SparseTensorSpecifier.
    for (Level lvl = 0, lvlRank = stt.getLvlRank(); lvl < lvlRank; lvl++) {
      assert(ShapedType::isStatic(stt.getDimShape()[lvl]));

      // Sets up the level size.
      auto lvlSize = constantIndex(rewriter, loc, stt.getLvlShape()[lvl]);
      desc.setLvlSize(rewriter, loc, lvl, lvlSize);
      // We use a single AOS array to store the trailing COO, so there is only
      // one memory size to set for the entire COO section.
      if (lvl > trailCOOStart)
        continue;

      // Sets up the memory size by reading the last value in position array.
      LevelType lt = stt.getLvlType(lvl);
      // Simply forwards the position index when this is a dense level.
      if (lt.isa<LevelFormat::Dense>()) {
        memSize = arith::MulIOp::create(rewriter, loc, lvlSize, memSize);
        posBack = arith::SubIOp::create(rewriter, loc, memSize, c1);
        continue;
      }
      if (lt.isa<LevelFormat::Batch>()) {
        // Skips batch levels as it is not linearized.
        // FIXME: this assumes that every batch has the same number of nse, need
        // to be generalized to handle varied-size batches.
        continue;
      }

      if (isWithPosLT(lt)) {
        assert(isCompressedLT(lt) || isLooseCompressedLT(lt));
        if (isLooseCompressedLT(lt)) {
          memSize = arith::MulIOp::create(rewriter, loc, memSize, c2);
          posBack = arith::SubIOp::create(rewriter, loc, memSize, c1);
        } else {
          assert(isCompressedLT(lt));
          posBack = memSize;
          memSize = arith::AddIOp::create(rewriter, loc, memSize, c1);
        }
        desc.setPosMemSize(rewriter, loc, lvl, memSize);
        // The last value in position array is the memory size for next level.
        // FIXME: this assumes that every batch has the same number of nse, need
        // to be generalized to handle varied-size batches.
        SmallVector<Value> batched(stt.getBatchLvlRank(),
                                   constantIndex(rewriter, loc, 0));
        batched.push_back(posBack);
        memSize = genIndexLoad(rewriter, loc, desc.getPosMemRef(lvl), batched);
        posBack = arith::SubIOp::create(rewriter, loc, posBack, c1);
      }
      assert(isWithCrdLT(lt) && lvl <= trailCOOStart);
      // FIXME: This seems to be unnecessarily complex, can we simplify it?
      if (lvl == trailCOOStart) {
        Value cooSz = arith::MulIOp::create(
            rewriter, loc, memSize, constantIndex(rewriter, loc, trailCOORank));
        desc.setCrdMemSize(rewriter, loc, lvl, cooSz);
      } else {
        desc.setCrdMemSize(rewriter, loc, lvl, memSize);
      }
    }
    desc.setValMemSize(rewriter, loc, memSize);

    rewriter.replaceOpWithMultiple(op, {desc.getFields()});
    return success();
  }
};

struct SparseDisassembleOpConverter
    : public OpConversionPattern<DisassembleOp> {
  using OpConversionPattern::OpConversionPattern;
  SparseDisassembleOpConverter(const TypeConverter &typeConverter,
                               MLIRContext *context)
      : OpConversionPattern(typeConverter, context) {}

  LogicalResult
  matchAndRewrite(DisassembleOp op, OneToNOpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto desc = getDescriptorFromTensorTuple(adaptor.getTensor(),
                                             op.getTensor().getType());
    Location loc = op.getLoc();
    SmallVector<Value> retMem;
    SmallVector<Value> retLen;
    desc.getLayout().foreachField([desc, loc, &rewriter, &op, &retMem,
                                   &retLen](FieldIndex fid,
                                            SparseTensorFieldKind fKind,
                                            Level lvl, LevelType lt) -> bool {
      if (fKind == SparseTensorFieldKind::StorageSpec)
        return true;
      SparseTensorType stt(desc.getRankedTensorType());
      Value sz, src;
      TypedValue<BaseMemRefType> dst;
      if (fKind == SparseTensorFieldKind::ValMemRef) {
        sz = desc.getValMemSize(rewriter, loc);
        src = desc.getValMemRef();
        dst = genToMemref(rewriter, loc, op.getOutValues());

        retMem.push_back(dst);
        Type valLenTp = op.getValLen().getType();
        retLen.push_back(genScalarToTensor(rewriter, loc, sz, valLenTp));
      } else {
        assert(fKind == SparseTensorFieldKind::PosMemRef ||
               fKind == SparseTensorFieldKind::CrdMemRef);

        sz = fKind == SparseTensorFieldKind::PosMemRef
                 ? desc.getPosMemSize(rewriter, loc, lvl)
                 : desc.getCrdMemSize(rewriter, loc, lvl);
        src = desc.getMemRefField(fid);
        dst = genToMemref(rewriter, loc, op.getOutLevels()[fid]);
        retMem.push_back(dst);
        // Retrieves the corresponding level length type.
        Type lvlLenTp = op.getLvlLens().getTypes()[retLen.size()];
        retLen.push_back(genScalarToTensor(rewriter, loc, sz, lvlLenTp));
      }
      Value flatOut = dst;
      if (dst.getType().getRank() > stt.getBatchLvlRank() + 1) {
        auto reassoc =
            getReassociationForFlattening(dst.getType(), stt.getBatchLvlRank());
        flatOut = memref::CollapseShapeOp::create(rewriter, loc, dst, reassoc);
      }
      Value dstMem = genSliceToSize(rewriter, loc, flatOut, sz);
      Value srcMem = genSliceToSize(rewriter, loc, src, sz);
      memref::CopyOp::create(rewriter, loc, srcMem, dstMem);
      return true;
    });

    // Converts MemRefs back to Tensors.
    SmallVector<Value> retValues = llvm::to_vector(
        llvm::map_range(retMem, [&rewriter, loc](Value v) -> Value {
          return bufferization::ToTensorOp::create(
              rewriter, loc, memref::getTensorTypeFromMemRefType(v.getType()),
              v);
        }));
    // Appends the actual memory length used in each buffer returned.
    retValues.append(retLen.begin(), retLen.end());
    rewriter.replaceOp(op, retValues);
    return success();
  }
};

struct SparseNewConverter : public OpConversionPattern<NewOp> {
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(NewOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Location loc = op.getLoc();
    const auto dstTp = getSparseTensorType(op.getResult());
    // Creating COO with NewOp is handled by direct IR codegen. All other cases
    // are handled by rewriting.
    if (!dstTp.hasEncoding() || dstTp.getAoSCOOStart() != 0)
      return failure();

    // Implement as follows:
    //   %reader = @createCheckedSparseTensorReader(%filename)
    //   %nse = @getSparseTensorNSE(%reader)
    //   %coo = bufferization.alloc_tensor an ordered COO with
    //          dst dim ordering, size_hint = %nse
    //   %coordinates = sparse_tensor.coordinates_buffer(%coo)
    //   %values = sparse_tensor.values(%coo)
    //   %isSorted = @sparseTensorReaderReadToBuffers(%coordinates, %values)
    //   if (! %isSorted) sparse_tensor.sort_coo(%nse, %coordinates, %values)
    //   update storage specifier
    //   @delSparseTensorReader(%reader)
    SmallVector<Value> dimSizesValues;
    Value dimSizesBuffer;
    Value reader = genReader(rewriter, loc, dstTp, adaptor.getOperands()[0],
                             dimSizesValues, dimSizesBuffer);

    // Get the number of stored entries.
    const Type indexTp = rewriter.getIndexType();
    Value nse = createFuncCall(rewriter, loc, "getSparseTensorReaderNSE",
                               {indexTp}, {reader}, EmitCInterface::Off)
                    .getResult(0);

    // Construct the lvl sizes and the dim2lvl/lvl2dim buffers.
    SmallVector<Value> lvlSizesValues;
    Value dim2lvlBuffer;
    Value lvl2dimBuffer;
    genMapBuffers(rewriter, loc, dstTp, dimSizesValues, dimSizesBuffer,
                  lvlSizesValues, dim2lvlBuffer, lvl2dimBuffer);

    // Construct allocation for each field.
    Value sizeHint = nse;
    SmallVector<Value> fields;
    createAllocFields(rewriter, loc, dstTp, /*enableInit=*/false, sizeHint,
                      lvlSizesValues, fields);

    // Read the COO tensor data.
    MutSparseTensorDescriptor desc(dstTp, fields);
    Value xs = desc.getAOSMemRef();
    Value ys = desc.getValMemRef();
    const Type boolTp = rewriter.getIntegerType(1);
    const Type elemTp = dstTp.getElementType();
    const Type crdTp = dstTp.getCrdType();
    SmallString<32> readToBuffersFuncName{"getSparseTensorReaderReadToBuffers",
                                          overheadTypeFunctionSuffix(crdTp),
                                          primaryTypeFunctionSuffix(elemTp)};
    Value isSorted =
        createFuncCall(rewriter, loc, readToBuffersFuncName, {boolTp},
                       {reader, dim2lvlBuffer, lvl2dimBuffer, xs, ys},
                       EmitCInterface::On)
            .getResult(0);

    // If the destination tensor is a sorted COO, we need to sort the COO tensor
    // data if the input elements aren't sorted yet.
    const Level lvlRank = dstTp.getLvlRank();
    if (dstTp.isOrderedLvl(lvlRank - 1)) {
      Value kFalse = constantI1(rewriter, loc, false);
      Value notSorted = arith::CmpIOp::create(
          rewriter, loc, arith::CmpIPredicate::eq, isSorted, kFalse);
      scf::IfOp ifOp =
          scf::IfOp::create(rewriter, loc, notSorted, /*else*/ false);
      rewriter.setInsertionPointToStart(&ifOp.getThenRegion().front());
      auto xPerm = rewriter.getMultiDimIdentityMap(lvlRank);
      SortOp::create(rewriter, loc, nse, xs, ValueRange{ys}, xPerm,
                     rewriter.getIndexAttr(0),
                     SparseTensorSortKind::HybridQuickSort);
      rewriter.setInsertionPointAfter(ifOp);
    }

    // Set PosMemRef0[1] = nse.
    const Value c1 = constantIndex(rewriter, loc, 1);
    const Value posMemref0 = desc.getPosMemRef(0);
    const Type posTp = dstTp.getPosType();
    const Value posNse = genCast(rewriter, loc, nse, posTp);
    memref::StoreOp::create(rewriter, loc, posNse, posMemref0, c1);

    // Update storage specifier.
    Value coordinatesSize = arith::MulIOp::create(
        rewriter, loc, nse, constantIndex(rewriter, loc, lvlRank));
    desc.setSpecifierField(rewriter, loc, StorageSpecifierKind::CrdMemSize, 0,
                           coordinatesSize);
    desc.setSpecifierField(rewriter, loc, StorageSpecifierKind::ValMemSize,
                           std::nullopt, nse);

    // Release the sparse tensor reader.
    createFuncCall(rewriter, loc, "delSparseTensorReader", {}, {reader},
                   EmitCInterface::Off);

    // Replace operation with resulting memrefs.
    rewriter.replaceOpWithMultiple(op, {fields});
    return success();
  }
};

struct SparseHasRuntimeLibraryConverter
    : public OpConversionPattern<HasRuntimeLibraryOp> {
  using OpConversionPattern::OpConversionPattern;
  LogicalResult
  matchAndRewrite(HasRuntimeLibraryOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto i1Type = rewriter.getI1Type();
    rewriter.replaceOpWithNewOp<arith::ConstantOp>(
        op, i1Type, rewriter.getIntegerAttr(i1Type, 0));
    return success();
  }
};

} // namespace

//===----------------------------------------------------------------------===//
// Public method for populating conversion rules.
//===----------------------------------------------------------------------===//

/// Populates the given patterns list with conversion rules required for
/// the sparsification of linear algebra operations.
void mlir::populateSparseTensorCodegenPatterns(
    const TypeConverter &typeConverter, RewritePatternSet &patterns,
    bool createSparseDeallocs, bool enableBufferInitialization) {
  patterns.add<
      SparseAssembleOpConverter, SparseDisassembleOpConverter,
      SparseReturnConverter, SparseCallConverter, SparseLvlOpConverter,
      SparseCastConverter, SparseExtractSliceConverter,
      SparseTensorLoadConverter, SparseExpandConverter, SparseCompressConverter,
      SparseInsertConverter, SparseReorderCOOConverter, SparseReMapConverter,
      SparseSliceGetterOpConverter<ToSliceOffsetOp,
                                   StorageSpecifierKind::DimOffset>,
      SparseSliceGetterOpConverter<ToSliceStrideOp,
                                   StorageSpecifierKind::DimStride>,
      SparseToPositionsConverter, SparseToCoordinatesConverter,
      SparseToCoordinatesBufferConverter, SparseToValuesConverter,
      SparseConvertConverter, SparseNewConverter,
      SparseNumberOfEntriesConverter, SparseHasRuntimeLibraryConverter>(
      typeConverter, patterns.getContext());
  patterns.add<SparseTensorDeallocConverter>(
      typeConverter, patterns.getContext(), createSparseDeallocs);
  patterns.add<SparseTensorAllocConverter, SparseTensorEmptyConverter>(
      typeConverter, patterns.getContext(), enableBufferInitialization);
}
