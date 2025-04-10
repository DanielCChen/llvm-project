; REQUIRES: asserts

; RUN: opt -mtriple arm64-linux -passes=loop-vectorize -mattr=+sve -debug-only=loop-vectorize -disable-output <%s 2>&1 | FileCheck %s

; Invariant register usage calculation should take into account if the
; invariant would be used in widened instructions. Only in such cases, a vector
; register would be required for holding the invariant. For all other cases
; such as below(where usage of %0 in loop doesnt require vector register), a
; general purpose register suffices.
; Check that below test doesn't crash while calculating register usage for
; invariant %0

@string = internal unnamed_addr constant [5 x i8] c"abcd\00", align 1

define void @get_invariant_reg_usage(ptr %z) {
; CHECK-LABEL: LV: Checking a loop in 'get_invariant_reg_usage'
; CHECK: LV(REG): VF = vscale x 16
; CHECK-NEXT: LV(REG): Found max usage: 2 item
; CHECK-NEXT: LV(REG): RegisterClass: Generic::ScalarRC, 2 registers
; CHECK-NEXT: LV(REG): RegisterClass: Generic::VectorRC, 1 registers
; CHECK-NEXT: LV(REG): Found invariant usage: 1 item
; CHECK-NEXT: LV(REG): RegisterClass: Generic::ScalarRC, 3 registers

L.entry:
  %0 = load i128, ptr %z, align 16
  %1 = icmp slt i128 %0, 1
  %a = getelementptr i8, ptr %z, i64 1
  br i1 %1, label %return, label %loopbody

loopbody:                  ;preds = %L.entry, %loopbody
  %b = phi ptr [ %2, %loopbody ], [ @string, %L.entry ]
  %len_input = phi i128 [ %len, %loopbody ], [ %0, %L.entry ]
  %len = add nsw i128 %len_input, -1
  %2 = getelementptr i8, ptr %b, i64 1
  %3 = load i8, ptr %b, align 1
  store i8 %3, ptr %a, align 4
  %.not = icmp eq i128 %len, 0
  br i1 %.not, label %return, label %loopbody

return:                    ;preds = %loopexit, %L.entry
  ret void
}

define void @load_and_compare_only_used_by_assume(ptr %a, ptr noalias %b) {
; CHECK-LABEL: LV: Checking a loop in 'load_and_compare_only_used_by_assume'
; CHECK: LV(REG): VF = vscale x 4
; CHECK-NEXT: LV(REG): Found max usage: 2 item
; CHECK-NEXT: LV(REG): RegisterClass: Generic::ScalarRC, 3 registers
; CHECK-NEXT: LV(REG): RegisterClass: Generic::VectorRC, 1 registers
; CHECK-NEXT: LV(REG): Found invariant usage: 1 item

entry:
  br label %loop

loop:
  %iv = phi i64 [ 0, %entry ], [ %iv.next, %loop ]
  %gep.a = getelementptr inbounds i32, ptr %a, i64 %iv
  %l.a = load i32, ptr %gep.a
  %gep.b = getelementptr inbounds i32, ptr %b, i64 %iv
  %l.b = load i32, ptr %gep.b
  %c = icmp ugt i32 %l.b, 10
  call void @llvm.assume(i1 %c)
  store i32 %l.a, ptr %gep.b
  %iv.next = add i64 %iv, 1
  %ec = icmp eq i64 %iv.next, 1000
  br i1 %ec, label %exit, label %loop

exit:
  ret void
}
