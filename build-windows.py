import subprocess
import sys
import os
import shutil
import glob


def compile_shader(input_path, output_path, stage, entry):
    cmd = [
        "slangc",
        input_path,
        "-profile",
        "spirv_1_0",
        "-target",
        "spirv",
        "-capability",
        "GLSL_330",
        "-o",
        output_path,
        "-stage",
        stage,
        "-entry",
        entry,
        "-emit-spirv-via-glsl",
    ]
    print(f"{input_path} -> {output_path}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"> Error compiling {input_path} ({stage}):\n{result.stderr}")
        sys.exit(1)


def compile_shaders(src, dir):
    os.makedirs(dir, exist_ok=True)
    for file in os.listdir(src):
        if file.endswith(".slang"):
            base = os.path.splitext(file)[0]
            input_path = os.path.join(src, file)
            output_vert = os.path.join(dir, f"{base}v.spv")
            output_frag = os.path.join(dir, f"{base}f.spv")

            compile_shader(input_path, output_vert, "vertex", "vs_main")
            compile_shader(input_path, output_frag, "fragment", "fs_main")


def run_compile_shaders():
    print("> Compiling shaders...")

    compile_shaders(os.path.normpath("app/res/shaders/src"), os.path.normpath("app/res/shaders/bin"))

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

    clean(["compile_commands.json", "build", ".cache"])

    print("> Cleaned successfully.")


def run_clean_shaders():
    print("> Cleaning shaders...")

    clean([os.path.normpath("thermite/res/shaders/bin"), os.path.normpath("app/res/shaders/bin")])

    print("> Cleaned shaders successfully.")


def run_premake():
    print("> Configuring...")
    try:
        result = subprocess.run(
            ["premake5", "--file=project.lua", "--os=windows", "gmake"], check=True, text=True
        )
        print("\n> Configured successfully.")
    except subprocess.CalledProcessError as e:
        print("\n> Configuraton failed.")
        sys.exit(e.returncode)
    except FileNotFoundError:
        print("\n> premake5 not found. Ensure it is in your Windows PATH.")
        sys.exit(1)


def run_compiledb():
    print("> Compiling...")
    try:
        subprocess.run(
            ["compiledb", "-o", "../../compile_commands.json", "make", "config=debug_windows"],
            cwd=os.path.join(os.getcwd(), "build", "makefiles"),
            check=True,
            text=True,
        )
        print("> Compiled successfully.")
    except subprocess.CalledProcessError as e:
        print("> Compilation failed.")
        sys.exit(e.returncode)
    except FileNotFoundError:
        print("> compiledb (or make) not found. Ensure they are in your Windows PATH.")
        sys.exit(1)


def run_post_build():
    print("> Running post-build steps...")
    target_dir = os.path.join("build", "bin", "windows_x86_64_debug")
    
    if not os.path.exists(target_dir):
        print(f"> Error: Target build directory '{target_dir}' does not exist.")
        return

    # 1. Copy 'app' folder (excluding 'src')
    src_app = "app"
    dst_app = os.path.join(target_dir, "app")
    
    if os.path.exists(src_app):
        if os.path.exists(dst_app):
            shutil.rmtree(dst_app)
            
        def ignore_patterns(path, names):
            if os.path.normpath(path) == os.path.normpath(src_app):
                return ["src"]
            return []

        try:
            shutil.copytree(src_app, dst_app, ignore=ignore_patterns)
            print("> 'app' folder copied successfully (excluding 'src').")
        except Exception as e:
            print(f"> Error copying 'app' folder: {e}")
    else:
        print(f"> Warning: Source folder '{src_app}' not found. Skipping copy.")

    # 2. Delete .lib files in the target folder
    lib_files = glob.glob(os.path.join(target_dir, "*.lib"))
    for lib in lib_files:
        try:
            os.remove(lib)
            print(f"> Deleted: {os.path.basename(lib)}")
        except Exception as e:
            print(f"> Error deleting {lib}: {e}")

    # 3. Rename the built .exe to vlither.exe
    exe_files = glob.glob(os.path.join(target_dir, "*.exe"))
    # Filter out if vlither.exe already exists from a previous step to avoid renaming it to itself
    exe_files = [f for f in exe_files if os.path.basename(f).lower() != "vlither.exe"]
    
    if len(exe_files) == 1:
        old_exe = exe_files[0]
        new_exe = os.path.join(target_dir, "vlither.exe")
        try:
            # If an old vlither.exe exists, remove it first so rename doesn't fail
            if os.path.exists(new_exe):
                os.remove(new_exe)
            os.rename(old_exe, new_exe)
            print(f"> Renamed executable: {os.path.basename(old_exe)} -> vlither.exe")
        except Exception as e:
            print(f"> Error renaming executable: {e}")
    elif len(exe_files) > 1:
        print(f"> Warning: Found multiple .exe files. Could not safely determine which one to rename to 'vlither.exe'.")
    else:
        # Check if vlither.exe is already there from a previous run
        if os.path.exists(os.path.join(target_dir, "vlither.exe")):
            print("> 'vlither.exe' already present.")
        else:
            print("> Warning: No executable found to rename.")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Error: Please provide a command number.")
        print("Usage: python build-windows.py <0|1|2|3>")
        print("0: Compile the program, 1: Clean, 2: Compile shaders, 3: Clean shaders")
        sys.exit(1)

    try:
        cmd = int(sys.argv[1])
    except ValueError:
        print("Error: Command must be an integer (0, 1, 2, or 3).")
        sys.exit(1)

    if cmd == 0:
        run_premake()
        os.system("cls" if os.name == "nt" else "clear")
        run_compiledb()
        run_post_build()  # Unified post-build processing hook
    elif cmd == 1:
        run_clean()
    elif cmd == 2:
        run_compile_shaders()
    elif cmd == 3:
        run_clean_shaders()
    else:
        print("Invalid command number.")