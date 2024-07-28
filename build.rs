fn main() {
    cxx_build::bridge("src/lib.rs")
        .define("NO_LOGGING", None)
        .file("src/Hector.cpp")
        .file("src/Observable.cpp")
        .file("hector/src/bc_component.cpp")
        .file("hector/src/carbon-cycle-model.cpp")
        .file("hector/src/carbon-cycle-solver.cpp")
        .file("hector/src/ch4_component.cpp")
        .file("hector/src/core.cpp")
        .file("hector/src/dependency_finder.cpp")
        .file("hector/src/forcing_component.cpp")
        .file("hector/src/h_interpolator.cpp")
        .file("hector/src/halocarbon_component.cpp")
        .file("hector/src/logger.cpp")
        .file("hector/src/n2o_component.cpp")
        .file("hector/src/o3_component.cpp")
        .file("hector/src/oc_component.cpp")
        .file("hector/src/ocean_component.cpp")
        .file("hector/src/ocean_csys.cpp")
        .file("hector/src/oceanbox.cpp")
        .file("hector/src/oh_component.cpp")
        .file("hector/src/simpleNbox.cpp")
        .file("hector/src/slr_component.cpp")
        .file("hector/src/so2_component.cpp")
        .file("hector/src/spline_forsythe.cpp")
        .file("hector/src/temperature_component.cpp")
        .file("hector/src/unitval.cpp")
        .include("include")
        .include("hector/inst/include")
        .flag_if_supported("-std=c++14")
        .flag_if_supported("/EHsc")
        .compile("hector-rs");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/Hector.cpp");
    println!("cargo:rerun-if-changed=include/Hector.h");
}
