! RUN: %flang_fc1 -triple powerpc64le-unknown-unknown -emit-llvm %s -o - | FileCheck --check-prefixes="LLVMIR","LLVMIR-LE" %s
! RUN: %flang_fc1 -triple powerpc64-unknown-unknown -emit-llvm %s -o - | FileCheck --check-prefixes="LLVMIR","LLVMIR-BE" %s
! REQUIRES: target=powerpc{{.*}}

!----------------
! vec_splat
!----------------

! CHECK-LABEL: vec_splat_testi8i8
subroutine vec_splat_testi8i8(x)
  vector(integer(1)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi8i8

! CHECK-LABEL: vec_splat_testi8i16
subroutine vec_splat_testi8i16(x)
  vector(integer(1)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi8i16

! CHECK-LABEL: vec_splat_testi8i32
subroutine vec_splat_testi8i32(x)
  vector(integer(1)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi8i32

! CHECK-LABEL: vec_splat_testi8i64
subroutine vec_splat_testi8i64(x)
  vector(integer(1)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi8i64

! CHECK-LABEL: vec_splat_testi16i8
subroutine vec_splat_testi16i8(x)
  vector(integer(2)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi16i8

! CHECK-LABEL: vec_splat_testi16i16
subroutine vec_splat_testi16i16(x)
  vector(integer(2)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi16i16

! CHECK-LABEL: vec_splat_testi16i32
subroutine vec_splat_testi16i32(x)
  vector(integer(2)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi16i32

! CHECK-LABEL: vec_splat_testi16i64
subroutine vec_splat_testi16i64(x)
  vector(integer(2)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi16i64

! CHECK-LABEL: vec_splat_testi32i8
subroutine vec_splat_testi32i8(x)
  vector(integer(4)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi32i8

! CHECK-LABEL: vec_splat_testi32i16
subroutine vec_splat_testi32i16(x)
  vector(integer(4)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi32i16

! CHECK-LABEL: vec_splat_testi32i32
subroutine vec_splat_testi32i32(x)
  vector(integer(4)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi32i32

! CHECK-LABEL: vec_splat_testi32i64
subroutine vec_splat_testi32i64(x)
  vector(integer(4)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi32i64

! CHECK-LABEL: vec_splat_testi64i8
subroutine vec_splat_testi64i8(x)
  vector(integer(8)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi64i8

! CHECK-LABEL: vec_splat_testi64i16
subroutine vec_splat_testi64i16(x)
  vector(integer(8)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi64i16

! CHECK-LABEL: vec_splat_testi64i32
subroutine vec_splat_testi64i32(x)
  vector(integer(8)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi64i32

! CHECK-LABEL: vec_splat_testi64i64
subroutine vec_splat_testi64i64(x)
  vector(integer(8)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testi64i64

! CHECK-LABEL: vec_splat_testf32i8
subroutine vec_splat_testf32i8(x)
  vector(real(4)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <4 x float>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x float> poison, float %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x float> %[[ins]], <4 x float> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x float> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf32i8

! CHECK-LABEL: vec_splat_testf32i16
subroutine vec_splat_testf32i16(x)
  vector(real(4)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <4 x float>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x float> poison, float %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x float> %[[ins]], <4 x float> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x float> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf32i16

! CHECK-LABEL: vec_splat_testf32i32
subroutine vec_splat_testf32i32(x)
  vector(real(4)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <4 x float>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x float> poison, float %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x float> %[[ins]], <4 x float> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x float> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf32i32

! CHECK-LABEL: vec_splat_testf32i64
subroutine vec_splat_testf32i64(x)
  vector(real(4)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <4 x float>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x float> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x float> poison, float %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x float> %[[ins]], <4 x float> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x float> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf32i64

! CHECK-LABEL: vec_splat_testf64i8
subroutine vec_splat_testf64i8(x)
  vector(real(8)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <2 x double>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x double> poison, double %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x double> %[[ins]], <2 x double> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x double> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf64i8

! CHECK-LABEL: vec_splat_testf64i16
subroutine vec_splat_testf64i16(x)
  vector(real(8)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <2 x double>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x double> poison, double %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x double> %[[ins]], <2 x double> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x double> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf64i16

! CHECK-LABEL: vec_splat_testf64i32
subroutine vec_splat_testf64i32(x)
  vector(real(8)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <2 x double>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x double> poison, double %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x double> %[[ins]], <2 x double> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x double> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf64i32

! CHECK-LABEL: vec_splat_testf64i64
subroutine vec_splat_testf64i64(x)
  vector(real(8)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <2 x double>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x double> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x double> poison, double %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x double> %[[ins]], <2 x double> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x double> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testf64i64

! CHECK-LABEL: vec_splat_testu8i8
subroutine vec_splat_testu8i8(x)
  vector(unsigned(1)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu8i8

! CHECK-LABEL: vec_splat_testu8i16
subroutine vec_splat_testu8i16(x)
  vector(unsigned(1)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu8i16

! CHECK-LABEL: vec_splat_testu8i32
subroutine vec_splat_testu8i32(x)
  vector(unsigned(1)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu8i32

! CHECK-LABEL: vec_splat_testu8i64
subroutine vec_splat_testu8i64(x)
  vector(unsigned(1)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <16 x i8>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <16 x i8> %[[x]], i64 15
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu8i64

! CHECK-LABEL: vec_splat_testu16i8
subroutine vec_splat_testu16i8(x)
  vector(unsigned(2)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu16i8

! CHECK-LABEL: vec_splat_testu16i16
subroutine vec_splat_testu16i16(x)
  vector(unsigned(2)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu16i16

! CHECK-LABEL: vec_splat_testu16i32
subroutine vec_splat_testu16i32(x)
  vector(unsigned(2)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu16i32

! CHECK-LABEL: vec_splat_testu16i64
subroutine vec_splat_testu16i64(x)
  vector(unsigned(2)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <8 x i16>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <8 x i16> %[[x]], i64 7
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu16i64

! CHECK-LABEL: vec_splat_testu32i8
subroutine vec_splat_testu32i8(x)
  vector(unsigned(4)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu32i8

! CHECK-LABEL: vec_splat_testu32i16
subroutine vec_splat_testu32i16(x)
  vector(unsigned(4)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu32i16

! CHECK-LABEL: vec_splat_testu32i32
subroutine vec_splat_testu32i32(x)
  vector(unsigned(4)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu32i32

! CHECK-LABEL: vec_splat_testu32i64
subroutine vec_splat_testu32i64(x)
  vector(unsigned(4)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <4 x i32>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <4 x i32> %[[x]], i64 3
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu32i64

! CHECK-LABEL: vec_splat_testu64i8
subroutine vec_splat_testu64i8(x)
  vector(unsigned(8)) :: x, y
  y = vec_splat(x, 0_1)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu64i8

! CHECK-LABEL: vec_splat_testu64i16
subroutine vec_splat_testu64i16(x)
  vector(unsigned(8)) :: x, y
  y = vec_splat(x, 0_2)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu64i16

! CHECK-LABEL: vec_splat_testu64i32
subroutine vec_splat_testu64i32(x)
  vector(unsigned(8)) :: x, y
  y = vec_splat(x, 0_4)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu64i32

! CHECK-LABEL: vec_splat_testu64i64
subroutine vec_splat_testu64i64(x)
  vector(unsigned(8)) :: x, y
  y = vec_splat(x, 0_8)

! LLVMIR: %[[x:.*]] = load <2 x i64>, ptr %{{[0-9]}}, align 16
! LLVMIR-LE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 0
! LLVMIR-BE: %[[ele:.*]] = extractelement <2 x i64> %[[x]], i64 1
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[ele]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splat_testu64i64

!----------------
! vec_splats
!----------------

! CHECK-LABEL: vec_splats_testi8
subroutine vec_splats_testi8(x)
  integer(1) :: x
  vector(integer(1)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load i8, ptr %{{[0-9]}}, align 1
! LLVMIR: %[[ins:.*]] = insertelement <16 x i8> poison, i8 %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <16 x i8> %[[ins]], <16 x i8> poison, <16 x i32> zeroinitializer
! LLVMIR: store <16 x i8> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testi8

! CHECK-LABEL: vec_splats_testi16
subroutine vec_splats_testi16(x)
  integer(2) :: x
  vector(integer(2)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load i16, ptr %{{[0-9]}}, align 2
! LLVMIR: %[[ins:.*]] = insertelement <8 x i16> poison, i16 %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <8 x i16> %[[ins]], <8 x i16> poison, <8 x i32> zeroinitializer
! LLVMIR: store <8 x i16> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testi16

! CHECK-LABEL: vec_splats_testi32
subroutine vec_splats_testi32(x)
  integer(4) :: x
  vector(integer(4)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load i32, ptr %{{[0-9]}}, align 4
! LLVMIR: %[[ins:.*]] = insertelement <4 x i32> poison, i32 %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x i32> %[[ins]], <4 x i32> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x i32> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testi32

! CHECK-LABEL: vec_splats_testi64
subroutine vec_splats_testi64(x)
  integer(8) :: x
  vector(integer(8)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load i64, ptr %{{[0-9]}}, align 8
! LLVMIR: %[[ins:.*]] = insertelement <2 x i64> poison, i64 %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x i64> %[[ins]], <2 x i64> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x i64> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testi64

! CHECK-LABEL: vec_splats_testf32
subroutine vec_splats_testf32(x)
  real(4) :: x
  vector(real(4)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load float, ptr %{{[0-9]}}, align 4
! LLVMIR: %[[ins:.*]] = insertelement <4 x float> poison, float %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <4 x float> %[[ins]], <4 x float> poison, <4 x i32> zeroinitializer
! LLVMIR: store <4 x float> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testf32

! CHECK-LABEL: vec_splats_testf64
subroutine vec_splats_testf64(x)
  real(8) :: x
  vector(real(8)) :: y
  y = vec_splats(x)

! LLVMIR: %[[x:.*]] = load double, ptr %{{[0-9]}}, align 8
! LLVMIR: %[[ins:.*]] = insertelement <2 x double> poison, double %[[x]], i32 0
! LLVMIR: %[[y:.*]] = shufflevector <2 x double> %[[ins]], <2 x double> poison, <2 x i32> zeroinitializer
! LLVMIR: store <2 x double> %[[y]], ptr %{{[0-9]}}, align 16
end subroutine vec_splats_testf64

! CHECK-LABEL: vec_splat_s32testi8
subroutine vec_splat_s32testi8()
  vector(integer(4)) :: y
  y = vec_splat_s32(7_1)

! LLVMIR: store <4 x i32> splat (i32 7), ptr %{{[0-9]}}, align 16
end subroutine vec_splat_s32testi8

! CHECK-LABEL: vec_splat_s32testi16
subroutine vec_splat_s32testi16()
  vector(integer(4)) :: y
  y = vec_splat_s32(7_2)

! LLVMIR: store <4 x i32> splat (i32 7), ptr %{{[0-9]}}, align 16
end subroutine vec_splat_s32testi16

! CHECK-LABEL: vec_splat_s32testi32
subroutine vec_splat_s32testi32()
  vector(integer(4)) :: y
  y = vec_splat_s32(7_4)

! LLVMIR: store <4 x i32> splat (i32 7), ptr %{{[0-9]}}, align 16
end subroutine vec_splat_s32testi32

! CHECK-LABEL: vec_splat_s32testi64
subroutine vec_splat_s32testi64()
  vector(integer(4)) :: y
  y = vec_splat_s32(7_8)

! LLVMIR: store <4 x i32> splat (i32 7), ptr %{{[0-9]}}, align 16
end subroutine vec_splat_s32testi64
