use hector_rs::{run_hector, emissions::{START_YEAR, get_emissions}};

fn main() {
    let end_year = 2100;
    let emissions = get_emissions(2050);
    assert_eq!(emissions["simpleNbox"]["ffi_emissions"].len(), 2050-START_YEAR);
    let tgav = unsafe {
        run_hector(end_year, &emissions)
    };
    println!("tgav: {:?}", tgav);
}
