; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py UTC_ARGS: --version 5
; RUN: llc < %s -mtriple=s390x-linux-gnu -mcpu=z16 -verify-machineinstrs \
; RUN:   | FileCheck %s
;
; Some tests with comparisons and their uses involving 16-bit floating point.

; fcmp half; select half
define half @fun0(half %Arg0, half %Arg1) {
; CHECK-LABEL: fun0:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    stmg %r14, %r15, 112(%r15)
; CHECK-NEXT:    .cfi_offset %r14, -48
; CHECK-NEXT:    .cfi_offset %r15, -40
; CHECK-NEXT:    aghi %r15, -176
; CHECK-NEXT:    .cfi_def_cfa_offset 336
; CHECK-NEXT:    std %f8, 168(%r15) # 8-byte Spill
; CHECK-NEXT:    std %f9, 160(%r15) # 8-byte Spill
; CHECK-NEXT:    .cfi_offset %f8, -168
; CHECK-NEXT:    .cfi_offset %f9, -176
; CHECK-NEXT:    ldr %f8, %f2
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    ldr %f9, %f0
; CHECK-NEXT:    ldr %f0, %f8
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    ltebr %f1, %f9
; CHECK-NEXT:    # kill: def $f0s killed $f0s def $v0
; CHECK-NEXT:    jl .LBB0_2
; CHECK-NEXT:  # %bb.1: # %entry
; CHECK-NEXT:    vgmf %v0, 2, 8
; CHECK-NEXT:  .LBB0_2: # %entry
; CHECK-NEXT:    # kill: def $f0s killed $f0s killed $v0
; CHECK-NEXT:    brasl %r14, __truncsfhf2@PLT
; CHECK-NEXT:    ld %f8, 168(%r15) # 8-byte Reload
; CHECK-NEXT:    ld %f9, 160(%r15) # 8-byte Reload
; CHECK-NEXT:    lmg %r14, %r15, 288(%r15)
; CHECK-NEXT:    br %r14
entry:
  %cmp = fcmp olt half %Arg0, 0xH0000
  %cond = select i1 %cmp, half %Arg1, half 1.0
  ret half %cond
}

; fcmp half; select i32
define i32 @fun1(half %Arg0, i32 %Arg1) {
; CHECK-LABEL: fun1:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    stmg %r13, %r15, 104(%r15)
; CHECK-NEXT:    .cfi_offset %r13, -56
; CHECK-NEXT:    .cfi_offset %r14, -48
; CHECK-NEXT:    .cfi_offset %r15, -40
; CHECK-NEXT:    aghi %r15, -160
; CHECK-NEXT:    .cfi_def_cfa_offset 320
; CHECK-NEXT:    lr %r13, %r2
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    ltebr %f0, %f0
; CHECK-NEXT:    lochinl %r13, 0
; CHECK-NEXT:    lr %r2, %r13
; CHECK-NEXT:    lmg %r13, %r15, 264(%r15)
; CHECK-NEXT:    br %r14
entry:
  %cmp = fcmp olt half %Arg0, 0xH0000
  %cond = select i1 %cmp, i32 %Arg1, i32 0
  ret i32 %cond
}

; icmp i32; select half
define half @fun2(i32 %Arg0, half %Arg1) {
; CHECK-LABEL: fun2:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    stmg %r13, %r15, 104(%r15)
; CHECK-NEXT:    .cfi_offset %r13, -56
; CHECK-NEXT:    .cfi_offset %r14, -48
; CHECK-NEXT:    .cfi_offset %r15, -40
; CHECK-NEXT:    aghi %r15, -160
; CHECK-NEXT:    .cfi_def_cfa_offset 320
; CHECK-NEXT:    lr %r13, %r2
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    # kill: def $f0s killed $f0s def $v0
; CHECK-NEXT:    cije %r13, 0, .LBB2_2
; CHECK-NEXT:  # %bb.1: # %entry
; CHECK-NEXT:    vgmf %v0, 2, 8
; CHECK-NEXT:  .LBB2_2: # %entry
; CHECK-NEXT:    # kill: def $f0s killed $f0s killed $v0
; CHECK-NEXT:    brasl %r14, __truncsfhf2@PLT
; CHECK-NEXT:    lmg %r13, %r15, 264(%r15)
; CHECK-NEXT:    br %r14
entry:
  %cmp = icmp eq i32 %Arg0, 0
  %cond = select i1 %cmp, half %Arg1, half 1.0
  ret half %cond
}

define i64 @fun3(i64 %a, i64 %b, half %f1, half %f2) #0 {
; CHECK-LABEL: fun3:
; CHECK:       # %bb.0:
; CHECK-NEXT:    stmg %r12, %r15, 96(%r15)
; CHECK-NEXT:    .cfi_offset %r12, -64
; CHECK-NEXT:    .cfi_offset %r13, -56
; CHECK-NEXT:    .cfi_offset %r14, -48
; CHECK-NEXT:    .cfi_offset %r15, -40
; CHECK-NEXT:    aghi %r15, -176
; CHECK-NEXT:    .cfi_def_cfa_offset 336
; CHECK-NEXT:    std %f8, 168(%r15) # 8-byte Spill
; CHECK-NEXT:    std %f9, 160(%r15) # 8-byte Spill
; CHECK-NEXT:    .cfi_offset %f8, -168
; CHECK-NEXT:    .cfi_offset %f9, -176
; CHECK-NEXT:    ldr %f8, %f0
; CHECK-NEXT:    ldr %f0, %f2
; CHECK-NEXT:    lgr %r13, %r3
; CHECK-NEXT:    lgr %r12, %r2
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    ldr %f9, %f0
; CHECK-NEXT:    ldr %f0, %f8
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    cebr %f0, %f9
; CHECK-NEXT:    ld %f8, 168(%r15) # 8-byte Reload
; CHECK-NEXT:    ld %f9, 160(%r15) # 8-byte Reload
; CHECK-NEXT:    selgre %r2, %r12, %r13
; CHECK-NEXT:    lmg %r12, %r15, 272(%r15)
; CHECK-NEXT:    br %r14
  %cond = call i1 @llvm.experimental.constrained.fcmp.f32(
                                               half %f1, half %f2,
                                               metadata !"oeq",
                                               metadata !"fpexcept.strict") #0
  %res = select i1 %cond, i64 %a, i64 %b
  ret i64 %res
}

define half @fun4(half %Arg0, ptr %Dst) {
; CHECK-LABEL: fun4:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    stmg %r13, %r15, 104(%r15)
; CHECK-NEXT:    .cfi_offset %r13, -56
; CHECK-NEXT:    .cfi_offset %r14, -48
; CHECK-NEXT:    .cfi_offset %r15, -40
; CHECK-NEXT:    aghi %r15, -160
; CHECK-NEXT:    .cfi_def_cfa_offset 320
; CHECK-NEXT:    lgr %r13, %r2
; CHECK-NEXT:    brasl %r14, __extendhfsf2@PLT
; CHECK-NEXT:    ltebr %f0, %f0
; CHECK-NEXT:    je .LBB4_2
; CHECK-NEXT:  # %bb.1: # %store
; CHECK-NEXT:    lzer %f0
; CHECK-NEXT:    vsteh %v0, 0(%r13), 0
; CHECK-NEXT:  .LBB4_2: # %exit
; CHECK-NEXT:    lzer %f0
; CHECK-NEXT:    lmg %r13, %r15, 264(%r15)
; CHECK-NEXT:    br %r14
entry:
  %cmp = fcmp oeq half %Arg0, 0.0
  br i1 %cmp, label %exit, label %store

store:
  store half 0.0, ptr %Dst
  br label %exit

exit:
  ret half 0.0
}

declare i1 @llvm.experimental.constrained.fcmp.f32(half, half, metadata, metadata)
