// RUN: %clang_cc1 -triple dxil-pc-shadermodel6.0-library -x hlsl -ast-dump -DEMPTY \
// RUN:   -DRESOURCE=RWBuffer %s | FileCheck -DRESOURCE=RWBuffer -check-prefix=EMPTY %s
//
// RUN: %clang_cc1 -triple dxil-pc-shadermodel6.0-library -x hlsl -ast-dump \
// RUN:   -DRESOURCE=RWBuffer %s | FileCheck -DRESOURCE=RWBuffer \
// RUN:   -check-prefixes=CHECK,CHECK-UAV %s

// TODO: Add AST tests for Buffer and RasterizerOrderedBuffer here

// This test tests two different AST generations each typed buffer.
// The "EMPTY" test mode verifies the AST generated by forward declaration
// of the HLSL types which happens on initializing the HLSL external AST with
// an AST Context.

// The non-empty mode has a use that requires the resource type be complete,
// which results in the AST being populated by the external AST source. That
// case covers the full implementation of the template declaration and the
// instantiated specialization.

// EMPTY: ClassTemplateDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit [[RESOURCE]]
// EMPTY-NEXT: TemplateTypeParmDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> typename depth 0 index 0 element_type
// EMPTY-NEXT: ConceptSpecializationExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'bool' Concept 0x{{[0-9A-Fa-f]+}} '__is_typed_resource_element_compatible'
// EMPTY-NEXT: ImplicitConceptSpecializationDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc>
// EMPTY-NEXT: TemplateArgument type 'type-parameter-0-0'
// EMPTY-NEXT: TemplateTypeParmType 0x{{[0-9A-Fa-f]+}} 'type-parameter-0-0' dependent depth 0 index 0
// EMPTY-NEXT: TemplateTypeParm 0x{{[0-9A-Fa-f]+}} depth 0 index 0
// EMPTY-NEXT: TemplateArgument type 'element_type':'type-parameter-0-0'
// EMPTY-NEXT: TemplateTypeParmType 0x{{[0-9A-Fa-f]+}} 'element_type' dependent depth 0 index 0
// EMPTY-NEXT: TemplateTypeParm 0x{{[0-9A-Fa-f]+}} 'element_type'
// EMPTY-NEXT: CXXRecordDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit <undeserialized declarations> class [[RESOURCE]]
// EMPTY-NEXT: FinalAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit final

// There should be no more occurrences of the resource type
// EMPTY-NOT: {{[^[:alnum:]]}}[[RESOURCE]]

#ifndef EMPTY

RESOURCE<float> Buffer;

#endif

// CHECK: ClassTemplateDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit [[RESOURCE]]
// CHECK-NEXT: TemplateTypeParmDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> typename depth 0 index 0 element_type
// CHECK-NEXT: ConceptSpecializationExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'bool' Concept 0x{{[0-9A-Fa-f]+}} '__is_typed_resource_element_compatible'
// CHECK-NEXT: ImplicitConceptSpecializationDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc>
// CHECK-NEXT: TemplateArgument type 'type-parameter-0-0'
// CHECK-NEXT: TemplateTypeParmType 0x{{[0-9A-Fa-f]+}} 'type-parameter-0-0' dependent depth 0 index 0
// CHECK-NEXT: TemplateTypeParm 0x{{[0-9A-Fa-f]+}} depth 0 index 0
// CHECK-NEXT: TemplateArgument type 'element_type':'type-parameter-0-0'
// CHECK-NEXT: TemplateTypeParmType 0x{{[0-9A-Fa-f]+}} 'element_type' dependent depth 0 index 0
// CHECK-NEXT: TemplateTypeParm 0x{{[0-9A-Fa-f]+}} 'element_type'
// CHECK-NEXT: CXXRecordDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit class [[RESOURCE]] definition

// CHECK: FinalAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit final
// CHECK-NEXT: FieldDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit __handle '__hlsl_resource_t
// CHECK-UAV-SAME{LITERAL}: [[hlsl::resource_class(UAV)]]
// CHECK-SAME{LITERAL}: [[hlsl::contained_type(element_type)]]
// CHECK-NEXT: HLSLResourceAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit TypedBuffer

// CHECK: CXXMethodDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> operator[] 'const element_type &(unsigned int) const'
// CHECK-NEXT: ParmVarDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> Index 'unsigned int'
// CHECK-NEXT: CompoundStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: ReturnStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: UnaryOperator 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type' prefix '*' cannot overflow
// CHECK-NEXT: CallExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type *'
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '<builtin fn type>' Function 0x{{[0-9A-Fa-f]+}} '__builtin_hlsl_resource_getpointer' 'void (...) noexcept'
// CHECK-NEXT: MemberExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '__hlsl_resource_t
// CHECK-SAME{LITERAL}: [[hlsl::resource_class(UAV)]]
// CHECK-SAME{LITERAL}: [[hlsl::contained_type(element_type)]]
// CHECK-SAME: ' lvalue .__handle 0x{{[0-9A-Fa-f]+}}
// CHECK-NEXT: CXXThisExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'const [[RESOURCE]]<element_type>' lvalue implicit this
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'unsigned int' ParmVar 0x{{[0-9A-Fa-f]+}} 'Index' 'unsigned int'
// CHECK-NEXT: AlwaysInlineAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit always_inline

// CHECK-NEXT: CXXMethodDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> operator[] 'element_type &(unsigned int)'
// CHECK-NEXT: ParmVarDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> Index 'unsigned int'
// CHECK-NEXT: CompoundStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: ReturnStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: UnaryOperator 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type' prefix '*' cannot overflow
// CHECK-NEXT: CallExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type *'
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '<builtin fn type>' Function 0x{{[0-9A-Fa-f]+}} '__builtin_hlsl_resource_getpointer' 'void (...) noexcept'
// CHECK-NEXT: MemberExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '__hlsl_resource_t
// CHECK-SAME{LITERAL}: [[hlsl::resource_class(UAV)]]
// CHECK-SAME{LITERAL}: [[hlsl::contained_type(element_type)]]
// CHECK-SAME: ' lvalue .__handle 0x{{[0-9A-Fa-f]+}}
// CHECK-NEXT: CXXThisExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '[[RESOURCE]]<element_type>' lvalue implicit this
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'unsigned int' ParmVar 0x{{[0-9A-Fa-f]+}} 'Index' 'unsigned int'
// CHECK-NEXT: AlwaysInlineAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit always_inline

// CHECK-NEXT: CXXMethodDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> Load 'element_type (unsigned int)'
// CHECK-NEXT: ParmVarDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> Index 'unsigned int'
// CHECK-NEXT: CompoundStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: ReturnStmt 0x{{[0-9A-Fa-f]+}} <<invalid sloc>>
// CHECK-NEXT: UnaryOperator 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type' prefix '*' cannot overflow
// CHECK-NEXT: CallExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'element_type *'
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '<builtin fn type>' Function 0x{{[0-9A-Fa-f]+}} '__builtin_hlsl_resource_getpointer' 'void (...) noexcept'
// CHECK-NEXT: MemberExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '__hlsl_resource_t
// CHECK-SAME{LITERAL}: [[hlsl::resource_class(UAV)]]
// CHECK-SAME{LITERAL}: [[hlsl::contained_type(element_type)]]
// CHECK-SAME: ' lvalue .__handle 0x{{[0-9A-Fa-f]+}}
// CHECK-NEXT: CXXThisExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> '[[RESOURCE]]<element_type>' lvalue implicit this
// CHECK-NEXT: DeclRefExpr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> 'unsigned int' ParmVar 0x{{[0-9A-Fa-f]+}} 'Index' 'unsigned int'
// CHECK-NEXT: AlwaysInlineAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit always_inline

// CHECK: ClassTemplateSpecializationDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> class [[RESOURCE]] definition

// CHECK: TemplateArgument type 'float'
// CHECK-NEXT: BuiltinType 0x{{[0-9A-Fa-f]+}} 'float'
// CHECK-NEXT: FinalAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit final
// CHECK-NEXT: FieldDecl 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> <invalid sloc> implicit __handle '__hlsl_resource_t
// CHECK-UAV-SAME{LITERAL}: [[hlsl::resource_class(UAV)]]
// CHECK-SAME{LITERAL}: [[hlsl::contained_type(float)]]
// CHECK-NEXT: HLSLResourceAttr 0x{{[0-9A-Fa-f]+}} <<invalid sloc>> Implicit TypedBuffer
