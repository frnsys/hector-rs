import json

emissions = json.load(open('data/emissions.json'))
scenario = json.load(open('data/rcp26.to_2050.json'))
data = scenario['data']

lines = []
for section, subsections in emissions.items():
    lines.append('    let mut {} = HashMap::default();'.format(section))
    for k in subsections:
        try:
            vals = data[k]
            lines.append('    {}.insert("{}", vec!{}[..n].to_vec());'.format(section, k, vals))
        except:
            continue
    lines.append('    emissions.insert("{}", {});'.format(section, section))

contents = '''use crate::Emissions;
use std::collections::HashMap;

pub const START_YEAR: usize = {};

pub fn get_emissions(to_year: usize) -> Emissions {{
    let n = to_year - START_YEAR;
    let mut emissions: Emissions = HashMap::new();
{}
    emissions
}}
'''.format(scenario['startYear'], '\n'.join(lines))

with open('src/emissions.rs', 'w') as f:
    f.write(contents)