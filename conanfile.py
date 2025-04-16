from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    default_options = {
    "libtorrent/*:shared": False,
    "boost/*:shared": False,
    "openssl/*:shared": False,  # Required for libtorrent
    "zlib/*:shared": False,     # Often needed by libcurl/OpenSSL
    "libcurl/*:shared": False,  # If using libcurl
    "ncurses/*:shared": False,  # If using ncurses
    # Add other deps here
}

    def requirements(self):
        self.requires("libtorrent/2.0.10")
        self.requires("gtest/1.15.0")
        self.requires("ncurses/6.5")
        self.requires("libcurl/8.10.1")
        self.requires("boost/1.83.0",override=True)

   

    def layout(self):
        cmake_layout(self)

    def build(self):
        self.run(
            f"conan install . --build=missing -s compiler.cppstd=17 -s build_type=Release -c tools.build:defines='BOOST_CSTDFLOAT_NO_LIBSTDCXX_FLOAT128'"
        )