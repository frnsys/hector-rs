"""
Run this to generate a Rust file
with a function to configue Hector.
This is kind of hacky but easier than parsing
the config within Rust itself :)
"""

import json

config = json.load(open('data/config.json'))

def emit_value(section, variable, value):
    if isinstance(value, list):
        if len(value) == 2 and isinstance(value[1], str):
            return 'h.set_double_unit("{}", "{}", {}, "{}")'.format(
                section, variable, value[0], value[1])
        else:
            for v in value:
                if len(v) == 3:
                    return 'h.set_timed_double_unit("{}", "{}", {}, {}, "{}")'.format(
                        section, variable, v[0], v[1], v[2])
                else:
                    if isinstance(v[1], int):
                        return 'h.set_timed_double("{}", "{}", {}, {}.)'.format(
                            section, variable, v[0], v[1])
                    else:
                        return 'h.set_timed_double("{}", "{}", {}, {})'.format(
                            section, variable, v[0], v[1])
    else:
        if isinstance(value, str):
            return 'h.set_string("{}", "{}", "{}")'.format(
                section, variable, value)
        elif isinstance(value, bool):
            return 'h.set_double("{}", "{}", {})'.format(
                section, variable, 1.0 if value else 0.0)
        elif isinstance(value, float):
            return 'h.set_double("{}", "{}", {})'.format(
                section, variable, value)
        elif isinstance(value, int):
            return 'h.set_double("{}", "{}", {}.)'.format(
                section, variable, value)
        else:
            raise Exception('Unrecognized value type: "{}"'.format(value))

lines = []
for section, subconf, in config.items():
    for variable, val in subconf.items():
        line = emit_value(section, variable, val)
        lines.append(line)

contents = '''use cxx::UniquePtr;
use crate::ffi::HectorClient;

pub fn configure(h: &UniquePtr<HectorClient>) {{
{}
}}
'''.format('\n'.join('    {};'.format(l) for l in lines))

with open('src/config.rs', 'w') as f:
    f.write(contents)