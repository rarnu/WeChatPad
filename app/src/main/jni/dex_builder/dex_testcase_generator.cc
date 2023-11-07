/* DexBuilder
 * Copyright (C) 2021 LSPosed
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * Copyright (C) 2018 The Android Open Source Project
 * Modifications copyright (C) 2021 LSPosed Developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dex_builder.h"
#include "slicer/dex_format.h"
#include "slicer/reader.h"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <iostream>
#include <vector>

// Adding tests here requires changes in several other places. See README.md in
// the view_compiler directory for more information.

using namespace startop::dex;
using namespace std;

void GenerateTrivialDexFile(const string &outdir) {
  DexBuilder dex_file;

  ClassBuilder cbuilder{
      dex_file.MakeClass("android.startop.test.testcases.Trivial")};
  cbuilder.set_source_file("dex_testcase_generator.cc#GenerateTrivialDexFile");

  slicer::MemView image{dex_file.CreateImage()};
  std::ofstream out_file(outdir + "/trivial.dex");
  out_file.write(image.ptr<const char>(), image.size());
}

// Generates test cases that test around 1 instruction.
void GenerateSimpleTestCases(const string &outdir) {
  DexBuilder dex_file;

  ClassBuilder cbuilder{
      dex_file.MakeClass("android.startop.test.testcases.SimpleTests")};
  cbuilder.set_source_file("dex_testcase_generator.cc#GenerateSimpleTestCases");

  auto static_field =
      cbuilder.CreateField("staticInteger", TypeDescriptor::Int);
  static_field.Encode();
  auto instance_field =
      cbuilder.CreateField("instanceField", TypeDescriptor::Int);
  instance_field.access_flags(dex::kAccPublic);
  instance_field.Encode();

  // int return5() { return 5; }
  auto return5{
      cbuilder.CreateMethod("return5", Prototype{TypeDescriptor::Int})};
  {
    LiveRegister r{return5.AllocRegister()};
    return5.BuildConst(r, 5);
    return5.BuildReturn(r);
  }
  return5.Encode();

  // int return5() { return 5; }
  auto returnInteger5{cbuilder.CreateMethod(
      "returnInteger5", Prototype{TypeDescriptor::ObjectInt})};
  [&](MethodBuilder &method) {
    LiveRegister five{method.AllocRegister()};
    method.BuildConst(five, 5);
    LiveRegister object{method.AllocRegister()};
    method.BuildNew(object, TypeDescriptor::ObjectInt,
                    Prototype{TypeDescriptor::Void, TypeDescriptor::Int}, five);
    method.BuildUnBoxIfPrimitive(five, TypeDescriptor::ObjectInt, object);
    method.BuildReturn(object, /*is_object=*/true);
  }(returnInteger5);
  returnInteger5.Encode();

  // // int returnParam(int x) { return x; }
  auto returnParam{cbuilder.CreateMethod(
      "returnParam", Prototype{TypeDescriptor::Int, TypeDescriptor::Int})};
  returnParam.BuildReturn(Value::Parameter(0));
  returnParam.Encode();

  // int returnStringLength(String x) { return x.length(); }
  MethodDeclData string_length{dex_file.GetOrDeclareMethod(
      TypeDescriptor::String, "length", Prototype{TypeDescriptor::Int})};

  auto returnStringLength{cbuilder.CreateMethod(
      "returnStringLength",
      Prototype{TypeDescriptor::Int, TypeDescriptor::String})};
  {
    LiveRegister result = returnStringLength.AllocRegister();
    returnStringLength.AddInstruction(Instruction::InvokeVirtual(
        string_length.id, result, Value::Parameter(0)));
    returnStringLength.BuildReturn(result);
  }
  returnStringLength.Encode();

  // int returnIfZero(int x) { if (x == 0) { return 5; } else { return 3; } }
  MethodBuilder returnIfZero{cbuilder.CreateMethod(
      "returnIfZero", Prototype{TypeDescriptor::Int, TypeDescriptor::Int})};
  {
    LiveRegister resultIfZero{returnIfZero.AllocRegister()};
    Value else_target{returnIfZero.MakeLabel()};
    returnIfZero.AddInstruction(
        Instruction::OpWithArgs(Instruction::Op::kBranchEqz, /*dest=*/{},
                                Value::Parameter(0), else_target));
    // else branch
    returnIfZero.BuildConst(resultIfZero, 3);
    returnIfZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturn, /*dest=*/{}, resultIfZero));
    // then branch
    returnIfZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kBindLabel, /*dest=*/{}, else_target));
    returnIfZero.BuildConst(resultIfZero, 5);
    returnIfZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturn, /*dest=*/{}, resultIfZero));
  }
  returnIfZero.Encode();

  // int returnIfNotZero(int x) { if (x != 0) { return 5; } else { return 3; } }
  MethodBuilder returnIfNotZero{cbuilder.CreateMethod(
      "returnIfNotZero", Prototype{TypeDescriptor::Int, TypeDescriptor::Int})};
  {
    LiveRegister resultIfNotZero{returnIfNotZero.AllocRegister()};
    Value else_target{returnIfNotZero.MakeLabel()};
    returnIfNotZero.AddInstruction(
        Instruction::OpWithArgs(Instruction::Op::kBranchNEqz, /*dest=*/{},
                                Value::Parameter(0), else_target));
    // else branch
    returnIfNotZero.BuildConst(resultIfNotZero, 3);
    returnIfNotZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturn, /*dest=*/{}, resultIfNotZero));
    // then branch
    returnIfNotZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kBindLabel, /*dest=*/{}, else_target));
    returnIfNotZero.BuildConst(resultIfNotZero, 5);
    returnIfNotZero.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturn, /*dest=*/{}, resultIfNotZero));
  }
  returnIfNotZero.Encode();

  // Make sure backwards branches work too.
  //
  // Pseudo code for test:
  // {
  //   zero = 0;
  //   result = 1;
  //   if (zero == 0) goto B;
  // A:
  //   return result;
  // B:
  //   result = 2;
  //   if (zero == 0) goto A;
  //   result = 3;
  //   return result;
  // }
  // If it runs correctly, this test should return 2.
  MethodBuilder backwardsBranch{
      cbuilder.CreateMethod("backwardsBranch", Prototype{TypeDescriptor::Int})};
  [](MethodBuilder &method) {
    LiveRegister zero = method.AllocRegister();
    LiveRegister result = method.AllocRegister();
    Value labelA = method.MakeLabel();
    Value labelB = method.MakeLabel();
    method.BuildConst(zero, 0);
    method.BuildConst(result, 1);
    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBranchEqz,
                                                  /*dest=*/{}, zero, labelB));

    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBindLabel,
                                                  /*dest=*/{}, labelA));
    method.BuildReturn(result);

    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBindLabel,
                                                  /*dest=*/{}, labelB));
    method.BuildConst(result, 2);
    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBranchEqz,
                                                  /*dest=*/{}, zero, labelA));

    method.BuildConst(result, 3);
    method.BuildReturn(result);
  }(backwardsBranch);
  backwardsBranch.Encode();

  // Test that we can make a null value. Basically:
  //
  // public static String returnNull() { return null; }
  MethodBuilder returnNull{
      cbuilder.CreateMethod("returnNull", Prototype{TypeDescriptor::String})};
  [](MethodBuilder &method) {
    LiveRegister zero = method.AllocRegister();
    method.BuildConst(zero, 0);
    method.BuildReturn(zero, /*is_object=*/true);
  }(returnNull);
  returnNull.Encode();

  // Test that we can make String literals. Basically:
  //
  // public static String makeString() { return "Hello, World!"; }
  MethodBuilder makeString{
      cbuilder.CreateMethod("makeString", Prototype{TypeDescriptor::String})};
  [](MethodBuilder &method) {
    LiveRegister string = method.AllocRegister();
    method.BuildConstString(string, "Hello, World!");
    method.BuildReturn(string, /*is_object=*/true);
  }(makeString);
  makeString.Encode();

  // Make sure strings are sorted correctly.
  //
  // int returnStringIfZeroAB(int x) { if (x == 0) { return "a"; } else { return
  // "b"; } }
  MethodBuilder returnStringIfZeroAB{cbuilder.CreateMethod(
      "returnStringIfZeroAB",
      Prototype{TypeDescriptor::String, TypeDescriptor::Int})};
  [&](MethodBuilder &method) {
    LiveRegister resultIfZero{method.AllocRegister()};
    Value else_target{method.MakeLabel()};
    method.AddInstruction(
        Instruction::OpWithArgs(Instruction::Op::kBranchEqz, /*dest=*/{},
                                Value::Parameter(0), else_target));
    // else branch
    method.BuildConstString(resultIfZero, "b");
    method.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturnObject, /*dest=*/{}, resultIfZero));
    // then branch
    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBindLabel,
                                                  /*dest=*/{}, else_target));
    method.BuildConstString(resultIfZero, "a");
    method.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturnObject, /*dest=*/{}, resultIfZero));
    method.Encode();
  }(returnStringIfZeroAB);
  // int returnStringIfZeroAB(int x) { if (x == 0) { return "b"; } else { return
  // "a"; } }
  MethodBuilder returnStringIfZeroBA{cbuilder.CreateMethod(
      "returnStringIfZeroBA",
      Prototype{TypeDescriptor::String, TypeDescriptor::Int})};
  [&](MethodBuilder &method) {
    LiveRegister resultIfZero{method.AllocRegister()};
    Value else_target{method.MakeLabel()};
    method.AddInstruction(
        Instruction::OpWithArgs(Instruction::Op::kBranchEqz, /*dest=*/{},
                                Value::Parameter(0), else_target));
    // else branch
    method.BuildConstString(resultIfZero, "a");
    method.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturnObject, /*dest=*/{}, resultIfZero));
    // then branch
    method.AddInstruction(Instruction::OpWithArgs(Instruction::Op::kBindLabel,
                                                  /*dest=*/{}, else_target));
    method.BuildConstString(resultIfZero, "b");
    method.AddInstruction(Instruction::OpWithArgs(
        Instruction::Op::kReturnObject, /*dest=*/{}, resultIfZero));
    method.Encode();
  }(returnStringIfZeroBA);

  // Make sure we can invoke static methods that return an object
  // String invokeStaticReturnObject(int n, int radix) { return
  // java.lang.Integer.toString(n, radix); }
  MethodBuilder invokeStaticReturnObject{cbuilder.CreateMethod(
      "invokeStaticReturnObject",
      Prototype{TypeDescriptor::String, TypeDescriptor::Int,
                TypeDescriptor::Int})};
  [&](MethodBuilder &method) {
    LiveRegister result{method.AllocRegister()};
    MethodDeclData to_string{dex_file.GetOrDeclareMethod(
        TypeDescriptor::FromClassname("java.lang.Integer"), "toString",
        Prototype{TypeDescriptor::String, TypeDescriptor::Int,
                  TypeDescriptor::Int})};
    method.AddInstruction(Instruction::InvokeStaticObject(
        to_string.id, result, Value::Parameter(0), Value::Parameter(1)));
    method.BuildReturn(result, /*is_object=*/true);
    method.Encode();
  }(invokeStaticReturnObject);

  // Make sure we can invoke virtual methods that return an object
  // String invokeVirtualReturnObject(String s, int n) { return s.substring(n);
  // }
  MethodBuilder invokeVirtualReturnObject{cbuilder.CreateMethod(
      "invokeVirtualReturnObject",
      Prototype{TypeDescriptor::String, TypeDescriptor::String,
                TypeDescriptor::Int})};
  [&](MethodBuilder &method) {
    LiveRegister result{method.AllocRegister()};
    MethodDeclData substring{dex_file.GetOrDeclareMethod(
        TypeDescriptor::String, "substring",
        Prototype{TypeDescriptor::String, TypeDescriptor::Int})};
    method.AddInstruction(Instruction::InvokeVirtualObject(
        substring.id, result, Value::Parameter(0), Value::Parameter(1)));
    method.BuildReturn(result, /*is_object=*/true);
    method.Encode();
  }(invokeVirtualReturnObject);

  // Make sure we can cast objects
  // String castObjectToString(Object o) { return (String)o; }
  MethodBuilder castObjectToString{cbuilder.CreateMethod(
      "castObjectToString",
      Prototype{TypeDescriptor::String, TypeDescriptor::Object})};
  [&](MethodBuilder &method) {
    const ir::Type *type_def =
        dex_file.GetOrAddType(TypeDescriptor::String.descriptor());
    method.AddInstruction(Instruction::Cast(Value::Parameter(0),
                                            Value::Type(type_def->orig_index)));
    method.BuildReturn(Value::Parameter(0), /*is_object=*/true);
    method.Encode();
  }(castObjectToString);

  // Read a static field
  // int readStaticField() { return TestClass.staticInteger; }
  MethodBuilder readStaticField{
      cbuilder.CreateMethod("readStaticField", Prototype{TypeDescriptor::Int})};
  [&](MethodBuilder &method) {
    const ir::FieldDecl *field = dex_file.GetOrAddField(
        cbuilder.descriptor(), "staticInteger", TypeDescriptor::Int);
    LiveRegister result{method.AllocRegister()};
    method.AddInstruction(
        Instruction::GetStaticField(field->orig_index, result));
    method.BuildReturn(result, /*is_object=*/false);
    method.Encode();
  }(readStaticField);

  // Set a static field
  // void setStaticField() { TestClass.staticInteger = 7; }
  MethodBuilder setStaticField{
      cbuilder.CreateMethod("setStaticField", Prototype{TypeDescriptor::Void})};
  [&](MethodBuilder &method) {
    const ir::FieldDecl *field = dex_file.GetOrAddField(
        cbuilder.descriptor(), "staticInteger", TypeDescriptor::Int);
    LiveRegister number{method.AllocRegister()};
    method.BuildConst(number, 7);
    method.AddInstruction(
        Instruction::SetStaticField(field->orig_index, number));
    method.BuildReturn();
    method.Encode();
  }(setStaticField);

  // Read an instance field
  // int readInstanceField(TestClass obj) { return obj.instanceField; }
  MethodBuilder readInstanceField{cbuilder.CreateMethod(
      "readInstanceField",
      Prototype{TypeDescriptor::Int, cbuilder.descriptor()})};
  [&](MethodBuilder &method) {
    const ir::FieldDecl *field = dex_file.GetOrAddField(
        cbuilder.descriptor(), "instanceField", TypeDescriptor::Int);
    LiveRegister result{method.AllocRegister()};
    method.AddInstruction(
        Instruction::GetField(field->orig_index, result, Value::Parameter(0)));
    method.BuildReturn(result, /*is_object=*/false);
    method.Encode();
  }(readInstanceField);

  // Set an instance field
  // void setInstanceField(TestClass obj) { obj.instanceField = 7; }
  MethodBuilder setInstanceField{cbuilder.CreateMethod(
      "setInstanceField",
      Prototype{TypeDescriptor::Void, cbuilder.descriptor()})};
  [&](MethodBuilder &method) {
    const ir::FieldDecl *field = dex_file.GetOrAddField(
        cbuilder.descriptor(), "instanceField", TypeDescriptor::Int);
    LiveRegister number{method.AllocRegister()};
    method.BuildConst(number, 7);
    method.AddInstruction(
        Instruction::SetField(field->orig_index, Value::Parameter(0), number));
    method.BuildReturn();
    method.Encode();
  }(setInstanceField);

  MethodBuilder newArray{
      cbuilder.CreateMethod("newArray", Prototype{TypeDescriptor::Void})};
  [&](MethodBuilder &method) {
    LiveRegister index{method.AllocRegister()};
    method.BuildConst(index, 5);
    LiveRegister array{method.AllocRegister()};
    method.BuildNewArray(array, TypeDescriptor::Object, index);
    LiveRegister object{method.AllocRegister()};
    for (int i = 0; i < 5; ++i) {
      method.BuildConst(index, i);
      method.BuildBoxIfPrimitive(object, TypeDescriptor::Int, index);
      method.BuildAput(Instruction::Op::kAputObject, array, object, index);
    }
    method.BuildReturn();
    method.Encode();
  }(newArray);

  slicer::MemView image{dex_file.CreateImage()};
  std::ofstream out_file(outdir + "/simple.dex");
  out_file.write(image.ptr<const char>(), image.size());
}

void GenerateHooker(const string &outdir) {
  DexBuilder dex_file;

  auto return_type = TypeDescriptor::Double;
  const auto parameter_types =
      std::vector{TypeDescriptor::Int,
                 TypeDescriptor::Long,
                  TypeDescriptor::FromClassname("io.github.lsposed.Test")};

  ClassBuilder cbuilder{dex_file.MakeClass("LSPHooker")};
  cbuilder.set_source_file("dex_testcase_generator.cc#GenerateHooker");

  auto hooker_type =
      TypeDescriptor::FromClassname("org.lsposed.lspd.hooker.LspHooker");

  auto *hooker_field = cbuilder.CreateField("hooker", hooker_type)
                           .access_flags(dex::kAccStatic)
                           .Encode();

  auto setupBuilder{cbuilder.CreateMethod(
      "setup", Prototype{TypeDescriptor::Void, hooker_type})};
  setupBuilder
      .AddInstruction(Instruction::SetStaticObjectField(
          hooker_field->decl->orig_index, Value::Parameter(0)))
      .BuildReturn()
      .Encode();

  auto hookBuilder{cbuilder.CreateMethod(
      "hook", Prototype{return_type, parameter_types})};
  // allocate tmp frist because of wide
  auto tmp{hookBuilder.AllocRegister()};
  hookBuilder.BuildConst(tmp, parameter_types.size());
  auto hook_params_array{hookBuilder.AllocRegister()};
  hookBuilder.BuildNewArray(hook_params_array, TypeDescriptor::Object, tmp);
  for (size_t i = 0u, j = 0u; i < parameter_types.size(); ++i, ++j) {
    hookBuilder.BuildBoxIfPrimitive(Value::Parameter(j), parameter_types[i],
                                    Value::Parameter(j));
    hookBuilder.BuildConst(tmp, i);
    hookBuilder.BuildAput(Instruction::Op::kAputObject, hook_params_array,
                          Value::Parameter(j), tmp);
    if (parameter_types[i].is_wide()) ++j;
  }
  auto handle_hook_method{dex_file.GetOrDeclareMethod(
      hooker_type, "handleHookedMethod",
      Prototype{TypeDescriptor::Object, TypeDescriptor::Object.ToArray()})};
  hookBuilder.AddInstruction(
      Instruction::GetStaticObjectField(hooker_field->decl->orig_index, tmp));
  hookBuilder.AddInstruction(Instruction::InvokeVirtualObject(
      handle_hook_method.id, tmp, tmp, hook_params_array));
  if (return_type == TypeDescriptor::Void) {
    hookBuilder.BuildReturn();
  } else if (return_type.is_primitive()) {
    auto box_type{return_type.ToBoxType()};
    const ir::Type *type_def = dex_file.GetOrAddType(box_type);
    hookBuilder.AddInstruction(
        Instruction::Cast(tmp, Value::Type(type_def->orig_index)));
    hookBuilder.BuildUnBoxIfPrimitive(tmp, box_type, tmp);
    hookBuilder.BuildReturn(tmp, false, return_type.is_wide());
  } else {
    const ir::Type *type_def = dex_file.GetOrAddType(return_type);
    hookBuilder.AddInstruction(
        Instruction::Cast(tmp, Value::Type(type_def->orig_index)));
    hookBuilder.BuildReturn(tmp, true);
  }
  auto *hook_method = hookBuilder.Encode();

  auto backup_builder{
      cbuilder.CreateMethod("backup", Prototype{return_type, parameter_types})};
  if (return_type == TypeDescriptor::Void) {
    backup_builder.BuildReturn();
  } else if(return_type.is_wide()){
    LiveRegister zero = backup_builder.AllocRegister();
    LiveRegister zero_wide = backup_builder.AllocRegister();
    backup_builder.BuildConstWide(zero, 0);
    backup_builder.BuildReturn(zero, /*is_object=*/true, true);
  } else {
    LiveRegister zero = backup_builder.AllocRegister();
    LiveRegister zero_wide = backup_builder.AllocRegister();
    backup_builder.BuildConst(zero, 0);
    backup_builder.BuildReturn(zero, /*is_object=*/true, false);
  }
  auto *back_method = backup_builder.Encode();

  slicer::MemView image{dex_file.CreateImage()};
  std::ofstream out_file(outdir + "/hooker.dex");
  out_file.write(image.ptr<const char>(), image.size());
}

int main(int argc, char **argv) {
  assert(argc == 2);

  string outdir = argv[1];
//   ifstream in(outdir + "/test.dex");

//   std::vector<uint8_t> buf{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};

//   dex::Reader reader(buf.data(), buf.size());

//   reader.CreateFullIr();
//   auto ir = reader.GetIr();
//   auto idx = reader.FindClassIndex("Lio/github/xposed/test/T;");
//   std::cout << ir->classes[idx]->direct_methods[1]->decl->prototype->shorty->c_str() << std::endl;

//   GenerateTrivialDexFile(outdir);
//   GenerateSimpleTestCases(outdir);
  GenerateHooker(outdir);
}
