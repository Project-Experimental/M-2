/* 
 *                  type_traits.h
 * 
 * Description : This idea is referenced by Fuchsia Kernel
 * 
 * Writer : Nakada Tokumei <nakada_tokumei@protonmail.com>
 * 
 */


#pragma once

#include <type_traits>

namespace m2l
{

using std::integral_constant;
using std::bool_constant;

using std::true_type;
using std::false_type;

// Primary type
using std::is_void;
using std::is_void_v;
using std::is_null_pointer;
using std::is_null_pointer_v;
using std::is_integral;
using std::is_integral_v;
using std::is_floating_point;
using std::is_floating_point_v;
using std::is_array;
using std::is_array_v;
using std::is_enum;
using std::is_enum_v;
using std::is_union;
using std::is_union_v;
using std::is_class;
using std::is_class_v;
using std::is_function;
using std::is_function_v;
using std::is_pointer;
using std::is_pointer_v;
using std::is_lvalue_reference;
using std::is_lvalue_reference_v;
using std::is_rvalue_reference;
using std::is_rvalue_reference_v;
using std::is_member_object_pointer;
using std::is_member_object_pointer_v;
using std::is_member_function_pointer;
using std::is_member_function_pointer_v;

// Composite type
using std::is_fundamental;
using std::is_fundamental_v;
using std::is_arithmetic;
using std::is_arithmetic_v;
using std::is_scalar;
using std::is_scalar_v;
using std::is_object;
using std::is_object_v;
using std::is_compound;
using std::is_compound_v;
using std::is_reference;
using std::is_reference_v;
using std::is_member_pointer;
using std::is_member_pointer_v;

};
