# ----------------------------------------------------------------------------------#
# //////////////////////////////////////////////////////////////////////////////////#
# ----------------------------------------------------------------------------------#
#
#  Copyright (C) 2018, StepToSky
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1.Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#  2.Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and / or other materials provided with the distribution.
#  3.Neither the name of StepToSky nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  Contacts: www.steptosky.com
#
# ----------------------------------------------------------------------------------#
# //////////////////////////////////////////////////////////////////////////////////#
# ----------------------------------------------------------------------------------#

import os
from conans import ConanFile, CMake
from conanfile_vcs import ConanVcs

vcs_data = ConanVcs()
vcs_data.load_vcs_data()


class LibConan(ConanFile):
    version = "0.1.2"
    name = 'sts-signals'
    url = 'https://github.com/steptosky/sts-signals'
    license = 'BSD 3-Clause'
    description = "Cross-platform headers only C++ library for working with signals-slots pattern."
    author = 'StepToSky <info@steptosky.com>'
    settings = "os", "compiler", "build_type", "arch"
    default_options = 'gtest:shared=False', 'gtest:build_gmock=True'
    exports = 'vcs_data', 'conanfile_vcs.py'
    exports_sources = 'CMakeLists.txt', 'src/*', 'src-test-unsafe/*', \
                      'src-test-safe/*', 'include/*', 'cmake/*', 'license*'
    no_copy_source = True
    generators = 'cmake'

    build_test_var = "CONAN_BUILD_TESTING"
    test_dir_var = "CONAN_TESTING_REPORT_DIR"

    def configure(self):
        if self.settings.compiler == "Visual Studio" and float(str(self.settings.compiler.version)) < 12:
            raise Exception("Visual Studio 12 (2013) or higher is required")

    def requirements(self):
        if os.getenv(self.build_test_var, "0") == "1":
            self.requires('gtest/1.8.0@bincrafters/stable', private=True)

    def build(self):
        build_testing = os.getenv(self.build_test_var, "0")
        test_dir = os.getenv(self.test_dir_var, "")
        cmake = CMake(self)
        vcs_data.setup_cmake(cmake)
        cmake.definitions["BUILD_TESTING"] = 'ON' if build_testing == "1" else 'OFF'
        if test_dir:
            cmake.definitions["TESTING_REPORT_DIR"] = test_dir
        cmake.configure()
        cmake.build()
        cmake.install()
        if build_testing == "1":
            cmake.test()

    def package(self):
        self.copy("license*", src=".", dst="licenses", ignore_case=True, keep_path=False)
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()

# ----------------------------------------------------------------------------------#
# //////////////////////////////////////////////////////////////////////////////////#
# ----------------------------------------------------------------------------------#
