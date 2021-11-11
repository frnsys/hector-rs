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
            lines.append('    {}.insert("{}", vec!{});'.format(section, k, vals))
        except:
            continue
    lines.append('    emissions.insert("{}", {});'.format(section, section))

contents = '''use crate::Emissions;
use std::collections::HashMap;

pub fn emissions() -> Emissions {{
    let mut emissions: Emissions = HashMap::new();
{}
    emissions
}}
'''.format('\n'.join(lines))

with open('src/emissions.rs', 'w') as f:
    f.write(contents)