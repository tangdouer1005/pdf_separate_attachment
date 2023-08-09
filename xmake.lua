add_rules("mode.debug", "mode.release")

package("lolly")
    set_homepage("https://github.com/XmacsLabs/lolly")
    set_description("Lolly is a C++ library")

    add_urls("https://github.com/XmacsLabs/lolly.git")
    add_urls("https://gitee.com/XmacsLabs/lolly.git")

    add_versions("v0.99.5", "8755ab6a1966a72ced2e6fcf26a88bd633ecdb11")

    on_install("linux", "macosx", "mingw", "wasm", function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
    end)
package_end()

set_languages("c++17")
add_requires("pdfhummus")
add_requires("lolly")


target("PdfDemo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("pdfhummus")
    add_packages("lolly")
    add_runenvs("PROJECT_PATH", "$(projectdir)")