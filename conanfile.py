import os
from conan import ConanFile
from conan.tools.files import copy


class Cytopia(ConanFile):
    name = "Cytopia"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # self.requires("microprofile/3.1")
        self.requires("angelscript/2.37.0")
        self.requires("libnoise/1.0.0")
        self.requires("libpng/1.6.44", force=True)
        self.requires("openal/1.22.2")
        self.requires("sdl/2.30.8", force=True)
        self.requires("sdl_image/2.6.3")
        self.requires("sdl_ttf/2.22.0")
        self.requires("vorbis/1.3.7")
        self.requires("zlib/1.3.1", force=True)
        self.requires("libwebp/1.4.0", override=True)
        if self.settings.os == "Linux":
            self.requires("xz_utils/5.4.5", override=True)
            self.requires("libalsa/1.2.12", override=True)

    def generate(self):
        for dep in self.dependencies.values():
            if len(dep.cpp_info.bindirs) > 0:
                self.cp_data(dep.cpp_info.bindirs[0])
            if len(dep.cpp_info.libdirs) > 0:
                self.cp_data(dep.cpp_info.libdirs[0])

    def cp_data(self, src):
        # bin, *.dll -> bin @ keep_path=False
        # lib, *.dll -> bin @ keep_path=False
        # lib, *.dylib -> bin @ keep_path=False
        # lib, *.so* -> bin @ keep_path=False
        bindir = os.path.join(self.build_folder, "bin")
        copy(self, "*.dll", src, bindir)
        copy(self, "*.dylib", src, bindir)
        copy(self, ".so*", src, bindir)
