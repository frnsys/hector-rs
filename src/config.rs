
use cxx::UniquePtr;
use crate::ffi::HectorClient;

pub fn configure(h: &UniquePtr<HectorClient>) {
    h.set_double("C2F6_halocarbon", "molarMass", 138.01);
    h.set_double("C2F6_halocarbon", "rho", 0.00026);
    h.set_double("C2F6_halocarbon", "tau", 10000.0);
    h.set_double("CCl4_halocarbon", "molarMass", 153.8);
    h.set_double("CCl4_halocarbon", "rho", 0.00013);
    h.set_double("CCl4_halocarbon", "tau", 26.0);
    h.set_double_unit("CF4_halocarbon", "H0", 35.0, "pptv");
    h.set_double("CF4_halocarbon", "molarMass", 88.0043);
    h.set_double("CF4_halocarbon", "rho", 8e-05);
    h.set_double("CF4_halocarbon", "tau", 50000.0);
    h.set_double("CFC113_halocarbon", "molarMass", 187.35);
    h.set_double("CFC113_halocarbon", "rho", 0.0003);
    h.set_double("CFC113_halocarbon", "tau", 85.0);
    h.set_double("CFC114_halocarbon", "molarMass", 170.9);
    h.set_double("CFC114_halocarbon", "rho", 0.00031);
    h.set_double("CFC114_halocarbon", "tau", 300.);
    h.set_double("CFC115_halocarbon", "molarMass", 154.45);
    h.set_double("CFC115_halocarbon", "rho", 0.00018);
    h.set_double("CFC115_halocarbon", "tau", 1700.);
    h.set_double("CFC11_halocarbon", "molarMass", 137.35);
    h.set_double("CFC11_halocarbon", "rho", 0.00025);
    h.set_double("CFC11_halocarbon", "tau", 45.0);
    h.set_double("CFC12_halocarbon", "molarMass", 120.9);
    h.set_double("CFC12_halocarbon", "rho", 0.00032);
    h.set_double("CFC12_halocarbon", "tau", 100.);
    h.set_double("CH3Br_halocarbon", "H0", 5.8);
    h.set_double("CH3Br_halocarbon", "molarMass", 94.9);
    h.set_double("CH3Br_halocarbon", "rho", 1e-05);
    h.set_double("CH3Br_halocarbon", "tau", 0.7);
    h.set_double("CH3CCl3_halocarbon", "molarMass", 133.35);
    h.set_double("CH3CCl3_halocarbon", "rho", 6e-05);
    h.set_double("CH3CCl3_halocarbon", "tau", 5.0);
    h.set_double("CH3Cl_halocarbon", "H0", 504.0);
    h.set_double("CH3Cl_halocarbon", "molarMass", 50.45);
    h.set_double("CH3Cl_halocarbon", "rho", 1e-05);
    h.set_double("CH3Cl_halocarbon", "tau", 1.3);
    h.set_double("CH4", "CH4N", 300.);
    h.set_double("CH4", "M0", 653.);
    h.set_double("CH4", "Tsoil", 160.);
    h.set_double("CH4", "Tstrat", 120.);
    h.set_double("CH4", "UC_CH4", 2.78);
    h.set_double("HCFC141b_halocarbon", "molarMass", 116.9);
    h.set_double("HCFC141b_halocarbon", "rho", 0.00014);
    h.set_double("HCFC141b_halocarbon", "tau", 9.3);
    h.set_double("HCFC142b_halocarbon", "molarMass", 100.45);
    h.set_double("HCFC142b_halocarbon", "rho", 0.0002);
    h.set_double("HCFC142b_halocarbon", "tau", 17.9);
    h.set_double("HCFC22_halocarbon", "molarMass", 86.45);
    h.set_double("HCFC22_halocarbon", "rho", 0.0002);
    h.set_double("HCFC22_halocarbon", "tau", 12.0);
    h.set_double("HFC125_halocarbon", "molarMass", 120.02);
    h.set_double("HFC125_halocarbon", "rho", 0.00023);
    h.set_double("HFC125_halocarbon", "tau", 29.0);
    h.set_double("HFC134a_halocarbon", "molarMass", 102.02);
    h.set_double("HFC134a_halocarbon", "rho", 0.00016);
    h.set_double("HFC134a_halocarbon", "tau", 14.0);
    h.set_double("HFC143a_halocarbon", "molarMass", 84.04);
    h.set_double("HFC143a_halocarbon", "rho", 0.00013);
    h.set_double("HFC143a_halocarbon", "tau", 52.0);
    h.set_double("HFC227ea_halocarbon", "molarMass", 170.03);
    h.set_double("HFC227ea_halocarbon", "rho", 0.00026);
    h.set_double("HFC227ea_halocarbon", "tau", 34.2);
    h.set_double("HFC23_halocarbon", "molarMass", 70.0);
    h.set_double("HFC23_halocarbon", "rho", 0.00019);
    h.set_double("HFC23_halocarbon", "tau", 270.0);
    h.set_double("HFC245fa_halocarbon", "molarMass", 134.0);
    h.set_double("HFC245fa_halocarbon", "rho", 0.00028);
    h.set_double("HFC245fa_halocarbon", "tau", 7.6);
    h.set_double("HFC32_halocarbon", "molarMass", 52.0);
    h.set_double("HFC32_halocarbon", "rho", 0.00011);
    h.set_double("HFC32_halocarbon", "tau", 4.9);
    h.set_double("HFC4310_halocarbon", "molarMass", 252.0);
    h.set_double("HFC4310_halocarbon", "rho", 0.0004);
    h.set_double("HFC4310_halocarbon", "tau", 15.9);
    h.set_double("N2O", "N0", 272.9596);
    h.set_timed_double("N2O", "N2O_natural_emissions", 1765, 11.);
    h.set_double("N2O", "TN2O0", 132.);
    h.set_double("N2O", "UC_N2O", 4.8);
    h.set_double("OH", "CCH4", -0.32);
    h.set_double("OH", "CCO", -0.000105);
    h.set_double("OH", "CNMVOC", -0.000315);
    h.set_double("OH", "CNOX", 0.0042);
    h.set_double("OH", "TOH0", 6.6);
    h.set_double("SF6_halocarbon", "molarMass", 146.06);
    h.set_double("SF6_halocarbon", "rho", 0.00052);
    h.set_double("SF6_halocarbon", "tau", 3200.0);
    h.set_double("carbon-cycle-solver", "dt", 0.25);
    h.set_double("carbon-cycle-solver", "eps_abs", 1e-06);
    h.set_double("carbon-cycle-solver", "eps_rel", 1e-06);
    h.set_double("carbon-cycle-solver", "eps_spinup", 0.001);
    h.set_double("core", "do_spinup", 1.0);
    h.set_double("core", "endDate", 2300.);
    h.set_double("core", "max_spinup", 2000.);
    h.set_string("core", "run_name", "wasm-hector-run");
    h.set_double("core", "startDate", 1745.);
    h.set_double("forcing", "baseyear", 1750.);
    h.set_double("halon1211_halocarbon", "molarMass", 165.35);
    h.set_double("halon1211_halocarbon", "rho", 3e-05);
    h.set_double("halon1211_halocarbon", "tau", 16.0);
    h.set_double("halon1301_halocarbon", "molarMass", 148.9);
    h.set_double("halon1301_halocarbon", "rho", 0.00032);
    h.set_double("halon1301_halocarbon", "tau", 65.0);
    h.set_double("halon2402_halocarbon", "molarMass", 259.8);
    h.set_double("halon2402_halocarbon", "rho", 0.00033);
    h.set_double("halon2402_halocarbon", "tau", 20.0);
    h.set_double("ocean", "enabled", 1.0);
    h.set_double("ocean", "spinup_chem", 0.);
    h.set_double("ocean", "tid", 200000000.);
    h.set_double("ocean", "tt", 72000000.);
    h.set_double("ocean", "tu", 49000000.);
    h.set_double("ocean", "twi", 12500000.);
    h.set_double("ozone", "PO3", 30.0);
    h.set_timed_double("simpleNbox", "Ftalbedo", 1750, 0.0);
    h.set_double("simpleNbox", "atmos_c", 588.071);
    h.set_double("simpleNbox", "beta", 0.36);
    h.set_double("simpleNbox", "detritus_c", 55.);
    h.set_double("simpleNbox", "f_litterd", 0.98);
    h.set_double("simpleNbox", "f_lucd", 0.01);
    h.set_double("simpleNbox", "f_lucv", 0.1);
    h.set_double("simpleNbox", "f_nppd", 0.6);
    h.set_double("simpleNbox", "f_nppv", 0.35);
    h.set_double("simpleNbox", "npp_flux0", 50.0);
    h.set_double("simpleNbox", "q10_rh", 2.0);
    h.set_double("simpleNbox", "soil_c", 1782.);
    h.set_double("simpleNbox", "veg_c", 550.);
    h.set_double("so2", "S0", 53841.2);
    h.set_double("so2", "SN", 42000.);
    h.set_timed_double("so2", "SV", 1765, 0.);
    h.set_double("temperature", "S", 3.0);
    h.set_double("temperature", "alpha", 1.0);
    h.set_double("temperature", "diff", 2.3);
    h.set_double("temperature", "enabled", 1.0);
    h.set_double("temperature", "volscl", 1.0);
}
