; Regression test for crash when compiling complex-arithmetic builtins
; (compiler-rt muldc3.c / divdc3.c) for powerpc-ibm-aix.
;
; After d1af60ff088b ("DAG: Stop legalizing poison to undef"), ISD::POISON nodes
; were no longer silently folded to UNDEF before LegalizeDAG ran.  On targets
; where ppcf128 is TypeExpandFloat (i.e. every PowerPC target), a POISON node
; carrying that type could be created inside LegalizeDAG (e.g. when emitting an
; error placeholder for an unsupported libcall) *after* type legalization had
; already run.  The resulting POISON ppcf128 node would then re-enter LegalizeOp
; and trigger the "Unexpected illegal type!" assertion.
;
; The fix is two-fold:
;   1. PPCISelLowering: setOperationAction(ISD::POISON, MVT::ppcf128, Expand)
;      so that when the type legalizer does see such a node it handles it.
;   2. LegalizeDAG: relax the #ifndef NDEBUG assertion to allow POISON nodes
;      that carry non-legal types (they are expanded in-place by ExpandNode).
;
; Just ensure these compile without crashing.  No specific codegen is checked.

; RUN: llc -O3 -mtriple=powerpc-ibm-aix    -verify-machineinstrs < %s | FileCheck %s
; RUN: llc -O3 -mtriple=powerpc64-ibm-aix  -verify-machineinstrs < %s | FileCheck %s

; CHECK-LABEL: test_ppcf128_fadd:
; CHECK:       blr
define ppc_fp128 @test_ppcf128_fadd(ppc_fp128 %a, ppc_fp128 %b) {
  %r = fadd ppc_fp128 %a, %b
  ret ppc_fp128 %r
}

; CHECK-LABEL: test_ppcf128_fmul:
; CHECK:       blr
define ppc_fp128 @test_ppcf128_fmul(ppc_fp128 %a, ppc_fp128 %b) {
  %r = fmul ppc_fp128 %a, %b
  ret ppc_fp128 %r
}

; CHECK-LABEL: test_ppcf128_fdiv:
; CHECK:       blr
define ppc_fp128 @test_ppcf128_fdiv(ppc_fp128 %a, ppc_fp128 %b) {
  %r = fdiv ppc_fp128 %a, %b
  ret ppc_fp128 %r
}

; CHECK-LABEL: test_ppcf128_select:
; CHECK:       blr
; A select can introduce an undef/poison of the branch-not-taken type.
define ppc_fp128 @test_ppcf128_select(i1 %cond, ppc_fp128 %a, ppc_fp128 %b) {
  %r = select i1 %cond, ppc_fp128 %a, ppc_fp128 %b
  ret ppc_fp128 %r
}
