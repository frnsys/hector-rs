mod config;
pub mod emissions;
use config::configure;
use std::collections::HashMap;
use emissions::START_YEAR;

#[cxx::bridge(namespace = "rust_hector")]
mod ffi {
    unsafe extern "C++" {
        include!("Hector.h");

        type HectorClient;

        fn new_hector() -> UniquePtr<HectorClient>;
        fn run(&self);
        fn shutdown(&self);
        fn set_string(&self, section: &str, variable: &str, value: &str);
        fn set_double(&self, section: &str, variable: &str, value: f64);
        fn set_double_unit(&self, section: &str, variable: &str, value: f64, unit: &str);
        fn set_timed_double(&self, section: &str, variable: &str, year: usize, value: f64);
        fn set_timed_double_unit(&self, section: &str, variable: &str, year: usize, value: f64, unit: &str);
        fn set_timed_array(&self, section: &str, variable: &str, years: &Vec<usize>, values: &Vec<f64>);
        fn add_observable(&self, component: &str, name: &str, need_date: bool, in_spinup: bool);
        fn get_observable(&self, component: &str, name: &str, in_spinup: bool) -> Vec<f64>;
    }
}

pub type Emissions = HashMap<&'static str, HashMap<&'static str, Vec<f64>>>;

pub unsafe fn run_hector(end_year: usize, emissions: &Emissions) -> f64 {
    let hector = ffi::new_hector();

    configure(&hector);

    // Set emissions
    let years: Vec<usize> = (START_YEAR..end_year).collect();
    for (section, sources) in emissions {
        for (source, vals) in sources {
            // Fill missing values with last known value
            let values = if vals.len() < years.len() {
                let n = years.len() - vals.len();
                let last = vals.last().unwrap();
                let fill = vec![*last; n];
                let mut values = vals.clone();
                values.extend(fill);
                values
            } else if vals.len() > years.len() {
                vals.clone()[..years.len()].to_vec()
            } else {
                vals.clone()
            };
            assert_eq!(years.len(), values.len());
            hector.set_timed_array(section, source, &years, &values);
        }
    }

    hector.set_double("core", "endDate", end_year as f64);

    hector.add_observable("temperature", "Tgav", false, false);
    hector.run();
    let tgavs = hector.get_observable("temperature", "Tgav", false);
    hector.shutdown();
    *tgavs.last().unwrap()
}
