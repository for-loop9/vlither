import subprocess
import sys
import os
import shutil

def compile_shader(input_path, output_path, stage, entry):
  cmd = [
    'slangc',
    input_path,
    '-profile', 'spirv_1_0',
    '-target', 'spirv',
    '-capability', 'GLSL_330',
    '-o', output_path,
    '-stage', stage,
    '-entry', entry,
    '-emit-spirv-via-glsl'
  ]
  print(f"{input_path} -> {output_path}")
  result = subprocess.run(cmd, capture_output=True, text=True)
  if result.returncode != 0:
    print(f"> Error compiling {input_path} ({stage}):\n{result.stderr}")
    sys.exit(1)

def compile_shaders(src, dir):
  os.makedirs(dir, exist_ok=True)
  for file in os.listdir(src):
    if file.endswith('.slang'):
      base = os.path.splitext(file)[0]
      input_path = os.path.join(src, file)
      output_vert = os.path.join(dir, f'{base}v.spv')
      output_frag = os.path.join(dir, f'{base}f.spv')

      compile_shader(input_path, output_vert, 'vertex', 'vs_main')
      compile_shader(input_path, output_frag, 'fragment', 'fs_main')

def run_compile_shaders():
  print("> Compiling shaders...")
  
  compile_shaders('thermite/res/shaders/src', 'thermite/res/shaders/bin')
  compile_shaders('app/res/shaders/src', 'app/res/shaders/bin')

  print("> Shaders compiled successfully.")

def clean(paths_to_delete):
  for path in paths_to_delete:
    if os.path.exists(path):
      print(f"Deleting {path}")
      if os.path.isfile(path):
        os.remove(path)
      elif os.path.isdir(path):
        shutil.rmtree(path)

def run_clean():
  print("> Cleaning...")

  clean([
    "compile_commands.json",
    "build",
    ".cache"
  ])

  print("> Cleaned successfully.")

def run_clean_shaders():
  print("> Cleaning shaders...")

  clean([
    "thermite/res/shaders/bin",
    "app/res/shaders/bin"
  ])

  print("> Cleaned shaders successfully.")

def run_premake():
  print("> Configuring...")
  try:
    result = subprocess.run(
      ["premake5", "--file=project.lua", "gmake"],
      check=True,
      text=True
    )
    print("\n> Configured successfully.")
  except subprocess.CalledProcessError as e:
    print("\n> Configuraton failed.")
    sys.exit(e.returncode)
  except FileNotFoundError:
    print("\n> premake5 not found.")
    sys.exit(1)

def run_compiledb():
  print("> Compiling...")
  try:
    subprocess.run(
      ["compiledb", "-o", "../../compile_commands.json", "make"],
      cwd=os.path.join(os.getcwd(), "build", "makefiles"),
      check=True,
      text=True
    )
    print("> Compiled successfully.")
  except subprocess.CalledProcessError as e:
    print("> Compilation failed.")
    sys.exit(e.returncode)
  except FileNotFoundError:
    print("> compiledb not found.")
    sys.exit(1)

if __name__ == "__main__":
  argc = len(sys.argv)
  
  if argc == 1:
    run_premake()
    os.system("clear")
    run_compiledb()
  elif argc == 2:
    run_clean()
  elif argc == 3:
    run_compile_shaders()
  elif argc == 4:
    run_clean_shaders()
