//===-- flang/unittests/Common/FastIntSetTest.cpp ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"
#include "flang/Support/Fortran-features.h"

namespace Fortran::common::details {

std::string CamelCaseToLowerCaseHyphenated(std::string_view x);

TEST(FortranFeaturesTest, CamelCaseToLowerCaseHyphenated) {
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BackslashEscapes)),
      "backslash-escapes");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OldDebugLines)),
      "old-debug-lines");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::FixedFormContinuationWithColumn1Ampersand)),
      "fixed-form-continuation-with-column1-ampersand");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::LogicalAbbreviations)),
      "logical-abbreviations");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::XOROperator)),
      "xor-operator");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::PunctuationInNames)),
      "punctuation-in-names");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OptionalFreeFormSpace)),
      "optional-free-form-space");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BOZExtensions)),
      "boz-extensions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EmptyStatement)),
      "empty-statement");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AlternativeNE)),
      "alternative-ne");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ExecutionPartNamelist)),
      "execution-part-namelist");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DECStructures)),
      "dec-structures");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DoubleComplex)),
      "double-complex");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Byte)),
      "byte");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::StarKind)),
      "star-kind");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ExponentMatchingKindParam)),
      "exponent-matching-kind-param");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::QuadPrecision)),
      "quad-precision");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SlashInitialization)),
      "slash-initialization");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::TripletInArrayConstructor)),
      "triplet-in-array-constructor");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::MissingColons)),
      "missing-colons");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SignedComplexLiteral)),
      "signed-complex-literal");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OldStyleParameter)),
      "old-style-parameter");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ComplexConstructor)),
      "complex-constructor");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::PercentLOC)),
      "percent-loc");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SignedMultOperand)),
      "signed-mult-operand");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::FileName)),
      "file-name");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::Carriagecontrol)),
      "carriagecontrol");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Convert)),
      "convert");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Dispose)),
      "dispose");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::IOListLeadingComma)),
      "io-list-leading-comma");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AbbreviatedEditDescriptor)),
      "abbreviated-edit-descriptor");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ProgramParentheses)),
      "program-parentheses");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::PercentRefAndVal)),
      "percent-ref-and-val");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OmitFunctionDummies)),
      "omit-function-dummies");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::CrayPointer)),
      "cray-pointer");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Hollerith)),
      "hollerith");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ArithmeticIF)),
      "arithmetic-if");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Assign)),
      "assign");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AssignedGOTO)),
      "assigned-goto");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Pause)),
      "pause");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::OpenACC)),
      "open-acc");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::OpenMP)),
      "open-mp");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::CUDA)),
      "cuda");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::CruftAfterAmpersand)),
      "cruft-after-ampersand");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ClassicCComments)),
      "classic-c-comments");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AdditionalFormats)),
      "additional-formats");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BigIntLiterals)),
      "big-int-literals");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::RealDoControls)),
      "real-do-controls");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EquivalenceNumericWithCharacter)),
      "equivalence-numeric-with-character");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EquivalenceNonDefaultNumeric)),
      "equivalence-non-default-numeric");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EquivalenceSameNonSequence)),
      "equivalence-same-non-sequence");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AdditionalIntrinsics)),
      "additional-intrinsics");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AnonymousParents)),
      "anonymous-parents");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OldLabelDoEndStatements)),
      "old-label-do-end-statements");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::LogicalIntegerAssignment)),
      "logical-integer-assignment");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EmptySourceFile)),
      "empty-source-file");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ProgramReturn)),
      "program-return");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ImplicitNoneTypeNever)),
      "implicit-none-type-never");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ImplicitNoneTypeAlways)),
      "implicit-none-type-always");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ImplicitNoneExternal)),
      "implicit-none-external");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ForwardRefImplicitNone)),
      "forward-ref-implicit-none");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OpenAccessAppend)),
      "open-access-append");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BOZAsDefaultInteger)),
      "boz-as-default-integer");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DistinguishableSpecifics)),
      "distinguishable-specifics");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DefaultSave)),
      "default-save");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::PointerInSeqType)),
      "pointer-in-seq-type");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::NonCharacterFormat)),
      "non-character-format");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SaveMainProgram)),
      "save-main-program");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SaveBigMainProgramVariables)),
      "save-big-main-program-variables");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DistinctArrayConstructorLengths)),
      "distinct-array-constructor-lengths");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::PPCVector)),
      "ppc-vector");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::RelaxedIntentInChecking)),
      "relaxed-intent-in-checking");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ForwardRefImplicitNoneData)),
      "forward-ref-implicit-none-data");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::NullActualForAllocatable)),
      "null-actual-for-allocatable");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::ActualIntegerConvertedToSmallerKind)),
      "actual-integer-converted-to-smaller-kind");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::HollerithOrCharacterAsBOZ)),
      "hollerith-or-character-as-boz");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BindingAsProcedure)),
      "binding-as-procedure");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::StatementFunctionExtensions)),
      "statement-function-extensions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::UseGenericIntrinsicWhenSpecificDoesntMatch)),
      "use-generic-intrinsic-when-specific-doesnt-match");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DataStmtExtensions)),
      "data-stmt-extensions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::RedundantContiguous)),
      "redundant-contiguous");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::RedundantAttribute)),
      "redundant-attribute");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::InitBlankCommon)),
      "init-blank-common");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EmptyBindCDerivedType)),
      "empty-bind-c-derived-type");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::MiscSourceExtensions)),
      "misc-source-extensions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AllocateToOtherLength)),
      "allocate-to-other-length");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::LongNames)),
      "long-names");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::IntrinsicAsSpecific)),
      "intrinsic-as-specific");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BenignNameClash)),
      "benign-name-clash");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BenignRedundancy)),
      "benign-redundancy");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::NullMoldAllocatableComponentValue)),
      "null-mold-allocatable-component-value");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::NopassScalarBase)),
      "nopass-scalar-base");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::MiscUseExtensions)),
      "misc-use-extensions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ImpliedDoIndexScope)),
      "implied-do-index-scope");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::DistinctCommonSizes)),
      "distinct-common-sizes");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::OddIndexVariableRestrictions)),
      "odd-index-variable-restrictions");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::IndistinguishableSpecifics)),
      "indistinguishable-specifics");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SubroutineAndFunctionSpecifics)),
      "subroutine-and-function-specifics");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::EmptySequenceType)),
      "empty-sequence-type");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::NonSequenceCrayPointee)),
      "non-sequence-cray-pointee");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BranchIntoConstruct)),
      "branch-into-construct");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::BadBranchTarget)),
      "bad-branch-target");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::HollerithPolymorphic)),
      "hollerith-polymorphic");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ListDirectedSize)),
      "list-directed-size");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::NonBindCInteroperability)),
      "non-bind-c-interoperability");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::CudaManaged)),
      "cuda-managed");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::CudaUnified)),
      "cuda-unified");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::
                    PolymorphicActualAllocatableOrPointerToMonomorphicDummy)),
      "polymorphic-actual-allocatable-or-pointer-to-monomorphic-dummy");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::RelaxedPureDummy)),
      "relaxed-pure-dummy");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::UndefinableAsynchronousOrVolatileActual)),
      "undefinable-asynchronous-or-volatile-actual");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AutomaticInMainProgram)),
      "automatic-in-main-program");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::PrintCptr)),
      "print-cptr");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::SavedLocalInSpecExpr)),
      "saved-local-in-spec-expr");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::PrintNamelist)),
      "print-namelist");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                LanguageFeature::AssumedRankPassedToNonAssumedRank)),
      "assumed-rank-passed-to-non-assumed-rank");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::IgnoreIrrelevantAttributes)),
      "ignore-irrelevant-attributes");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(LanguageFeature::Unsigned)),
      "unsigned");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::AmbiguousStructureConstructor)),
      "ambiguous-structure-constructor");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ContiguousOkForSeqAssociation)),
      "contiguous-ok-for-seq-association");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(LanguageFeature::ForwardRefExplicitTypeDummy)),
      "forward-ref-explicit-type-dummy");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::Portability)),
      "portability");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::PointerToUndefinable)),
      "pointer-to-undefinable");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::NonTargetPassedToTarget)),
      "non-target-passed-to-target");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::PointerToPossibleNoncontiguous)),
      "pointer-to-possible-noncontiguous");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ShortCharacterActual)),
      "short-character-actual");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ShortArrayActual)),
      "short-array-actual");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ImplicitInterfaceActual)),
      "implicit-interface-actual");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::PolymorphicTransferArg)),
      "polymorphic-transfer-arg");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::PointerComponentTransferArg)),
      "pointer-component-transfer-arg");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::TransferSizePresence)),
      "transfer-size-presence");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::F202XAllocatableBreakingChange)),
      "f202-x-allocatable-breaking-change");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::OptionalMustBePresent)),
      "optional-must-be-present");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::CommonBlockPadding)),
      "common-block-padding");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::LogicalVsCBool)),
      "logical-vs-c-bool");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::BindCCharLength)),
      "bind-c-char-length");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ProcDummyArgShapes)),
      "proc-dummy-arg-shapes");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ExternalNameConflict)),
      "external-name-conflict");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::FoldingException)),
      "folding-exception");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::FoldingAvoidsRuntimeCrash)),
      "folding-avoids-runtime-crash");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::FoldingValueChecks)),
      "folding-value-checks");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::FoldingFailure)),
      "folding-failure");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::FoldingLimit)),
      "folding-limit");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::Interoperability)),
      "interoperability");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::CharacterInteroperability)),
      "character-interoperability");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::Bounds)),
      "bounds");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::Preprocessing)),
      "preprocessing");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::Scanning)),
      "scanning");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::OpenAccUsage)),
      "open-acc-usage");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ProcPointerCompatibility)),
      "proc-pointer-compatibility");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::VoidMold)),
      "void-mold");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::KnownBadImplicitInterface)),
      "known-bad-implicit-interface");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::EmptyCase)),
      "empty-case");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::CaseOverflow)),
      "case-overflow");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::CUDAUsage)),
      "cuda-usage");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::IgnoreTKRUsage)),
      "ignore-tkr-usage");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ExternalInterfaceMismatch)),
      "external-interface-mismatch");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::DefinedOperatorArgs)),
      "defined-operator-args");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::Final)),
      "final");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::ZeroDoStep)),
      "zero-do-step");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::UnusedForallIndex)),
      "unused-forall-index");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::OpenMPUsage)),
      "open-mp-usage");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::DataLength)),
      "data-length");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::IgnoredDirective)),
      "ignored-directive");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::HomonymousSpecific)),
      "homonymous-specific");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::HomonymousResult)),
      "homonymous-result");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::IgnoredIntrinsicFunctionType)),
      "ignored-intrinsic-function-type");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::PreviousScalarUse)),
      "previous-scalar-use");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::RedeclaredInaccessibleComponent)),
      "redeclared-inaccessible-component");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::ImplicitShared)),
      "implicit-shared");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::IndexVarRedefinition)),
      "index-var-redefinition");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::IncompatibleImplicitInterfaces)),
      "incompatible-implicit-interfaces");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::VectorSubscriptFinalization)),
      "vector-subscript-finalization");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::UndefinedFunctionResult)),
      "undefined-function-result");
  EXPECT_EQ(
      CamelCaseToLowerCaseHyphenated(EnumToString(UsageWarning::UselessIomsg)),
      "useless-iomsg");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::MismatchingDummyProcedure)),
      "mismatching-dummy-procedure");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::SubscriptedEmptyArray)),
      "subscripted-empty-array");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::UnsignedLiteralTruncation)),
      "unsigned-literal-truncation");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                UsageWarning::CompatibleDeclarationsFromDistinctModules)),
      "compatible-declarations-from-distinct-modules");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                UsageWarning::NullActualForDefaultIntentAllocatable)),
      "null-actual-for-default-intent-allocatable");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(EnumToString(
                UsageWarning::UseAssociationIntoSameNameSubprogram)),
      "use-association-into-same-name-subprogram");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::HostAssociatedIntentOutInSpecExpr)),
      "host-associated-intent-out-in-spec-expr");
  EXPECT_EQ(CamelCaseToLowerCaseHyphenated(
                EnumToString(UsageWarning::NonVolatilePointerToVolatile)),
      "non-volatile-pointer-to-volatile");
}

TEST(FortranFeaturesTest, HintLanguageControlFlag) {
  LanguageFeatureControl control{};
  EXPECT_EQ(control.getDefaultCliSpelling(LanguageFeature::BenignNameClash),
      "benign-name-clash");
  EXPECT_EQ(
      control.getDefaultCliSpelling(UsageWarning::Portability), "portability");
}

} // namespace Fortran::common::details
