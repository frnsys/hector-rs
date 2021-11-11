# Rust interface to Hector

This was developed for a specific project (basically to just calculate the temperature anomaly) so it may not suit all needs, but hopefully provides a foundation for a more general-purpose Rust interface for Hector.

# Setup

1. Run `setup.sh` to clone the Hector source code and apply a patch that removes logging (removes boost filesystem dependency)

# Notes

- Run `convert_config.py` to create/update `src/config.rs`, the default Hector configuration.
- Run `convert_emissions.py` to create/update `src/emissions.rs`, the default emissions scenario (RCP 2.6 up to 2050)
- Run `cargo run --example example` to run the example.
- The glue code is based off of [pyhector](https://github.com/openclimatedata/pyhector)