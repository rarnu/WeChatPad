#include "dex_helper.h"
#include <cstdint>
#include <endian.h>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

std::ostream &operator<<(std::ostream &out, const DexHelper::Class &clazz) {
  return out << clazz.name;
}

std::ostream &operator<<(std::ostream &out, const DexHelper::Field &field) {
  return out << field.declaring_class.name << "->" << field.name << ":"
             << field.type.name;
}

std::ostream &operator<<(std::ostream &out, const DexHelper::Method &method) {
  out << method.declaring_class.name << "->" << method.name << "(";

  for (auto &param : method.parameters) {
    out << param.name;
  }

  return out << ")" << method.return_type.name;
}

int main(int argc, char *argv[]) {
  std::vector<std::tuple<const void *, size_t>> dexs;
  for (int i = 1; i <= 100; ++i) {
    std::string path = "dexs/classes" +
                       (i == 1 ? std::string("") : std::to_string(i)) + ".dex";
    int raw_dex = open(path.data(), O_RDONLY);
    if (raw_dex == -1) {
        break;
    }
    struct stat s {};
    fstat(raw_dex, &s);
    auto *out = reinterpret_cast<const dex::u1 *>(
        mmap(nullptr, s.st_size, PROT_READ, MAP_PRIVATE, raw_dex, 0));
    dexs.emplace_back(out, s.st_size);
  }

  std::string_view to_find = argv[2];
  DexHelper helper(dexs);

  auto class_idx = helper.CreateClassIndex("Ljava/lang/Object;");
  auto clazz = helper.DecodeClass(class_idx);
  std::cout << "got class: " << clazz << std::endl;

  auto field_indices = helper.FindField(class_idx, {}, true);
  if (!field_indices.empty()) {
    auto field_idx = field_indices[0];
    auto field = helper.DecodeField(field_idx);
    std::cout << "got field: " << field << std::endl;
    {
      auto method_indices = helper.FindMethodSettingField(field_idx, -1, -1, "",
                                                          -1, {}, {}, {}, true);
      if (!method_indices.empty()) {
        auto method_idx = method_indices[0];
        auto method = helper.DecodeMethod(method_idx);
        std::cout << "got method settings field " << field << " : " << method << std::endl;
      }
    }
    {
      auto method_indices = helper.FindMethodGettingField(field_idx, -1, -1, "",
                                                          -1, {}, {}, {}, true);
      if (!method_indices.empty()) {
        auto method_idx = method_indices[0];
        auto method = helper.DecodeMethod(method_idx);
        std::cout << "got method getting field " << field << " : " << method << std::endl;
      }
    }
  }
  auto method_indices = helper.FindMethodUsingString(
      "isNullableType", false, -1, 1, "VI", -1, {}, {}, {}, true);
  if (!method_indices.empty()) {
    auto method_idx = method_indices[0];
    auto method = helper.DecodeMethod(method_idx);
    std::cout << "got method with string: " << method << std::endl;
    {
      auto method_indices = helper.FindMethodInvoking(method_idx, -1, -1, "",
                                                      -1, {}, {}, {}, true);
      if (!method_indices.empty()) {
        auto method_idx = method_indices[0];
        auto callee = helper.DecodeMethod(method_idx);
        std::cout << "got method " << method << " invoking " << callee << std::endl;
      }
    }
    {
      auto method_indices =
          helper.FindMethodInvoked(method_idx, -1, -1, "", -1, {}, {}, {}, true);
      if (!method_indices.empty()) {
        auto method_idx = method_indices[0];
        auto caller = helper.DecodeMethod(method_idx);
        std::cout << "got method invoked by " << caller << " : " << method << std::endl;
      }
    }
  }
  //   helper.CreateFullCache();
}