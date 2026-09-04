import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeConfigDeps
from conan.tools.files import copy


class Cytopia(ConanFile):
    name = "Cytopia"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        # self.requires("microprofile/3.1")
        self.requires("angelscript/2.37.0")
        self.requires("libnoise/1.0.0")
        self.requires("openal-soft/1.24.3")
        self.requires("sdl/3.4.14", force=True)
        self.requires("sdl_image/2.8.12")
        self.requires("sdl_ttf/2.24.0")
        self.requires("vorbis/1.3.7")
        self.requires("zlib/1.3.1", force=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeConfigDeps(self)
        deps.generate()
        if self.settings.os == "Windows" and self.settings.build_type == "Release":
            deps.configuration = "RelWithDebInfo"
            deps.generate()

        for dep in self.dependencies.values():
            for f in dep.cpp_info.bindirs:
                self.cp_data(f)
            for f in dep.cpp_info.libdirs:
                self.cp_data(f)

    def cp_data(self, src):
        bindir = os.path.join(self.build_folder, "bin")
        copy(self, "*.dll", src, bindir, False)
        copy(self, "*.so*", src, bindir, False)
