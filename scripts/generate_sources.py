import os
from jinja2 import Environment, FileSystemLoader

# Configuration for different types
types_config = [
    {
        "type": "float",
        "math_suffix": "f",
        "math_prefix": "",
    },
    {
        "type": "double",
        "math_suffix": "",
        "math_prefix": "",
    },
    {
        "type": "int32_t",
        "math_suffix": "f",  # Use float math for generation
        "math_prefix": "(int32_t)",  # Cast result back to int32_t
    },
]

# Templates and their output filename patterns
templates = [
    ("complex.h.j2", "complex_{type}.h"),
    ("complex.c.j2", "complex_{type}.c"),
    ("constants.h.j2", "constants_{type}.h"),
    ("constants.c.j2", "constants_{type}.c"),
    ("waveform_gen.h.j2", "waveform_gen_{type}.h"),
    ("waveform_gen.c.j2", "waveform_gen_{type}.c"),
    ("spectrum_gen.h.j2", "spectrum_gen_{type}.h"),
    ("spectrum_gen.c.j2", "spectrum_gen_{type}.c"),
]


def generate():
    # Templates are in the 'templates' directory
    env = Environment(loader=FileSystemLoader("templates"))

    # Ensure we are in the project root (assuming script is run from there)
    # or handle paths relative to this script

    for config in types_config:
        for template_name, output_pattern in templates:
            try:
                template = env.get_template(template_name)
                output_name = output_pattern.format(type=config["type"])
                output_path = os.path.join("inc", output_name)

                print(f"Generating {output_path}...")
                content = template.render(config)
                with open(output_path, "w") as f:
                    f.write(content)
            except Exception as e:
                print(f"Error generating {output_name}: {e}")


if __name__ == "__main__":
    generate()
