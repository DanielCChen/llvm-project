; RUN: llc -mtriple=powerpc-ibm-aix -mcpu=pwr7 -O3 < %s | FileCheck %s --check-prefix=AIX32
;
; Regression test for PR https://github.com/llvm/llvm-project/issues/XXXXX
;
; On 32-bit PowerPC (e.g. powerpc-ibm-aix -m32), i64 is not a legal type.
; expandIS_FPCLASS() was incorrectly creating i64 AND/setcc nodes after type
; legalization had already run, triggering an assertion in LegalizeOp:
;   "Unexpected illegal type!" (operand type i64 with typeAction=TypeExpandInteger)
;
; The root cause is in compiler-rt/lib/builtins/muldc3.c and divdc3.c which
; contain isinf/finite checks that lower to IS_FPCLASS nodes.
;
; Fix: expandIS_FPCLASS returns SDValue() when the integer type matching the FP
; width is not legal.  PPCTargetLowering::LowerIS_FPCLASS was extended to handle
; fcInf and fcFinite tests via fabs(x) == +inf FP comparisons.

; AIX32-LABEL: test_isinf_f64:
; AIX32:         xsabsdp
; AIX32:         lfs
; AIX32:         fcmpu
define i1 @test_isinf_f64(double %x) {
entry:
  ; fcInf = fcPosInf | fcNegInf = 0x200 | 0x004 = 516
  %class = call i1 @llvm.is.fpclass.f64(double %x, i32 516)
  ret i1 %class
}

; AIX32-LABEL: test_isfinite_f64:
; AIX32:         xsabsdp
; AIX32:         lfs
; AIX32:         fcmpu
define i1 @test_isfinite_f64(double %x) {
entry:
  ; fcNan | fcFinite = ~fcInf = 0x1FB = 507 (!isinf)
  %class = call i1 @llvm.is.fpclass.f64(double %x, i32 507)
  ret i1 %class
}

; AIX32-LABEL: test_isinf_f32:
; AIX32:         xsabsdp
; AIX32:         lfs
; AIX32:         fcmpu
define i1 @test_isinf_f32(float %x) {
entry:
  %class = call i1 @llvm.is.fpclass.f32(float %x, i32 516)
  ret i1 %class
}

; AIX32-LABEL: test_isnan_f64:
; AIX32:         xscmpudp
define i1 @test_isnan_f64(double %x) {
entry:
  ; fcNan = fcSNan | fcQNan = 3
  %class = call i1 @llvm.is.fpclass.f64(double %x, i32 3)
  ret i1 %class
}

; AIX32-LABEL: test_isinf_or_nan_f64:
; AIX32:         xsabsdp
; AIX32:         lfs
; AIX32:         fcmpu
define i1 @test_isinf_or_nan_f64(double %x) {
entry:
  ; fcInf | fcNan = 516 | 3 = 519
  %class = call i1 @llvm.is.fpclass.f64(double %x, i32 519)
  ret i1 %class
}

declare i1 @llvm.is.fpclass.f64(double, i32 immarg)
declare i1 @llvm.is.fpclass.f32(float, i32 immarg)
