// RUN: mlir-opt -allow-unregistered-dialect -test-scf-if-utils -split-input-file %s | FileCheck %s

// -----

//      CHECK: func @outlined_then0(%{{.*}}: i1, %{{.*}}: memref<?xf32>) -> i8 {
// CHECK-NEXT:   %{{.*}} = "some_op"(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> i8
// CHECK-NEXT:   return %{{.*}} : i8
// CHECK-NEXT: }
//      CHECK: func @outlined_else0(%{{.*}}: i8) -> i8 {
// CHECK-NEXT:   return %{{.*}}0 : i8
// CHECK-NEXT: }
//      CHECK: func @outline_if_else(
// CHECK-NEXT:   %{{.*}} = scf.if %{{.*}} -> (i8) {
// CHECK-NEXT:     %{{.*}} = func.call @outlined_then0(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> i8
// CHECK-NEXT:     scf.yield %{{.*}} : i8
// CHECK-NEXT:   } else {
// CHECK-NEXT:     %{{.*}} = func.call @outlined_else0(%{{.*}}) : (i8) -> i8
// CHECK-NEXT:     scf.yield %{{.*}} : i8
// CHECK-NEXT:   }
// CHECK-NEXT:   return
// CHECK-NEXT: }
func.func @outline_if_else(%cond: i1, %a: index, %b: memref<?xf32>, %c: i8) {
  %r = scf.if %cond -> (i8) {
    %r = "some_op"(%cond, %b) : (i1, memref<?xf32>) -> (i8)
    scf.yield %r : i8
  } else {
    scf.yield %c : i8
  }
  return
}

// -----

//      CHECK: func @outlined_then0(%{{.*}}: i1, %{{.*}}: memref<?xf32>) {
// CHECK-NEXT:   "some_op"(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> ()
// CHECK-NEXT:   return
// CHECK-NEXT: }
//      CHECK: func @outline_if(
// CHECK-NEXT:   scf.if %{{.*}} {
// CHECK-NEXT:     func.call @outlined_then0(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> ()
// CHECK-NEXT:   }
// CHECK-NEXT:   return
// CHECK-NEXT: }
func.func @outline_if(%cond: i1, %a: index, %b: memref<?xf32>, %c: i8) {
  scf.if %cond {
    "some_op"(%cond, %b) : (i1, memref<?xf32>) -> ()
    scf.yield
  }
  return
}

// -----

//      CHECK: func @outlined_then0() {
// CHECK-NEXT:   return
// CHECK-NEXT: }
//      CHECK: func @outlined_else0(%{{.*}}: i1, %{{.*}}: memref<?xf32>) {
// CHECK-NEXT:   "some_op"(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> ()
// CHECK-NEXT:   return
// CHECK-NEXT: }
//      CHECK: func @outline_empty_if_else(
// CHECK-NEXT:   scf.if %{{.*}} {
// CHECK-NEXT:     func.call @outlined_then0() : () -> ()
// CHECK-NEXT:   } else {
// CHECK-NEXT:     func.call @outlined_else0(%{{.*}}, %{{.*}}) : (i1, memref<?xf32>) -> ()
// CHECK-NEXT:   }
// CHECK-NEXT:   return
// CHECK-NEXT: }
func.func @outline_empty_if_else(%cond: i1, %a: index, %b: memref<?xf32>, %c: i8) {
  scf.if %cond {
  } else {
    "some_op"(%cond, %b) : (i1, memref<?xf32>) -> ()
  }
  return
}

// -----

// This test checks scf utils can work on llvm func.

//      CHECK: func @outlined_then0() {
// CHECK-NEXT:   return
// CHECK-NEXT: }
//      CHECK: func @outlined_else0(%{{.*}}: i1, %{{.*}}: i32) {
// CHECK-NEXT:   "some_op"(%{{.*}}, %{{.*}}) : (i1, i32) -> ()
// CHECK-NEXT:   return
// CHECK-NEXT: }
//      CHECK: llvm.func @llvm_func(%{{.*}}: i1, %{{.*}}: i32) {
// CHECK-NEXT:   scf.if %{{.*}} {
// CHECK-NEXT:     func.call @outlined_then0() : () -> ()
// CHECK-NEXT:   } else {
// CHECK-NEXT:     func.call @outlined_else0(%{{.*}}, %{{.*}}) : (i1, i32) -> ()
// CHECK-NEXT:   }
// CHECK-NEXT:   llvm.return
// CHECK-NEXT: }
llvm.func @llvm_func(%cond: i1, %a: i32) {
  scf.if %cond {
  } else {
    "some_op"(%cond, %a) : (i1, i32) -> ()
  }
  llvm.return
}
