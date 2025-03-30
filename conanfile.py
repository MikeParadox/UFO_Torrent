from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("libtorrent/2.0.10")
        self.requires("gtest/1.15.0")
        self.requires("ncurses/6.5")
        self.requires("libcurl/8.10.1")
        self.requires("boost/1.81.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        self.run(
            f"conan install . --build=missing -s compiler.cppstd=17 -s build_type=Release -c tools.build:defines='BOOST_CSTDFLOAT_NO_LIBSTDCXX_FLOAT128'"
        )
