use hector_rs::{run_hector, emissions::emissions};

fn main() {
    let start_year = 1765;
    let end_year = 2100;
    let ems = emissions();
    unsafe {
        let tgav = run_hector(start_year, end_year, &ems);
        println!("tgav: {:?}", tgav);
    }
}
