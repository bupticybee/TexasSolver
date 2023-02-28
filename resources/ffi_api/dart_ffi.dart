import 'dart:ffi';
import 'package:ffi/ffi.dart'; // Generally only available in Flutter project

typedef Calc_Equity = Int Function(Pointer<Utf8> input_file, Pointer<Utf8> resource_dir, Pointer<Utf8> mode); // Native types
typedef CalcEquity = int Function(Pointer<Utf8> input_file, Pointer<Utf8> resource_dir, Pointer<Utf8> mode); // Dart types, in this case there is not Dart equivalent for Pointer or UTF8 strings
final nativeApiLib = DynamicLibrary.open('api.dll');
final CalcEquity _calculateEquity = nativeApiLib.lookup<NativeFunction<Calc_Equity>>('calculateEquity').asFunction();