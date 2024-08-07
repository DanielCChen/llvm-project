; RUN: not opt -S -dxil-op-lower -mtriple=dxil-pc-shadermodel6.3-library %s 2>&1 | FileCheck %s

; DXIL operation frac does not support double overload type
; CHECK: in function frac_double
; CHECK-SAME: Cannot create Frac operation: Invalid overload type

; Function Attrs: noinline nounwind optnone
define noundef double @frac_double(double noundef %a) #0 {
entry:
  %a.addr = alloca double, align 8
  store double %a, ptr %a.addr, align 8
  %0 = load double, ptr %a.addr, align 8
  %dx.frac = call double @llvm.dx.frac.f64(double %0)
  ret double %dx.frac
}
